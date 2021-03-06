/**
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * The controller. Keeps callbacks based on action types, entity_ids that have focus and keys
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
#ifndef SLENDERER_CONTROLLER_H
#define SLENDERER_CONTROLLER_H

#include <stdio.h>

#include <vul_hash_map.h>
#include <vul_resizable_array.h>

#include "vul_cmath.h"
#include "renderer/scene.h"
#include "slenderer.h"


#define SL_TRUE 1
#define SL_FALSE 0

#define SL_CONTROLLER_UNIVERSAL 0xffffffff

#define SL_MOUSE_BUCKET_COUNT 64
#define SL_KEY_BUCKET_COUNT 32
#define SL_WINDOW_BUCKET_COUNT 4

#define SL_MOUSE_ENTER_EXIT_CALLBACK( name ) void (*name)( GLFWwindow *win_handle, int entered )
#define SL_MOUSE_DOWN_CALLBACK( name ) void (*name)( unsigned int scene_id, unsigned int entity_id, int button )
#define SL_MOUSE_UP_CALLBACK( name ) void (*name)( unsigned int scene_id, unsigned int entity_id, int button )
#define SL_MOUSE_OVER_CALLBACK( name ) void (*name)( unsigned int scene_id, unsigned int entity_id )
#define SL_MOUSE_OUT_CALLBACK( name ) void (*name)( unsigned int scene_id, unsigned int entity_id )
#define SL_KEY_PRESSED( name ) void (*name)( GLFWwindow *win_handle, int key, int scancode, int modifiers )
#define SL_KEY_RELEASED( name ) void (*name)( GLFWwindow *win_handle, int key, int scancode, int modifiers )
#define SL_KEY_REPEAT( name ) void (*name)( GLFWwindow *win_handle, int key, int scancode, int modifiers )


#define SL_MOUSE_ENTER_EXIT_PROTO void (*)( GLFWwindow*, int )
#define SL_MOUSE_DOWN_PROTO void (*)( unsigned int, unsigned int, int )
#define SL_MOUSE_UP_PROTO void (*)( unsigned int, unsigned int, int )
#define SL_MOUSE_OVER_PROTO void (*)( unsigned int, unsigned int )
#define SL_MOUSE_OUT_PROTO void (*)( unsigned int, unsigned int )
#define SL_KEY_PRESSED_PROTO void (*)( GLFWwindow*, int, int, int )
#define SL_KEY_RELEASED_PROTO void (*)( GLFWwindow*, int, int, int )
#define SL_KEY_REPEAT_PROTO void (*)( GLFWwindow*, int, int, int )

typedef struct {
	vul_hash_map *mouse_enter_exit_callbacks; // Hashmap of < GLFWwindow*, SL_MOUSE_ENTER_EXIT_CALLBACK >
	vul_hash_map *mouse_down_callbacks; // Hashmap of < entity_id, SL_MOUSE_DOWN_CALLBACK >
	vul_hash_map *mouse_up_callbacks; // Hashmap of < entity_id, SL_MOUSE_UP_CALLBACK >
	vul_hash_map *mouse_over_callbacks; // Hashmap of < entity_id, SL_MOUSE_OVER_CALLBACK >
	vul_hash_map *mouse_out_callbacks; // Hashmap of < entity_id, SL_MOUSE_OUT_CALLBACK >
	vul_hash_map *key_pressed_callbacks; // Hashmap of < key, SL_KEY_PRESSED >
	vul_hash_map *key_released_callbacks; // Hashmap of < key, SL_KEY_RELEASED >
	vul_hash_map *key_repeat_callbacks; // Hashmap of < key, SL_KEY_REPEAT >
	
	vul_list_element *hash_map_keys; // List of unsigned integers we use as keys in the hashmaps.
	vul_list_element *hash_map_key_pairs; // List of unsigned integers we use as keys in the hashmaps.
	vul_list_element *hash_map_ptr_keys; // List of pointers we use as keys in the hashmaps.

	v2 mouse_pos;
	v2 mouse_pos_prev;
	vul_vector *mouse_overs; // Vector of entity_id (unisgned integers) that the mouse is currently over.
} sl_controller;

/**
 * The global controller object.
 */
sl_controller *sl_controller_global;

/**
 * Creates a new controller. Must take a mouse move callback, although it may be NULL.
 */
void sl_controller_create( );

/**
 * Destroys a controller.
 */
void sl_controller_destroy( );

/**
 * Register the controller for the given window.
 */
