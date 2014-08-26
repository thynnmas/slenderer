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
#include "physics/simulator.h"
#include "slenderer.h"

void sl_simulator_create( sl_simulator *sim, sl_scene *scene )
{
	sim->quads = vul_vector_create( sizeof( sl_simulator_quad ), 0 );
	sim->collision_callbacks = vul_map_create( SL_SIMULATOR_CALLBACK_BUCKETS, sl_simulator_callback_hash, sl_simulator_callback_comp );
	sim->collission_callback_keys = NULL;
	sim->scene_id = scene->scene_id;
	sim->clock = vul_timer_create( );
	sim->last_time = 0;
}

void sl_simulator_destroy( sl_simulator *sim )
{
	sl_simulator_quad *it, *lit;

	vul_foreach( sl_simulator_quad, it, lit, sim->quads )
	{
		vul_vector_destroy( it->forces );
	}
	vul_vector_destroy( sim->quads );
	vul_map_destroy( sim->collision_callbacks );
	vul_list_destroy( sim->collission_callback_keys );
	vul_timer_destroy( sim->clock );
}

sl_simulator_quad *sl_simulator_add_quad( sl_simulator *sim, unsigned int quad_id, sl_vec *start_velocity )
{
	sl_simulator_quad *q, *it, *last_it;
	sl_scene *s;
		
	vul_foreach( sl_simulator_quad, it, last_it, sim->quads )
	{
		// If it already exists in our vector, update and return
		if( it->quad->quad_id == quad_id ) {
			it->velocity = *start_velocity;
			return it;
		}
	}

	// Otherwise, add it.
	s = sl_renderer_get_scene_by_id( sim->scene_id );
	q = ( sl_simulator_quad* )vul_vector_add_empty( sim->quads );
	q->forces = vul_vector_create( sizeof( sl_vec ), 0 );
	q->quad = sl_scene_get_const_quad( s, quad_id, 0xffffffff );
	q->velocity = *start_velocity;
	sl_vset( &q->pos,
			 sl_mget4( &q->quad->world_matrix, 3, 0 ),
			 sl_mget4( &q->quad->world_matrix, 3, 1 ) );
		

	return q;
}

sl_vec *sl_simulator_add_force( sl_simulator *sim, unsigned int quad_id, sl_vec *force )
{
	sl_simulator_quad *it, *last_it;
	sl_vec *ret;

	vul_foreach( sl_simulator_quad, it, last_it, sim->quads )
	{
		if( it->quad->quad_id == quad_id ) {
			ret = ( sl_vec* )vul_vector_add_empty( it->forces );			
			sl_vcopy( ret, force );
			return ret;
		}
	}

#ifdef SL_DEBUG
	assert( 0 );
#else
	fprintf( stderr, "Attempted to add force to an unknown quad %d.\n", quad_id );
#endif
	return 0;
}

void sl_simulator_add_impulse( sl_simulator *sim, unsigned int quad_id, sl_vec *impulse )
{
	sl_simulator_quad *it, *last_it;

	vul_foreach( sl_simulator_quad, it, last_it, sim->quads )
	{
		if( it->quad->quad_id == quad_id ) {
			sl_vadd( &it->velocity, &it->velocity, impulse );
			return;
		}
	}

#ifdef SL_DEBUG
	assert( 0 );
#else
	fprintf( stderr, "Attempted to add impulse to an unknown quad %d.\n", quad_id );
#endif
}

void sl_simulator_add_callback( sl_simulator *sim, unsigned int quad_id_a, unsigned int quad_id_b, sl_simulator_collider_pair_callback callback )
{
	sl_simulator_collider_pair pair;
	vul_hash_map_element_t *element, el;
	vul_list_element_t *le;

	pair.quad_id_a = min( quad_id_a, quad_id_b );
	pair.quad_id_b = max( quad_id_a, quad_id_b );

	element = vul_map_get( sim->collision_callbacks, ( ui8_t* )&pair, sizeof( pair ) );
	if( element != NULL ) {
		// Update element
		element->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sim->collission_callback_keys, &pair, sizeof( sl_simulator_collider_pair ), sl_simulator_callback_comp );

		// Create a new element
		el.data = callback;
		el.data_size = sizeof( callback );
		el.key = ( ui8_t* )le->data;
		el.key_size = sizeof( pair );
		vul_map_insert( sim->collision_callbacks, &el );
	}
}

