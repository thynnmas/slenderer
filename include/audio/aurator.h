/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
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
#ifndef SLENDERER_AUDATOR_H
#define SLENDERER_AUDATOR_H

#include <portaudio.h>
//#include <stb_audio_mixer.h>
#include <vul_types.h>
#include <vul_resizable_array.h>
#include <vul_timer.h>
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

typedef enum sl_aurator_clip_state {
	SL_AURATOR_STOPPED,
	SL_AURATOR_RUNNING,
	SL_AURATOR_RUNNING_LOOPED,	// Wraps around the end
	SL_AURATOR_RUNNING_KEEP, // Runs once, but instead of deleting it we keep it as stopped. Useful for reuse
	SL_AURATOR_COUNT
} sl_aurator_clip_state;

typedef struct sl_aurator_clip {
	ui32_t id;
	
	sl_aurator_clip_state state;
	
	ui32_t sample_current;
	ui32_t sample_count;
	i16_t *stream; // Stream of samples; channels are interleaved.
} sl_aurator_clip;

typedef struct sl_aurator_steam_data {
	ui32_t sample_count;
	ui32_t sample_rate;
	ui32_t channel_count;
	
	i32_t *samples;
} sl_aurator_steam_data;

typedef struct sl_aurator {
	// Need a reference to the parent scene
	ui32_t scene_id;
	// Contains all the clips currently playing.
	vul_vector_t *clips; 
	// Unique id to give the next added clip
	ui32_t next_id;
	// Portaudio stream.
	PaStream *portaudio_stream; 
	PaStreamParameters portaudio_params;
	// Mixer data
	ui32_t frames_per_buffer;
	sl_aurator_steam_data data;
	// Timer
	vul_timer_t *clk;
	ui64_t last_time;
	ui64_t wait_time;
	// Volume
	f32_t volume;
} sl_aurator;

/*
 * Some static state concerning the portaudio system.
 */
static SL_BOOL sl_aurator_pa_intialized;

/*
 * Create an aurator instance in place.
 * Takes the number of channels as input parameter.
 * We sceify the nubmer of time per second we GUARANTEE that
 * _update will be called; if we can't keep up we WILL have audio stutter!
 */
void sl_aurator_create( sl_aurator *ret, ui32_t parent_scene, ui32_t channel_count, ui32_t sample_rate, ui32_t updates_per_second_guaranteed );

/*
 * Cleans up the aurator, @TODO: stopping all it's streams.
 */
void sl_aurator_destroy( sl_aurator *aurator );

/*
 * Call on exit, this cleans up the portaudio system.
 * Assumes all aurators are destroyed already.
 */
void sl_aurator_finalize_system( );

/* 
 * Uses stb_vorbis to load an ogg vorbis file into the given clip.
 */
void sl_aurator_load_ogg( sl_aurator *aurator, sl_aurator_clip **clip, char *path );

/*
 * Allocates an empty clip and returns its id.
 * It's state is set to stopped initially.
 */
ui32_t sl_aurator_allocate_clip( sl_aurator *aurator );

/*
 * Adds a clip to the scene and returns its id.
 * Samples must have the same number of channels as the aurator
 * was created for. Samples is used _in place_, channels interleaved!
 */
ui32_t sl_aurator_add_clip( sl_aurator *aurator, ui32_t sample_count, i16_t *samples, SL_BOOL loop );

/*
 * Returns the clip for a given id.
 */
sl_aurator_clip *sl_aurator_get_clip( sl_aurator *aurator, ui32_t id );

/*
 * Updates the aurator. This mixes all currently playing clips into 
 * the sl_aurator_steam_data buffer and removes finished clips.
 */
void sl_aurator_update( sl_aurator *aurator );

/*
 * Volume controls. Valid values are in trange [0, 1]
 */
void sl_aurator_set_volume( sl_aurator *aurator, f32_t vol );
f32_t sl_aurator_get_volume( sl_aurator *aurator );

#endif