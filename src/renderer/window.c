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
#include "slenderer.h"

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

	glfwSetWindowSizeCallback( win->handle, sl_window_size_callback );
}

void sl_window_create_fbo( sl_window *win, unsigned int width, unsigned int height )
{
	GLenum status;
	
	/* Create the framebufferobject we render to before post */
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &win->fbo_texture );
	glBindTexture( GL_TEXTURE_2D, win->fbo_texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glBindTexture( GL_TEXTURE_2D, 0 );

	/* Depth buffer */
	glGenRenderbuffers( 1, &win->rbo_depth );
	glBindRenderbuffer( GL_RENDERBUFFER, win->rbo_depth );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	/* Link them */
	glGenFramebuffers( 1, &win->fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, win->fbo );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, win->fbo_texture, 0 );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, win->rbo_depth );
	
	assert( ( status = glCheckFramebufferStatus( GL_FRAMEBUFFER ) ) == GL_FRAMEBUFFER_COMPLETE );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void sl_window_destroy_fbo( sl_window *win )
{
	glDeleteRenderbuffers( 1, &win->rbo_depth );
	glDeleteTextures( 1, &win->fbo_texture);
	glDeleteFramebuffers( 1, &win->fbo );
}

void sl_window_destroy( sl_window* win )
{
	sl_window_destroy_fbo( win );

	glfwDestroyWindow( win->handle );
}

void sl_window_bind_framebuffer_fbo( sl_window* win )
{
	int w, h;

	// Make the context current
	glfwMakeContextCurrent( win->handle );

	// Set up viewport
	glfwGetFramebufferSize( win->handle, &w, &h );
	glViewport( 0, 0, w, h );

	// Select the FBO
	glBindFramebuffer( GL_FRAMEBUFFER, win->fbo );

	// Clear the famebuffer
	glClearColor( 0.f, 0.f, 0.f, 1.f ); // @TODO: Make this black again I guess..
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void sl_window_bind_framebuffer_post( sl_window* win )
{
	int w, h;

	// Make the context current
	glfwMakeContextCurrent( win->handle );

	// Set up viewport
	glfwGetFramebufferSize( win->handle, &w, &h );
	glViewport( 0, 0, w, h );
	
	// Select the Window
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

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


void sl_window_size_callback( GLFWwindow* window, int width, int height )
{
	sl_window *win;
	// Change the size of the FBO
	// @TODO: Make this less "tear down the world"-y; less slow & horrible.
	// although resizing the window should be punished...

	win = sl_renderer_get_window_by_handle( window );

	// Tear it down
	sl_window_destroy_fbo( win );

	// Recreate it
	sl_window_create_fbo( win, width, height );
}