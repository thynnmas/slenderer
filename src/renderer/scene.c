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
#include "renderer/scene.h"

#define VUL_DEFINE
#include <vul_resizable_array.h>
#include <vul_timer.h>
#include <vul_sort.h> // We need definitions for vul_sort only here
#include <vul_cmath.h>
#undef VUL_DEFINE

#include "slenderer.h"

void sl_scene_create( sl_scene *scene, u32 parent_window_id, unsigned int scene_id, u32 post_program_id )
{
	unsigned int i;
	sl_box uvs;

	for( i = 0; i < SL_MAX_LAYERS; ++i ) {
		scene->layers[ i ] = vul_vector_create( sizeof( sl_entity ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	}
	scene->layer_dirty = 0;
	scene->next_entity_id = 0;
	scene->window_id = parent_window_id;
	scene->scene_id = scene_id;

	/* Create the default post processing program */
	scene->post_program_id = post_program_id;
	scene->post_program_callback = NULL;

	/* Create renderable and quad for post rendering */
	sl_bset_scalar( &uvs, 0.f, 0.f, 1.f, 1.f );
	sl_renderable_create_quad( &scene->post_renderable, &uvs );

	scene->camera_pos = vec2( 0.0f, 0.f );
}

void sl_scene_destroy( sl_scene *scene )
{
	unsigned int i;

	for( i = 0; i < SL_MAX_LAYERS; ++i ) {
		vul_vector_destroy( scene->layers[ i ] );
	}
	scene->layer_dirty = 0;
	scene->next_entity_id = 0;
}

void sl_scene_sort( sl_scene *scene )
{
	unsigned int i;

	for( i = 0; i < SL_MAX_LAYERS; ++i ) {
		if( ( scene->layer_dirty & ( 1 << i ) ) == 0 ) {
			continue;
		}
		vul_sort_vector( scene->layers[ i ], &sl_entity_sort, 0, vul_vector_size( scene->layers[ i ] ) - 1 );
	}
	scene->layer_dirty = 0;
}

void sl_scene_set_post( sl_scene *scene, sl_program *prog, void (*post_program_callback)( sl_program *post_program ) )
{
	scene->post_program_id = prog->program_id;
	scene->post_program_callback = post_program_callback;
}

unsigned int sl_scene_add_sprite( sl_scene *scene, const unsigned int layer, 
								  const v2 *center, const v2 *scale,
								  const float rotation, const unsigned int texture_id,
								  const unsigned int program_id, const unsigned int renderable_id,
								  const sl_box *uvs, const v2 *flip_uvs,
								  const float color[ 4 ], unsigned char is_hidden )
{
	sl_entity *q;

#ifdef SL_DEBUG
	assert( layer < SL_MAX_LAYERS );
#endif

	q = ( sl_entity* )vul_vector_add_empty( scene->layers[ layer ] );
	q->hidden = is_hidden;
	q->entity_id = scene->next_entity_id++;
	q->texture_id = texture_id;
	q->program_id = program_id;
	q->renderable_id = renderable_id;
	q->uvs = *uvs;
	q->flip_uvs = *flip_uvs;
	sl_entity_create_world_matrix( q, center, scale, rotation );

	if( color == NULL ) {
		q->color[ 0 ] = q->color[ 1 ] = q->color[ 2 ] = q->color[ 3 ] = 1.f;
	} else {
		q->color[ 0 ] = color[ 0 ];
		q->color[ 1 ] = color[ 1 ];
		q->color[ 2 ] = color[ 2 ];
		q->color[ 3 ] = color[ 3 ];
	}

	return q->entity_id;
}
void sl_scene_remove_sprite( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, j;
	sl_entity *it, *last_it;

	if( layer == 0xffffffff ) {
		for( i = 0; i < SL_MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				j = 0;
				vul_foreach( sl_entity, it, last_it, scene->layers[ i ] ) {
					if( it->entity_id == id ) {
						vul_vector_remove_cascade( scene->layers[ i ], j ); 
						// After this operation, it is no longer stable, but we don't care, since
						// we return anyway!
						return;
					}
					++j;
				}
			}
		}
	} else {
#ifdef SL_DEBUG
		assert( layer < SL_MAX_LAYERS );
#endif
		if( scene->layers[ layer ] ) {
			j = 0;
			vul_foreach( sl_entity, it, last_it, scene->layers[ layer ] ) {
				if( it->entity_id == id ) {
					vul_vector_remove_cascade( scene->layers[ layer ], j ); 
					// After this operation, it is no longer stable, but we don't care, since
					// we return anyway!
					return;
				}
			}
		}
	}
}

sl_entity *sl_scene_get_volitile_entity( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, lret;
	sl_entity *ret;
	sl_entity *it, *last_it;

	ret = NULL;
	if( layer == 0xffffffff ) {
		for( i = 0; i < SL_MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				vul_foreach( sl_entity, it, last_it, scene->layers[ i ] ) {
					if( it->entity_id == id ) {
						ret = it;
						lret = i;
					}
				}
			}
		}
	} else {
#ifdef SL_DEBUG
		assert( layer < SL_MAX_LAYERS );
#endif
		lret = layer;
		if( scene->layers[ layer ] ) {
			vul_foreach( sl_entity, it, last_it, scene->layers[ layer ] ) {
				if( it->entity_id == id ) {
					ret = it;
				}
			}
		}
	}

	if( ret ) {
		scene->layer_dirty |= 1 << lret;
	}

	return ret;
}

const sl_entity *sl_scene_get_const_entity( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, lret;
	sl_entity *it, *last_it;
	
	if( layer == 0xffffffff ) {
		for( i = 0; i < SL_MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				vul_foreach( sl_entity, it, last_it, scene->layers[ i ] ) {
					if( it->entity_id == id ) {
						return it;
					}
				}
			}
		}
	} else {
#ifdef SL_DEBUG
		assert( layer < SL_MAX_LAYERS );
#endif
		if( scene->layers[ layer ] ) {
			vul_foreach( sl_entity, it, last_it, scene->layers[ layer ] ) {
				if( it->entity_id == id ) {
					return it;
				}
			}
		}
	}

	// We have failed, return NULL
	return NULL;
}

void sl_scene_get_entities_at_pos( vul_vector *vec, sl_scene *scene, v2 *pos )
{
	sl_entity *it, *last_it;
	int i;
	sl_box aabb;

	for( i = SL_MAX_LAYERS - 1; i >= 0; --i ) {
		if( scene->layers[ i ] == NULL ) {
			continue;
		}
		vul_foreach( sl_entity, it, last_it, scene->layers[ i ] )
		{
			sl_entity_aabb( &aabb, it );
			if( sl_binside( &aabb, pos ) && !it->hidden ) {
				vul_vector_add( vec, &it->entity_id );
			}
		}
	}
}
