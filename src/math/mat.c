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
#include "math/mat.h"

void sl_mset2_cols( sl_mat2 *result, const sl_vec *c1, const sl_vec *c2 )
{
	result->data[ 0 ] = c1->x;
	result->data[ 1 ] = c1->y;
	result->data[ 2 ] = c2->x;
	result->data[ 3 ] = c2->y;
}
void sl_mset2( sl_mat2 *result, const float c1r1, const float c2r1,
								const float c1r2, const float c2r2 )
{
	result->data[ 0 ] = c1r1;
	result->data[ 1 ] = c1r2;
	result->data[ 2 ] = c2r1;
	result->data[ 3 ] = c2r2;
}
void sl_mset3( sl_mat3 *result, const float c1r1, const float c2r1, const float c3r1,
								const float c1r2, const float c2r2, const float c3r2,
								const float c1r3, const float c2r3, const float c3r3 )
{
	result->data[ 0 ] = c1r1;
	result->data[ 1 ] = c1r2;
	result->data[ 2 ] = c1r3;
	result->data[ 3 ] = c2r1;
	result->data[ 4 ] = c2r2;
	result->data[ 5 ] = c2r3;
	result->data[ 6 ] = c3r1;
	result->data[ 7 ] = c3r2;
	result->data[ 8 ] = c3r3;
}
void sl_mset4( sl_mat4 *result, const float c1r1, const float c2r1, const float c3r1, const float c4r1,
								const float c1r2, const float c2r2, const float c3r2, const float c4r2,
								const float c1r3, const float c2r3, const float c3r3, const float c4r3,
								const float c1r4, const float c2r4, const float c3r4, const float c4r4 )
{
	result->data[ 0 ] = c1r1;
	result->data[ 1 ] = c1r2;
	result->data[ 2 ] = c1r3;
	result->data[ 3 ] = c1r4;
	result->data[ 4 ] = c2r1;
	result->data[ 5 ] = c2r2;
	result->data[ 6 ] = c2r3;
	result->data[ 7 ] = c2r4;
	result->data[ 8 ] = c3r1;
	result->data[ 9 ] = c3r2;
	result->data[ 10 ] = c3r3;
	result->data[ 11 ] = c3r4;
	result->data[ 12 ] = c4r1;
	result->data[ 13 ] = c4r2;
	result->data[ 14 ] = c4r3;
	result->data[ 15 ] = c4r4;
}
void sl_mcopy2( sl_mat2 *result, const sl_mat2 *ref )
{
	memcpy( result->data, ref->data, 4 * sizeof( float ) );
}
void sl_mcopy3( sl_mat3 *result, const sl_mat3 *ref )
{
	memcpy( result->data, ref->data, 9 * sizeof( float ) );
}
void sl_mcopy4( sl_mat4 *result, const sl_mat4 *ref )
{
	memcpy( result->data, ref->data, 16 * sizeof( float ) );
}

void sl_midentity2( sl_mat2 *result )
{
	result->data[ 0 ] = result->data[ 3 ] = 1.0f;
	result->data[ 1 ] = result->data[ 2 ] = 0.0f;
}
void sl_midentity3( sl_mat3 *result )
{
	result->data[ 0 ] = result->data[ 4 ] = result->data[ 8 ] = 1.0f;
	result->data[ 1 ] = result->data[ 2 ] = 0.0f;
	result->data[ 3 ] = result->data[ 5 ] = 0.0f;
	result->data[ 6 ] = result->data[ 7 ] = 0.0f;
}
void sl_midentity4( sl_mat4 *result )
{
	result->data[ 0 ]  = result->data[ 5 ]  = result->data[ 10 ] = result->data[ 15 ] = 1.0f;
	result->data[ 1 ]  = result->data[ 2 ]  = result->data[ 3 ]  = 0.0f;
	result->data[ 4 ]  = result->data[ 6 ]  = result->data[ 7 ]  = 0.0f;
	result->data[ 8 ]  = result->data[ 9 ]  = result->data[ 11 ] = 0.0f;
	result->data[ 12 ] = result->data[ 13 ] = result->data[ 14 ] = 0.0f;
}

void sl_madd2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b )
{
	result->data[ 0 ] = a->data[ 0 ] + b->data[ 0 ];
	result->data[ 1 ] = a->data[ 1 ] + b->data[ 1 ];
	result->data[ 2 ] = a->data[ 2 ] + b->data[ 2 ];
	result->data[ 3 ] = a->data[ 3 ] + b->data[ 3 ];
}
void sl_madd3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b )
{
	result->data[ 0 ] = a->data[ 0 ] + b->data[ 0 ];
	result->data[ 1 ] = a->data[ 1 ] + b->data[ 1 ];
	result->data[ 2 ] = a->data[ 2 ] + b->data[ 2 ];
	result->data[ 3 ] = a->data[ 3 ] + b->data[ 3 ];
	result->data[ 4 ] = a->data[ 4 ] + b->data[ 4 ];
	result->data[ 5 ] = a->data[ 5 ] + b->data[ 5 ];
	result->data[ 6 ] = a->data[ 6 ] + b->data[ 6 ];
	result->data[ 7 ] = a->data[ 7 ] + b->data[ 7 ];
	result->data[ 8 ] = a->data[ 8 ] + b->data[ 8 ];
}
void sl_madd4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b )
{
	result->data[ 0 ]  = a->data[ 0 ]  + b->data[ 0 ];
	result->data[ 1 ]  = a->data[ 1 ]  + b->data[ 1 ];
	result->data[ 2 ]  = a->data[ 2 ]  + b->data[ 2 ];
	result->data[ 3 ]  = a->data[ 3 ]  + b->data[ 3 ];
	result->data[ 4 ]  = a->data[ 4 ]  + b->data[ 4 ];
	result->data[ 5 ]  = a->data[ 5 ]  + b->data[ 5 ];
	result->data[ 6 ]  = a->data[ 6 ]  + b->data[ 6 ];
	result->data[ 7 ]  = a->data[ 7 ]  + b->data[ 7 ];
	result->data[ 8 ]  = a->data[ 8 ]  + b->data[ 8 ];
	result->data[ 9 ]  = a->data[ 9 ]  + b->data[ 9 ];
	result->data[ 10 ] = a->data[ 10 ] + b->data[ 10 ];
	result->data[ 11 ] = a->data[ 11 ] + b->data[ 11 ];
	result->data[ 12 ] = a->data[ 12 ] + b->data[ 12 ];
	result->data[ 13 ] = a->data[ 13 ] + b->data[ 13 ];
	result->data[ 14 ] = a->data[ 14 ] + b->data[ 14 ];
	result->data[ 15 ] = a->data[ 15 ] + b->data[ 15 ];
}

