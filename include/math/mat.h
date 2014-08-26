/**
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * 2-, 3- and 4D floating point matrices.
 * All are column major.
 * I apologize to anyone reading this, the users of the internet whoms
 * bandwidth I'm using and compiler optimization writers for unrolling
 * the loops manually just in case it helps...
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
#ifndef SLENDERER_MAT_H
#define SLENDERER_MAT_H
#include <string.h>

#include "vec.h"
/* If debug, assert */
#define SL_DEBUG
#ifdef SL_DEBUG
	#include <assert.h>
#endif

typedef struct {
	float data[ 4 ];
} sl_mat2;

typedef struct {
	float data[ 9 ];
} sl_mat3;

typedef struct {
	float data[ 16 ];
} sl_mat4;

/**
 * Set the contents of a 2D matrix from 2 column vectors.
 */
void sl_mset2_cols( sl_mat2 *result, const sl_vec *c1, const sl_vec *c2 );

/**
 * Set the contents of a 2D matrix.
 */
void sl_mset2( sl_mat2 *result, const float c1r1, const float c2r1,
								const float c1r2, const float c2r2 );

/**
 * Set the contents of a 3D matrix.
 */
void sl_mset3( sl_mat3 *result, const float c1r1, const float c2r1, const float c3r1,
								const float c1r2, const float c2r2, const float c3r2,
								const float c1r3, const float c2r3, const float c3r3 );
/**
 * Set the contents of a 4D matrix.
 */
void sl_mset4( sl_mat4 *result, const float c1r1, const float c2r1, const float c3r1, const float c4r1,
								const float c1r2, const float c2r2, const float c3r2, const float c4r2,
								const float c1r3, const float c2r3, const float c3r3, const float c4r3,
								const float c1r4, const float c2r4, const float c3r4, const float c4r4 );

/**
 * Sets the content of a 2D matrix from another 4D matrix.
 */
void sl_mcopy2( sl_mat2 *result, const sl_mat2 *ref );
/**
 * Sets the content of a 3D matrix from another 3D matrix.
 */
void sl_mcopy3( sl_mat3 *result, const sl_mat3 *ref );
/**
 * Sets the content of a 4D matrix from another 2D matrix.
 */
void sl_mcopy4( sl_mat4 *result, const sl_mat4 *ref );

/**
 * Sets the content of a 2D identity matrix.
 */
void sl_midentity2( sl_mat2 *result );
/**
 * Sets the content of a 3D identity matrix.
 */
void sl_midentity3( sl_mat3 *result );
/**
 * Sets the content of a 4D identity matrix.
 */
void sl_midentity4( sl_mat4 *result );

/**
 * Componentwise addition of two 2D matrices.
 */
void sl_madd2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b );
/**
 * Componentwise addition of two 3D matrices.
 */
void sl_madd3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b );
/**
 * Componentwise addition of two 4D matrices.
 */
void sl_madd4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b );

/**
 * Componentwise subtraction of two 2D matrices, a - b.
 */
void sl_msub2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b );
/**
 * Componentwise subtraction of two 3D matrices, a - b.
 */
void sl_msub3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b );
/**
 * Componentwise subtraction of two 4D matrices, a - b.
 */
void sl_msub4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b );

/**
 * Componentwise addition of a 2D matrix and a scalar.
 */
void sl_madds2( sl_mat2 *result, const sl_mat2 *m, const float s );
/**
 * Componentwise addition of a 3D matrix and a scalar.
 */
void sl_madds3( sl_mat3 *result, const sl_mat3 *m, const float s );
/**
 * Componentwise addition of a 4D matrix and a scalar.
 */
void sl_madds4( sl_mat4 *result, const sl_mat4 *m, const float s );

/**
 * Componentwise subtraction of a scalar from a 2D matrix.
 */
void sl_msubs2( sl_mat2 *result, const sl_mat2 *m, const float s );
/**
 * Componentwise subtraction of a scalar from a 3D matrix.
 */
void sl_msubs3( sl_mat3 *result, const sl_mat3 *m, const float s );
/**
 * Componentwise subtraction of a scalar from a 4D matrix.
 */
