/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "audio/aurator.h"

SL_BOOL sl_aurator_pa_intialized = SL_FALSE;

void sl_aurator_create( sl_aurator *ret, ui32_t channel_count, ui32_t sample_rate, ui32_t updates_per_second_guaranteed )
{
	PaError err;

	// First initialize portaudio if we haven't
	if( !sl_aurator_pa_intialized ) {
		err = Pa_Initialize( );
		if( err != paNoError ){
			assert( SL_FALSE ); // Could not initialize audio system
								// @DEBUG: This shouldn't be an assert....
			Pa_Terminate( );
			return;
		} else {
			sl_aurator_pa_intialized = SL_TRUE;
		}
	}

	// Create the clip array
	ret->clips = vul_vector_create( sizeof( sl_aurator_clip ), 0 );
	// State
	ret->next_id = 0;
	ret->frames_per_buffer = sample_rate / updates_per_second_guaranteed; // @TODO: Move to next power of 2 or something
	ret->volume = 0.5f;

	// Timing, we wait at least half a frame before uploading the next one.
	ret->clk = vul_timer_create( );
	ret->last_time = vul_timer_get_millis( ret->clk );
	ret->wait_time = ( ui64_t )( ( f64_t )500.0 / ( f64_t )updates_per_second_guaranteed );

	// Internal buffer state
	ret->data.sample_count = channel_count * ret->frames_per_buffer;
	ret->data.channel_count = channel_count;
	ret->data.samples = ( i32_t* )malloc( sizeof( i32_t ) * ret->data.sample_count * channel_count );
	memset( ret->data.samples, ( i32_t )0, sizeof( i32_t ) * ret->data.sample_count * channel_count );

	// Finally create the portaudio stream
	// @TODO: Make device selection possible.
	ret->portaudio_params.device = Pa_GetDefaultOutputDevice( );
	if( ret->portaudio_params.device == paNoDevice ) {
		assert( SL_FALSE ); // No audio device! @DEBUG: This shouldn't be an assert....
		Pa_Terminate( );
		sl_aurator_pa_intialized = SL_FALSE; // If we have no device, might as well shut doen the system...
		return;
	}
	ret->portaudio_params.channelCount = channel_count;
	ret->portaudio_params.sampleFormat = paInt32; // Clips are 16-bit (since that is what stb_voris returns), but we mix in 32-bit.
	ret->portaudio_params.suggestedLatency = Pa_GetDeviceInfo( ret->portaudio_params.device )->defaultLowOutputLatency;
	ret->portaudio_params.hostApiSpecificStreamInfo = NULL;
	err = Pa_OpenStream( &ret->portaudio_stream, NULL, /* No input */
						 &ret->portaudio_params,
						 sample_rate, ret->frames_per_buffer,
						 paNoFlag, /* Clip by default, we don't trust our users! */
						 NULL, NULL ); /* No callback and userdata */
	if( err != paNoError ) {
		assert( SL_FALSE ); // Couldn't create the audio stream
							// @DEBUG: Shouldn't be an assert
		return;
	}

	// And finally, start the stream
	err = Pa_StartStream( ret->portaudio_stream );
	if( err != paNoError ) {
		assert( SL_FALSE ); // Couldn't start stream
							// @DEBUG: Shouldn't be an assert
		return;
	}
}

void sl_aurator_destroy( sl_aurator *aurator )
{
	PaError err;
	sl_aurator_clip *ci, *cil;

	assert( aurator );

	// Stop the stream
	err = Pa_StopStream( aurator->portaudio_stream );
	if( err != paNoError ) {
		assert( SL_FALSE ); // Could not stop stream!
							// @DEBUG: Should not be an assert!
	}

	// Close the stream
	err = Pa_CloseStream( aurator->portaudio_stream );
	if( err != paNoError ) {
		assert( SL_FALSE ); // Could not close stream!
							// @DEBUG: Should not be an assert!
	}

	// Free the clips
	vul_foreach( sl_aurator_clip, ci, cil, aurator->clips )
	{
		free( ci->stream );
	}
	vul_vector_destroy( aurator->clips );

	// Free the stream data
	free( aurator->data.samples );
}

void sl_aurator_finalize_system( )
{
	if( sl_aurator_pa_intialized ) {
		Pa_Terminate( );
	}
}

ui32_t sl_aurator_allocate_clip( sl_aurator *aurator )
{
	sl_aurator_clip *clip;

	assert( aurator );

	clip = ( sl_aurator_clip* )vul_vector_add_empty( aurator->clips );
	clip->state = SL_AURATOR_STOPPED;
	clip->sample_current = 0;
	clip->id = aurator->next_id++;
	
	return clip->id;
}

ui32_t sl_aurator_add_clip( sl_aurator *aurator, ui32_t sample_count, i16_t *samples, SL_BOOL loop )
{
	sl_aurator_clip *clip;

	assert( aurator );

	clip = ( sl_aurator_clip* )vul_vector_add_empty( aurator->clips );
	clip->id = aurator->next_id++;
	clip->state = loop ? SL_AURATOR_RUNNING_LOOPED : SL_AURATOR_RUNNING;
	clip->sample_current = 0;
	clip->sample_count = sample_count;
	clip->stream = samples;

	return clip->id;
}

/*
 * Returns the clip for a given id.
 */
