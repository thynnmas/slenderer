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
#include <GLFW/glfw3.h>

#define SL_DEBUG
#ifdef SL_DEBUG
	#include <assert.h>
#else
	#include <stdio.h>
#endif

typedef struct {
	unsigned int window_id;
	GLFWwindow *handle;
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
 * Binds a window's framebuffer for rendering.
 */
void sl_window_bind_framebuffer( sl_window *win );

/**
 * Swaps the window's framebuffers.
 */
void sl_window_swap_buffers( sl_window *win );


#endif
