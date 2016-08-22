/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain?
 * 
 * Audio manager. Contains a list of sound clips and their states. Updates
 * the audio streams upon call. Depends on portaudio, stb_audio_mixer and
 * stb_vorbis.
 *
 * Requires  sl_aurator_update to be called at least
 * 1000 / updates_per_second_guaranteed ms.
 * We mix the audio in _update and store it in the state's buffer.
 * The portaudio stream is written from that buffer in _update.
 * Mixing is correct for up to 2^16-1 clips. It might overflow if we have more.
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
#ifndef SLENDERER_AUDATOR_H
#define SLENDERER_AUDATOR_H

#include <limits.h>

#include <vul_types.h>
#include <vul_resizable_array.h>
#include <vul_timer.h>
#define VUL_AUDIO_ERROR_STDERR
#define VUL_AUDIO_SAMPLE_16BIT
#include <vul_audio.h>

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.h>

#ifndef SL_BOOL
	#define SL_BOOL int
	#define SL_TRUE 1
	#define SL_FALSE 0
#endif

#ifndef SL_MIN
	#define SL_MIN(a, b) ( ( a ) <= ( b ) ? ( a ) : ( b ) )
#endif

void sl_aurator_update( );

typedef struct sl_aurator {
	// Need a reference to the parent scene
	u32 scene_id;
   // All clips belonging to this aurator
   u64 *clips;
   u64 clip_count;
} sl_aurator;

/*
 * We only have one audio device handle for vul_audio
 */
static  vul_audio_device *sl_aurator_device;

/*
 * Create an aurator instance in place.
 * Takes the number of channels as input parameter.
 * We sceify the nubmer of time per second we GUARANTEE that
 * _update will be called; if we can't keep up we WILL have audio stutter!
 */
#ifdef VUL_WINDOWS
void sl_aurator_create( sl_aurator *ret, u32 parent_scene, u32 channel_count, u32 sample_rate, HWND win );
#else
void sl_aurator_create( sl_aurator *ret, u32 parent_scene, u32 channel_count, u32 sample_rate );
								
#endif

/*
 * Cleans up the aurator
 */
void sl_aurator_destroy( sl_aurator *aurator );

/*
 * Cleans up the audio system library.
 */
void sl_aurator_finalize( );

/* 
 * Uses stb_vorbis to load an ogg vorbis file and returns and ID for it.
 */
u64 sl_aurator_load_ogg( sl_aurator *aurator, char *path );

/*
 * Play a clip (looping if wanted)
 */
void sl_aurator_play( sl_aurator *aurator, u64 clip_id, b32 looping, b32 keep );
/* 
 * Stop a clip (reseting it's state if wanted).
 */
void sl_aurator_stop( sl_aurator *aurator, u64 clip_id, b32 reset );

/*
 * Halts playback of all clips in this aurator.
 */
void sl_aurator_pause_all( sl_aurator *aurator, b32 reset );
/*
 * Resumes playback of all clips in this aurator.
 */
void sl_aurator_resume_all( sl_aurator *aurator );

/*
 * Volume controls. Valid values are in trange [0, 1]
 */
void sl_aurator_set_volume( sl_aurator *aurator, f32 vol );
f32 sl_aurator_get_volume( sl_aurator *aurator );

#endif
