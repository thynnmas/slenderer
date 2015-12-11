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
#include "renderer/renderable.h"

void sl_renderable_create_quad( sl_renderable *ren, sl_box *uvs )
{
#ifdef SL_LEGACY_OPENGL
	ren->vertices = SL_ALLOC( sizeof( sl_vertex ) * 4 );
	sl_vset( &ren->vertices[ 0 ].position,  -1.0f, -1.0f );
	sl_vset( &ren->vertices[ 0 ].texcoords,  uvs->min_p.x,  uvs->min_p.y );

	sl_vset( &ren->vertices[ 1 ].position,   1.0f, -1.0f );
	sl_vset( &ren->vertices[ 1 ].texcoords,  uvs->max_p.x,  uvs->min_p.y );

	sl_vset( &ren->vertices[ 2 ].position,   1.0f,  1.0f );
	sl_vset( &ren->vertices[ 2 ].texcoords,  uvs->max_p.x,  uvs->max_p.y );

	sl_vset( &ren->vertices[ 3 ].position,  -1.0f,  1.0f );
	sl_vset( &ren->vertices[ 3 ].texcoords,  uvs->min_p.x,  uvs->max_p.y );

	ren->indices = SL_ALLOC( sizeof( unsigned short ) * 6 );
	ren->indices[ 0 ] = 0;
	ren->indices[ 1 ] = 1;
	ren->indices[ 2 ] = 2;
	ren->indices[ 3 ] = 0;
	ren->indices[ 4 ] = 2;
	ren->indices[ 5 ] = 3;
#else
	sl_vertex vertices[ 4 ];
	unsigned short triangles[ 6 ];

	ren->index_count = 6;
	ren->vertex_count = 4;

	// Create our vertices
	vertices[ 0 ].position = vec2( -1.0f, -1.0f );
	vertices[ 0 ].texcoords = vec2( uvs->min_p.x,  uvs->min_p.y );

	vertices[ 1 ].position = vec2( 1.0f, -1.0f );
	vertices[ 1 ].texcoords = vec2( uvs->max_p.x,  uvs->min_p.y );

	vertices[ 2 ].position = vec2( 1.0f,  1.0f );
	vertices[ 2 ].texcoords = vec2( uvs->max_p.x,  uvs->max_p.y );

	vertices[ 3 ].position = vec2( -1.0f,  1.0f );
	vertices[ 3 ].texcoords = vec2( uvs->min_p.x,  uvs->max_p.y );

	// Create the triangles
	triangles[ 0 ] = 0;
	triangles[ 1 ] = 1;
	triangles[ 2 ] = 2;
	triangles[ 3 ] = 0;
	triangles[ 4 ] = 2;
	triangles[ 5 ] = 3;

	// Create the vertex array object
	glGenVertexArrays( 1, &ren->arrayBufferIndex );
	glBindVertexArray( ren->arrayBufferIndex );
	
	// Create and upload the vertex data
	glGenBuffers(1, &ren->vertBufferObjectIndex);
	glBindBuffer( GL_ARRAY_BUFFER, ren->vertBufferObjectIndex );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( sl_vertex ), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 4 * sizeof( sl_vertex ), vertices);

	// Set attribute pointers
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( sl_vertex ), 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_TRUE, sizeof( sl_vertex ), (GLvoid*)( 2 * sizeof( GLfloat ) ) );

	// Unbind buffer object and buffer
	glEnableVertexAttribArray( 0 );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Create the index buffer and upload it
	glGenBuffers( 1, &ren->indexBufferObjectIndex );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ren->indexBufferObjectIndex );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof( unsigned short ), NULL, GL_STATIC_DRAW );
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 2 * 3 * sizeof( unsigned short ), triangles );

	// Unbind the Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
}