sl_aurator_clip *sl_aurator_get_clip( sl_aurator *aurator, ui32_t id )
{
	sl_aurator_clip *ci, *cil;

	assert( aurator );

	vul_foreach( sl_aurator_clip, ci, cil, aurator->clips )
	{
		if( ci->id == id ) {
			return ci;
		}
	}

	return NULL;
}

static SL_BOOL done = SL_FALSE;

void sl_aurator_update( sl_aurator *aurator )
{
	sl_aurator_clip *ci, *cil;
	ui32_t count, i, c;
	i32_t expanded;
	SL_BOOL deleted;
	PaError err;
	ui64_t now;

	assert( aurator );
	
	now = vul_timer_get_millis( aurator->clk );
	if( now - aurator->last_time < aurator->wait_time ) {
		return;
	}
	aurator->last_time = now;

	// Clear our buffer before mixing
	memset( aurator->data.samples,
		    0,
			sizeof( i32_t ) * aurator->frames_per_buffer * aurator->data.channel_count );
	// Mix into it @TODO: We should probably mix into a higher accuracy than the samples are; use in32 f.e.,
	// so we don't run into numerical accuracy issues; using int32 we can have 65k clips before we should have any isses
	// with unneccesary clamping.
	vul_foreach( sl_aurator_clip, ci, cil, aurator->clips )
	{
		if( ci->state == SL_AURATOR_STOPPED ) {
			continue;
		}
		// Add the samples into the 32-bit buffer.
		count = SL_MIN( aurator->frames_per_buffer, ci->sample_count - ci->sample_current );
		for( i = 0; i < count; ++i ) {
			for( c = 0; c < aurator->data.channel_count; ++c ) {
				aurator->data.samples[ i * aurator->data.channel_count + c ]
					+= ( i32_t )ci->stream[ ci->sample_current + ( i * aurator->data.channel_count ) + c ];
			}
		}
		ci->sample_current += count * aurator->data.channel_count;
	}

	// Remove clips that are done. @TODO: This is very slow this way, we should change it...
	while( 1 ) {
		deleted = SL_FALSE;
		i = 0;
		vul_foreach( sl_aurator_clip, ci, cil, aurator->clips )
		{
			if( ci->sample_current >= ci->sample_count ) {
				if( ci->state == SL_AURATOR_RUNNING_LOOPED ) {
					ci->sample_current = 0;
				} else if( ci->state == SL_AURATOR_RUNNING_KEEP ) {
					ci->state = SL_AURATOR_STOPPED;
				} else if ( ci->state == SL_AURATOR_STOPPED ) {
					// We ignore stopped ones
				} else {
					// Delete it
					vul_vector_remove_swap( aurator->clips, i );
					// This leaves the vector iterators in an unstable state, so break and start over
					deleted = SL_TRUE;
					break;
				}
			}
			++i;
		}
		// No more deletions means we're done
		if( !deleted ) {
			break;
		}
	}

	// Expand & clamp the mixed data
	for( i = 0; i < aurator->frames_per_buffer; ++i ) {
		for( c = 0; c < aurator->data.channel_count; ++c ) {
			if( aurator->data.samples[ i * aurator->data.channel_count + c ] > SHRT_MAX ) {
				expanded = INT_MAX;
			} else if( aurator->data.samples[ i * aurator->data.channel_count + c ] < SHRT_MIN ) {
				expanded = -INT_MAX;
			} else {
				expanded = aurator->data.samples[ i * aurator->data.channel_count + c ] * SHRT_MAX;
			}
			aurator->data.samples[ i * aurator->data.channel_count + c ] = ( i32_t )( ( f64_t )expanded * ( f64_t )aurator->volume );
		}
	}

	// And finally, write to the stream	
	err = Pa_WriteStream( aurator->portaudio_stream,
						  aurator->data.samples,
						  aurator->frames_per_buffer );
	if( err != paNoError ) {
		assert( SL_FALSE); // We couldn't write to stream
						   // Definitely shouldn't be an assert!
	}
}

void sl_aurator_load_ogg( sl_aurator *aurator, sl_aurator_clip **clip, char *path )
{
	i32_t channel_count;
	i16_t *str;
	ui32_t i, c;

	( *clip )->sample_count = stb_vorbis_decode_filename( path, &channel_count, &( *clip )->stream );
	if( ( ui32_t )channel_count > aurator->data.channel_count ) {
		assert( SL_FALSE ); // We have too many channels
	} else if( ( ui32_t )channel_count < aurator->data.channel_count ) {
		// Expand it by repeating the last channel
		str = ( i16_t* )malloc( sizeof( i16_t ) * ( *clip )->sample_count * aurator->data.channel_count );
		for( i = 0; i < ( *clip )->sample_count; i += aurator->data.channel_count ) {
			for( c = 0; c < aurator->data.channel_count; ++c ) {
				if( c < ( ui32_t )channel_count ) {
					str[ i * aurator->data.channel_count + c ] = i&(*clip)->stream[ i * channel_count + c ];
				} else {
					str[ i * aurator->data.channel_count + c ] = i&(*clip)->stream[ i * channel_count + ( channel_count - 1 ) ];
				}
			}
		}
	}
}


void sl_aurator_set_volume( sl_aurator *aurator, f32_t vol )
{
	assert( 0.f <= vol && vol <= 1.f );

	aurator->volume = vol;
}
f32_t sl_aurator_get_volume( sl_aurator *aurator )
{
	return aurator->volume;
}

