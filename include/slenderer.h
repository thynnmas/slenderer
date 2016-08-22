/**
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * Slenderer is a tiny 2D rendering library written in C.
 * Everything is in normalized screen coordinates. The scene is a collection of flat arrays
 * structured as layers. Need more layers? Increase MAX_LAYERS or just render another scene
 * after the first one. Provides simple animation; sprite-based and transforms.
 * 
 * @NOTE: This has been released before it's been fully tested due to time contraints
 * prior to LD30. The renderer works as it should (at least I haven't encountered any bugs yet),
 * the simulator has had some minor testing (but I can't vouch for it being bug free; forces
 * and collission detection works but there seem to be some issues nontheless).
 * The animator has NOT been tested yet. Input works (although mouse is untested, scroll is not
 * implemented yet ).
 * This should all get better over the weekend as I use it in LD30 and it gets seriously tested;
 * If you want to jump in the deep end (and you'd be somewhat dumb to do so), feel free, but
 * this thing will see sizeable changes during the next week. I'm hoping to get a proper release up
 * next week when things have been tested.
 * 
 * @DEPENDANCIES: It depends on the files in the "./dependancies" folder, being stb_image.c
 * by Sean Barret and a bunch of VUL-files, my own header-only libraries that have not priorly been
 * released due to a slight lack of testing; again, this should get better over the weekend, and I
 * might throw more of them up (these are only a small selection of them).
 * In addittion the renderer depends on GLFW 3 (and naturally its dependancies).
 *
 * @FUTURE:
 * -Font rendering (stb_truetype)
 * -Proper audio mixing; atm we're being very, very lazy (and plain wrong)
 * -OpenGL ES support/mobile if possible with glfw; useful for ARM support.
 * -Legacy GL (ffp) & no-audio mode to reduce dependancies and make it work on toasters.
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
#ifndef SLENDERER_H
#define SLENDERER_H

#ifdef _DEBUG
	#define SL_DEBUG
#endif
#include <assert.h>
#include <stdio.h>

#define SL_BOOL int
#define SL_TRUE 1
#define SL_FALSE 0

/*
* We support using your own allocators, just define these
* three with the replacements and these won't overwrite them.
*/
#ifndef SL_ALLOC
	#define SL_ALLOC malloc
	#define SL_REALLOC realloc
	#define SL_DEALLOC free
#endif

// Define this in your build if you want legacy GL support
// This will disable post-processing entirely
// #define SL_LEGACY_OPENGL

// @IMPORTANT: Define VUL_DEFINE in your main .c file.
#include <vul_resizable_array.h>

/**
* Print a formatted string to either stderr or debug console + logfile,
* depending on environment.
*/
void sl_print( u32 max_length, const char *fmt, ... );

#include "renderer/window.h"
#include "renderer/scene.h"
#include "renderer/texture.h"
#include "renderer/program.h"
#include "renderer/animator.h"
#include "renderer/renderable.h"
#include "physics/simulator.h"
#include "input/controller.h"

#ifndef SL_NO_AUDIO
#include "audio/aurator.h"
#define SL_AUDIO_CHANNEL_COUNT 2
#define SL_AUDIO_SAMPLE_RATE 44100
#endif

typedef struct {
	vul_vector *windows; // Vector of sl_window
	vul_vector *scenes; // Vector of sl_scene
	vul_vector *animators; // Vector of sl_animator
	vul_vector *simulators; // Vector of sl_simulator
	vul_vector *textures; // Vector of sl_texture. An array with all the textures
	vul_vector *programs; // Vector of sl_program. An array with all the programs
	vul_vector *renderables; // Vector of sl_renderable. Ones that are not sprite-animated
							   // should use the same one. Ohters should have their own.
#ifndef SL_NO_AUDIO
	vul_vector *aurators; // Vector of sl_aurator.
#endif
	u32 next_scene_id;
} sl_renderer;

sl_renderer *sl_renderer_global;

/**
 * Initializes GLFW and our renderer's context.
 */
void sl_renderer_create( );

/**
  * Destroys our renderer.
  */
void sl_renderer_destroy( );

/**
 * Creates a new window and adds it to the renderer.
 */
sl_window *sl_renderer_open_window( unsigned int width, unsigned int height, const char *title, int fullscreen, int vsync );

/**
 * Closes a window and removes it from the renderer.
 */
void sl_renderer_close_window( u32 win_id );

/**
 * Renders the scene at the given index to the window at the given index.
 * Applies the given camera offset to all coorindates before rendering.
 */
void sl_renderer_render_scene( unsigned int scene_index, unsigned int window_index, SL_BOOL swap_buffers );

/**
 * Creates a new scene and adds it to the renderer.
 */
sl_scene *sl_renderer_add_scene( u32 win_id, u32 post_program_id );

/**
 * Cleans up a scene 
 */
void sl_renderer_finalize_scene( unsigned int scene_id );

/**
 * Retrieves the animator for the scene with the given id
 */
sl_animator *sl_renderer_get_animator_for_scene( unsigned int scene_id );

/**
 * Retrieves the aurator for the scene with the given id
 */
#ifndef SL_NO_AUDIO
sl_aurator *sl_renderer_get_aurator_for_scene( unsigned int scene_id );
#endif

/**
 * Retrieves the simulator for the scene with the given id
 */
sl_simulator *sl_renderer_get_simulator_for_scene( unsigned int scene_id );

/**
 * Allocates a new texture and sets its ID.
 */
sl_texture *sl_renderer_allocate_texture( );

/**
 * Allocates a new renderable and sets its ID.
 */
sl_renderable *sl_renderer_allocate_renderable( );

/**
 * Allocates a new program and sets it's ID.
 */
sl_program *sl_renderer_allocate_program( );

#ifdef SL_LEGACY_OPENGL
/**
 * Renders a single quad using leagcy GL
 */
void sl_renderer_draw_legacy_instance( v2 *camera_offset, sl_renderable *rend, sl_entity *entity );
#else
/**
 * Renders a single quad instance. Binds the world matrix uniform, then draws.
 */
void sl_renderer_draw_instance( sl_renderable *ren );
#endif

/**
 * A callback function that handles errors in GLFW. We assert false in debug mode,
 * and print to STDERR in release mode.
 */
void sl_renderer_glfw_error_callback( int error, const char *desc );

/** 
 * Populate a list of scenes based on a window pointer.
 */
void sl_renderer_get_scenes_by_window_handle( vul_vector *vec, GLFWwindow *win_handle );

/** 
 * Retrieve a scene by id
 */
sl_scene *sl_renderer_get_scene_by_id( unsigned int id );

/**
 * Retrieve a texture by id.
 */
sl_texture *sl_renderer_get_texture_by_id( unsigned int id );

/**
 * Retrieve a program by id.
 */
sl_program *sl_renderer_get_program_by_id( unsigned int id );

/**
 * Retrieve a window by id.
 */
sl_window *sl_renderer_get_window_by_id( unsigned int id );

/**
 * Retrieve a window by the GLFW window handle.
 */
sl_window *sl_renderer_get_window_by_handle( GLFWwindow *win_handle );

#endif
