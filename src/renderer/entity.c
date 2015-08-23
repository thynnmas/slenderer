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
#include "renderer/entity.h"

void sl_entity_aabb( sl_box *result, const sl_entity *q )
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

void sl_entity_create_world_matrix( sl_entity *result, const sl_vec *center, const sl_vec *scale, const float rotation )
{
	float cosr, sinr;

	cosr = ( float )cos( rotation );
	sinr = ( float )sin( rotation );

	result->world_matrix.data[ 0 ] = scale->x * cosr;
	result->world_matrix.data[ 1 ] = scale->x * -sinr;
	result->world_matrix.data[ 2 ] = result->world_matrix.data[ 3 ] = 0.0f;
	
	result->world_matrix.data[ 4 ] = -scale->y * sinr;
	result->world_matrix.data[ 5 ] = -scale->y * cosr; // Y is flipped in OpenGL
	result->world_matrix.data[ 6 ] = result->world_matrix.data[ 7 ] = 0.0f;

	result->world_matrix.data[ 10 ] = 1.0f;
	result->world_matrix.data[ 8 ] = result->world_matrix.data[ 9 ] = result->world_matrix.data[ 11 ] = 0.0f;

	result->world_matrix.data[ 12 ] = center->x;
	result->world_matrix.data[ 13 ] = center->y;
	result->world_matrix.data[ 14 ] = 0.0f;
	result->world_matrix.data[ 15 ] = 1.0f;
}

int sl_entity_sort( const void *a, const void *b )
{
	const sl_entity *qa, *qb;

	qa = ( const sl_entity* )a;
	qb = ( const sl_entity* )b;

	return qa->program_id != qb->program_id ?
				( qa->program_id < qb->program_id )
			  : ( qa->texture_id < qb->texture_id );
}

void sl_entity_bind( sl_entity *entity, sl_vec *camera_offset, sl_program *prog )
{
	sl_mat4 mat;
	sl_box uvs;
	f32_t tmp;

	// Calculate offset into matrix
	sl_mcopy4( &mat, &entity->world_matrix );
	mat.data[ 12 ] -= camera_offset->x;
	mat.data[ 13 ] -= camera_offset->y;

	// Calculate the uvs; they may be flipped
	sl_bset( &uvs, &entity->uvs );
	if( entity->flip_uvs.x ) {
		tmp = uvs.min_p.x;
		uvs.min_p.x = uvs.max_p.x;
		uvs.max_p.x = tmp;
	}
	if( entity->flip_uvs.y ) {
		tmp = uvs.min_p.y;
		uvs.min_p.y = uvs.max_p.y;
		uvs.max_p.y = tmp;
	}

	// Set world matrix
	glUniformMatrix4fv( glGetUniformLocation( prog->gl_prog_id, "mvp" ), 1, GL_FALSE, ( ( GLfloat* )&mat.data[ 0 ] ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "color" ), 1, ( ( GLfloat* )&entity->color ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "texcoord_offset_scale" ), 1, ( ( GLfloat* )&uvs ) );
}