void sl_simulator_update( sl_simulator *sim )
{
	sl_simulator_quad *it, *lit, *it2, *lit2;
	sl_scene *s;
	sl_vec *vit, *lvit, tmp;
	sl_box aabb, aabb2;
	const vul_hash_map_element_t *el;
	sl_simulator_collider_pair pair;
	sl_quad *q;
	float time_delta_in_s;
	unsigned long long time_now;

	// Get time delta and reset clock
	time_now = vul_timer_get_micros( sim->clock );
	time_delta_in_s = ( float )( ( double )( time_now - sim->last_time ) / 1000000.0 );
	sim->last_time = time_now;

	vul_foreach( sl_simulator_quad, it, lit, sim->quads )
	{
		// Aplly all forces
		vul_foreach( sl_vec, vit, lvit, it->forces )
		{
			sl_vmuls( &tmp, vit, time_delta_in_s );
			sl_vadd( &it->velocity, &it->velocity, &tmp );
		}
		// Update position
		sl_vmuls( &tmp, &it->velocity, time_delta_in_s );
		sl_vadd( &it->pos, &it->pos, &tmp );
	}

	// Update the rendering quads (if this simulation quad has one
	s = sl_renderer_get_scene_by_id( sim->scene_id );
	vul_foreach( sl_simulator_quad, it, lit, sim->quads )
	{
		q = sl_scene_get_volitile_quad( s, it->quad->quad_id, 0xffffffff );
		sl_mseti4( &q->world_matrix, 3, 0, it->pos.x );
		sl_mseti4( &q->world_matrix, 3, 1, it->pos.y );
	}

	// With the new positions, calculate collissions
	vul_foreach( sl_simulator_quad, it, lit, sim->quads )
	{
		vul_foreach( sl_simulator_quad, it2, lit2, sim->quads )
		{
			if( it == it2 ) {
				continue; // Skip self-collissions
			}
			sl_quad_aabb( &aabb, it->quad );
			sl_quad_aabb( &aabb2, it2->quad );
			if( sl_bintersect( &aabb, &aabb2 ) ) {
				// Call callback if there is one (if not, the collission isn't handled!)
				// @NOTE: If this adjusts positions, you need to update the rendering quads from the callback!
				pair.quad_id_a = min( it->quad->quad_id, it2->quad->quad_id );
				pair.quad_id_b = max( it->quad->quad_id, it2->quad->quad_id );
				el = vul_map_get_const( sim->collision_callbacks, ( ui8_t* )&pair, sizeof( pair ) );
				if( el != NULL ) {
					( ( sl_simulator_collider_pair_callback )el->data )( s, it, it2, time_delta_in_s );
				}
			}
		}
	}
}



ui32_t sl_simulator_callback_hash( const ui8_t *key, ui32_t keylen )
{
	ui32_t qa, qb;

#ifdef SL_DEBUG
	assert( keylen == sizeof( sl_simulator_collider_pair ) );
#else
	if( keylen !=  sizeof( sl_simulator_collider_pair ) ) {
		fprintf( stderr, "Key length in simulator callback hash function is not %d, but %d.\n", sizeof( sl_simulator_collider_pair ), keylen );
		return 0;
	}
#endif
	
	qa = *( ui32_t* )key;
	qb = *( ui32_t* )( key + sizeof( ui32_t ) );

	return ( qa << 16 ) + qb; 
}

int sl_simulator_callback_comp( void *a, void *b )
{	
	vul_hash_map_element_t *ea, *eb;
	ui32_t qaa, qab, qba, qbb;

	ea = ( vul_hash_map_element_t* )a;
	eb = ( vul_hash_map_element_t* )b;

	qaa = ( ( sl_simulator_collider_pair* )ea->key )->quad_id_a;
	qab = ( ( sl_simulator_collider_pair* )ea->key )->quad_id_b;
	qba = ( ( sl_simulator_collider_pair* )eb->key )->quad_id_a;
	qbb = ( ( sl_simulator_collider_pair* )eb->key )->quad_id_b;

	if( qaa != qba ) {
		return ( int )( ( i32_t )qaa - ( i32_t )qba ); // Compare a with a first
	} else {
		return ( int )( ( i32_t )qab - ( i32_t )qbb ); // If a == a, compare bs
	}
}

