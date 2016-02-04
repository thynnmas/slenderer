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

#include "renderer/animator.h"
#include "slenderer.h"

void sl_animator_create( sl_animator *animator, sl_scene *scene )
{
	animator->transforms = vul_vector_create( sizeof( sl_animation_transform ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	animator->sprites = vul_vector_create( sizeof( sl_animation_sprite ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );

	animator->next_animation_id = 0;

	animator->clock = vul_timer_create( );
	animator->last_time = vul_timer_get_millis( animator->clock );

	animator->scene_id = scene->scene_id;
}
void sl_animator_destroy( sl_animator *animator )
{
	vul_vector_destroy( animator->transforms );
	vul_vector_destroy( animator->sprites );

	vul_timer_destroy( animator->clock );
}

unsigned int sl_animator_add_transform( sl_animator *animator, unsigned int entity_id, const m44 *end_world_matrix, unsigned long long length_in_ms, sl_animation_state state )
{
	sl_animation_transform* t;
	sl_scene *s;

	s = sl_renderer_get_scene_by_id( animator->scene_id );
	t = ( sl_animation_transform* )vul_vector_add_empty( animator->transforms );
	t->animation_id = animator->next_animation_id++;
	t->entity_id = entity_id;
	t->start_time = vul_timer_get_millis( animator->clock );
	t->end_time = t->start_time + length_in_ms;
	t->start_world_mat = sl_scene_get_const_entity( s, entity_id, 0xffffffff )->world_matrix;
	t->end_world_mat = *end_world_matrix;
	assert( state == SL_ANIMATION_RUNNING || SL_ANIMATION_RUNNING_LOOPED || SL_ANIMATION_RUNNING_PERIODIC );
	t->state = state;

	return t->animation_id;
}

unsigned int sl_animator_add_sprite( sl_animator *animator, unsigned int entity_id, vul_vector *frames, unsigned long long ms_per_frame, sl_animation_state state )
{
	sl_animation_sprite* t;

	t = ( sl_animation_sprite* )vul_vector_add_empty( animator->sprites );
	t->animation_id = animator->next_animation_id++;
	t->entity_id = entity_id;
	t->time_per_frame_in_ms = ms_per_frame;
	t->time_since_current_frame = 0ull;
	t->frames = frames;
	t->frame_count = vul_vector_size( frames );
	t->current_frame = 0;
	assert( state == SL_ANIMATION_RUNNING || SL_ANIMATION_RUNNING_LOOPED || SL_ANIMATION_RUNNING_PERIODIC );
	t->state = state;
	t->period_rising = SL_TRUE;

	return t->animation_id;
}

void sl_animator_remove_animation( sl_animator *animator, unsigned int id, sl_animation_type type )
{
	unsigned int i;
	sl_animation_transform *ita, *last_ita;
	sl_animation_sprite *its, *last_its;

	if( type == SL_ANIMATION_TRANSFORM || type == SL_ANIMATION_EITHER )
	{
		i = 0;
		vul_foreach( sl_animation_transform, ita, last_ita, animator->transforms )
		{
			if( ita->animation_id == id ) {
				vul_vector_remove_swap( animator->transforms, i );
				// Now the iterator is unstable, but we break so it's ok.
				break;
			}
			++i;
		}
	}
	if( type == SL_ANIMATION_SPRITE || type == SL_ANIMATION_EITHER )
	{
		i = 0;
		vul_foreach( sl_animation_sprite, its, last_its, animator->sprites )
		{
			if( its->animation_id == id ) {
				vul_vector_remove_swap( animator->sprites, i );
				// Now the iterator is unstable, but we break so it's ok.
				break;
			}
			++i;
		}
	}
}

void sl_animator_update( sl_animator *animator )
{
	unsigned long long now, elapsed, now_relative, total, length_in_ms;
	unsigned int i;
	float t;
	sl_animation_transform *ita, *last_ita;
	sl_animation_sprite *its, *last_its;
	sl_entity *entity;
	sl_animation_sprite_state *state;
	sl_scene *s;
	int deleted;
	m44 tmp;

	// Calculate time since last frame
	now = vul_timer_get_millis( animator->clock );
	elapsed = now - animator->last_time;
	animator->last_time = now;

	s = sl_renderer_get_scene_by_id( animator->scene_id );
	
	// @TODO: For stopped animations; add elapsed to all times!

	// First, remove any animations that are done. @NOTE: This is slow, since
	// we may have resizes in vul_vector at any time, so any deletion leads to a restart
	// of the entire search. BUT, it should be a small vector and we should have relatively
	// few overlaps anyway, so we're fine...
	while( 1 ) {
		deleted = 0;
		i = 0;
		vul_foreach( sl_animation_transform, ita, last_ita, animator->transforms )
		{
			if( ita->end_time < now ) {
				if( ita->state == SL_ANIMATION_RUNNING_LOOPED ) {
					// Reset to beginning
					length_in_ms = ita->end_time - ita->start_time;
					ita->start_time = now;
					ita->end_time = now + length_in_ms;
					// @TODO: Add callbacks that are called at loop reset/periodic reset
					// to f.ex. add an effect there.
				} else if( ita->state == SL_ANIMATION_RUNNING_PERIODIC ) {
					// Reset time beginning
					length_in_ms = ita->end_time - ita->start_time;
					ita->start_time = now;
					ita->end_time = now + length_in_ms;
					// Swap end and beginning matrices
					memcpy( &tmp, &ita->end_world_mat, sizeof( m44 ) );
					memcpy( &ita->end_world_mat, &ita->start_world_mat, sizeof( m44 ) );
					memcpy( &ita->start_world_mat, &tmp, sizeof( m44 ) );
					// @TODO: Add callbacks that are called at loop reset/periodic reset
					// to f.ex. add an effect there.
				} else {
					// Move to final matrix when done
					entity = sl_scene_get_volitile_entity( s, ita->entity_id, 0xffffffff );
					memcpy( &entity->world_matrix, &ita->end_world_mat, sizeof( m44 ) );
					ita->state = SL_ANIMATION_FINISHED;
				}
				if( ita->state == SL_ANIMATION_FINISHED ) {
					deleted = 1;
					vul_vector_remove_swap( animator->transforms, i );
					break;
				}
			}
			++i;
		}
		if( !deleted ) {
			break; // We did not delete any more items, so we are done.
		}
	}
	while( 1 ) {
		deleted = 0;
		i = 0;
		vul_foreach( sl_animation_sprite, its, last_its, animator->sprites )
		{
			if( its->current_frame == ( int )its->frame_count - 1 && its->period_rising
				&& ( elapsed + its->time_since_current_frame ) >= its->time_per_frame_in_ms ) {
				if( its->state == SL_ANIMATION_RUNNING_LOOPED ) {
					// Reset to beginning
					its->current_frame = -1;
				} else if( its->state == SL_ANIMATION_RUNNING_PERIODIC ) {
					// Change period
					its->period_rising = SL_FALSE;
				} else {
					its->state = SL_ANIMATION_FINISHED;
				}
				if( its->state == SL_ANIMATION_FINISHED ) {
					deleted = 1;
					vul_vector_remove_swap( animator->sprites, i );
					break;
				}
			} else if ( its->current_frame == 0 && !its->period_rising
				&& ( elapsed + its->time_since_current_frame ) >= its->time_per_frame_in_ms ) {
				// Can only happen for oscillating things
				its->period_rising = SL_TRUE;
			}
			++i;
		}
		if( !deleted ) {
			break; // We did not delete any more items, so we are done.
		}
	}

	// Iterate over the transforms and update them
	vul_foreach( sl_animation_transform, ita, last_ita, animator->transforms )
	{		
		// Calculate t
		total = ita->end_time - ita->start_time;
		now_relative = now - ita->start_time;
		t = ( float )( ( double )now_relative / ( double )total );
		if( t < 0.f ) {
			t = 0.f;
		} else if ( t > 1.f ) {
			t = 1.f;
		}

		// Grab the entity
		entity = sl_scene_get_volitile_entity( s, ita->entity_id, 0xffffffff ); // @TODO: This is why we want layer info when we add a transform!

		// Set the new matrix, a linear interpolation at t. @TODO: Might want to quaternion it up at some point; slerp for the orientation here!
		entity->world_matrix = mlerp44( &ita->start_world_mat, &ita->end_world_mat, t );
	}

	// Iterate over the sprites and update them.
	vul_foreach( sl_animation_sprite, its, last_its, animator->sprites )
	{
		its->time_since_current_frame += elapsed;
		if( its->time_since_current_frame > its->time_per_frame_in_ms ) {
			// Advance the animation
			its->time_since_current_frame -= its->time_per_frame_in_ms;

			entity = sl_scene_get_volitile_entity( s, its->entity_id, 0xffffffff ); // @TODO: This is why we want layer info when we add a sprite!
			if( its->period_rising ) {
				state = ( sl_animation_sprite_state* )vul_vector_get( its->frames, ++its->current_frame );
			} else {
				state = ( sl_animation_sprite_state* )vul_vector_get( its->frames, --its->current_frame );
			}

			entity->uvs = state->uvs;
			entity->texture_id = state->texture_id;
		}
	}
}