void sl_msub2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b )
{
	result->data[ 0 ] = a->data[ 0 ] - b->data[ 0 ];
	result->data[ 1 ] = a->data[ 1 ] - b->data[ 1 ];
	result->data[ 2 ] = a->data[ 2 ] - b->data[ 2 ];
	result->data[ 3 ] = a->data[ 3 ] - b->data[ 3 ];
}
void sl_msub3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b )
{
	result->data[ 0 ] = a->data[ 0 ] - b->data[ 0 ];
	result->data[ 1 ] = a->data[ 1 ] - b->data[ 1 ];
	result->data[ 2 ] = a->data[ 2 ] - b->data[ 2 ];
	result->data[ 3 ] = a->data[ 3 ] - b->data[ 3 ];
	result->data[ 4 ] = a->data[ 4 ] - b->data[ 4 ];
	result->data[ 5 ] = a->data[ 5 ] - b->data[ 5 ];
	result->data[ 6 ] = a->data[ 6 ] - b->data[ 6 ];
	result->data[ 7 ] = a->data[ 7 ] - b->data[ 7 ];
	result->data[ 8 ] = a->data[ 8 ] - b->data[ 8 ];
}
void sl_msub4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b )
{
	result->data[ 0 ]  = a->data[ 0 ]  - b->data[ 0 ];
	result->data[ 1 ]  = a->data[ 1 ]  - b->data[ 1 ];
	result->data[ 2 ]  = a->data[ 2 ]  - b->data[ 2 ];
	result->data[ 3 ]  = a->data[ 3 ]  - b->data[ 3 ];
	result->data[ 4 ]  = a->data[ 4 ]  - b->data[ 4 ];
	result->data[ 5 ]  = a->data[ 5 ]  - b->data[ 5 ];
	result->data[ 6 ]  = a->data[ 6 ]  - b->data[ 6 ];
	result->data[ 7 ]  = a->data[ 7 ]  - b->data[ 7 ];
	result->data[ 8 ]  = a->data[ 8 ]  - b->data[ 8 ];
	result->data[ 9 ]  = a->data[ 9 ]  - b->data[ 9 ];
	result->data[ 10 ] = a->data[ 10 ] - b->data[ 10 ];
	result->data[ 11 ] = a->data[ 11 ] - b->data[ 11 ];
	result->data[ 12 ] = a->data[ 12 ] - b->data[ 12 ];
	result->data[ 13 ] = a->data[ 13 ] - b->data[ 13 ];
	result->data[ 14 ] = a->data[ 14 ] - b->data[ 14 ];
	result->data[ 15 ] = a->data[ 15 ] - b->data[ 15 ];
}

void sl_madds2( sl_mat2 *result, const sl_mat2 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] + s;
	result->data[ 1 ] = m->data[ 1 ] + s;
	result->data[ 2 ] = m->data[ 2 ] + s;
	result->data[ 3 ] = m->data[ 3 ] + s;
}
void sl_madds3( sl_mat3 *result, const sl_mat3 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] + s;
	result->data[ 1 ] = m->data[ 1 ] + s;
	result->data[ 2 ] = m->data[ 2 ] + s;
	result->data[ 3 ] = m->data[ 3 ] + s;
	result->data[ 4 ] = m->data[ 4 ] + s;
	result->data[ 5 ] = m->data[ 5 ] + s;
	result->data[ 6 ] = m->data[ 6 ] + s;
	result->data[ 7 ] = m->data[ 7 ] + s;
	result->data[ 8 ] = m->data[ 8 ] + s;
}
void sl_madds4( sl_mat4 *result, const sl_mat4 *m, const float s )
{
	result->data[ 0 ]  = m->data[ 0 ]  + s;
	result->data[ 1 ]  = m->data[ 1 ]  + s;
	result->data[ 2 ]  = m->data[ 2 ]  + s;
	result->data[ 3 ]  = m->data[ 3 ]  + s;
	result->data[ 4 ]  = m->data[ 4 ]  + s;
	result->data[ 5 ]  = m->data[ 5 ]  + s;
	result->data[ 6 ]  = m->data[ 6 ]  + s;
	result->data[ 7 ]  = m->data[ 7 ]  + s;
	result->data[ 8 ]  = m->data[ 8 ]  + s;
	result->data[ 9 ]  = m->data[ 9 ]  + s;
	result->data[ 10 ] = m->data[ 10 ] + s;
	result->data[ 11 ] = m->data[ 11 ] + s;
	result->data[ 12 ] = m->data[ 12 ] + s;
	result->data[ 13 ] = m->data[ 13 ] + s;
	result->data[ 14 ] = m->data[ 14 ] + s;
	result->data[ 15 ] = m->data[ 15 ] + s;
}

