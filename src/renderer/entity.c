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
	v2 min_p1, max_p1;
	v2 min_p2, max_p2, offset;
	m22 m2;

	min_p1 = vec2( -1.0f, -1.0f );
	max_p1 = vec2( 1.0f,  1.0f );
	m2 = mtruncate42( &q->world_matrix );

	min_p2 = vmulm2( &m2, min_p1 );
	max_p2 = vmulm2( &m2, max_p1 );

	offset.x = q->world_matrix.A[ 12 ];
	offset.y = q->world_matrix.A[ 13 ];

	min_p1 = vadd2( min_p2, offset );
	max_p1 = vadd2( max_p2, offset );

	result->min_p = vmin2( min_p1, max_p1 );
	result->max_p = vmax2( min_p1, max_p1 );
}

void sl_entity_create_world_matrix( sl_entity *result, const v2 *center, const v2 *scale, const float rotation )
{
	float cosr, sinr;

	cosr = ( float )cos( rotation );
	sinr = ( float )sin( rotation );

	result->world_matrix.A[ 0 ] = scale->x * cosr;
	result->world_matrix.A[ 1 ] = scale->x * -sinr;
	result->world_matrix.A[ 2 ] = result->world_matrix.A[ 3 ] = 0.0f;
	
	result->world_matrix.A[ 4 ] = -scale->y * sinr;
	result->world_matrix.A[ 5 ] = -scale->y * cosr; // Y is flipped in OpenGL
	result->world_matrix.A[ 6 ] = result->world_matrix.A[ 7 ] = 0.0f;

	result->world_matrix.A[ 10 ] = 1.0f;
	result->world_matrix.A[ 8 ] = result->world_matrix.A[ 9 ] = result->world_matrix.A[ 11 ] = 0.0f;

	result->world_matrix.A[ 12 ] = center->x;
	result->world_matrix.A[ 13 ] = center->y;
	result->world_matrix.A[ 14 ] = 0.0f;
	result->world_matrix.A[ 15 ] = 1.0f;
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

void sl_entity_bind( sl_entity *entity, v2 *camera_offset, sl_program *prog )
{
	m44 mat;
	sl_box uvs;
	f32 tmp;

	// Calculate offset into matrix
	memcpy( &mat, &entity->world_matrix, sizeof( m44 ) );
	mat.A[ 12 ] -= camera_offset->x;
	mat.A[ 13 ] -= camera_offset->y;

	// Calculate the uvs; they may be flipped
	sl_bset( &uvs, &entity->uvs );
	if( entity->flip_uvs.x != 0.f ) {
		tmp = uvs.min_p.x;
		uvs.min_p.x = uvs.max_p.x;
		uvs.max_p.x = tmp;
	}
	if( entity->flip_uvs.y != 0.f ) {
		tmp = uvs.min_p.y;
		uvs.min_p.y = uvs.max_p.y;
		uvs.max_p.y = tmp;
	}

	// Set world matrix
	glUniformMatrix4fv( glGetUniformLocation( prog->gl_prog_id, "mvp" ), 1, GL_FALSE, ( ( GLfloat* )&mat.A[ 0 ] ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "color" ), 1, ( ( GLfloat* )&entity->color ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "texcoord_offset_scale" ), 1, ( ( GLfloat* )&uvs ) );
}