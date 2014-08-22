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
#include "renderer/texture.h"

void sl_texture_create( sl_texture* tex, void *data, unsigned int width, unsigned int height, GLenum format, GLenum internal_format )
{
	GLint i;

	tex->width = width;
	tex->height = height;
	tex->format = format;
	tex->internal_format = internal_format;

	glGenTextures( 1, &tex->gl_id );

	sl_texture_bind( NULL, tex );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // @TODO: Make this an option; let people have it blurry if they want
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
	
	sl_texture_unbind( tex );
}

void sl_texture_destroy( sl_texture* tex )
{
	glDeleteTextures( 1, &tex->gl_id );
}

void sl_texture_bind( sl_program *prog, sl_texture* tex )
{
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex->gl_id );

	if( prog != NULL ) {
		glUniform1i( glGetUniformLocation( prog->gl_prog_id, "texture" ), 0 );
	}
}

void sl_texture_unbind( sl_texture* tex )
{
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}
