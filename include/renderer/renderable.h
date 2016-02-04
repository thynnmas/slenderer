/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * The base renderable struct and operations.
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
#ifndef SLENDERER_RENDERABLE_H
#define SLENDERER_RENDERABLE_H

#include "vul_cmath.h"
#include "renderer/entity.h"
#include "renderer/window.h"

#ifndef M_PI
	#define M_PI ( f32 )3.1415926535897932384626433832795
#endif

typedef struct {
	v2 position;
	v2 texcoords;
} sl_vertex;

typedef struct sl_renderable {
#ifdef SL_LEGACY_OPENGL
	sl_vertex *vertices;
	unsigned short *indices;
#else
	GLuint arrayBufferIndex;
	GLuint vertBufferObjectIndex;
	GLuint indexBufferObjectIndex;
#endif
	unsigned int vertex_count;
	unsigned int index_count;
	unsigned int renderable_id;
} sl_renderable;

/**
 * Creates a renderable quad with the given uvs at min and max corners.
 */
void sl_renderable_create_quad( sl_renderable *ren, sl_box *uvs );

/**
 * Creates a renderable pointy topped hex with the given UV aabb.
 * @NOTE(thynn): This is inefficient use of the texture, but it's simple
 * for animation, so until I really, really need a better solution, this works.
 */
void sl_renderable_create_hex_pointy( sl_renderable *ren, sl_box *uv_aabb );

/**
* Creates a renderable flat topped hex with the given UV aabb.
* @NOTE(thynn): This is inefficient use of the texture, but it's simple
* for animation, so until I really, really need a better solution, this works.
*/
void sl_renderable_create_hex_flat( sl_renderable *ren, sl_box *uv_aabb );

/**
 * Destroys a renderable quad.
 */
void sl_renderable_destroy( sl_renderable *ren );

/**
 * Bind the Vertex Array and its buffers for rendering.
 */
void sl_renderable_bind( sl_renderable *ren );

/**
 * Unbind the Vertex Array and its buffers after rendering.
 */
void sl_renderable_unbind( );

#endif
