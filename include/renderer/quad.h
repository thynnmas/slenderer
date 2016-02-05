/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * A simple quad, and our base renderable.
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
#ifndef SLENDERER_QUAD_H
#define SLENDERER_QUAD_H

#include "math/box.h"

typedef struct {
	unsigned int quad_id;
	m44 world_matrix;
	sl_box uvs;
	v2 flip_uvs;
	unsigned int texture_id;
	unsigned int program_id;
	unsigned int renderable_id;
	float color[ 4 ];
	unsigned char hidden; // Boolean, 0 if false
} sl_quad;

/**
 * Calculate the AABB of a quad.
 */
void sl_quad_aabb( sl_box *result, const sl_quad *q );

/**
 * Calculate the world matrix of a quad.
 */
void sl_quad_create_world_matrix( sl_quad *result, const v2 *center, const v2 *scale, const float rotation );

/**
 * Used to sort quads internally in layers, this first compares
 * program ids, then texture ids.
 */
int sl_quad_sort( const void *a, const void *b );

#endif
