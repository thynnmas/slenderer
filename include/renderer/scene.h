/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * A scene of maximum 16 layers.
 * Layers are rendered back to front, naturally. No guarantees are made
 * for order within a layer.
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
#ifndef SLENDERER_SCENE_H
#define SLENDERER_SCENE_H
#ifdef VUL_OSX
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <vul_resizable_array.h>
#include <vul_timer.h>
#include <vul_sort.h> // We need definitions for vul_sort only here
#include <vul_cmath.h>

#include "math/box.h"
#include "renderer/entity.h"
#include "renderer/renderable.h"
#include "renderer/texture.h"
#include "renderer/window.h"

#define SL_MAX_LAYERS 16
#define SL_LAYER_CHUNK_SIZE 8
// Texture ID indicating that this is a textureless quad.
#define SL_INVISIBLE_TEXTURE 0xffffffff

typedef struct {
	vul_vector *layers[ SL_MAX_LAYERS ]; // Vector of sl_entity. MAX_LAYERS arrays of entities, one for each layer.
	unsigned short layer_dirty; // Each bit indicates whether a layer must be re-sorted
	unsigned int next_entity_id; // ID of the next added entity.
	u32 window_id;
	unsigned int scene_id;
	u32 post_program_id; // Post processing program; by default the normal shader!
	void (*post_program_callback)( sl_program *post_program ); // Called last second before tendering the post process quad; use to set parameters for the program.
	sl_renderable post_renderable;
	v2 camera_pos;
} sl_scene;

/**
 * Create a scene.
 */
void sl_scene_create( sl_scene *scene, u32 parent_window_id, unsigned int scene_id, u32 post_program_id );
/**
 * Destroy a scene.
 */
void sl_scene_destroy( sl_scene *scene );

/**
 * Resorts all the layers' quads into the correct order for rendering if needed.
 */
void sl_scene_sort( sl_scene *scene );

/**
 * Sets the post processing program for the scene.
 */
void sl_scene_set_post( sl_scene *scene, sl_program *prog, void (*post_program_callback)( sl_program *post_program ) );

/**
 * Adds a sprite with the given texture & uvs at the given location.
 * Returns the unique id of the quad.
 */
unsigned int sl_scene_add_sprite( sl_scene *scene, const unsigned int layer, 
								  const v2 *center, const v2 *scale,
								  const float rotation, const unsigned int texture_id,
								  const unsigned int program_id, const unsigned int renderable_id,
								  const sl_box *uvs, const v2 *flip_uvs,
								  const float color[ 4 ], unsigned char is_hidden );

/**
 * Removes the quad with the given id. If a layer == 0xffffffff, all layers are searched.
 */
void sl_scene_remove_sprite( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Returns a volitile pointer to the quad with the given id.
 * If a layer == 0xffffffff, all layers are searched.
 * The layer of the quad is marked as dirty.
 */
sl_entity *sl_scene_get_volitile_entity( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Returns a const reference to the quad with the given id.
 * If a layer == 0xffffffff, all layers are searched.
 */
const sl_entity *sl_scene_get_const_entity( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Populates a vector of all quad ids that intersect a ray into the scene at the given position.
 * It searches top down, so the first hit is the topmost intersected quad.
 */
void sl_scene_get_entities_at_pos( vul_vector *vec, sl_scene *scene, v2 *pos );

#endif
