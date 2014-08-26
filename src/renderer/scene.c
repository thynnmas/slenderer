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
#define VUL_DEFINE
#include "renderer/scene.h"

void sl_scene_create( sl_scene *scene, ui32_t parent_window_id, unsigned int scene_id, ui32_t post_program_id )
{
	unsigned int i;
	sl_box uvs;

	for( i = 0; i < MAX_LAYERS; ++i ) {
		scene->layers[ i ] = vul_vector_create( sizeof( sl_quad ), 0 );
	}
	scene->layer_dirty = 0;
	scene->next_quad_id = 0;
	scene->window_id = parent_window_id;
	scene->scene_id = scene_id;

	/* Create the default post processing program */
	scene->post_program_id = post_program_id;
	scene->post_program_callback = NULL;

	/* Create renderable and quad for post rendering */
	sl_bset_scalar( &uvs, 0.f, 0.f, 1.f, 1.f );
	sl_renderable_create( &scene->post_renderable, &uvs );

	sl_vset( &scene->camera_pos, 0.0f, 0.f );
}

void sl_scene_destroy( sl_scene *scene )
{
	unsigned int i;

	for( i = 0; i < MAX_LAYERS; ++i ) {
		vul_vector_destroy( scene->layers[ i ] );
	}
	scene->layer_dirty = 0;
	scene->next_quad_id = 0;
}

void sl_scene_sort( sl_scene *scene )
{
	unsigned int i;

	for( i = 0; i < MAX_LAYERS; ++i ) {
		if( ( scene->layer_dirty & ( 1 << i ) ) == 0 ) {
			continue;
		}
		vul_sort_vector( scene->layers[ i ], &sl_quad_sort, 0, vul_vector_size( scene->layers[ i ] ) - 1 );
	}
	scene->layer_dirty = 0;
}

void sl_scene_set_post( sl_scene *scene, sl_program *prog, void (*post_program_callback)( sl_program *post_program ) )
{
	scene->post_program_id = prog->program_id;
	scene->post_program_callback = post_program_callback;
}

unsigned int sl_scene_add_sprite( sl_scene *scene, const unsigned int layer, 
								  const sl_vec *center, const sl_vec *scale,
								  const float rotation, const unsigned int texture_id,
								  const unsigned int program_id, const unsigned int renderable_id,
								  const sl_box *uvs, const sl_bvec *flip_uvs,
								  const float color[ 4 ], unsigned char is_hidden )
{
	sl_quad *q;

#ifdef SL_DEBUG
	assert( layer < MAX_LAYERS );
#endif

	q = ( sl_quad* )vul_vector_add_empty( scene->layers[ layer ] );
	q->hidden = is_hidden;
	q->quad_id = scene->next_quad_id++;
	q->texture_id = texture_id;
	q->program_id = program_id;
	q->renderable_id = renderable_id;
	q->uvs = *uvs;
	q->flip_uvs = *flip_uvs;
	sl_quad_create_world_matrix( q, center, scale, rotation );

	if( color == NULL ) {
		q->color[ 0 ] = q->color[ 1 ] = q->color[ 2 ] = q->color[ 3 ] = 1.f;
	} else {
		q->color[ 0 ] = color[ 0 ];
		q->color[ 1 ] = color[ 1 ];
		q->color[ 2 ] = color[ 2 ];
		q->color[ 3 ] = color[ 3 ];
	}

	return q->quad_id;
}
void sl_scene_remove_sprite( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, j;
	sl_quad *it, *last_it;

	if( layer == 0xffffffff ) {
		for( i = 0; i < MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				j = 0;
				vul_foreach( sl_quad, it, last_it, scene->layers[ i ] ) {
					if( it->quad_id == id ) {
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
		assert( layer < MAX_LAYERS );
#endif
		if( scene->layers[ layer ] ) {
			j = 0;
			vul_foreach( sl_quad, it, last_it, scene->layers[ layer ] ) {
				if( it->quad_id == id ) {
					vul_vector_remove_cascade( scene->layers[ layer ], j ); 
					// After this operation, it is no longer stable, but we don't care, since
					// we return anyway!
					return;
				}
			}
		}
	}
}

sl_quad *sl_scene_get_volitile_quad( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, lret;
	sl_quad *ret;
	sl_quad *it, *last_it;

	ret = NULL;
	if( layer == 0xffffffff ) {
		for( i = 0; i < MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				vul_foreach( sl_quad, it, last_it, scene->layers[ i ] ) {
					if( it->quad_id == id ) {
						ret = it;
						lret = i;
					}
				}
			}
		}
	} else {
#ifdef SL_DEBUG
		assert( layer < MAX_LAYERS );
#endif
		lret = layer;
		if( scene->layers[ layer ] ) {
			vul_foreach( sl_quad, it, last_it, scene->layers[ layer ] ) {
				if( it->quad_id == id ) {
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

const sl_quad *sl_scene_get_const_quad( sl_scene *scene, const unsigned int id, const unsigned int layer )
{
	unsigned int i, lret;
	sl_quad *it, *last_it;
	
	if( layer == 0xffffffff ) {
		for( i = 0; i < MAX_LAYERS; ++i ) {
			if( scene->layers[ i ] ) {
				vul_foreach( sl_quad, it, last_it, scene->layers[ i ] ) {
					if( it->quad_id == id ) {
						return it;
					}
				}
			}
		}
	} else {
#ifdef SL_DEBUG
		assert( layer < MAX_LAYERS );
#endif
		lret = layer;
		if( scene->layers[ layer ] ) {
			vul_foreach( sl_quad, it, last_it, scene->layers[ layer ] ) {
				if( it->quad_id == id ) {
					return it;
				}
			}
		}
	}

	// We have failed, return NULL
	return NULL;
}

void sl_scene_get_quads_at_pos( vul_vector_t *vec, sl_scene *scene, sl_vec *pos )
{
	sl_quad *it, *last_it;
	int i;
	sl_box aabb;

	for( i = MAX_LAYERS - 1; i >= 0; --i ) {
		if( scene->layers[ i ] == NULL ) {
			continue;
		}
		vul_foreach( sl_quad, it, last_it, scene->layers[ i ] )
		{
			sl_quad_aabb( &aabb, it );
			if( sl_binside( &aabb, pos ) && !it->hidden ) {
				vul_vector_add( vec, &it->quad_id );
			}
		}
	}
}
