/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * Our base renderable, currently supporting quads and hexes
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
#ifndef SLENDERER_ENTITY_H
#define SLENDERER_ENTITY_H

#include <vul_types.h>
#include "math/box.h"
#include "vul_cmath.h"
#include "renderer/program.h"

typedef struct {
	unsigned int entity_id;
	m44 world_matrix;
	sl_box uvs;
	v2 flip_uvs;
	unsigned int texture_id;
	unsigned int program_id;
	unsigned int renderable_id;
	float color[ 4 ];
	unsigned char hidden; // Boolean, 0 if false
} sl_entity;

/**
 * Calculate the AABB of a quad.
 */
void sl_entity_aabb( sl_box *result, const sl_entity *q );

/**
 * Calculate the world matrix of a quad.
 */
void sl_entity_create_world_matrix( sl_entity *result, const v2 *center, const v2 *scale, const float rotation );

/**
 * Used to sort quads internally in layers, this first compares
 * program ids, then texture ids.
 */
int sl_entity_sort( const void *a, const void *b );

/**
 * Bind the quad's parameters to the rendering program. This means we upload
 * MVP matrix, color and texture coordinate scales and offsets.
 */
void sl_entity_bind( sl_entity *entity, v2 *camera_offset, sl_program *prog );

#endif