void sl_simulator_callback_quad_quad( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	sl_vec inv_vel_a, inv_vel_b;				// Inverse velocities to find intersection
	sl_vec offset_a, offset_b;					// The offset to backtrack
	sl_vec t_intersect;							// Intersection point in both directions
	float t;									// The first intersection point
	sl_box aabb_a, aabb_b;						// AABBs
	sl_vec corner_a, corner_b;					// AABB corner closest to intersection point

	// Get the aabbs @TODO: Pass these in for less recalculations?
	sl_quad_aabb( &aabb_a, a->quad );
	sl_quad_aabb( &aabb_b, b->quad );

	// Get corner of AABB closest to intersection
	if( a->velocity.x > 0.0f ){
		corner_a.x = aabb_a.max_p.x;
	} else {
		corner_a.x = aabb_a.min_p.x;
	}
	if( a->velocity.y > 0.0f ){
		corner_a.y = aabb_a.max_p.y;
	} else {
		corner_a.y = aabb_a.min_p.y;
	}
	if( b->velocity.x > 0.0f ){
		corner_b.x = aabb_b.max_p.x;
	} else {
		corner_b.x = aabb_b.min_p.x;
	}
	if( b->velocity.y > 0.0f ){
		corner_b.y = aabb_b.max_p.y;
	} else {
		corner_b.y = aabb_b.min_p.y;
	}

	// Inverse velocities
	sl_vmuls( &inv_vel_a, &a->velocity, -1.0f );
	sl_vmuls( &inv_vel_b, &b->velocity, -1.0f );

	// Find t where they overlap. x_a + iv_a * t = x_b + iv_b * t => t = ( x_a + x_b ) / ( iv_a + iv_b )
	sl_vset( &t_intersect, 0.0f, 0.0f );
	if( ( inv_vel_a.x - inv_vel_b.x ) > 0 ) {
		t_intersect.x = ( corner_a.x - corner_b.x ) / ( inv_vel_a.x - inv_vel_b.x );
	}
	if( ( inv_vel_a.x - inv_vel_b.x ) > 0 ) {
		t_intersect.y = ( corner_a.y - corner_b.y ) / ( inv_vel_a.y - inv_vel_b.y );
	}

	// We only care about intersections in the last frame
	if( t_intersect.x <= 0.0f || time_frame_delta <= t_intersect.x ) {
		t_intersect.x = 0.0f;
	}	
	if( t_intersect.y <= 0.0f || time_frame_delta <= t_intersect.y ) {
		t_intersect.y = 0.0f;
	}
	// And we only care about the first intersection; we don't reflect the second, since it won't without the first happen!
	if( t_intersect.x != 0.0f && t_intersect.y != 0.0f ) {
		t_intersect.x = t_intersect.x > t_intersect.y ? t_intersect.x : 0.0f;
		t_intersect.y = t_intersect.y > t_intersect.x ? t_intersect.y : 0.0f;
	}
	t = t_intersect.x > 0.0f ? t_intersect.x : t_intersect.y;
	// And we only invert the direction in the direction we first intersect
	inv_vel_a.x = t_intersect.x == 0.0f ? -inv_vel_a.x : inv_vel_a.x;
	inv_vel_a.y = t_intersect.y == 0.0f ? -inv_vel_a.y : inv_vel_a.y;
	inv_vel_b.x = t_intersect.x == 0.0f ? -inv_vel_b.x : inv_vel_b.x;
	inv_vel_b.y = t_intersect.y == 0.0f ? -inv_vel_b.y : inv_vel_b.y;
	// We collided in the given time frame. Backtrack, and reflect the same amount
	sl_vmuls( &offset_a, &inv_vel_a, t * 2.0f );
	sl_vadd( &a->pos, &a->pos, &offset_a );
	sl_vmuls( &offset_b, &inv_vel_b, t * 2.0f );
	sl_vadd( &b->pos, &b->pos, &offset_b );

	// And store inverse velocities
	sl_vcopy( &a->velocity, &inv_vel_a );
	sl_vcopy( &b->velocity, &inv_vel_b );

	// @NOTE: We don't care about rotation here...
	sl_mseti4( &sl_scene_get_volitile_quad( scene, a->quad->quad_id, 0xffffffff )->world_matrix, 
			   3, 0, a->pos.x );
	sl_mseti4( &sl_scene_get_volitile_quad( scene, a->quad->quad_id, 0xffffffff )->world_matrix, 
			   3, 1, a->pos.y );
	sl_mseti4( &sl_scene_get_volitile_quad( scene, b->quad->quad_id, 0xffffffff )->world_matrix,
			   3, 0, b->pos.x );
	sl_mseti4( &sl_scene_get_volitile_quad( scene, b->quad->quad_id, 0xffffffff )->world_matrix,
			   3, 1, b->pos.y );
}