void sl_msubs2( sl_mat2 *result, const sl_mat2 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] - s;
	result->data[ 1 ] = m->data[ 1 ] - s;
	result->data[ 2 ] = m->data[ 2 ] - s;
	result->data[ 3 ] = m->data[ 3 ] - s;
}
void sl_msubs3( sl_mat3 *result, const sl_mat3 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] - s;
	result->data[ 1 ] = m->data[ 1 ] - s;
	result->data[ 2 ] = m->data[ 2 ] - s;
	result->data[ 3 ] = m->data[ 3 ] - s;
	result->data[ 4 ] = m->data[ 4 ] - s;
	result->data[ 5 ] = m->data[ 5 ] - s;
	result->data[ 6 ] = m->data[ 6 ] - s;
	result->data[ 7 ] = m->data[ 7 ] - s;
	result->data[ 8 ] = m->data[ 8 ] - s;
}
void sl_msubs4( sl_mat4 *result, const sl_mat4 *m, const float s )
{
	result->data[ 0 ]  = m->data[ 0 ]  - s;
	result->data[ 1 ]  = m->data[ 1 ]  - s;
	result->data[ 2 ]  = m->data[ 2 ]  - s;
	result->data[ 3 ]  = m->data[ 3 ]  - s;
	result->data[ 4 ]  = m->data[ 4 ]  - s;
	result->data[ 5 ]  = m->data[ 5 ]  - s;
	result->data[ 6 ]  = m->data[ 6 ]  - s;
	result->data[ 7 ]  = m->data[ 7 ]  - s;
	result->data[ 8 ]  = m->data[ 8 ]  - s;
	result->data[ 9 ]  = m->data[ 9 ]  - s;
	result->data[ 10 ] = m->data[ 10 ] - s;
	result->data[ 11 ] = m->data[ 11 ] - s;
	result->data[ 12 ] = m->data[ 12 ] - s;
	result->data[ 13 ] = m->data[ 13 ] - s;
	result->data[ 14 ] = m->data[ 14 ] - s;
	result->data[ 15 ] = m->data[ 15 ] - s;
}

void sl_mmuls2( sl_mat2 *result, const sl_mat2 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] * s;
	result->data[ 1 ] = m->data[ 1 ] * s;
	result->data[ 2 ] = m->data[ 2 ] * s;
	result->data[ 3 ] = m->data[ 3 ] * s;
}
void sl_mmuls3( sl_mat3 *result, const sl_mat3 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] * s;
	result->data[ 1 ] = m->data[ 1 ] * s;
	result->data[ 2 ] = m->data[ 2 ] * s;
	result->data[ 3 ] = m->data[ 3 ] * s;
	result->data[ 4 ] = m->data[ 4 ] * s;
	result->data[ 5 ] = m->data[ 5 ] * s;
	result->data[ 6 ] = m->data[ 6 ] * s;
	result->data[ 7 ] = m->data[ 7 ] * s;
	result->data[ 8 ] = m->data[ 8 ] * s;
}
void sl_mmuls4( sl_mat4 *result, const sl_mat4 *m, const float s )
{	
	result->data[ 0 ]  = m->data[ 0 ]  * s;
	result->data[ 1 ]  = m->data[ 1 ]  * s;
	result->data[ 2 ]  = m->data[ 2 ]  * s;
	result->data[ 3 ]  = m->data[ 3 ]  * s;
	result->data[ 4 ]  = m->data[ 4 ]  * s;
	result->data[ 5 ]  = m->data[ 5 ]  * s;
	result->data[ 6 ]  = m->data[ 6 ]  * s;
	result->data[ 7 ]  = m->data[ 7 ]  * s;
	result->data[ 8 ]  = m->data[ 8 ]  * s;
	result->data[ 9 ]  = m->data[ 9 ]  * s;
	result->data[ 10 ] = m->data[ 10 ] * s;
	result->data[ 11 ] = m->data[ 11 ] * s;
	result->data[ 12 ] = m->data[ 12 ] * s;
	result->data[ 13 ] = m->data[ 13 ] * s;
	result->data[ 14 ] = m->data[ 14 ] * s;
	result->data[ 15 ] = m->data[ 15 ] * s;
}

void sl_mdivs2( sl_mat2 *result, const sl_mat2 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] / s;
	result->data[ 1 ] = m->data[ 1 ] / s;
	result->data[ 2 ] = m->data[ 2 ] / s;
	result->data[ 3 ] = m->data[ 3 ] / s;
}
void sl_mdivs3( sl_mat3 *result, const sl_mat3 *m, const float s )
{
	result->data[ 0 ] = m->data[ 0 ] / s;
	result->data[ 1 ] = m->data[ 1 ] / s;
	result->data[ 2 ] = m->data[ 2 ] / s;
	result->data[ 3 ] = m->data[ 3 ] / s;
	result->data[ 4 ] = m->data[ 4 ] / s;
	result->data[ 5 ] = m->data[ 5 ] / s;
	result->data[ 6 ] = m->data[ 6 ] / s;
	result->data[ 7 ] = m->data[ 7 ] / s;
	result->data[ 8 ] = m->data[ 8 ] / s;
}
void sl_mdivs4( sl_mat4 *result, const sl_mat4 *m, const float s )
{
	result->data[ 0 ]  = m->data[ 0 ]  / s;
	result->data[ 1 ]  = m->data[ 1 ]  / s;
	result->data[ 2 ]  = m->data[ 2 ]  / s;
	result->data[ 3 ]  = m->data[ 3 ]  / s;
	result->data[ 4 ]  = m->data[ 4 ]  / s;
	result->data[ 5 ]  = m->data[ 5 ]  / s;
	result->data[ 6 ]  = m->data[ 6 ]  / s;
	result->data[ 7 ]  = m->data[ 7 ]  / s;
	result->data[ 8 ]  = m->data[ 8 ]  / s;
	result->data[ 9 ]  = m->data[ 9 ]  / s;
	result->data[ 10 ] = m->data[ 10 ] / s;
	result->data[ 11 ] = m->data[ 11 ] / s;
	result->data[ 12 ] = m->data[ 12 ] / s;
	result->data[ 13 ] = m->data[ 13 ] / s;
	result->data[ 14 ] = m->data[ 14 ] / s;
	result->data[ 15 ] = m->data[ 15 ] / s;
}

