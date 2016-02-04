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
#include <vul_hash_map.h>
#undef VUL_DEFINE
#include "input/controller.h"

sl_controller *sl_controller_global = NULL;

void sl_controller_create( )
{
	sl_controller_global = ( sl_controller* )SL_ALLOC( sizeof( sl_controller ) );

	sl_controller_global->mouse_enter_exit_callbacks = vul_map_create( SL_WINDOW_BUCKET_COUNT, sl_controller_hash_ptr_func, sl_controller_compare_ptr_func, malloc, free );
	sl_controller_global->mouse_down_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func_pair, sl_controller_compare_func_pair, malloc, free );
	sl_controller_global->mouse_up_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func_pair, sl_controller_compare_func_pair, malloc, free );
	sl_controller_global->mouse_over_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func_pair, sl_controller_compare_func_pair, malloc, free );
	sl_controller_global->mouse_out_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func_pair, sl_controller_compare_func_pair, malloc, free );
	sl_controller_global->key_pressed_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func, sl_controller_compare_func, malloc, free );
	sl_controller_global->key_released_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func, sl_controller_compare_func, malloc, free );
	sl_controller_global->key_repeat_callbacks = vul_map_create( SL_MOUSE_BUCKET_COUNT, sl_controller_hash_func, sl_controller_compare_func, malloc, free );

	sl_controller_global->hash_map_keys = NULL;
	sl_controller_global->hash_map_key_pairs = NULL;
	sl_controller_global->hash_map_ptr_keys = NULL;

	sl_controller_global->mouse_pos = vec2( 0.0f, 0.0f );
	sl_controller_global->mouse_pos_prev = vec2( 0.0f, 0.0f );
	sl_controller_global->mouse_overs = vul_vector_create( sizeof( unsigned int ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
}

void sl_controller_destroy( )
{
	if( sl_controller_global == NULL ) {
		return;
	}
	vul_map_destroy( sl_controller_global->mouse_down_callbacks );
	vul_map_destroy( sl_controller_global->mouse_up_callbacks );
	vul_map_destroy( sl_controller_global->mouse_over_callbacks );
	vul_map_destroy( sl_controller_global->mouse_out_callbacks );
	vul_map_destroy( sl_controller_global->key_pressed_callbacks );
	vul_map_destroy( sl_controller_global->key_released_callbacks );
	vul_map_destroy( sl_controller_global->key_repeat_callbacks );

	vul_list_destroy( sl_controller_global->hash_map_keys, free );
	vul_list_destroy( sl_controller_global->hash_map_key_pairs, free );
	vul_list_destroy( sl_controller_global->hash_map_ptr_keys, free );

	vul_vector_destroy( sl_controller_global->mouse_overs );

	SL_DEALLOC( sl_controller_global );
}

void sl_controller_add_mouse_enter_exit_callback( GLFWwindow* win_handle, SL_MOUSE_ENTER_EXIT_CALLBACK( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	e = vul_map_get( sl_controller_global->mouse_enter_exit_callbacks, ( u8* )&win_handle, sizeof( GLFWwindow* ) );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_ptr_keys, &win_handle, sizeof( GLFWwindow* ), sl_controller_compare_ptr_func_raw, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( win_handle );

		// Insert it
		vul_map_insert( sl_controller_global->mouse_enter_exit_callbacks, &element ); 
	}
}

void sl_controller_register_window( sl_window *win )
{
	glfwSetKeyCallback( win->handle, sl_controller_glfw_key_callback );
	glfwSetMouseButtonCallback( win->handle, sl_controller_glfw_mouse_button_callback );
	glfwSetScrollCallback( win->handle, sl_controller_glfw_mouse_wheel_callback );
	glfwSetCursorEnterCallback( win->handle, sl_controller_glfw_mouse_enter_exit_callback );
	glfwSetCursorPosCallback( win->handle, sl_controller_glfw_mouse_pos_callback );
	glfwSetCharCallback( win->handle, sl_controller_glfw_char_callback );
}

void sl_controller_add_key_press_callback( int key, SL_KEY_PRESSED( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	e = vul_map_get( sl_controller_global->key_pressed_callbacks, ( u8* )&key, sizeof( int ) );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_keys, &key, sizeof( key ), sl_controller_compare_func_raw, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( key );

		// Insert it
		vul_map_insert( sl_controller_global->key_pressed_callbacks, &element ); 
	}
}

void sl_controller_add_key_release_callback( int key, SL_KEY_RELEASED( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	e = vul_map_get( sl_controller_global->key_released_callbacks, ( u8* )&key, sizeof( int ) );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_keys, &key, sizeof( key ), sl_controller_compare_func_raw, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( key );

		// Insert it
		vul_map_insert( sl_controller_global->key_released_callbacks, &element ); 
	}
}

void sl_controller_add_key_repeat_callback( int key, SL_KEY_REPEAT( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	e = vul_map_get( sl_controller_global->key_repeat_callbacks, ( u8* )&key, sizeof( int ) );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_keys, &key, sizeof( key ), sl_controller_compare_func_raw, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( key );

		// Insert it
		vul_map_insert( sl_controller_global->key_repeat_callbacks, &element ); 
	}
}

void sl_controller_add_mouse_down_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_DOWN_CALLBACK( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;
	unsigned int pair[ 2 ];

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	pair[ 0 ] = scene_id;
	pair[ 1 ] = entity_id;
	e = vul_map_get( sl_controller_global->mouse_down_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_key_pairs, pair, sizeof( unsigned int ) * 2, sl_controller_compare_func_raw_pair, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( unsigned int ) * 2;

		// Insert it
		vul_map_insert( sl_controller_global->mouse_down_callbacks, &element ); 
	}
}

void sl_controller_add_mouse_up_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_UP_CALLBACK( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;
	unsigned int pair[ 2 ];

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	pair[ 0 ] = scene_id;
	pair[ 1 ] = entity_id;
	e = vul_map_get( sl_controller_global->mouse_up_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_key_pairs, pair, sizeof( unsigned int ) * 2, sl_controller_compare_func_raw_pair, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( unsigned int ) * 2;

		// Insert it
		vul_map_insert( sl_controller_global->mouse_up_callbacks, &element ); 
	}
}

void sl_controller_add_mouse_over_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_OVER_CALLBACK( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;
	unsigned int pair[ 2 ];

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	pair[ 0 ] = scene_id;
	pair[ 1 ] = entity_id;
	e = vul_map_get( sl_controller_global->mouse_over_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_key_pairs, pair, sizeof( unsigned int ) * 2, sl_controller_compare_func_raw_pair, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( unsigned int ) * 2;

		// Insert it
		vul_map_insert( sl_controller_global->mouse_over_callbacks, &element ); 
	}
}

void sl_controller_add_mouse_out_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_OUT_CALLBACK( callback ) )
{
	vul_hash_map_element *e, element;
	vul_list_element *le;
	unsigned int pair[ 2 ];

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	// Check if this key is already in our map
	pair[ 0 ] = scene_id;
	pair[ 1 ] = entity_id;
	e = vul_map_get( sl_controller_global->mouse_out_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
	if( e != NULL ) {
		e->data = callback;
	} else {
		// Store our key
		le = vul_list_insert( sl_controller_global->hash_map_key_pairs, pair, sizeof( unsigned int ) * 2, sl_controller_compare_func_raw_pair, malloc );

		// Create the element
		element.data = callback;
		element.data_size = sizeof( callback );
		element.key = ( u8* )le->data;
		element.key_size = sizeof( unsigned int ) * 2;

		// Insert it
		vul_map_insert( sl_controller_global->mouse_out_callbacks, &element ); 
	}
}

u32 sl_controller_hash_func( const u8* data, u32 len )
{
#ifdef SL_DEBUG
	assert( len == sizeof( unsigned int ) );
#endif

	return *( u32* )data;
}

u32 sl_controller_hash_func_pair( const u8* data, u32 len )
{
#ifdef SL_DEBUG
	assert( len == sizeof( unsigned int ) * 2 );
#endif

	return ( ( ( u32* )data )[ 0 ] << 16 ) + ( ( u32* )data )[ 1 ];
}

int sl_controller_compare_func_raw( void *a, void *b )
{
	u32 *ea, *eb;

	ea = ( u32* )a;
	eb = ( u32* )b;

	return ( s32 )( ( s64 )ea - ( s64 )eb );
}

int sl_controller_compare_func_raw_pair( void *a, void *b )
{
	u32 *ea, *eb;
	int d;

	ea = ( u32* )a;
	eb = ( u32* )b;

	d = ( s32 )( ( s64 )ea[ 0 ] - ( s64 )eb[ 0 ] );
	if( d != 0 ) {
		return d;
	}

	return ( s32 )( ( s64 )ea[ 1 ] - ( s64 )eb[ 1 ] );
}

int sl_controller_compare_func( void *a, void *b )
{
	vul_hash_map_element *ea, *eb;

	ea = ( vul_hash_map_element* )a;
	eb = ( vul_hash_map_element* )b;

	return ( s32 )( ( s64 )*( u32* )ea->key - ( s64 )*( u32* )eb->key );
}

int sl_controller_compare_func_pair( void *a, void *b )
{
	vul_hash_map_element *ea, *eb;
	int d;

	ea = ( vul_hash_map_element* )a;
	eb = ( vul_hash_map_element* )b;

	d = ( s32 )( ( s64 )( ( u32* )ea->key )[ 0 ] - ( s64 )( ( u32* )eb->key )[ 0 ] );
	if( d != 0 ) {
		return d;
	}

	return ( s32 )( ( s64 )( ( u32* )ea->key )[ 1 ] - ( s64 )( ( u32* )eb->key )[ 1 ] );
}

u32 sl_controller_hash_ptr_func( const u8* data, u32 len )
{
#ifndef SL_DEBUG
	u32 i, r;
#endif

	if( len == 4 ) {
		return *( u32* )( data );
	} else if ( len == 8 ) {
		return ( u32 )( *( u64* )( data ) );
	}
#ifdef SL_DEBUG
	assert( 0 ); return 0;
#else
	sl_print( 256, "Some really weird pointer size encountered: %d bytes", len );
	r = 0;
	for( i = 0; i < len; ++i ) {
		r += ( u32 )data[ i ];
	}
	return r;
#endif
}

int sl_controller_compare_ptr_func( void *a, void *b )
{
	vul_hash_map_element *ea, *eb;
	u32 ai, bi;
	u64 al, bl;

	ea = ( vul_hash_map_element* )a;
	eb = ( vul_hash_map_element* )b;

	if( sizeof( GLFWwindow* ) == 4 ) {
		ai = *( u32* )ea->key;
		bi = *( u32* )eb->key;
		return ( int )( ( long long )ai - ( long long )bi );
	} else if ( sizeof( GLFWwindow* ) == 8 ) {
		al = *( u64* )ea->key;
		bl = *( u64* )eb->key;
		return ( int )( ( s64 )al - ( s64 )bl );
	}
#ifdef SL_DEBUG
	assert( 0 );
#else
	sl_print( 256, "Some really weird pointer size encountered: %d bytes", sizeof( GLFWwindow* ) );
	return *( u8* )a == *( u8* )b ? 0 : -1; // Don't care about order here
#endif
}

int sl_controller_compare_ptr_func_raw( void *a, void *b )
{
	u32 ai, bi;
	u64 al, bl;
	
	if( sizeof( GLFWwindow* ) == 4 ) {
		ai = *( u32* )a;
		bi = *( u32* )b;
		return ( int )( ( long long )ai - ( long long )bi );
	} else if( sizeof( GLFWwindow* ) == 8 ) {
		al = *( u64* )a;
		bl = *( u64* )b;
		return ( int )( ( s64 )al - ( s64 )bl );
	}
#ifdef SL_DEBUG
	assert( 0 );
#else
	sl_print( 256, "Some really weird pointer size encountered: %d bytes", sizeof( GLFWwindow* ) );
	return *( u8* )a == *( u8* )b ? 0 : -1; // Don't care about order here
#endif
}

// Callback functions registered with glfw. These call the controller's equivalent functions.
void sl_controller_glfw_mouse_enter_exit_callback( GLFWwindow *win_handle, int state )
{
	const vul_hash_map_element *e;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif
	
	e = vul_map_get_const( sl_controller_global->mouse_enter_exit_callbacks, ( u8 * )&win_handle,sizeof( GLFWwindow* ) );

	if( e != NULL ) {
		( ( SL_MOUSE_ENTER_EXIT_PROTO )e->data )( win_handle, state == GL_TRUE ? SL_TRUE : SL_FALSE );
	}
}

void sl_controller_glfw_mouse_pos_callback( GLFWwindow *win_handle, double x, double y )
{
	sl_scene **its, **last_its, **its2, **last_its2;
	vul_vector *over, *scenes;
	unsigned int *it, *last_it, *it2, *last_it2, i, pair[ 2 ];
	int found, deleted;
	vul_hash_map_element *e;
	int ww, wh;
	v2 scene_local_pos;

	// Update mouse position
	glfwGetWindowSize( win_handle, &ww, &wh );
	sl_controller_global->mouse_pos_prev.x = sl_controller_global->mouse_pos.x;
	sl_controller_global->mouse_pos_prev.y = sl_controller_global->mouse_pos.y;
	sl_controller_global->mouse_pos = vec2( ( ( float )x / ( float )ww ) * 2.f - 1.f,
										   -( ( float )y / ( float )wh ) * 2.f + 1.f );
	
	// Get the scene
	scenes = vul_vector_create( sizeof( sl_scene* ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	sl_renderer_get_scenes_by_window_handle( scenes, win_handle );

	vul_foreach( sl_scene*, its, last_its, scenes )
	{
		// Calculate scene local position
		scene_local_pos = vadd2( sl_controller_global->mouse_pos, ( *its )->camera_pos );
		// Get all entities we are hovering over
		over = vul_vector_create( sizeof( unsigned int ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
		sl_scene_get_entities_at_pos( over, *its, &scene_local_pos );

		// Prune out all entities we were over but aren't any more. Slow!
		while( 1 ) {
			deleted = 0;			
			i = 0;
			vul_foreach( unsigned int, it, last_it, sl_controller_global->mouse_overs )
			{
				found = 0;
				vul_foreach( unsigned int, it2, last_it2, over )
				{
					if( *it2 == *it ) {
						found = 1;
						break;
					}
				}
				if( !found ) {
					// Call mouse out for all scenes.
					vul_foreach( sl_scene*, its2, last_its2, scenes )
					{
						// Play mouse out iteration!
						pair[ 0 ] = ( *its2 )->scene_id;
						pair[ 1 ] = *it;
						e = vul_map_get( sl_controller_global->mouse_out_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
						if( e != NULL ) {
							( ( SL_MOUSE_OUT_PROTO )e->data )( ( *its2 )->scene_id, *it );
						}
					}
					// Remove it
					vul_vector_remove_swap( sl_controller_global->mouse_overs, i );
					deleted = 1;
					break;
				}
				++i;
			}
			if( !deleted ) {
				break;
			}
		}

		// Add new entities
		vul_foreach( unsigned int, it, last_it, over )
		{
			found = 0;
			i = 0;
			vul_foreach( unsigned int, it2, last_it2, sl_controller_global->mouse_overs )
			{
				if( *it2 == *it ) {
					found = 1;
					break;
				}
				++i;
			}
			if( !found ) {
				vul_vector_add( sl_controller_global->mouse_overs, it );
			}
		}

		// Iterate over them
		vul_foreach( unsigned int, it, last_it, over )
		{
			pair[ 0 ] = ( *its )->scene_id;
			pair[ 1 ] = *it;
			e = vul_map_get( sl_controller_global->mouse_over_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
			if( e != NULL ) {
				( ( SL_MOUSE_OVER_PROTO )e->data )( ( *its )->scene_id, *it );
			}
		}
	}
}

void sl_controller_glfw_mouse_button_callback( GLFWwindow *win_handle, int button, int action, int mods )
{
	const vul_hash_map_element *e;
	sl_scene **its, **last_its;
	vul_vector *over, *scenes;
	unsigned int *it, *last_it, pair[ 2 ];
	v2 scene_local_pos;

#ifdef SL_DEBUG
	assert( sl_controller_global != NULL );
#endif

	scenes = vul_vector_create( sizeof( sl_scene* ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
	sl_renderer_get_scenes_by_window_handle( scenes, win_handle );

	vul_foreach( sl_scene*, its, last_its, scenes )
	{
		// Calculate scene local position
		scene_local_pos = vadd2( sl_controller_global->mouse_pos, ( *its )->camera_pos );
		// Get entities we overlap
		over = vul_vector_create( sizeof( unsigned int ), 0, SL_ALLOC, SL_DEALLOC, SL_REALLOC );
		sl_scene_get_entities_at_pos( over, *its, &scene_local_pos );
		
		vul_foreach( unsigned int, it, last_it, over )
		{
			pair[ 0 ] = ( *its )->scene_id;
			pair[ 1 ] = *it;
			if( action == GLFW_PRESS ) {
				e = vul_map_get_const( sl_controller_global->mouse_down_callbacks, ( u8 * )pair, sizeof( unsigned int ) * 2 );
				if( e != NULL ) {
					( ( SL_MOUSE_DOWN_PROTO )e->data )( ( *its )->scene_id, *it, button );
				}
			} else if( action == GLFW_RELEASE ) {
				e = vul_map_get_const( sl_controller_global->mouse_up_callbacks, ( u8 * )pair, sizeof( unsigned int ) * 2 );
				if( e != NULL ) {
					( ( SL_MOUSE_UP_PROTO )e->data )( ( *its )->scene_id, *it, button );
				}
			}
		}

		// Finally, dispatch the global callback, if it exist
		pair[ 0 ] = ( *its )->scene_id;
		pair[ 1 ] = SL_CONTROLLER_UNIVERSAL;
		if( action == GLFW_PRESS ) {
			e = vul_map_get( sl_controller_global->mouse_down_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
			if( e != NULL ) {
				( ( SL_MOUSE_DOWN_PROTO )e->data )( ( *its )->scene_id, SL_CONTROLLER_UNIVERSAL, button );
			}
		} else if( action == GLFW_RELEASE ) {
			e = vul_map_get( sl_controller_global->mouse_up_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
			if( e != NULL ) {
				( ( SL_MOUSE_UP_PROTO )e->data )( ( *its )->scene_id, SL_CONTROLLER_UNIVERSAL, button );
			}
		}
	}


	// Finally, dispatch the global callback, if it exist
	pair[ 0 ] = SL_CONTROLLER_UNIVERSAL;
	pair[ 1 ] = SL_CONTROLLER_UNIVERSAL;
	if( action == GLFW_PRESS ) {
		e = vul_map_get( sl_controller_global->mouse_down_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
		if( e != NULL ) {
			( ( SL_MOUSE_DOWN_PROTO )e->data )( SL_CONTROLLER_UNIVERSAL, SL_CONTROLLER_UNIVERSAL, button );
		}
	} else if( action == GLFW_RELEASE ) {
		e = vul_map_get( sl_controller_global->mouse_up_callbacks, ( u8* )pair, sizeof( unsigned int ) * 2 );
		if( e != NULL ) {
			( ( SL_MOUSE_UP_PROTO )e->data )( SL_CONTROLLER_UNIVERSAL, SL_CONTROLLER_UNIVERSAL, button );
		}
	}
	// @TODO: might want to add proper click-handling, so press and release on same button = click instead of realease = click where we release it.
}

void sl_controller_glfw_mouse_wheel_callback( GLFWwindow *win_handle, double x, double y )
{
	// @TODO: Scroll support
}

void sl_controller_glfw_key_callback( GLFWwindow *win_handle, int key, int scancode, int action, int mods )
{
	vul_hash_map_element *e;

	if( action == GLFW_PRESS ) {
		e = vul_map_get( sl_controller_global->key_pressed_callbacks, ( u8* )&key, sizeof( int ) );
		if( e != NULL ) {
			( ( SL_KEY_PRESSED_PROTO )e->data )( win_handle, key, scancode, mods );
		}
	} else if( action == GLFW_RELEASE ) {
		e = vul_map_get( sl_controller_global->key_released_callbacks, ( u8* )&key, sizeof( int ) );
		if( e != NULL ) {
			( ( SL_KEY_RELEASED_PROTO )e->data )( win_handle, key, scancode, mods );
		}
	} else if( action == GLFW_REPEAT ) {
		e = vul_map_get( sl_controller_global->key_repeat_callbacks, ( u8* )&key, sizeof( int ) );
		if( e != NULL ) {
			( ( SL_KEY_REPEAT_PROTO )e->data )( win_handle, key, scancode, mods );
		}
	} // No other options
}

void sl_controller_glfw_char_callback( GLFWwindow *win_handle, unsigned int unicode_char )
{
	// @TODO: Character callback
}
