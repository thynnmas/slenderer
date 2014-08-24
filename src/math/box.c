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
#include "math/box.h"

void sl_bset_vec( sl_box *result, const sl_vec *min_p, const sl_vec *max_p )
{
	result->min_p.x = min_p->x;
	result->min_p.y = min_p->y;
	result->max_p.x = max_p->x;
	result->max_p.y = max_p->y;
}
void sl_bset( sl_box *result, const sl_box *copy )
{
	result->min_p.x = copy->min_p.x;
	result->min_p.y = copy->min_p.y;
	result->max_p.x = copy->max_p.x;
	result->max_p.y = copy->max_p.y;
}
void sl_bset_scalar( sl_box *result, const float min_x, const float min_y,
											   const float max_x, const float max_y )
{
	result->min_p.x = min_x;
	result->min_p.y = min_y;
	result->max_p.x = max_x;
	result->max_p.y = max_y;
}

void sl_bcenter( sl_vec *result, const sl_box *b )
{
	result->x = ( b->max_p.x - b->min_p.x ) * 0.5f + b->min_p.x;
	result->y = ( b->max_p.y - b->min_p.y ) * 0.5f + b->min_p.y;
}
void sl_bextent( sl_vec *result, const sl_box *b )
{
	result->x = b->max_p.x - b->min_p.x;
	result->y = b->max_p.y - b->min_p.y;
}

void sl_btranslate( sl_box *result, const sl_box *b, const sl_vec *offset )
{
	sl_vadd( &result->min_p, &b->min_p, offset );
	sl_vadd( &result->max_p, &b->max_p, offset );
}
void sl_bscale( sl_box *result, const sl_box *b, const sl_vec *scale )
{
	sl_vmul( &result->min_p, &b->min_p, scale );
	sl_vmul( &result->max_p, &b->max_p, scale );
}

int sl_binside( const sl_box *b, const sl_vec *p )
{
	sl_bvec gt, lt;
	
	// Is p >= min
	sl_vgte( &gt, p, &b->min_p );
	// Is p <= max
	sl_vlte( &lt, p, &b->max_p );

	// Test if all components are true
	return sl_vall( &gt ) && sl_vall( &lt );
}

int sl_bintersect( const sl_box *a, const sl_box *b )
{
	return sl_binside( b, &a->min_p ) || sl_binside( b, &a->max_p );
}

int sl_bcontains( const sl_box *outer, const sl_box *inner )
{
	return sl_binside( outer, &inner->min_p ) && sl_binside( outer, &inner->max_p );
}

