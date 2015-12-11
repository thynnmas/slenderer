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

void sl_bset_vec( sl_box *result, const v2 *min_p, const v2 *max_p )
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

void sl_bcenter( v2 *result, const sl_box *b )
{
	result->x = ( b->max_p.x - b->min_p.x ) * 0.5f + b->min_p.x;
	result->y = ( b->max_p.y - b->min_p.y ) * 0.5f + b->min_p.y;
}
void sl_bextent( v2 *result, const sl_box *b )
{
	result->x = b->max_p.x - b->min_p.x;
	result->y = b->max_p.y - b->min_p.y;
}

void sl_btranslate( sl_box *result, const sl_box *b, const v2 *offset )
{
	result->min_p = vadd2( b->min_p, *offset );
	result->max_p = vadd2( b->max_p, *offset );
}
void sl_bscale( sl_box *result, const sl_box *b, const v2 *scale )
{
	result->min_p = vmul2( b->min_p, *scale );
	result->max_p = vmul2( b->max_p, *scale );
}

int sl_binside( const sl_box *b, const v2 *p )
{
	uint32_t gtx, gty, ltx, lty;
	
	// Is p >= min
	gtx = p->x >= b->min_p.x ? 0xf : 0;
	gty = p->y >= b->min_p.y ? 0xf : 0;
	// Is p <= max
	ltx = p->x <= b->min_p.x ? 0xf : 0;
	lty = p->y <= b->min_p.y ? 0xf : 0;

	// Test if all components are true
	return gtx && gty && ltx && lty;
}

int sl_bintersect( const sl_box *a, const sl_box *b )
{
	return sl_binside( b, &a->min_p ) || sl_binside( b, &a->max_p );
}

int sl_bcontains( const sl_box *outer, const sl_box *inner )
{
	return sl_binside( outer, &inner->min_p ) && sl_binside( outer, &inner->max_p );
}