float sl_mget2( const sl_mat2 *m, const unsigned int col, const unsigned int row )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 2 );
	assert( row >= 0 && row < 2 );
#endif
	return m->data[ col * 2 + row ];
}
float sl_mget3( const sl_mat3 *m, const unsigned int col, const unsigned int row )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 3 );
	assert( row >= 0 && row < 3 );
#endif
	return m->data[ col * 3 + row ];
}
float sl_mget4( const sl_mat4 *m, const unsigned int col, const unsigned int row )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 4 );
	assert( row >= 0 && row < 4 );
#endif
	return m->data[ col * 4 + row ];
}

void sl_mseti2( sl_mat2 *m, const unsigned int col, const unsigned int row, const float s )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 2 );
	assert( row >= 0 && row < 2 );
#endif
	m->data[ col * 4 + row ] = s;
}
void sl_mseti3( sl_mat3 *m, const unsigned int col, const unsigned int row, const float s )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 3 );
	assert( row >= 0 && row < 3 );
#endif
	m->data[ col * 4 + row ] = s;
}
void sl_mseti4( sl_mat4 *m, const unsigned int col, const unsigned int row, const float s )
{
#ifdef SL_DEBUG
	assert( col >= 0 && col < 4 );
	assert( row >= 0 && row < 4 );
#endif
	m->data[ col * 4 + row ] = s;
}

void sl_mtruncate23( sl_mat2 *result, const sl_mat3 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 1 ];

	result->data[ 2 ] = m->data[ 3 ];
	result->data[ 3 ] = m->data[ 4 ];
}
void sl_mtruncate24( sl_mat2 *result, const sl_mat4 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 1 ];

	result->data[ 2 ] = m->data[ 4 ];
	result->data[ 3 ] = m->data[ 5 ];
}
void sl_mtruncate34( sl_mat3 *result, const sl_mat4 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 1 ];
	result->data[ 2 ] = m->data[ 2 ];
	
	result->data[ 3 ] = m->data[ 4 ];
	result->data[ 4 ] = m->data[ 5 ];
	result->data[ 5 ] = m->data[ 6 ];
	
	result->data[ 6 ] = m->data[ 8 ];
	result->data[ 7 ] = m->data[ 9 ];
	result->data[ 8 ] = m->data[ 10 ];
}

void sl_mcopy32( sl_mat3 *result, const sl_mat2 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 1 ];

	result->data[ 3 ] = m->data[ 2 ];
	result->data[ 4 ] = m->data[ 3 ];
}
void sl_mcopy43( sl_mat4 *result, const sl_mat3 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 1 ];
	result->data[ 2 ] = m->data[ 2 ];
	
	result->data[ 4 ] = m->data[ 3 ];
	result->data[ 5 ] = m->data[ 4 ];
	result->data[ 6 ] = m->data[ 5 ];
	
	result->data[ 8 ]  = m->data[ 6 ];
	result->data[ 9 ]  = m->data[ 7 ];
	result->data[ 10 ] = m->data[ 8 ];
}

void sl_mlerp2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b, const float t )
{
	const float t1 = 1.0f - t;

	result->data[ 0 ] = a->data[ 0 ] * t1 + b->data[ 0 ] * t;
	result->data[ 1 ] = a->data[ 1 ] * t1 + b->data[ 1 ] * t;
	result->data[ 2 ] = a->data[ 2 ] * t1 + b->data[ 2 ] * t;
	result->data[ 3 ] = a->data[ 3 ] * t1 + b->data[ 3 ] * t;
}
void sl_mlerp3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b, const float t )
{
	const float t1 = 1.0f - t;

	result->data[ 0 ] = a->data[ 0 ] * t1 + b->data[ 0 ] * t;
	result->data[ 1 ] = a->data[ 1 ] * t1 + b->data[ 1 ] * t;
	result->data[ 2 ] = a->data[ 2 ] * t1 + b->data[ 2 ] * t;
	result->data[ 3 ] = a->data[ 3 ] * t1 + b->data[ 3 ] * t;
	result->data[ 4 ] = a->data[ 4 ] * t1 + b->data[ 4 ] * t;
	result->data[ 5 ] = a->data[ 5 ] * t1 + b->data[ 5 ] * t;
	result->data[ 6 ] = a->data[ 6 ] * t1 + b->data[ 6 ] * t;
	result->data[ 7 ] = a->data[ 7 ] * t1 + b->data[ 7 ] * t;
	result->data[ 8 ] = a->data[ 8 ] * t1 + b->data[ 8 ] * t;
}
void sl_mlerp4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b, const float t )
{
	const float t1 = 1.0f - t;

	result->data[ 0 ]  = a->data[ 0 ]  * t1 + b->data[ 0 ]  * t;
	result->data[ 1 ]  = a->data[ 1 ]  * t1 + b->data[ 1 ]  * t;
	result->data[ 2 ]  = a->data[ 2 ]  * t1 + b->data[ 2 ]  * t;
	result->data[ 3 ]  = a->data[ 3 ]  * t1 + b->data[ 3 ]  * t;
	result->data[ 4 ]  = a->data[ 4 ]  * t1 + b->data[ 4 ]  * t;
	result->data[ 5 ]  = a->data[ 5 ]  * t1 + b->data[ 5 ]  * t;
	result->data[ 6 ]  = a->data[ 6 ]  * t1 + b->data[ 6 ]  * t;
	result->data[ 7 ]  = a->data[ 7 ]  * t1 + b->data[ 7 ]  * t;
	result->data[ 8 ]  = a->data[ 8 ]  * t1 + b->data[ 8 ]  * t;
	result->data[ 9 ]  = a->data[ 9 ]  * t1 + b->data[ 9 ]  * t;
	result->data[ 10 ] = a->data[ 10 ] * t1 + b->data[ 10 ] * t;
	result->data[ 11 ] = a->data[ 11 ] * t1 + b->data[ 11 ] * t;
	result->data[ 12 ] = a->data[ 12 ] * t1 + b->data[ 12 ] * t;
	result->data[ 13 ] = a->data[ 13 ] * t1 + b->data[ 13 ] * t;
	result->data[ 14 ] = a->data[ 14 ] * t1 + b->data[ 14 ] * t;
	result->data[ 15 ] = a->data[ 15 ] * t1 + b->data[ 15 ] * t;
}

