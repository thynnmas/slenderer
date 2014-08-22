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
#include "math/vec.h"

void sl_vset( sl_vec *result, float x, float y )
{
	result->x = x;
	result->y = y;
}
void sl_vcopy( sl_vec *result, const sl_vec *ref )
{
	result->x = ref->x;
	result->y = ref->y;
}

void sl_vadd( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x + b->x;
	result->y = a->y + b->y;
}
void sl_vsub( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x - b->x;
	result->y = a->y - b->y;
}
void sl_vmul( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x * b->x;
	result->y = a->y * b->y;
}
void sl_vdiv( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x / b->x;
	result->y = a->y / b->y;
}

void sl_vadds( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x + s;
	result->y = v->y + s;
}
void sl_vsubs( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x - s;
	result->y = v->y - s;
}
void sl_vmuls( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x * s;
	result->y = v->y * s;
}
void sl_vdivs( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x / s;
	result->y = v->y / s;
}

void sl_veq( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x == b->x ? 0xf : 0;
	result->y = a->y == b->y ? 0xf : 0;
}
void sl_vneq( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x != b->x ? 0xf : 0;
	result->y = a->y != b->y ? 0xf : 0;
}
void sl_vgt( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x > b->x ? 0xf : 0;
	result->y = a->y > b->y ? 0xf : 0;
}
void sl_vlt( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x < b->x ? 0xf : 0;
	result->y = a->y < b->y ? 0xf : 0;
}
void sl_vgte( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x >= b->x ? 0xf : 0;
	result->y = a->y >= b->y ? 0xf : 0;
}
void sl_vlte( sl_bvec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x <= b->x ? 0xf : 0;
	result->y = a->y <= b->y ? 0xf : 0;
}

void sl_vneg( sl_vec *result, const sl_vec *v )
{
	result->x = -v->x;
	result->y = -v->y;
}

float sl_vselect( const sl_vec *v )
{
	return v->x != 0.f ? v->x : v->y;
}

float sl_vcross( const sl_vec *a, const sl_vec *b )
{
	return a->x * b->y - a->y * b->x;
}
float sl_vdot( const sl_vec *a, const sl_vec *b )
{
	return a->x * b->x + a->y * b->y;
}
float sl_vnorm( const sl_vec *v )
{
	return ( float )sqrt( v->x * v->x + v->y * v->y );
}

void sl_vnormalize( sl_vec *result, const sl_vec *v )
{
	const float norm = sl_vnorm( v );
	result->x = v->x / norm;
	result->y = v->y / norm;
}

void sl_vmin( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x < b->x ? a->x : b->x;
	result->y = a->y < b->y ? a->y : b->y;
}
void sl_vmax( sl_vec *result, const sl_vec *a, const sl_vec *b )
{
	result->x = a->x > b->x ? a->x : b->x;
	result->y = a->y > b->y ? a->y : b->y;
}
void sl_vmins( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x < s ? v->x : s;
	result->y = v->y < s ? v->y : s;
}
void sl_vmaxs( sl_vec *result, const sl_vec *v, const float s )
{
	result->x = v->x > s ? v->x : s;
	result->y = v->y > s ? v->y : s;
}
void sl_vabs( sl_vec *result, const sl_vec *v )	
{
	result->x = ( float )fabs( v->x );
	result->y = ( float )fabs( v->y );
}
void sl_vclamp( sl_vec *result, const sl_vec *v, const float lb, const float ub )
{
	result->x = v->x < lb ? lb : ( v->x > ub ? ub : v->x );
	result->y = v->y < lb ? lb : ( v->y > ub ? ub : v->y );
}
void sl_vsaturate( sl_vec *result, const sl_vec *v )
{
	sl_vclamp( result, v, 0.0f, 1.0f );
}

void sl_vlerp( sl_vec *result, const sl_vec *a, const sl_vec *b, const float t )
{
	const float t1 = 1.0f - t;
	result->x = a->x * t + b->x * t1;
	result->y = a->y * t + b->y * t1;
}

float sl_vminc( const sl_vec *v )
{
	return v->x < v->y ? v->x : v->y;
}
float sl_vmaxc( const sl_vec *v )
{
	return v->x > v->y ? v->x : v->y;
}

int sl_vall( const sl_bvec *v )
{
	return v->x && v->y;
}

int sl_vany( const sl_bvec *v )
{
	return v->x || v->y;
}
