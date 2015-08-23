/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * Abstracts OpenGL windows and contexts away. 
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
#ifndef SLENDERER_WINDOW_H
#define SLENDERER_WINDOW_H

#include <GL/glew.h>
#ifdef SL_OPENGL_ES
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

#ifdef SL_DEBUG
	#include <assert.h>
#else
	#include <stdio.h>
#endif

typedef struct sl_window {
	unsigned int window_id;
	GLFWwindow *handle;
	GLuint fbo, fbo_texture, rbo_depth;
} sl_window;

/**
 * Creates a new OpenGL rendering window & context.
 * Windows are destroyed when the context is destroyed if not manually
 * destroyed prior.
 * If context_share is not NULL, context is shared with that window.
 * \note: requires sl_renderer_init to have been called prior.
 */
void sl_window_create( sl_window* win, unsigned int width, unsigned int height, const char *title, int fullscreen, int vsync, sl_window *context_share );

/**
 * Destroys a window.
 */
void sl_window_destroy( sl_window *win );

/**
 * Create an FBO; allows the window to do post processing.
 * Can't be done in window creation because we aren't guaranteed to have
 * a complete OpenGL context at that point.
 */
void sl_window_create_fbo( sl_window *win, unsigned int width, unsigned int height );

/**
 * Destroys the window's FBO.
 */
void sl_window_destroy_fbo( sl_window *win );

/**
 * Binds a window's FBO for rendering.
 */
void sl_window_bind_framebuffer_fbo( sl_window *win );

/**
 * Binds a window's framebuffer for rendering.
 */
void sl_window_bind_framebuffer_post( sl_window *win );

/**
 * Swaps the window's framebuffers.
 */
void sl_window_swap_buffers( sl_window *win );

/**
 * Window size change callback.
 * Recreates the FBO in the new size.
 * This is SLOW!
 */
void sl_window_size_callback( GLFWwindow* window, int width, int height );

#endif
