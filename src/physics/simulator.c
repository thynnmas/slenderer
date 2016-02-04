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
	sim->entities = vul_vector_create( sizeof( sl_simulator_entity ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	sim->collision_callbacks = vul_map_create( SL_SIMULATOR_CALLBACK_BUCKETS, sl_simulator_callback_hash, sl_simulator_callback_comp, SL_ALLOC, SL_DEALLOC );
	sim->collission_callback_keys = NULL;
	sim->scene_id = scene->scene_id;
	sim->clock = vul_timer_create( );
	sim->last_time = 0;
}

void sl_simulator_destroy( sl_simulator *sim )
{
	sl_simulator_entity *it, *lit;

	vul_foreach( sl_simulator_entity, it, lit, sim->entities )
	{
		vul_vector_destroy( it->forces );
	}
	vul_vector_destroy( sim->entities );
	vul_map_destroy( sim->collision_callbacks );
	vul_list_destroy( sim->collission_callback_keys, free );
	vul_timer_destroy( sim->clock );
}

sl_simulator_entity *sl_simulator_add_entity( sl_simulator *sim, unsigned int entity_id, v2 *start_velocity )
{
	sl_simulator_entity *q, *it, *last_it;
	sl_scene *s;
		
	vul_foreach( sl_simulator_entity, it, last_it, sim->entities )
	{
		// If it already exists in our vector, update and return
		if( it->entity->entity_id == entity_id ) {
			it->velocity = *start_velocity;
			return it;
		}
	}

	// Otherwise, add it.
	s = sl_renderer_get_scene_by_id( sim->scene_id );
	q = ( sl_simulator_entity* )vul_vector_add_empty( sim->entities );
	q->forces = vul_vector_create( sizeof( v2 ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	q->entity = sl_scene_get_const_entity( s, entity_id, 0xffffffff );
	q->velocity = *start_velocity;
	q->pos = vec2( q->entity->world_matrix.a30, q->entity->world_matrix.a31 );
		

	return q;
}

v2 *sl_simulator_add_force( sl_simulator *sim, unsigned int entity_id, v2 *force )
{
	sl_simulator_entity *it, *last_it;
	v2 *ret;

	vul_foreach( sl_simulator_entity, it, last_it, sim->entities )
	{
		if( it->entity->entity_id == entity_id ) {
			ret = ( v2* )vul_vector_add_empty( it->forces );			
			ret->x = force->x;
			ret->y = force->y;
			return ret;
		}
	}

#ifdef SL_DEBUG
	assert( 0 );
#else
	sl_print( 256, "Attempted to add force to an unknown entity %d.\n", entity_id );
#endif
	return 0;
}

void sl_simulator_add_impulse( sl_simulator *sim, unsigned int entity_id, v2 *impulse )
{
	sl_simulator_entity *it, *last_it;

	vul_foreach( sl_simulator_entity, it, last_it, sim->entities )
	{
		if( it->entity->entity_id == entity_id ) {
			it->velocity = vadd2( it->velocity, *impulse );
			return;
		}
	}

#ifdef SL_DEBUG
	assert( 0 );
#else
	sl_print( 256, "Attempted to add impulse to an unknown entity %d.\n", entity_id );
#endif
}

void sl_simulator_add_callback( sl_simulator *sim, unsigned int entity_id_a, unsigned int entity_id_b, sl_simulator_collider_pair_callback callback )
{
	sl_simulator_collider_pair pair;
	vul_hash_map_element *element, el;
	vul_list_element *le;

	pair.entity_id_a = SL_MIN( entity_id_a, entity_id_b );
	pair.entity_id_b = SL_MAX( entity_id_a, entity_id_b );

	element = vul_map_get( sim->collision_callbacks, ( u8* )&pair, sizeof( pair ) );
	if( element != NULL ) {
		// Update element
		element->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sim->collission_callback_keys, &pair, sizeof( sl_simulator_collider_pair ), sl_simulator_callback_comp, malloc );

		// Create a new element
		el.data = callback;
		el.data_size = sizeof( callback );
		el.key = ( u8* )le->data;
		el.key_size = sizeof( pair );
		vul_map_insert( sim->collision_callbacks, &el );
	}
}

void sl_simulator_update( sl_simulator *sim )
{
	sl_simulator_entity *it, *lit, *it2, *lit2;
	sl_scene *s;
	v2 *vit, *lvit, tmp;
	sl_box aabb, aabb2;
	const vul_hash_map_element *el;
	sl_simulator_collider_pair pair;
	sl_entity *q;
	float time_delta_in_s;
	unsigned long long time_now;

	// Get time delta and reset clock
	time_now = vul_timer_get_micros( sim->clock );
	time_delta_in_s = ( float )( ( double )( time_now - sim->last_time ) / 1000000.0 );
	sim->last_time = time_now;

	vul_foreach( sl_simulator_entity, it, lit, sim->entities )
	{
		// Aplly all forces
		vul_foreach( v2, vit, lvit, it->forces )
		{
			tmp = vmuls2( *vit, time_delta_in_s );
			it->velocity = vadd2( it->velocity, tmp );
		}
		// Update position
		tmp = vmuls2( it->velocity, time_delta_in_s );
		it->pos = vadd2( it->pos, tmp );
	}

	// Update the rendering quads (if this simulation quad has one
	s = sl_renderer_get_scene_by_id( sim->scene_id );
	vul_foreach( sl_simulator_entity, it, lit, sim->entities )
	{
		q = sl_scene_get_volitile_entity( s, it->entity->entity_id, 0xffffffff );
		q->world_matrix.a30 = it->pos.x;
		q->world_matrix.a31 = it->pos.y;
	}

	// With the new positions, calculate collissions
	vul_foreach( sl_simulator_entity, it, lit, sim->entities )
	{
		vul_foreach( sl_simulator_entity, it2, lit2, sim->entities )
		{
			if( it == it2 ) {
				continue; // Skip self-collissions
			}
			sl_entity_aabb( &aabb, it->entity );
			sl_entity_aabb( &aabb2, it2->entity );
			if( sl_bintersect( &aabb, &aabb2 ) ) {
				// Call callback if there is one (if not, the collission isn't handled!)
				// @NOTE: If this adjusts positions, you need to update the rendering quads from the callback!
				pair.entity_id_a = SL_MIN( it->entity->entity_id, it2->entity->entity_id );
				pair.entity_id_b = SL_MAX( it->entity->entity_id, it2->entity->entity_id );
				el = vul_map_get_const( sim->collision_callbacks, ( u8* )&pair, sizeof( pair ) );
				if( el != NULL ) {
					( ( sl_simulator_collider_pair_callback )el->data )( s, it, it2, time_delta_in_s );
				}
			}
		}
	}
}



u32 sl_simulator_callback_hash( const u8 *key, u32 keylen )
{
	u32 qa, qb;

#ifdef SL_DEBUG
	assert( keylen == sizeof( sl_simulator_collider_pair ) );
#else
	if( keylen !=  sizeof( sl_simulator_collider_pair ) ) {
		sl_print( 256, "Key length in simulator callback hash function is not %d, but %d.\n", sizeof( sl_simulator_collider_pair ), keylen );
		return 0;
	}
#endif
	
	qa = *( u32* )key;
	qb = *( u32* )( key + sizeof( u32 ) );

	return ( qa << 16 ) + qb; 
}

int sl_simulator_callback_comp( void *a, void *b )
{	
	vul_hash_map_element *ea, *eb;
	u32 qaa, qab, qba, qbb;

	ea = ( vul_hash_map_element* )a;
	eb = ( vul_hash_map_element* )b;

	qaa = ( ( sl_simulator_collider_pair* )ea->key )->entity_id_a;
	qab = ( ( sl_simulator_collider_pair* )ea->key )->entity_id_b;
	qba = ( ( sl_simulator_collider_pair* )eb->key )->entity_id_a;
	qbb = ( ( sl_simulator_collider_pair* )eb->key )->entity_id_b;

	if( qaa != qba ) {
		return ( int )( ( s32 )qaa - ( s32 )qba ); // Compare a with a first
	} else {
		return ( int )( ( s32 )qab - ( s32 )qbb ); // If a == a, compare bs
	}
}

void sl_simulator_callback_quad_quad( sl_scene *scene, sl_simulator_entity *a, sl_simulator_entity *b, double time_frame_delta )
{
	v2 inv_vel_a, inv_vel_b;				// Inverse velocities to find intersection
	v2 offset_a, offset_b;					// The offset to backtrack
	v2 t_intersect;							// Intersection point in both directions
	float t;									// The first intersection point
	sl_box aabb_a, aabb_b;						// AABBs
	v2 corner_a, corner_b;					// AABB corner closest to intersection point

	// Get the aabbs @TODO: Pass these in for less recalculations?
	sl_entity_aabb( &aabb_a, a->entity );
	sl_entity_aabb( &aabb_b, b->entity );

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
	inv_vel_a = vmuls2( a->velocity, -1.0f );
	inv_vel_b = vmuls2( b->velocity, -1.0f );

	// Find t where they overlap. x_a + iv_a * t = x_b + iv_b * t => t = ( x_a + x_b ) / ( iv_a + iv_b )
	t_intersect = vec2( 0.0f, 0.0f );
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
	offset_a = vmuls2( inv_vel_a, t * 2.0f );
	a->pos = vadd2( a->pos, offset_a );
	offset_b = vmuls2( inv_vel_b, t * 2.0f );
	b->pos = vadd2( b->pos, offset_b );

	// And store inverse velocities
	a->velocity.x = inv_vel_a.x;
	a->velocity.y = inv_vel_a.y;
	b->velocity.x = inv_vel_b.x;
	b->velocity.y = inv_vel_b.y;

	// @NOTE: We don't care about rotation here...
	sl_scene_get_volitile_entity( scene, a->entity->entity_id, 0xffffffff )->world_matrix.a30 = a->pos.x;
	sl_scene_get_volitile_entity( scene, a->entity->entity_id, 0xffffffff )->world_matrix.a31 = a->pos.y;
	sl_scene_get_volitile_entity( scene, b->entity->entity_id, 0xffffffff )->world_matrix.a30 = b->pos.x;
	sl_scene_get_volitile_entity( scene, b->entity->entity_id, 0xffffffff )->world_matrix.a31 = b->pos.y;
}

void sl_simulator_callback_quad_sphere( sl_scene *scene, sl_simulator_entity *quad, sl_simulator_entity *sphere, double time_frame_delta )
{
	v2 quad_closest;					// Closest point to the center of the sphere on the quad.
	v2 sphere_center, sphere_radius;	// Shpere properties
	sl_box quad_aabb, sphere_aabb;			// AABBs
	float angle, half_inv_cos_a;			// Rotation-temporaries we need to calculate scale/radius
	v2 normal, nnormal;					// Direction of collission normal, absolute and normalized.
	float radius_n, cos_n, sin_n;			// Radius at the normal & temps
	float t;								// Intersection point
	v2 combined_vel;					// The combined velocity, i.e velocity relative to each other.
	v2 n2, n2m;							// Temporaries used
	float vdotn;							// to calculate reflection.
	v2 old_vel_q, old_vel_s;			// Store old velocities
	
	// Get aabbs
	sl_entity_aabb( &quad_aabb, quad->entity );
	sl_entity_aabb( &sphere_aabb, sphere->entity );

	// Get sphere radius
	angle = ( float )asin( sphere->entity->world_matrix.A[ 1 ] );
	half_inv_cos_a = 1.0f / ( ( float )cos( angle ) * 2.0f );
	sphere_radius = vec2( sphere->entity->world_matrix.A[ 0 ] * half_inv_cos_a,
						  sphere->entity->world_matrix.A[ 5 ] * half_inv_cos_a );
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
	normal = vsub2( sphere_center, quad_closest );
	nnormal = vnormalize2( normal );

	// Find radius at normal.
	sin_n = ( float )sin( nnormal.x );
	cos_n = ( float )cos( nnormal.y );
	radius_n = ( float )sqrt( cos_n * cos_n * sphere_radius.x + sin_n * sin_n * sphere_radius.y );

	// Intersection time it radius - length of normal / combined_velocities
	combined_vel = vadd2( quad->velocity, sphere->velocity );
	t = radius_n * vnorm2( normal ) / vnorm2( combined_vel );

	// Check that the intersection was this frame.
	if( 0.0f < t && t <= time_frame_delta )
	{
		// Store the old velocities
		old_vel_q.x =  quad->velocity.x;
		old_vel_q.y =  quad->velocity.y;
		old_vel_s.x = sphere->velocity.x;
		old_vel_s.y = sphere->velocity.y;

		// Reflect the velocity vectors
		// Q_new = -2*(Q_old dot N)*N + Q_old
		vdotn = vdot2( quad->velocity, nnormal );
		n2 = vmuls2( nnormal,  2.0f * vdotn );
		quad->velocity = vadd2( n2, quad->velocity );
		// S_new = 2*(S_old dot N)*N + S_old
		vdotn = vdot2( sphere->velocity, nnormal );
		n2m = vmuls2( nnormal, -2.0f * vdotn );
		sphere->velocity = vadd2( n2m, sphere->velocity );

		// Correct for the over-move
		old_vel_q = vmuls2( old_vel_q, t );
		quad->pos = vadd2( quad->pos, old_vel_q );
		old_vel_s = vmuls2( old_vel_s, t );
		sphere->pos = vadd2( sphere->pos, old_vel_s );

		// Move along the new direction
		old_vel_q = vmuls2( quad->velocity, t );
		quad->pos = vadd2( quad->pos, old_vel_q );
		old_vel_s = vmuls2( sphere->velocity, t );
		sphere->pos = vadd2( sphere->pos, old_vel_s );
	}
}

void sl_simulator_callback_sphere_sphere( sl_scene *scene, sl_simulator_entity *a, sl_simulator_entity *b, double time_frame_delta )
{
	v2 center_a, radius_a, center_b, radius_b;	// Shpere properties
	sl_box aabb_a, aabb_b;							// AABBs
	float angle_a, half_inv_cos_a;					// Rotation-temporaries we need to calculate scale/radius
	float angle_b, half_inv_cos_b;					// Rotation-temporaries we need to calculate scale/radius
	v2 normal, nnormal;							// Direction of collission normal, absolute and normalized.
	float radius_n_a, radius_n_b, cos_n, sin_n;		// Radius at the normal & temps
	float t;										// Intersection point
	v2 combined_vel;							// The combined velocity, i.e velocity relative to each other.
	v2 n2, n2m;									// Temporaries used
	float vdotn;									// to calculate reflection.
	v2 old_vel_a, old_vel_b;					// Store old velocities
	
	// Get aabbs
	sl_entity_aabb( &aabb_a, a->entity );
	sl_entity_aabb( &aabb_b, b->entity );

	// Get sphere radius
	angle_a = ( float )asin( a->entity->world_matrix.A[ 1 ] );
	half_inv_cos_a = 1.0f / ( ( float )cos( angle_a ) * 2.0f );
	radius_a = vec2( a->entity->world_matrix.A[ 0 ] * half_inv_cos_a,
					 a->entity->world_matrix.A[ 5 ] * half_inv_cos_a );
	angle_b = ( float )asin( b->entity->world_matrix.A[ 1 ] );
	half_inv_cos_b = 1.0f / ( ( float )cos( angle_b ) * 2.0f );
	radius_b = vec2( b->entity->world_matrix.A[ 0 ] * half_inv_cos_b,
					 b->entity->world_matrix.A[ 5 ] * half_inv_cos_b );
	// Get sphere center
	sl_bcenter( &center_a, &aabb_a );
	sl_bcenter( &center_b, &aabb_b );

	// Find normal
	normal = vsub2( center_b, center_a );
	nnormal = vnormalize2( normal );

	// Find radius at normal.
	sin_n = ( float )sin( nnormal.x );
	cos_n = ( float )cos( nnormal.y );
	radius_n_a = ( float )sqrt( cos_n * cos_n * radius_a.x + sin_n * sin_n * radius_a.y );
	radius_n_b = ( float )sqrt( cos_n * cos_n * radius_b.x + sin_n * sin_n * radius_b.y );

	// Intersection time it radius - length of normal / combined_velocities
	combined_vel = vadd2( a->velocity, b->velocity );
	t = radius_n_a * vnorm2( normal ) / vnorm2( combined_vel );

	// Check that the intersection was this frame.
	if( 0.0f < t && t <= time_frame_delta )
	{
		// Store the old velocities
		old_vel_a.x = a->velocity.x;
		old_vel_a.y = a->velocity.y;
		old_vel_b.x = b->velocity.x;
		old_vel_b.y = b->velocity.y;

		// Reflect the velocity vectors
		// Q_new = -2*(Q_old dot N)*N + Q_old
		vdotn = vdot2( a->velocity, nnormal );
		n2 = vmuls2( nnormal,  2.0f * vdotn );
		a->velocity = vadd2( n2, a->velocity );
		// S_new = 2*(S_old dot N)*N + S_old
		vdotn = vdot2( b->velocity, nnormal );
		n2m = vmuls2( nnormal, -2.0f * vdotn );
		b->velocity = vadd2( n2m, b->velocity );

		// Correct for the over-move
		old_vel_a = vmuls2( old_vel_a, t );
		a->pos = vadd2( a->pos, old_vel_a );
		old_vel_b = vmuls2( old_vel_b, t );
		b->pos = vadd2( b->pos, old_vel_b );

		// Move along the new direction
		old_vel_a = vmuls2( a->velocity, t );
		a->pos = vadd2( a->pos, old_vel_a );
		old_vel_b = vmuls2( b->velocity, t );
		b->pos = vadd2( b->pos, old_vel_b );
	}
}