void sl_mmul2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b )
{
	result->data[ 0 ] = a->data[ 0 ] * b->data[ 0 ] + a->data[ 2 ] * b->data[ 1 ];
	result->data[ 1 ] = a->data[ 1 ] * b->data[ 0 ] + a->data[ 3 ] * b->data[ 1 ];

	result->data[ 2 ] = a->data[ 0 ] * b->data[ 2 ] + a->data[ 2 ] * b->data[ 3 ];
	result->data[ 3 ] = a->data[ 1 ] * b->data[ 2 ] + a->data[ 3 ] * b->data[ 3 ];
}
void sl_mmul3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b )
{
	result->data[ 0 ] = a->data[ 0 ] * b->data[ 0 ] + a->data[ 3 ] * b->data[ 1 ] + a->data[ 6 ] * b->data[ 2 ];
	result->data[ 1 ] = a->data[ 1 ] * b->data[ 0 ] + a->data[ 4 ] * b->data[ 1 ] + a->data[ 7 ] * b->data[ 2 ];
	result->data[ 2 ] = a->data[ 2 ] * b->data[ 0 ] + a->data[ 5 ] * b->data[ 1 ] + a->data[ 8 ] * b->data[ 2 ];

	result->data[ 3 ] = a->data[ 0 ] * b->data[ 3 ] + a->data[ 3 ] * b->data[ 4 ] + a->data[ 6 ] * b->data[ 5 ];
	result->data[ 4 ] = a->data[ 1 ] * b->data[ 3 ] + a->data[ 4 ] * b->data[ 4 ] + a->data[ 7 ] * b->data[ 5 ];
	result->data[ 5 ] = a->data[ 2 ] * b->data[ 3 ] + a->data[ 5 ] * b->data[ 4 ] + a->data[ 8 ] * b->data[ 5 ];

	result->data[ 6 ] = a->data[ 0 ] * b->data[ 6 ] + a->data[ 3 ] * b->data[ 7 ] + a->data[ 6 ] * b->data[ 8 ];
	result->data[ 7 ] = a->data[ 1 ] * b->data[ 6 ] + a->data[ 4 ] * b->data[ 7 ] + a->data[ 7 ] * b->data[ 8 ];
	result->data[ 8 ] = a->data[ 2 ] * b->data[ 6 ] + a->data[ 5 ] * b->data[ 7 ] + a->data[ 8 ] * b->data[ 8 ];
}
void sl_mmul4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b )
{
	result->data[ 0 ] = a->data[ 0 ] * b->data[ 0 ] + a->data[ 4 ] * b->data[ 1 ] + a->data[ 8 ]  * b->data[ 2 ] + a->data[ 12 ] * b->data[ 3 ];
	result->data[ 1 ] = a->data[ 1 ] * b->data[ 0 ] + a->data[ 5 ] * b->data[ 1 ] + a->data[ 9 ]  * b->data[ 2 ] + a->data[ 13 ] * b->data[ 3 ];
	result->data[ 2 ] = a->data[ 2 ] * b->data[ 0 ] + a->data[ 6 ] * b->data[ 1 ] + a->data[ 10 ] * b->data[ 2 ] + a->data[ 14 ] * b->data[ 3 ];
	result->data[ 3 ] = a->data[ 3 ] * b->data[ 0 ] + a->data[ 7 ] * b->data[ 1 ] + a->data[ 11 ] * b->data[ 2 ] + a->data[ 15 ] * b->data[ 3 ];

	result->data[ 4 ] = a->data[ 0 ] * b->data[ 4 ] + a->data[ 4 ] * b->data[ 5 ] + a->data[ 8 ]  * b->data[ 6 ] + a->data[ 12 ] * b->data[ 7 ];
	result->data[ 5 ] = a->data[ 1 ] * b->data[ 4 ] + a->data[ 5 ] * b->data[ 5 ] + a->data[ 9 ]  * b->data[ 6 ] + a->data[ 13 ] * b->data[ 7 ];
	result->data[ 6 ] = a->data[ 2 ] * b->data[ 4 ] + a->data[ 6 ] * b->data[ 5 ] + a->data[ 10 ] * b->data[ 6 ] + a->data[ 14 ] * b->data[ 7 ];
	result->data[ 7 ] = a->data[ 3 ] * b->data[ 4 ] + a->data[ 7 ] * b->data[ 5 ] + a->data[ 11 ] * b->data[ 6 ] + a->data[ 15 ] * b->data[ 7 ];
	
	result->data[ 8 ]  = a->data[ 0 ] * b->data[ 8 ] + a->data[ 4 ] * b->data[ 9 ] + a->data[ 8 ]  * b->data[ 10 ] + a->data[ 12 ] * b->data[ 11 ];
	result->data[ 9 ]  = a->data[ 1 ] * b->data[ 8 ] + a->data[ 5 ] * b->data[ 9 ] + a->data[ 9 ]  * b->data[ 10 ] + a->data[ 13 ] * b->data[ 11 ];
	result->data[ 10 ] = a->data[ 2 ] * b->data[ 8 ] + a->data[ 6 ] * b->data[ 9 ] + a->data[ 10 ] * b->data[ 10 ] + a->data[ 14 ] * b->data[ 11 ];
	result->data[ 11 ] = a->data[ 3 ] * b->data[ 8 ] + a->data[ 7 ] * b->data[ 9 ] + a->data[ 11 ] * b->data[ 10 ] + a->data[ 15 ] * b->data[ 11 ];

	result->data[ 12 ] = a->data[ 0 ] * b->data[ 12 ] + a->data[ 4 ] * b->data[ 13 ] + a->data[ 8 ]  * b->data[ 14 ] + a->data[ 12 ] * b->data[ 15 ];
	result->data[ 13 ] = a->data[ 1 ] * b->data[ 12 ] + a->data[ 5 ] * b->data[ 13 ] + a->data[ 9 ]  * b->data[ 14 ] + a->data[ 13 ] * b->data[ 15 ];
	result->data[ 14 ] = a->data[ 2 ] * b->data[ 12 ] + a->data[ 6 ] * b->data[ 13 ] + a->data[ 10 ] * b->data[ 14 ] + a->data[ 14 ] * b->data[ 15 ];
	result->data[ 15 ] = a->data[ 3 ] * b->data[ 12 ] + a->data[ 7 ] * b->data[ 13 ] + a->data[ 11 ] * b->data[ 14 ] + a->data[ 15 ] * b->data[ 15 ];
}

