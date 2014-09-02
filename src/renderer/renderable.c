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

void sl_renderable_create( sl_renderable *ren, sl_box *uvs )
{
#ifdef SL_LEGACY_OPENGL
	sl_vset( &ren->vertices[ 0 ].position,  -1.0f, -1.0f );
	sl_vset( &ren->vertices[ 0 ].texcoords,  uvs->min_p.x,  uvs->min_p.y );

	sl_vset( &ren->vertices[ 1 ].position,   1.0f, -1.0f );
	sl_vset( &ren->vertices[ 1 ].texcoords,  uvs->max_p.x,  uvs->min_p.y );

	sl_vset( &ren->vertices[ 2 ].position,   1.0f,  1.0f );
	sl_vset( &ren->vertices[ 2 ].texcoords,  uvs->max_p.x,  uvs->max_p.y );

	sl_vset( &ren->vertices[ 3 ].position,  -1.0f,  1.0f );
	sl_vset( &ren->vertices[ 3 ].texcoords,  uvs->min_p.x,  uvs->max_p.y );
#else
	sl_vertex vertices[ 4 ];
	unsigned short triangles[ 6 ];

	// Create our vertices
	sl_vset( &vertices[ 0 ].position,  -1.0f, -1.0f );
	sl_vset( &vertices[ 0 ].texcoords,  uvs->min_p.x,  uvs->min_p.y );

	sl_vset( &vertices[ 1 ].position,   1.0f, -1.0f );
	sl_vset( &vertices[ 1 ].texcoords,  uvs->max_p.x,  uvs->min_p.y );

	sl_vset( &vertices[ 2 ].position,   1.0f,  1.0f );
	sl_vset( &vertices[ 2 ].texcoords,  uvs->max_p.x,  uvs->max_p.y );

	sl_vset( &vertices[ 3 ].position,  -1.0f,  1.0f );
	sl_vset( &vertices[ 3 ].texcoords,  uvs->min_p.x,  uvs->max_p.y );

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

void sl_renderable_destroy( sl_renderable *ren )
{
#ifndef SL_LEGACY_OPENGL
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


	
