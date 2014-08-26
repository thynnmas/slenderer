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
#include "renderer/quad.h"

void sl_quad_aabb( sl_box *result, const sl_quad *q )
{
	sl_vec min_p1, max_p1;
	sl_vec min_p2, max_p2, offset;
	sl_mat2 m2;

	sl_vset( &min_p1, -1.0f, -1.0f );
	sl_vset( &max_p1,  1.0f,  1.0f );
	sl_mtruncate24( &m2, &q->world_matrix );

	sl_mul_post( &min_p2, &m2, &min_p1 );
	sl_mul_post( &max_p2, &m2, &max_p1 );

	offset.x = q->world_matrix.data[ 12 ];
	offset.y = q->world_matrix.data[ 13 ];

	sl_vadd( &min_p1, &min_p2, &offset );
	sl_vadd( &max_p1, &max_p2, &offset );

	sl_vmin( &result->min_p, &min_p1, &max_p1 );
	sl_vmax( &result->max_p, &min_p1, &max_p1 );
}

void sl_quad_create_world_matrix( sl_quad *result, const sl_vec *center, const sl_vec *scale, const float rotation )
{
	float cosr, sinr;

	cosr = ( float )cos( rotation );
	sinr = ( float )sin( rotation );

	result->world_matrix.data[ 0 ] = scale->x * cosr;
	result->world_matrix.data[ 1 ] = sinr;
	result->world_matrix.data[ 2 ] = result->world_matrix.data[ 3 ] = 0.0f;
	
	result->world_matrix.data[ 4 ] = -sinr;
	result->world_matrix.data[ 5 ] = -scale->y * cosr; // Y is flipped in OpenGL
	result->world_matrix.data[ 6 ] = result->world_matrix.data[ 7 ] = 0.0f;

	result->world_matrix.data[ 10 ] = 1.0f;
	result->world_matrix.data[ 8 ] = result->world_matrix.data[ 9 ] = result->world_matrix.data[ 11 ] = 0.0f;

	result->world_matrix.data[ 12 ] = center->x;
	result->world_matrix.data[ 13 ] = center->y;
	result->world_matrix.data[ 14 ] = 0.0f;
	result->world_matrix.data[ 15 ] = 1.0f;
}

int sl_quad_sort( const void *a, const void *b )
{
	const sl_quad *qa, *qb;

	qa = ( const sl_quad* )a;
	qb = ( const sl_quad* )b;

	return qa->program_id != qb->program_id ?
				( qa->program_id < qb->program_id )
			  : ( qa->texture_id < qb->texture_id );
}