void sl_controller_register_window( sl_window *win );

/**
 * Adds a mouse enter/exit callback.
 */
void sl_controller_add_mouse_enter_exit_callback( GLFWwindow* win_handle, SL_MOUSE_ENTER_EXIT_CALLBACK( callback ) );

/**
 * Adds a key down callback for the given key. Replaces any already registered callback for that key.
 */
void sl_controller_add_key_press_callback( int key, SL_KEY_PRESSED( callback ) );

/**
 * Adds a key up callback for the given key. Replaces any already registered callback for that key.
 */
void sl_controller_add_key_release_callback( int key, SL_KEY_RELEASED( callback ) );

/**
 * Adds a key up callback for the given key. Replaces any already registered callback for that key.
 */
void sl_controller_add_key_repeat_callback( int key, SL_KEY_REPEAT( callback ) );

/**
 * Adds a mouse down callback for the given entity. 
 * Replaces any already registered callback for that entity.
 * If the entity is SL_CONTROLLER_UNIVERSAL, it is registered as a universal 
 * callback for that button in that scene. If scene is SL_CONTROLLER_UNIVERSAL,
 * it is registered as universal for all scenes (for that entity id).
 */
void sl_controller_add_mouse_down_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_DOWN_CALLBACK( callback ) );

/**
 * Adds a mouse up callback for the given entity. 
 * Replaces any already registered callback for that entity.
 * If the entity is SL_CONTROLLER_UNIVERSAL, it is registered as a universal 
 * callback for that button in that scene. If scene is SL_CONTROLLER_UNIVERSAL,
 * it is registered as universal for all scenes (for that entity id).
 */
void sl_controller_add_mouse_up_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_UP_CALLBACK( callback ) );

/**
 * Adds a mouse over callback for the given entity. 
 * Replaces any already registered callback for that entity.
 */
void sl_controller_add_mouse_over_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_OVER_CALLBACK( callback ) );

/**
 * Adds a mouse out callback for the given entity. 
 * Replaces any already registered callback for that entity.
 */
void sl_controller_add_mouse_out_callback( unsigned int scene_id, unsigned int entity_id, SL_MOUSE_OUT_CALLBACK( callback ) );

/**
 * Hashes an integer key. Since we know our entity_ids are consecutive, unique ids,
 * and keys are also unique and somewhat uniformely distributed, we just use the
 * integers directly.
 */
u32 sl_controller_hash_func( const u8* data, u32 len );

/**
 * Compares two integer keys of a vul_hash_map_element
 */
int sl_controller_compare_func( void *a, void *b );

/**
* Compares two unsigned integers (used when comparing vul_list_element)
*/
int sl_controller_compare_func_raw( void *a, void *b );

/**
 * Hashes a key of two integers. We shift the first 16 bits left, then add the second.
 */
u32 sl_controller_hash_func_pair( const u8* data, u32 len );

/**
 * Compares two two-integer keys of a vul_hash_map_element
 */
int sl_controller_compare_func_pair( void *a, void *b );
/**
* Compares two two-uint keys (used when comparing vul_list_element)
*/
int sl_controller_compare_func_raw_pair( void *a, void *b );
/**
 * Hashes a pointer key. Since we know our entity_ids are consecutive, unique ids,
 * and keys are also unique and somewhat uniformely distributed, we just use the
 * integers directly.
 */
u32 sl_controller_hash_ptr_func( const u8* data, u32 len );

/**
 * Compares two pointer keys of a vul_hash_map_element
 */
int sl_controller_compare_ptr_func( void *a, void *b );

/**
* Compares two pointer keys (used when comparing vul_list_element)
*/
int sl_controller_compare_ptr_func_raw( void *a, void *b );

// Callback functions registered with glfw. These call the controller's equivalent functions.

void sl_controller_glfw_mouse_enter_exit_callback( GLFWwindow *win_handle, int state );

void sl_controller_glfw_mouse_pos_callback( GLFWwindow *win_handle, double x, double y );

void sl_controller_glfw_mouse_button_callback( GLFWwindow *win_handle, int button, int action, int mods );

void sl_controller_glfw_mouse_wheel_callback( GLFWwindow *win_handle, double x, double y); // @TODO: This doesn't do anything yet!

void sl_controller_glfw_key_callback( GLFWwindow *win_handle, int key, int scancode, int action, int mods );

void sl_controller_glfw_char_callback( GLFWwindow *win_handle, unsigned int unicode_char );

#endif