void sl_mul_pre( sl_vec *result, const sl_vec *v, const sl_mat2 *m )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 2 ] * v->y;
	result->y = m->data[ 1 ] * v->x + m->data[ 3 ] * v->y;
}
void sl_mul_post( sl_vec *result, const sl_mat2 *m, const sl_vec *v )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 1 ] * v->y;
	result->y = m->data[ 2 ] * v->x + m->data[ 3 ] * v->y;
}

void sl_mul3_pre( sl_vec *result, const sl_vec *v, const sl_mat3 *m )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 3 ] * v->y + m->data[ 2 ]/* * 1.f */;
	result->y = m->data[ 1 ] * v->x + m->data[ 4 ] * v->y + m->data[ 7 ]/* * 1.f */;
	/* and z is irrelevant */
}
void sl_mul3_post( sl_vec *result, const sl_mat3 *m, const sl_vec *v )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 1 ] * v->y + m->data[ 2 ]/* * 1.f */;
	result->y = m->data[ 3 ] * v->x + m->data[ 4 ] * v->y + m->data[ 5 ]/* * 1.f */;
	/* and z is irrelevant */
}
void sl_mul4_pre( sl_vec *result, const sl_vec *v, const sl_mat4 *m )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 3 ] * v->y + m->data[ 9 ]/* * 1.f */;
	result->y = m->data[ 1 ] * v->x + m->data[ 4 ] * v->y + m->data[ 10 ]/* * 1.f */;
	/* and z is irrelevant */
}
void sl_mul4_post( sl_vec *result, const sl_mat4 *m, const sl_vec *v )
{
	result->x = m->data[ 0 ] * v->x + m->data[ 1 ] * v->y + m->data[ 9 ]/* * 1.f */;
	result->y = m->data[ 3 ] * v->x + m->data[ 4 ] * v->y + m->data[ 10 ]/* * 1.f */;
	/* and z is irrelevant */
}
void sl_minverse2( sl_mat2 *result, const sl_mat2 *m )
{
	const float inv_det = 1.0f / sl_mdeterminant2( m );

	result->data[ 0 ] =  m->data[ 3 ] * inv_det;
	result->data[ 1 ] = -m->data[ 1 ] * inv_det;
	result->data[ 2 ] = -m->data[ 2 ] * inv_det;
	result->data[ 3 ] =  m->data[ 0 ] * inv_det;
}
void sl_minverse3( sl_mat3 *result, const sl_mat3 *m )
{
	sl_mat3 adjugate;
	sl_mat2 m2;
	int c, r, i, j, a, b;
	float det;

	for( c = 0; c < 3; ++c ) {
		for( r = 0; r < 3; ++r ) {
			for( i = 0, a = 0; i < 3; ++i ) {
				if( i == c ) { 
					continue;
				}
				for( j = 0, b = 0; j < 3; ++j ) {
					if( j == r ) {
						continue;
					}
					m2.data[ a * 2 + b ] = m->data[ i * 3 + j ];
				}
			}
			adjugate.data[ r * 3 + c ] = ( float )pow( -1.0f, c + r ) * sl_mdeterminant2( &m2 );
		}
		// Skip out some work if determinant is 0, and store the determinant for later
		if( c == 0 ) {
			det = 0.0f;
			for( r = 0; r < 3; ++r ) {
				det += m->data[ c * 3 + r ] * adjugate.data[ r * 3 + c ];
			}
			assert( det != 0 && "sl_minverse<sl_mat3>: Determinant is zero, inverse is not defined!" );
		}
	}

	sl_mmuls3( result, &adjugate, 1.f / det );
}
void sl_minverse4( sl_mat4 *result, const sl_mat4 *m )
{
	sl_mat4 adjugate;
	sl_mat3 m3;
	int c, r, i, j, a, b;
	float det;

	for( c = 0; c < 4; ++c ) {
		for( r = 0; r < 4; ++r ) {
			for( i = 0, a = 0; i < 4; ++i ) {
				if( i == c ) { 
					continue;
				}
				for( j = 0, b = 0; j < 4; ++j ) {
					if( j == r ) {
						continue;
					}
					m3.data[ a * 3 + b ] = m->data[ i * 4 + j ];
				}
			}
			adjugate.data[ r * 4 + c ] = ( float )pow( -1.0f, c + r ) * sl_mdeterminant3( &m3 );
		}
		// Skip out some work if determinant is 0, and store the determinant for later
		if( c == 0 ) {
			det = 0.0f;
			for( r = 0; r < 4; ++r ) {
				det += m->data[ c * 4 + r ] * adjugate.data[ r * 4 + c ];
			}
			assert( det != 0 && "sl_minverse<sl_mat4>: Determinant is zero, inverse is not defined!" );
		}
	}

	sl_mmuls4( result, &adjugate, 1.f / det );
}

