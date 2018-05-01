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

#define VUL_DEFINE
#include <vul_audio.h>

static vul_audio_device *sl_aurator_device = 0;

#ifdef VUL_WINDOWS
void sl_aurator_create( sl_aurator *ret, u32 parent_scene, u32 channel_count, u32 sample_rate, HWND win )
#else
void sl_aurator_create( sl_aurator *ret, u32 parent_scene, u32 channel_count, u32 sample_rate )
#endif
{
	vul_audio_return err;

	// Create the clip array
	ret->scene_id = parent_scene;

   ret->clips = 0;
   ret->clip_count = 0;

	if( !sl_aurator_device ) {
		sl_aurator_device = ( vul_audio_device* )SL_ALLOC( sizeof( vul_audio_device ) );
#if defined( VUL_WINDOWS ) || defined( VUL_OSX )
		err = vul_audio_init( sl_aurator_device, 
									 VUL_AUDIO_MODE_PLAYBACK,
									 channel_count, sample_rate, 0x1000, 0, 0 );
#elif VUL_LINUX
		err = vul_audio_init( sl_aurator_device, NULL, NULL, "Wormings",
									 VUL_AUDIO_MODE_PLAYBACK,
									 channel_count, sample_rate, 0x1000, 0, 0 );
#endif
		if( err != VUL_OK ) {
			assert( SL_FALSE );
			return;
		}
	}

}

void sl_aurator_destroy( sl_aurator *aurator )
{
	assert( aurator );

   if( aurator->clip_count ) {
      SL_DEALLOC( aurator->clips );
      aurator->clip_count = 0;
      aurator->clips = 0;
   }
}

void sl_aurator_finalize( )
{
	if( sl_aurator_device ) {
		vul_audio_destroy( sl_aurator_device, SL_FALSE );
		SL_DEALLOC( sl_aurator_device );
	}
}

void sl_aurator_play( sl_aurator *aurator, u64 clip_id, b32 looping, b32 keep )
{
   assert( VUL_OK == vul_audio_clip_play( sl_aurator_device, clip_id, looping, keep ) );
}

void sl_aurator_stop( sl_aurator *aurator, u64 clip_id, b32 reset )
{
   vul_audio_clip_pause( sl_aurator_device, clip_id, reset );
}

void sl_aurator_remove_all( sl_aurator *aurator )
{
   u64 i;
   if( !aurator ) {
      return;
   }
   for( i = 0; i < aurator->clip_count; ++i ) {
      vul_audio_clip_remove( sl_aurator_device, aurator->clips[ i ] );
   }
}

void sl_aurator_pause_all( sl_aurator *aurator, b32 reset )
{
   u64 i;
   if( !aurator ) {
      return;
   }

   for( i = 0; i < aurator->clip_count; ++i ) {
      vul_audio_clip_pause( sl_aurator_device, aurator->clips[ i ], reset );
   }
}

void sl_aurator_resume_all( sl_aurator *aurator )
{
   u64 i;
   if( !aurator ) {
      return;
   }

   for( i = 0; i < aurator->clip_count; ++i ) {
      vul_audio_clip_resume( sl_aurator_device, aurator->clips[ i ] );
   }
}

u64 sl_aurator_load_ogg( sl_aurator *aurator, char *path )
{
	s32 channel_count;
	s32 sample_rate;
	s16 *str, *stream;
	u32 i, c;
   u64 sample_count, id;

	channel_count = 0;
	sample_rate = 0;
	str = 0;

	sample_count = stb_vorbis_decode_filename( path, &channel_count, &sample_rate, &stream );
	if( sample_count == -1 ) {
		printf("Failed to open file %s.\n", path );
		assert( SL_FALSE );
	}
	if( sl_aurator_device->sample_rate != sample_rate ) {
		printf("Sample rate mismatch: set %d vs wanted %d (file %s)\n", sl_aurator_device->sample_rate, sample_rate, path );
		assert( SL_FALSE ); // Sample rates don't match, we don't support live resampling
	}
	if( ( u32 )channel_count > sl_aurator_device->mixer.channels ) {
		assert( SL_FALSE ); // We have too many channels
	} else if( ( u32 )channel_count < sl_aurator_device->mixer.channels ) {
		// Expand it by repeating the last channel
		str = ( s16* )SL_ALLOC( sizeof( s16 ) * sample_count * sl_aurator_device->mixer.channels );
		for( i = 0; i < sample_count; ++i ) {
			for( c = 0; c < sl_aurator_device->mixer.channels; ++c ) {
				if( c < ( u32 )channel_count ) {
					str[ i * sl_aurator_device->mixer.channels + c ] = stream[ i * channel_count + c ];
				} else {
					str[ i * sl_aurator_device->mixer.channels + c ] = stream[ i * channel_count + ( channel_count - 1 ) ];
				}
			}
		}
		SL_DEALLOC( stream );
		stream = str;
	}

   id = vul_audio_clip_add( sl_aurator_device, stream, sample_count, 
                            sl_aurator_device->mixer.channels, 1.0f );

   if( aurator->clip_count == 0 ) {
      aurator->clips = ( u64* )SL_ALLOC( sizeof( u64 ) );
   } else {
      u64 *p = ( u64* )SL_REALLOC( aurator->clips, sizeof( u64 ) * ( aurator->clip_count + 1 ) );
      assert( p );
      aurator->clips = p;
   }
   aurator->clips[ aurator->clip_count++ ] = id;

   return id;
}

void sl_aurator_set_volume( sl_aurator *aurator, f32 vol )
{
	assert( 0.f <= vol && vol <= 1.f );

   vul_audio_set_global_volume( sl_aurator_device, vol );
}

f32 sl_aurator_get_volume( sl_aurator *aurator )
{
   return sl_aurator_device->mixer.volume;
}