void sl_renderable_create_hex_pointy( sl_renderable *ren, sl_box *uv_aabb )
{
	v2 center, extent;
#ifdef SL_LEGACY_OPENGL
	sl_bcenter( &center, uv_aabb );
	sl_bextent( &extent, uv_aabb );
	ren->vertices = SL_ALLOC( sizeof( sl_vertex ) * 7 );
	sl_vset( &ren->vertices[ 0 ].position, 0.0f, 0.0f );
	sl_vset( &ren->vertices[ 0 ].texcoords, center.x, center.y );

	for( int i = 0; i < 6; ++i ) {
		f32_t ang = ( M_PI / 3.0f ) * i + ( M_PI / 6.0f );
		f32_t px = cosf( ang ),
			  py = sinf( ang );
		f32_t uvx = center.x + extent.x * px * 0.5f,
			  uvy = center.y + extent.y * py * 0.5f;
		sl_vset( &ren->vertices[ 1 + i ].position, px, py );
		sl_vset( &ren->vertices[ 1 + i ].texcoords, uvx, uvy );
	}	

	ren->indices = SL_ALLOC( sizeof( unsigned short ) * 18 );
	for( int i = 0; i < 6; ++i ) {
		ren->indices[ i * 3 ] = 0;
		ren->indices[ i * 3 + 1 ] = 1 + i;
		ren->indices[ i * 3 + 2 ] = 1 + ( ( 1 + i ) % 6 );
	}
#else
	sl_vertex vertices[ 7 ];
	unsigned short triangles[ 18 ];

	ren->index_count = 18;
	ren->vertex_count = 7;

	// Create our vertices
	sl_bcenter( &center, uv_aabb );
	sl_bextent( &extent, uv_aabb );
	vertices[ 0 ].position = vec2( 0.0f, 0.0f );
	vertices[ 0 ].texcoords = vec2( center.x, center.y );

	for( int i = 0; i < 6; ++i ) {
		f32_t ang = ( M_PI / 3.0f ) * i + ( M_PI / 6.0f );
		f32_t px = cosf( ang ),
			  py = sinf( ang );
		f32_t uvx = center.x + extent.x * px * 0.5f,
			  uvy = center.y + extent.y * py * 0.5f;
		vertices[ 1 + i ].position = vec2( px, py );
		vertices[ 1 + i ].texcoords = vec2( uvx, uvy );
	}

	// Create the triangles
	for( int i = 0; i < 6; ++i ) {
		triangles[ i * 3 ] = 0;
		triangles[ i * 3 + 1 ] = 1 + i;
		triangles[ i * 3 + 2 ] = 1 + ( ( 1 + i ) % 6 );
	}
	
	// Create the vertex array object
	glGenVertexArrays( 1, &ren->arrayBufferIndex );
	glBindVertexArray( ren->arrayBufferIndex );

	// Create and upload the vertex data
	glGenBuffers( 1, &ren->vertBufferObjectIndex );
	glBindBuffer( GL_ARRAY_BUFFER, ren->vertBufferObjectIndex );
	glBufferData( GL_ARRAY_BUFFER, 7 * sizeof( sl_vertex ), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 7 * sizeof( sl_vertex ), vertices );

	// Set attribute pointers
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( sl_vertex ), 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_TRUE, sizeof( sl_vertex ), ( GLvoid* )( 2 * sizeof( GLfloat ) ) );

	// Unbind buffer object and buffer
	glEnableVertexAttribArray( 0 );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Create the index buffer and upload it
	glGenBuffers( 1, &ren->indexBufferObjectIndex );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ren->indexBufferObjectIndex );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof( unsigned short ), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, 18 * sizeof( unsigned short ), triangles );

	// Unbind the Index buffer
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
#endif
}