void sl_mtranspose2( sl_mat2 *result, const sl_mat2 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 2 ];

	result->data[ 2 ] = m->data[ 1 ];
	result->data[ 3 ] = m->data[ 3 ];
}
void sl_mtranspose3( sl_mat3 *result, const sl_mat3 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 3 ];
	result->data[ 2 ] = m->data[ 6 ];

	result->data[ 3 ] = m->data[ 1 ];
	result->data[ 4 ] = m->data[ 4 ];
	result->data[ 5 ] = m->data[ 7 ];

	result->data[ 6 ] = m->data[ 2 ];
	result->data[ 7 ] = m->data[ 5 ];
	result->data[ 8 ] = m->data[ 8 ];
}
void sl_mtranspose4( sl_mat4 *result, const sl_mat4 *m )
{
	result->data[ 0 ] = m->data[ 0 ];
	result->data[ 1 ] = m->data[ 4 ];
	result->data[ 2 ] = m->data[ 8 ];
	result->data[ 3 ] = m->data[ 12 ];

	result->data[ 4 ] = m->data[ 1 ];
	result->data[ 5 ] = m->data[ 5 ];
	result->data[ 6 ] = m->data[ 9 ];
	result->data[ 7 ] = m->data[ 13 ];

	result->data[ 8 ]  = m->data[ 2 ];
	result->data[ 9 ]  = m->data[ 6 ];
	result->data[ 10 ] = m->data[ 10 ];
	result->data[ 11 ] = m->data[ 14 ];

	result->data[ 12 ] = m->data[ 3 ];
	result->data[ 13 ] = m->data[ 7 ];
	result->data[ 14 ] = m->data[ 11 ];
	result->data[ 15 ] = m->data[ 15 ];
}

float sl_mdeterminant2( const sl_mat2 *m )
{
	return m->data[ 0 ] * m->data[ 3 ]
		   - m->data[ 1 ] * m->data[ 2 ];
}
float sl_mdeterminant3( const sl_mat3 *m )
{
	sl_mat2 m2;
	float det;
	
	// We use the last column, starting at the bottom
	sl_mtruncate23( &m2, m );
	det = m->data[ 8 ] * sl_mdeterminant2( &m2 );

	m2.data[ 0 ] = m->data[ 0 ];
	m2.data[ 1 ] = m->data[ 2 ];
	m2.data[ 2 ] = m->data[ 3 ];
	m2.data[ 3 ] = m->data[ 5 ];
	det += m->data[ 7 ] * sl_mdeterminant2( &m2 );
	
	m2.data[ 0 ] = m->data[ 1 ];
	m2.data[ 1 ] = m->data[ 2 ];
	m2.data[ 2 ] = m->data[ 4 ];
	m2.data[ 3 ] = m->data[ 5 ];
	det += m->data[ 6 ] * sl_mdeterminant2( &m2 );

	return det;
}
float sl_mdeterminant4( const sl_mat4 *m )
{
	sl_mat3 m3;
	float det;
	
	// We use the last column, starting at the bottom
	sl_mtruncate34( &m3, m );
	det = m->data[ 15 ] * sl_mdeterminant3( &m3 );

	m3.data[ 0 ] = m->data[ 0 ];
	m3.data[ 1 ] = m->data[ 1 ];
	m3.data[ 2 ] = m->data[ 3 ];
	m3.data[ 3 ] = m->data[ 4 ];
	m3.data[ 4 ] = m->data[ 5 ];
	m3.data[ 5 ] = m->data[ 7 ];
	m3.data[ 6 ] = m->data[ 8 ];
	m3.data[ 7 ] = m->data[ 9 ];
	m3.data[ 8 ] = m->data[ 11 ];
	det += m->data[ 14 ] * sl_mdeterminant3( &m3 );
	
	m3.data[ 0 ] = m->data[ 0 ];
	m3.data[ 1 ] = m->data[ 2 ];
	m3.data[ 2 ] = m->data[ 3 ];
	m3.data[ 3 ] = m->data[ 4 ];
	m3.data[ 4 ] = m->data[ 6 ];
	m3.data[ 5 ] = m->data[ 7 ];
	m3.data[ 6 ] = m->data[ 8 ];
	m3.data[ 7 ] = m->data[ 10 ];
	m3.data[ 8 ] = m->data[ 11 ];
	det += m->data[ 13 ] * sl_mdeterminant3( &m3 );
	
	m3.data[ 0 ] = m->data[ 1 ];
	m3.data[ 1 ] = m->data[ 2 ];
	m3.data[ 2 ] = m->data[ 3 ];
	m3.data[ 3 ] = m->data[ 5 ];
	m3.data[ 4 ] = m->data[ 6 ];
	m3.data[ 5 ] = m->data[ 7 ];
	m3.data[ 6 ] = m->data[ 9 ];
	m3.data[ 7 ] = m->data[ 10 ];
	m3.data[ 8 ] = m->data[ 11 ];
	det += m->data[ 12 ] * sl_mdeterminant3( &m3 );

	return det;
}

