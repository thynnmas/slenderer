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

#include "math/vec.h"
#include "renderer/quad.h"
#include "renderer/window.h"

typedef struct {
	sl_vec position;
	sl_vec texcoords;
} sl_vertex;

typedef struct sl_renderable {
	GLuint arrayBufferIndex;
	GLuint vertBufferObjectIndex;
	GLuint indexBufferObjectIndex;
	unsigned int renderable_id;
} sl_renderable;

/**
 * Creates a renderable quad with the given uvs at min and max corners.
 */
void sl_renderable_create( sl_renderable *ren, sl_box *uvs );

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
