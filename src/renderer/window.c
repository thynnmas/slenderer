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
#include "renderer/window.h"

void sl_window_create( sl_window* win, unsigned int width, unsigned int height, const char *title, int fullscreen, int vsync, sl_window *context_share )
{
	win->handle = glfwCreateWindow( width, 
									height, 
									title, 
									fullscreen ? glfwGetPrimaryMonitor( ) : NULL, /* <-- @TODO: Multimonitor support */
									context_share == NULL ? NULL : context_share->handle );

#ifdef SL_DEBUG
	assert( win->handle );
#else
	if ( win->handle )
	{
		printf("Failed to create a GLFW window\n");
		return;
	}
#endif
	if ( !fullscreen ) {
		// If window, make it non-resizable
		glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
	}
			
	if( !vsync ) {
		// If no VSYNC, turn it off.
		glfwMakeContextCurrent( win->handle );
		glfwSwapInterval( 0 );
	}
}

void sl_window_destroy( sl_window* win )
{
	glfwDestroyWindow( win->handle );
}

void sl_window_bind_framebuffer( sl_window* win )
{
	int w, h;

	// Make the context current
	glfwMakeContextCurrent( win->handle );

	// Set up viewport
	glfwGetFramebufferSize( win->handle, &w, &h );
	glViewport( 0, 0, w, h );

	// Clear the famebuffer
	glClearColor( 0.f, 0.f, 0.f, 1.f ); // @TODO: Make this black again I guess..
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void sl_window_swap_buffers( sl_window *win )
{
	// Issue the command to swap buffers
	glfwSwapBuffers( win->handle );

	// Poll events before we hand over the spotlight.
	glfwPollEvents( );
}
