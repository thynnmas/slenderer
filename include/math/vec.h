/**
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * 2D floating point vector ops.
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
#ifndef SLENDERER_VEC_H
#define SLENDERER_VEC_H

#include <math.h>

/**
 * The sl_vector struct itself.
 */
typedef struct sl_vec {
	float x, y;
} sl_vec;

/**
 * Set the values of a sl_vector
 */
void sl_vset( sl_vec *result, float x, float y );
/**
 * Set the values of a sl_vector
 */
void sl_vcopy( sl_vec *result, const sl_vec *ref );

/**
 * Componentwise addition of two sl_vectors.
 */
void sl_vadd( sl_vec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise subtraction of sl_vector b from a.
 */
void sl_vsub( sl_vec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise multiplication of two sl_vectors.
 */
void sl_vmul( sl_vec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise division of sl_vector a by b.
 */
void sl_vdiv( sl_vec *result, const sl_vec *a, const sl_vec *b );

/**
 * Componentswise addition of a sl_vector and a scalar.
 */
void sl_vadds( sl_vec *result, const sl_vec *v, const float s );
/**
 * Componentswise subtraction of a scalar from a sl_vector.
 */
void sl_vsubs( sl_vec *result, const sl_vec *v, const float s );
/**
 * Componentswise multiplication of two sl_vectors.
 */
void sl_vmuls( sl_vec *result, const sl_vec *v, const float s );
/**
 * Componentswise division of a sl_vector by a scalar.
 */
void sl_vdivs( sl_vec *result, const sl_vec *v, const float s );

/**
 * ?: operator. Returns:
 *   ( x ? x : ( y ? y : 0.f ) )
 */
float sl_vselect( const sl_vec *v );
/**
 * The 2D cross product.
 * See http://allenchou.net/2013/07/cross-product-of-2d-sl_vectors/ for our definition.
 */
float sl_vcross( const sl_vec *a, const sl_vec *b );
/**
 * The 2D inner product of two sl_vectors in euclidian space.
 */
float sl_vdot( const sl_vec *a, const sl_vec *b );
/**
 * The norm of a esl_vector.
 */
float sl_vnorm( const sl_vec *v );
/**
 * Normalizes a sl_vector.
 */
void sl_vnormalize( sl_vec *result, const sl_vec *v );

/**
 * Componentwise minimum of two sl_vectors
 */
void sl_vmin( sl_vec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise maximum of two sl_vectors
 */
void sl_vmax( sl_vec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise minimum of a sl_vector and a scalar.
 */
void sl_vmins( sl_vec *result, const sl_vec *v, const float s );
/**
 * Componentwise maximum of a sl_vector and a scalar.
 */
void sl_vmaxs( sl_vec *result, const sl_vec *v, const float s );
/**
 * Componentwise absolute value of a sl_vector.
 */
void sl_vabs( sl_vec *result, const sl_vec *v );
/**
 * Componentwise clamp of a sl_vector between lower and upper bounds inclusive.
 */
void sl_vclamp( sl_vec *result, const sl_vec *v, const float lb, const float ub );
/**
 * Componentwise clamp of a sl_vector between 0.0 and 1.0 inclusive.
 */
void sl_vsaturate( sl_vec *result, const sl_vec *v );

/**
 * Componentwise interpolation between two sl_vectors (a,b) at a point t.
 * t may be any value, t=0 is a, t=1 is b.
 */
void sl_vlerp( sl_vec *result, const sl_vec *a, const sl_vec *b, const float t );

/**
 * Returns the minimum component of a sl_vector.
 */
float sl_vminc( const sl_vec *v );
/**
 * Returns the maximum component of a sl_vector.
 */
float sl_vmaxc( const sl_vec *v );


/**
 * A 2D boolean sl_vector.
 * Uses bitfields and is a single byte large.
 */

typedef struct {
	unsigned int x : 4;
	unsigned int y : 4;
} sl_bvec;

/**
 * Returns whether all elements of the sl_vector are true.
 */
int sl_vall( const sl_bvec *v );
/**
 * Returns whether any element of the sl_vector is true.
 */
int sl_vany( const sl_bvec *v );
/**
 * Componentwise equality check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_veq( sl_bvec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise inequality check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_vneq( sl_bvec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise greater than check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_vgt( sl_bvec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise less than check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_vlt( sl_bvec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise greater than or equal check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_vgte( sl_bvec *result, const sl_vec *a, const sl_vec *b );
/**
 * Componentwise less than or equal check. Fills the boolean sl_vector 
 * result with the equivalent coponentwise resutls.
 */
void sl_vlte( sl_bvec *result, const sl_vec *a, const sl_vec *b );

/**
 * Componentwise negation.
 */
void sl_vneg( sl_vec *result, const sl_vec *v );

#endif