void sl_simulator_callback_quad_sphere( sl_scene *scene, sl_simulator_quad *quad, sl_simulator_quad *sphere, double time_frame_delta )
{
	sl_vec quad_closest;					// Closest point to the center of the sphere on the quad.
	sl_vec sphere_center, sphere_radius;	// Shpere properties
	sl_box quad_aabb, sphere_aabb;			// AABBs
	float angle, half_inv_cos_a;			// Rotation-temporaries we need to calculate scale/radius
	sl_vec normal, nnormal;					// Direction of collission normal, absolute and normalized.
	float radius_n, cos_n, sin_n;			// Radius at the normal & temps
	float t;								// Intersection point
	sl_vec combined_vel;					// The combined velocity, i.e velocity relative to each other.
	sl_vec n2, n2m;							// Temporaries used
	float vdotn;							// to calculate reflection.
	sl_vec old_vel_q, old_vel_s;			// Store old velocities
	
	// Get aabbs
	sl_quad_aabb( &quad_aabb, quad->quad );
	sl_quad_aabb( &sphere_aabb, sphere->quad );

	// Get sphere radius
	angle = ( float )asin( sphere->quad->world_matrix.data[ 1 ] );
	half_inv_cos_a = 1.0f / ( ( float )cos( angle ) * 2.0f );
	sl_vset( &sphere_radius,
		sphere->quad->world_matrix.data[ 0 ] * half_inv_cos_a,
		sphere->quad->world_matrix.data[ 5 ] * half_inv_cos_a );
	// Get sphere center
	sl_bcenter( &sphere_center, &sphere_aabb );

	// Find the closes point to the center of the sphere on the quad: if quad.min.x < center.x < quad.max.x, center.x
	if( quad_aabb.min_p.x > sphere_center.x ) {
		quad_closest.x = quad_aabb.min_p.x;
	} else if( quad_aabb.max_p.x < sphere_center.x ) {
		quad_closest.x = quad_aabb.max_p.x;
	} else {
		quad_closest.x = sphere_center.x;
	}
	if( quad_aabb.min_p.y > sphere_center.y ) {
		quad_closest.y = quad_aabb.min_p.y;
	} else if( quad_aabb.max_p.y < sphere_center.y ) {
		quad_closest.y = quad_aabb.max_p.y;
	} else {
		quad_closest.y = sphere_center.y;
	}
	// Find normal
	sl_vsub( &normal, &sphere_center, &quad_closest );
	sl_vnormalize( &nnormal, &normal );

	// Find radius at normal.
	sin_n = ( float )sin( nnormal.x );
	cos_n = ( float )cos( nnormal.y );
	radius_n = ( float )sqrt( cos_n * cos_n * sphere_radius.x + sin_n * sin_n * sphere_radius.y );

	// Intersection time it radius - length of normal / combined_velocities
	sl_vadd( &combined_vel, &quad->velocity, &sphere->velocity );
	t = radius_n * sl_vnorm( &normal ) / sl_vnorm( &combined_vel );

	// Check that the intersection was this frame.
	if( 0.0f < t && t <= time_frame_delta )
	{
		// Store the old velocities
		sl_vcopy( &old_vel_q, &quad->velocity );
		sl_vcopy( &old_vel_s, &sphere->velocity );

		// Reflect the velocity vectors
		// Q_new = -2*(Q_old dot N)*N + Q_old
		vdotn = sl_vdot( &quad->velocity, &nnormal );
		sl_vmuls( &n2,  &nnormal,  2.0f * vdotn );
		sl_vadd( &quad->velocity, &n2, &quad->velocity );
		// S_new = 2*(S_old dot N)*N + S_old
		vdotn = sl_vdot( &sphere->velocity, &nnormal );
		sl_vmuls( &n2m, &nnormal, -2.0f * vdotn );
		sl_vadd( &sphere->velocity, &n2m, &sphere->velocity );

		// Correct for the over-move
		sl_vmuls( &old_vel_q, &old_vel_q, t );
		sl_vadd( &quad->pos, &quad->pos, &old_vel_q );
		sl_vmuls( &old_vel_s, &old_vel_s, t );
		sl_vadd( &sphere->pos, &sphere->pos, &old_vel_s );

		// Move along the new direction
		sl_vmuls( &old_vel_q, &quad->velocity, t );
		sl_vadd( &quad->pos, &quad->pos, &old_vel_q );
		sl_vmuls( &old_vel_s, &sphere->velocity, t );
		sl_vadd( &sphere->pos, &sphere->pos, &old_vel_s );
	}
}