void sl_msubs4( sl_mat4 *result, const sl_mat4 *m, const float s );

/**
 * Componentwise multiplication of a 2D matrix and a scalar.
 */
void sl_mmuls2( sl_mat2 *result, const sl_mat2 *m, const float s );
/**
 * Componentwise multiplication of a 3D matrix and a scalar.
 */
void sl_mmuls3( sl_mat3 *result, const sl_mat3 *m, const float s );
/**
 * Componentwise multiplication of a 4D matrix and a scalar.
 */
void sl_mmuls4( sl_mat4 *result, const sl_mat4 *m, const float s );

/**
 * Componentwise division of a 2D matrix by a scalar.
 */
void sl_mdivs2( sl_mat2 *result, const sl_mat2 *m, const float s );
/**
 * Componentwise division of a 3D matrix by a scalar.
 */
void sl_mdivs3( sl_mat3 *result, const sl_mat3 *m, const float s );
/**
 * Componentwise division of a 4D matrix by a scalar.
 */
void sl_mdivs4( sl_mat4 *result, const sl_mat4 *m, const float s );

/**
 * Indexing into a 2D matrix.
 */
float sl_mget2( const sl_mat2 *m, const unsigned int col, const unsigned int row );
/**
 * Indexing into a 3D matrix.
 */
float sl_mget3( const sl_mat3 *m, const unsigned int col, const unsigned int row );
/**
 * Indexing into a 4D matrix.
 */
float sl_mget4( const sl_mat4 *m, const unsigned int col, const unsigned int row );

/**
 * Indexed set into a 2D matrix.
 */
void sl_mseti2( sl_mat2 *m, const unsigned int col, const unsigned int row, const float s );
/**
 * Indexed set into a 3D matrix.
 */
void sl_mseti3( sl_mat3 *m, const unsigned int col, const unsigned int row, const float s );
/**
 * Indexed set into a 4D matrix.
 */
void sl_mseti4( sl_mat4 *m, const unsigned int col, const unsigned int row, const float s );

/**
 * Copies the topleft 2D matrix of a 3D matrix.
 */
void sl_mtruncate23( sl_mat2 *result, const sl_mat3 *m );
/**
 * Copies the topleft 2D matrix of a 4D matrix.
 */
void sl_mtruncate24( sl_mat2 *result, const sl_mat4 *m );
/**
 * Copies the topleft 3D matrix of a 4D matrix.
 */
void sl_mtruncate34( sl_mat3 *result, const sl_mat4 *m );

/**
 * Copies a 2D matrix into the top left of a 3D matrix.
 */
void sl_mcopy32( sl_mat3 *result, const sl_mat2 *m );
/**
 * Copies a 3D matrix into the top left of a 4D matrix.
 */
void sl_mcopy43( sl_mat4 *result, const sl_mat3 *m );

/**
 * Componentwise linear interpolation between two 2D matrices (a,b) at a point t.
 * t may be any value, t=0 is a, t=1 is b.
 */
void sl_mlerp2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b, const float t );
/**
 * Componentwise linear interpolation between two 3D matrices (a,b) at a point t.
 * t may be any value, t=0 is a, t=1 is b.
 */
void sl_mlerp3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b, const float t );
/**
 * Componentwise linear interpolation between two 4D matrices (a,b) at a point t.
 * t may be any value, t=0 is a, t=1 is b.
 */
void sl_mlerp4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b, const float t );

/**
 * Multiply two 2D matrices. NOT componentwise.
 */
void sl_mmul2( sl_mat2 *result, const sl_mat2 *a, const sl_mat2 *b );
/**
 * Multiply two 3D matrices. NOT componentwise.
 */
void sl_mmul3( sl_mat3 *result, const sl_mat3 *a, const sl_mat3 *b );
/**
 * Multiply two 4D matrices. NOT componentwise.
 */
void sl_mmul4( sl_mat4 *result, const sl_mat4 *a, const sl_mat4 *b );

/**
 * Premultiply a 2D matrix by a 2D vector: v * M
 */
void sl_mul_pre( sl_vec *result, const sl_vec *v, const sl_mat2 *m );
/**
 * Postmultiply a 2D matrix by a 2D vector: M * v
 */