void sl_meq2( sl_bmat2 *result, const sl_mat2 *a, const sl_mat2 *b )
{
	result->data =     ( ( a->data[ 0 ] == b->data[ 0 ] ) & 1 )
				   | ( ( a->data[ 1 ] == b->data[ 1 ] ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ] == b->data[ 2 ] ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ] == b->data[ 3 ] ) & ( 1 << 3 ) );
}
void sl_meq3( sl_bmat3 *result, const sl_mat3 *a, const sl_mat3 *b )
{
	result->data =     ( ( a->data[ 0 ] == b->data[ 0 ] ) & 1 )
				   | ( ( a->data[ 1 ] == b->data[ 1 ] ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ] == b->data[ 2 ] ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ] == b->data[ 3 ] ) & ( 1 << 3 ) )
				   | ( ( a->data[ 4 ] == b->data[ 4 ] ) & ( 1 << 4 ) )
				   | ( ( a->data[ 5 ] == b->data[ 5 ] ) & ( 1 << 5 ) )
				   | ( ( a->data[ 6 ] == b->data[ 6 ] ) & ( 1 << 6 ) )
				   | ( ( a->data[ 7 ] == b->data[ 7 ] ) & ( 1 << 7 ) )
				   | ( ( a->data[ 8 ] == b->data[ 8 ] ) & ( 1 << 8 ) );
}
void sl_meq4( sl_bmat4 *result, const sl_mat4 *a, const sl_mat4 *b )
{
	result->data =     ( ( a->data[ 0 ]  == b->data[ 0 ]  ) & 1 )
				   | ( ( a->data[ 1 ]  == b->data[ 1 ]  ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ]  == b->data[ 2 ]  ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ]  == b->data[ 3 ]  ) & ( 1 << 3 ) )
				   | ( ( a->data[ 4 ]  == b->data[ 4 ]  ) & ( 1 << 4 ) )
				   | ( ( a->data[ 5 ]  == b->data[ 5 ]  ) & ( 1 << 5 ) )
				   | ( ( a->data[ 6 ]  == b->data[ 6 ]  ) & ( 1 << 6 ) )
				   | ( ( a->data[ 7 ]  == b->data[ 7 ]  ) & ( 1 << 7 ) )
				   | ( ( a->data[ 8 ]  == b->data[ 8 ]  ) & ( 1 << 8 ) )
				   | ( ( a->data[ 9 ]  == b->data[ 9 ]  ) & ( 1 << 9 ) )
				   | ( ( a->data[ 10 ] == b->data[ 10 ] ) & ( 1 << 10 ) )
				   | ( ( a->data[ 11 ] == b->data[ 11 ] ) & ( 1 << 11 ) )
				   | ( ( a->data[ 12 ] == b->data[ 12 ] ) & ( 1 << 12 ) )
				   | ( ( a->data[ 13 ] == b->data[ 13 ] ) & ( 1 << 13 ) )
				   | ( ( a->data[ 14 ] == b->data[ 14 ] ) & ( 1 << 14 ) )
				   | ( ( a->data[ 15 ] == b->data[ 15 ] ) & ( 1 << 15 ) );
}

void sl_mneq2( sl_bmat2 *result, const sl_mat2 *a, const sl_mat2 *b )
{
	result->data =     ( ( a->data[ 0 ] != b->data[ 0 ] ) & 1 )
				   | ( ( a->data[ 1 ] != b->data[ 1 ] ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ] != b->data[ 2 ] ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ] != b->data[ 3 ] ) & ( 1 << 3 ) );
}
void sl_mneq3( sl_bmat3 *result, const sl_mat3 *a, const sl_mat3 *b )
{
	result->data =     ( ( a->data[ 0 ] != b->data[ 0 ] ) & 1 )
				   | ( ( a->data[ 1 ] != b->data[ 1 ] ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ] != b->data[ 2 ] ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ] != b->data[ 3 ] ) & ( 1 << 3 ) )
				   | ( ( a->data[ 4 ] != b->data[ 4 ] ) & ( 1 << 4 ) )
				   | ( ( a->data[ 5 ] != b->data[ 5 ] ) & ( 1 << 5 ) )
				   | ( ( a->data[ 6 ] != b->data[ 6 ] ) & ( 1 << 6 ) )
				   | ( ( a->data[ 7 ] != b->data[ 7 ] ) & ( 1 << 7 ) )
				   | ( ( a->data[ 8 ] != b->data[ 8 ] ) & ( 1 << 8 ) );
}
void sl_mneq4( sl_bmat4 *result, const sl_mat4 *a, const sl_mat4 *b )
{
	result->data =     ( ( a->data[ 0 ]  != b->data[ 0 ]  ) & 1 )
				   | ( ( a->data[ 1 ]  != b->data[ 1 ]  ) & ( 1 << 1 ) )
				   | ( ( a->data[ 2 ]  != b->data[ 2 ]  ) & ( 1 << 2 ) )
				   | ( ( a->data[ 3 ]  != b->data[ 3 ]  ) & ( 1 << 3 ) )
				   | ( ( a->data[ 4 ]  != b->data[ 4 ]  ) & ( 1 << 4 ) )
				   | ( ( a->data[ 5 ]  != b->data[ 5 ]  ) & ( 1 << 5 ) )
				   | ( ( a->data[ 6 ]  != b->data[ 6 ]  ) & ( 1 << 6 ) )
				   | ( ( a->data[ 7 ]  != b->data[ 7 ]  ) & ( 1 << 7 ) )
				   | ( ( a->data[ 8 ]  != b->data[ 8 ]  ) & ( 1 << 8 ) )
				   | ( ( a->data[ 9 ]  != b->data[ 9 ]  ) & ( 1 << 9 ) )
				   | ( ( a->data[ 10 ] != b->data[ 10 ] ) & ( 1 << 10 ) )
				   | ( ( a->data[ 11 ] != b->data[ 11 ] ) & ( 1 << 11 ) )
				   | ( ( a->data[ 12 ] != b->data[ 12 ] ) & ( 1 << 12 ) )
				   | ( ( a->data[ 13 ] != b->data[ 13 ] ) & ( 1 << 13 ) )
				   | ( ( a->data[ 14 ] != b->data[ 14 ] ) & ( 1 << 14 ) )
				   | ( ( a->data[ 15 ] != b->data[ 15 ] ) & ( 1 << 15 ) );
}

int sl_mall2( const sl_bmat2 *m )
{
	return ( m->data & 0xf ) == 0xf; // 4 bits
}
int sl_mall3( const sl_bmat3 *m )
{
	return ( m->data & 0x1ff ) == 0x1ff; // 9 bits
}
int sl_mall4( const sl_bmat4 *m )
{
	return m->data == 0xffff; // 16 bits
}

int sl_many2( const sl_bmat2 *m )
{
	return ( int )( m->data & 0xf ); // 4 bits
}
int sl_many3( const sl_bmat3 *m )
{
	return ( int )( m->data & 0x1ff ); // 9 bits
}
int sl_many4( const sl_bmat4 *m )
{
	return ( int )m->data; // 16 bits
}