void sl_simulator_callback_sphere_sphere( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	sl_vec center_a, radius_a, center_b, radius_b;	// Shpere properties
	sl_box aabb_a, aabb_b;							// AABBs
	float angle_a, half_inv_cos_a;					// Rotation-temporaries we need to calculate scale/radius
	float angle_b, half_inv_cos_b;					// Rotation-temporaries we need to calculate scale/radius
	sl_vec normal, nnormal;							// Direction of collission normal, absolute and normalized.
	float radius_n_a, radius_n_b, cos_n, sin_n;		// Radius at the normal & temps
	float t;										// Intersection point
	sl_vec combined_vel;							// The combined velocity, i.e velocity relative to each other.
	sl_vec n2, n2m;									// Temporaries used
	float vdotn;									// to calculate reflection.
	sl_vec old_vel_a, old_vel_b;					// Store old velocities
	
	// Get aabbs
	sl_quad_aabb( &aabb_a, a->quad );
	sl_quad_aabb( &aabb_b, b->quad );

	// Get sphere radius
	angle_a = ( float )asin( a->quad->world_matrix.data[ 1 ] );
	half_inv_cos_a = 1.0f / ( ( float )cos( angle_a ) * 2.0f );
	sl_vset( &radius_a,
		a->quad->world_matrix.data[ 0 ] * half_inv_cos_a,
		a->quad->world_matrix.data[ 5 ] * half_inv_cos_a );
	angle_b = ( float )asin( b->quad->world_matrix.data[ 1 ] );
	half_inv_cos_b = 1.0f / ( ( float )cos( angle_b ) * 2.0f );
	sl_vset( &radius_b,
		b->quad->world_matrix.data[ 0 ] * half_inv_cos_b,
		b->quad->world_matrix.data[ 5 ] * half_inv_cos_b );
	// Get sphere center
	sl_bcenter( &center_b, &aabb_b );
	sl_bcenter( &center_b, &aabb_b );

	// Find normal
	sl_vsub( &normal, &center_b, &center_a );
	sl_vnormalize( &nnormal, &normal );

	// Find radius at normal.
	sin_n = ( float )sin( nnormal.x );
	cos_n = ( float )cos( nnormal.y );
	radius_n_a = ( float )sqrt( cos_n * cos_n * radius_a.x + sin_n * sin_n * radius_a.y );
	radius_n_b = ( float )sqrt( cos_n * cos_n * radius_b.x + sin_n * sin_n * radius_b.y );

	// Intersection time it radius - length of normal / combined_velocities
	sl_vadd( &combined_vel, &a->velocity, &b->velocity );
	t = radius_n_a * sl_vnorm( &normal ) / sl_vnorm( &combined_vel );

	// Check that the intersection was this frame.
	if( 0.0f < t && t <= time_frame_delta )
	{
		// Store the old velocities
		sl_vcopy( &old_vel_a, &a->velocity );
		sl_vcopy( &old_vel_b, &b->velocity );

		// Reflect the velocity vectors
		// Q_new = -2*(Q_old dot N)*N + Q_old
		vdotn = sl_vdot( &a->velocity, &nnormal );
		sl_vmuls( &n2,  &nnormal,  2.0f * vdotn );
		sl_vadd( &a->velocity, &n2, &a->velocity );
		// S_new = 2*(S_old dot N)*N + S_old
		vdotn = sl_vdot( &b->velocity, &nnormal );
		sl_vmuls( &n2m, &nnormal, -2.0f * vdotn );
		sl_vadd( &b->velocity, &n2m, &b->velocity );

		// Correct for the over-move
		sl_vmuls( &old_vel_a, &old_vel_a, t );
		sl_vadd( &a->pos, &a->pos, &old_vel_a );
		sl_vmuls( &old_vel_b, &old_vel_b, t );
		sl_vadd( &b->pos, &b->pos, &old_vel_b );

		// Move along the new direction
		sl_vmuls( &old_vel_a, &a->velocity, t );
		sl_vadd( &a->pos, &a->pos, &old_vel_a );
		sl_vmuls( &old_vel_b, &b->velocity, t );
		sl_vadd( &b->pos, &b->pos, &old_vel_b );
	}
}
