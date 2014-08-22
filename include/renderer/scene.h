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
#include <malloc.h>

#include <vul_resizable_array.h>
#include <vul_timer.h>
#include <vul_sort.h> // We need definitions for vul_sort only here

#include "math/box.h"
#include "math/mat.h"
#include "renderer/quad.h"
#include "renderer/texture.h"
#include "renderer/window.h"

#define MAX_LAYERS 16
#define LAYER_CHUNK_SIZE 8

typedef struct {
	vul_vector_t *layers[ MAX_LAYERS ]; // Vector of sl_quad. MAX_LAYERS arrays of quads, one for each layer.
	unsigned short layer_dirty; // Each bit indicates whether a layer must be re-sorted
	unsigned int next_quad_id; // ID of the next added quad.
	sl_window *window;
	unsigned int scene_id;
} sl_scene;

/**
 * Create a scene.
 */
void sl_scene_create( sl_scene *scene, sl_window *parent_window, unsigned int scene_id );
/**
 * Destroy a scene.
 */
void sl_scene_destroy( sl_scene *scene );

/**
 * Resorts all the layers' quads into the correct order for rendering if needed.
 */
void sl_scene_sort( sl_scene *scene );

/**
 * Adds a sprite with the given texture & uvs at the given location.
 * Returns the unique id of the quad.
 */
unsigned int sl_scene_add_sprite( sl_scene *scene, const unsigned int layer, const sl_vec *center, const sl_vec *scale, const float rotation, const unsigned int texture_id, const unsigned int program_id, const unsigned int renderable_id, const sl_box *uvs, const float color[ 4 ], unsigned char is_hidden );

/**
 * Removes the quad with the given id. If a layer == 0xffffffff, all layers are searched.
 */
void sl_scene_remove_sprite( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Returns a volitile pointer to the quad with the given id.
 * If a layer == 0xffffffff, all layers are searched.
 * The layer of the quad is marked as dirty.
 */
sl_quad *sl_scene_get_volitile_quad( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Returns a const reference to the quad with the given id.
 * If a layer == 0xffffffff, all layers are searched.
 */
const sl_quad *sl_scene_get_const_quad( sl_scene *scene, const unsigned int id, const unsigned int layer );

/**
 * Populates a vector of all quad ids that intersect a ray into the scene at the given position.
 * It searches top down, so the first hit is the topmost intersected quad.
 */
void sl_scene_get_quads_at_pos( vul_vector_t *vec, sl_scene *scene, sl_vec *pos );

#endif
