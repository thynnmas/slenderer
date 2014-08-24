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
 * -Audio, somehow...
 * -OpenGL ES support/mobile if possible with glfw; useful for ARM support.
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SL_DEBUG
#ifdef SL_DEBUG
	#include <assert.h>
#endif
#include <stdio.h>

#define SL_BOOL int
#define SL_TRUE 1
#define SL_FALSE 0

// @IMPORTANT: Define VUL_DEFINE in your main .c file.
#include <vul_resizable_array.h>

#include "renderer/window.h"
#include "renderer/scene.h"
#include "renderer/texture.h"
#include "renderer/program.h"
#include "renderer/animator.h"
#include "renderer/renderable.h"
#include "physics/simulator.h"

#include "input/controller.h"

typedef struct {
	vul_vector_t *windows; // Vector of sl_window
	vul_vector_t *scenes; // Vector of sl_scene
	vul_vector_t *animators; // Vector of sl_animator
	vul_vector_t *simulators; // Vector of sl_simulator
	vul_vector_t *textures; // Vector of sl_texture. An array with all the textures
	vul_vector_t *programs; // Vector of sl_program. An array with all the programs
	vul_vector_t *renderables; // Vector of sl_renderable. Ones that are not sprite-animated
							   // should use the same one. Ohters should have their own.
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
void sl_renderer_close_window( sl_window *win );

/**
 * Renders the scene at the given index to the window at the given index.
 */
void sl_renderer_render_scene( unsigned int scene_index, unsigned int window_index, SL_BOOL swap_buffers );

/**
 * Creates a new scene and adds it to the renderer.
 */
sl_scene *sl_renderer_add_scene( sl_window *win, sl_program *post_program );

/**
 * Retrieves the animator for the scene with the given id
 */
sl_animator *sl_renderer_get_animator_for_scene( unsigned int scene_id );

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

/**
 * Renders a single quad instance. Binds the world matrix uniform, then draws.
 */
void sl_renderer_draw_instance( sl_program *prog, sl_quad *quad );

/**
 * A callback function that handles errors in GLFW. We assert false in debug mode,
 * and print to STDERR in release mode.
 */
void sl_renderer_glfw_error_callback( int error, const char *desc );

/** 
 * Populate a list of scenes based on a window pointer.
 */
void sl_renderer_get_scenes_by_window_handle( vul_vector_t *vec, GLFWwindow *win_handle );

/** 
 * Retrieve a scene by id
 */
sl_scene *sl_renderer_get_scene_by_id( unsigned int id );

/**
 * Retrieve a texture by id.
 */
sl_texture *sl_renderer_get_texture_by_id( unsigned int id );

#endif