void sl_renderable_create_hex_flat( sl_renderable *ren, sl_box *uv_aabb )
{
	v2 center, extent;
#ifdef SL_LEGACY_OPENGL
	sl_bcenter( &center, uv_aabb );
	sl_bextent( &extent, uv_aabb );
	ren->vertices = SL_ALLOC( sizeof( sl_vertex ) * 7 );
	sl_vset( &ren->vertices[ 0 ].position, 0.0f, 0.0f );
	sl_vset( &ren->vertices[ 0 ].texcoords, center.x, center.y );

	for( int i = 0; i < 6; ++i ) {
		f32_t ang = ( M_PI / 3.0f ) * i;
		f32_t px = cosf( ang ),
			  py = sinf( ang );
		f32_t uvx = center.x + extent.x * px * 0.5f,
			  uvy = center.y + extent.y * py * 0.5f;
		sl_vset( &ren->vertices[ 1 + i ].position, px, py );
		sl_vset( &ren->vertices[ 1 + i ].texcoords, uvx, uvy );
	}

	ren->indices = SL_ALLOC( sizeof( unsigned short ) * 18 );
	for( int i = 0; i < 6; ++i ) {
		ren->indices[ i * 3 ] = 0;
		ren->indices[ i * 3 + 1 ] = 1 + i;
		ren->indices[ i * 3 + 2 ] = 1 + ( ( 1 + i ) % 6 );
	}
#else
	sl_vertex vertices[ 7 ];
	unsigned short triangles[ 18 ];

	ren->index_count = 18;
	ren->vertex_count = 7;

	// Create our vertices
	sl_bcenter( &center, uv_aabb );
	sl_bextent( &extent, uv_aabb );
	vertices[ 0 ].position = vec2( 0.0f, 0.0f );
	vertices[ 0 ].texcoords = vec2( center.x, center.y );

	for( int i = 0; i < 6; ++i ) {
		f32_t ang = ( M_PI / 3.0f ) * i;
		f32_t px = cosf( ang ),
			  py = sinf( ang );
		f32_t uvx = center.x + extent.x * px * 0.5f,
			  uvy = center.y + extent.y * py * 0.5f;
		vertices[ 1 + i ].position = vec2( px, py );
		vertices[ 1 + i ].texcoords = vec2( uvx, uvy );
	}

	// Create the triangles
	for( int i = 0; i < 6; ++i ) {
		triangles[ i * 3 ] = 0;
		triangles[ i * 3 + 1 ] = 1 + i;
		triangles[ i * 3 + 2 ] = 1 + ( ( 1 + i ) % 6 );
	}

	// Create the vertex array object
	glGenVertexArrays( 1, &ren->arrayBufferIndex );
	glBindVertexArray( ren->arrayBufferIndex );

	// Create and upload the vertex data
	glGenBuffers( 1, &ren->vertBufferObjectIndex );
	glBindBuffer( GL_ARRAY_BUFFER, ren->vertBufferObjectIndex );
	glBufferData( GL_ARRAY_BUFFER, 7 * sizeof( sl_vertex ), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 7 * sizeof( sl_vertex ), vertices );

	// Set attribute pointers
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( sl_vertex ), 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_TRUE, sizeof( sl_vertex ), ( GLvoid* )( 2 * sizeof( GLfloat ) ) );

	// Unbind buffer object and buffer
	glEnableVertexAttribArray( 0 );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Create the index buffer and upload it
	glGenBuffers( 1, &ren->indexBufferObjectIndex );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ren->indexBufferObjectIndex );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 18 * sizeof( unsigned short ), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, 18 * sizeof( unsigned short ), triangles );

	// Unbind the Index buffer
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
#endif
}

void sl_renderable_destroy( sl_renderable *ren )
{
#ifdef SL_LEGACY_OPENGL
	SL_DEALLOC( ren->vertices );
	SL_DEALLOC( ren->indices );
#else
	// Destroy our buffers
	glDeleteBuffers( 1, &ren->indexBufferObjectIndex );
	glDeleteBuffers( 1, &ren->vertBufferObjectIndex );
	glDeleteVertexArrays( 1, &ren->arrayBufferIndex );
#endif
}

void sl_renderable_bind( sl_renderable *ren )
{
#ifndef SL_LEGACY_OPENGL
	glBindVertexArray( ren->arrayBufferIndex );
	glBindBuffer( GL_ARRAY_BUFFER, ren->vertBufferObjectIndex );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ren->indexBufferObjectIndex );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
#endif
}

void sl_renderable_unbind( )
{
#ifndef SL_LEGACY_OPENGL
	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
#endif
}


	
