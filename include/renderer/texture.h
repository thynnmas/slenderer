/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * Abstracts OpenGL texture ops away. 
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
#ifndef SLENDERER_TEXTURE_H
#define SLENDERER_TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer/program.h"

typedef struct {
	unsigned int texture_id;
	GLuint gl_id;
	unsigned int width;
	unsigned int height;
	GLenum format;
	GLenum internal_format;
} sl_texture;

/**
 * Creates a new texture with the given data.
 * \note: requires an OpenGL context to pre-exist.
 */
void sl_texture_create( sl_texture* tex, void *data, unsigned int width, unsigned int height, GLenum format, GLenum internal_format );

/**
 * Destroys a texture. Frees host memory if not NULL.
 */
void sl_texture_destroy( sl_texture* tex );

/**
 * Binds a texture for rendering/editing.
 */
void sl_texture_bind( sl_program *prog, sl_texture* tex );

/**
 * Unbinds a texture after rendering/editing.
 */
void sl_texture_unbind( sl_texture* tex );

#endif