void sl_mul_post( sl_vec *result, const sl_mat2 *m, const sl_vec *v );
/**
 * Premultiply a 3D matrix by a 3D elevation of a 2D vector at z = 1: v * M.
 * Fill result with the first two dimensions of the result.
 */
void sl_mul3_pre( sl_vec *result, const sl_vec *v, const sl_mat3 *m );
/**
 * Postmultiply a 3D matrix by a 3D elevation of a 2D vector at z = 1: M * v.
 * Fill result with the first two dimensions of the result.
 */
void sl_mul3_post( sl_vec *result, const sl_mat3 *m, const sl_vec *v );

/**
 * Create the inverse of a 2D matrix.
 */
void sl_minverse2( sl_mat2 *result, const sl_mat2 *m );
/**
 * Create the inverse of a 3D matrix.
 */
void sl_minverse3( sl_mat3 *result, const sl_mat3 *m );
/**
 * Create the inverse of a 4D matrix.
 */
void sl_minverse4( sl_mat4 *result, const sl_mat4 *m );

/**
 * Create the transpose of a 2D matrix.
 */
void sl_mtranspose2( sl_mat2 *result, const sl_mat2 *m );
/**
 * Create the transpose of a 3D matrix.
 */
void sl_mtranspose3( sl_mat3 *result, const sl_mat3 *m );
/**
 * Create the transpose of a 4D matrix.
 */
void sl_mtranspose4( sl_mat4 *result, const sl_mat4 *m );

/**
 * Calculate the determinant of a 2D matrix.
 */
float sl_mdeterminant2( const sl_mat2 *m );
/**
 * Calculate the determinant of a 3D matrix.
 */
float sl_mdeterminant3( const sl_mat3 *m );
/**
 * Calculate the determinant of a 4D matrix.
 */
float sl_mdeterminant4( const sl_mat4 *m );

/**
 * Boolean 2D matrix. Each component is a single bit, stored in the lower 4 bits of data.
 */
typedef struct {
	unsigned char data;
} sl_bmat2;
/**
 * Boolean 3D matrix. Each component is a single bit, stored in the lower 9 bits of data.
 */
typedef struct {
	unsigned short data;
} sl_bmat3;
/**
 * Boolean 3D matrix. Each component is a single bit, stored in the 16 bits of data.
 */
typedef struct {
	unsigned short data;
} sl_bmat4;

/**
 * Componentwise equality check of 2D matrices.
 */
void sl_meq2( sl_bmat2 *result, const sl_mat2 *a, const sl_mat2 *b );
/**
 * Componentwise equality check of 3D matrices.
 */
void sl_meq3( sl_bmat3 *result, const sl_mat3 *a, const sl_mat3 *b );
/**
 * Componentwise equality check of 4D matrices.
 */
void sl_meq4( sl_bmat4 *result, const sl_mat4 *a, const sl_mat4 *b );
/**
 * Componentwise inequality check of 2D matrices.
 */
void sl_mneq2( sl_bmat2 *result, const sl_mat2 *a, const sl_mat2 *b );
/**
 * Componentwise inequality check of 3D matrices.
 */
void sl_mneq3( sl_bmat3 *result, const sl_mat3 *a, const sl_mat3 *b );
/**
 * Componentwise inequality check of 4D matrices.
 */
void sl_mneq4( sl_bmat4 *result, const sl_mat4 *a, const sl_mat4 *b );

/**
 * Returns whether all elements of a 2D matrix are true.
 */
int sl_mall2( const sl_bmat2 *m );
/**
 * Returns whether all elements of a 3D matrix are true.
 */
int sl_mall3( const sl_bmat3 *m );
/**
 * Returns whether all elements of a 4D matrix are true.
 */
int sl_mall4( const sl_bmat4 *m );
/**
 * Returns whether any elements of a 2D matrix are true.
 */
int sl_many2( const sl_bmat2 *m );
/**
 * Returns whether any elements of a 3D matrix are true.
 */
int sl_many3( const sl_bmat3 *m );
/**
 * Returns whether any elements of a 4D matrix are true.
 */
int sl_many4( const sl_bmat4 *m );

#endif
