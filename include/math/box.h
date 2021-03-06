/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * A 2D axis aligned Box
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

#ifndef SLENDERER_BOX_H
#define SLENDERER_BOX_H
#include <vul_cmath.h>

typedef struct {
	v2 min_p;
	v2 max_p;
} sl_box;

/**
 * Sets the contents of a box.
 */
void sl_bset_vec( sl_box *result, const v2 *min_p, const v2 *max_p );
/**
 * Sets the contents of a box.
 */
void sl_bset( sl_box *result, const sl_box *copy );
/**
 * Sets the contents of a box.
 */
void sl_bset_scalar( sl_box *result, const float min_x, const float min_y,
											   const float max_x, const float max_y );
/**
 * Returns the center of a box.
 */
void sl_bcenter( v2 *result, const sl_box *b );
/**
 * Returns the extent of a box.
 */
void sl_bextent( v2 *result, const sl_box *b );

/**
 * Translates a box by a vector.
 */
void sl_btranslate( sl_box *result, const sl_box *b, const v2 *offset );
/**
 * Scales a box by a vector.
 */
void sl_bscale( sl_box *result, const sl_box *b, const v2 *scale );

/**
 * Tests whether a point is inside a box.
 */
int sl_binside( const sl_box *b, const v2 *p );

/**
 * Tests whether two boxes intersect.
 */
int sl_bintersect( const sl_box *a, const sl_box *b );

/**
 * Tests whether a box is entirely inside another box.
 */
int sl_bcontains( const sl_box *outer, const sl_box *inner );

#endif
