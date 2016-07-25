/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain?
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
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

#include "slenderer.h"

static SL_BOOL sl_aurator_pa_intialized = SL_FALSE;

void sl_aurator_create( sl_aurator *ret, u32 parent_scene, u32 channel_count, u32 sample_rate, u32 updates_per_second_guaranteed )
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
	ret->clips = vul_vector_create( sizeof( sl_aurator_clip ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	// State
	ret->scene_id = parent_scene;
	ret->next_id = 0;
	ret->frames_per_buffer = sample_rate / updates_per_second_guaranteed; // @TODO: Move to next power of 2 or something
	ret->volume = 0.5f;

	// Timing, we wait at least half a frame before uploading the next one.
	ret->clk = vul_timer_create( );
	ret->last_time = vul_timer_get_millis( ret->clk );
	ret->wait_time = ( u64 )( ( f64 )500.0 / ( f64 )updates_per_second_guaranteed );

	// Internal buffer state
	ret->data.sample_count = channel_count * ret->frames_per_buffer;
	ret->data.sample_rate = sample_rate;
	ret->data.channel_count = channel_count;
	ret->data.samples = ( s32* )SL_ALLOC( sizeof( s32 ) * ret->data.sample_count * channel_count );
	memset( ret->data.samples, ( s32 )0, sizeof( s32 ) * ret->data.sample_count * channel_count );

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
						 sl_aurator_upload, ret ); /* No callback and userdata */
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
		SL_DEALLOC( ci->stream );
	}
	vul_vector_destroy( aurator->clips );

	// Free the stream data
	SL_DEALLOC( aurator->data.samples );
}

void sl_aurator_finalize_system( )
{
	if( sl_aurator_pa_intialized ) {
		Pa_Terminate( );
	}
}

u32 sl_aurator_allocate_clip( sl_aurator *aurator )
{
	sl_aurator_clip *clip;

	assert( aurator );

	clip = ( sl_aurator_clip* )vul_vector_add_empty( aurator->clips );
	clip->state = SL_AURATOR_STOPPED;
	clip->sample_current = 0;
	clip->id = aurator->next_id++;
	
	return clip->id;
}

u32 sl_aurator_add_clip( sl_aurator *aurator, u32 sample_count, s16 *samples, SL_BOOL loop )
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
sl_aurator_clip *sl_aurator_get_clip( sl_aurator *aurator, u32 id )
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
	u32 count, i, c;
	s32 expanded;
	SL_BOOL deleted;
	u64 now;

	assert( aurator );
	
	now = vul_timer_get_millis( aurator->clk );
	if( now - aurator->last_time < aurator->wait_time ) {
		return;
	}
	aurator->last_time = now;

	// Clear our buffer before mixing
	memset( aurator->data.samples,
		    0,
			sizeof( s32 ) * aurator->frames_per_buffer * aurator->data.channel_count );
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
					+= ( s32 )ci->stream[ ci->sample_current + ( i * aurator->data.channel_count ) + c ];
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
			aurator->data.samples[ i * aurator->data.channel_count + c ] = ( s32 )( ( f64 )expanded * ( f64 )aurator->volume );
		}
	}
}

int sl_aurator_upload( const void *input, void *output,
					   unsigned long frame_count,
					   const PaStreamCallbackTimeInfo *time_info,
					   PaStreamCallbackFlags status_flags,
					   void *user_data )
{
	sl_aurator *aurator;

	( void )input; // Avoid unused variable warnings
	aurator = ( sl_aurator* )user_data;

	// Write to the buffer
	memcpy( output, aurator->data.samples, aurator->data.channel_count * frame_count );

	return 0;
}

void sl_aurator_load_ogg( sl_aurator *aurator, sl_aurator_clip **clip, char *path )
{
	s32 channel_count;
	s32 sample_rate;
	s16 *str;
	u32 i, c;

	( *clip )->sample_count = stb_vorbis_decode_filename( path, &channel_count, &sample_rate, &( *clip )->stream );
	if( aurator->data.sample_rate != sample_rate ) {
		assert( SL_FALSE ); // Sample rates don't match, we don't support live resampling
	}
	if( ( u32 )channel_count > aurator->data.channel_count ) {
		assert( SL_FALSE ); // We have too many channels
	} else if( ( u32 )channel_count < aurator->data.channel_count ) {
		// Expand it by repeating the last channel
		str = ( s16* )SL_ALLOC( sizeof( s16 ) * ( *clip )->sample_count * aurator->data.channel_count );
		for( i = 0; i < ( *clip )->sample_count; i += aurator->data.channel_count ) {
			for( c = 0; c < aurator->data.channel_count; ++c ) {
				if( c < ( u32 )channel_count ) {
					str[ i * aurator->data.channel_count + c ] = i&(*clip)->stream[ i * channel_count + c ];
				} else {
					str[ i * aurator->data.channel_count + c ] = i&(*clip)->stream[ i * channel_count + ( channel_count - 1 ) ];
				}
			}
		}
		SL_DEALLOC( ( *clip )->stream );
		( *clip )->stream = str;
	}
}

void sl_aurator_set_volume( sl_aurator *aurator, f32 vol )
{
	assert( 0.f <= vol && vol <= 1.f );

	aurator->volume = vol;
}
f32 sl_aurator_get_volume( sl_aurator *aurator )
{
	return aurator->volume;
}

