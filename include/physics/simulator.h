/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * Simple physics simulator.
 * Only collisssions for which a callback is registered are handled!
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
#ifndef SLENDERER_SIMULATOR_H
#define SLENDERER_SIMULATOR_H

#include <vul_resizable_array.h>
#include <vul_hash_map.h>
#include <vul_timer.h>

#include "math/vec.h"
#include "math/box.h"
#include "renderer/quad.h"
#include "renderer/scene.h"

#define SL_SIMULATOR_CALLBACK_BUCKETS 16

typedef struct sl_simulator_quad {
	const sl_quad *quad;
	sl_vec pos;
	sl_vec velocity;
	// @TODO: Mass!
	vul_vector_t *forces; // Vector of sl_vecs
} sl_simulator_quad;

typedef void ( *sl_simulator_collider_pair_callback )( sl_scene* s, sl_simulator_quad* a, sl_simulator_quad *b, double time_frame_delta );

typedef struct {
	unsigned int quad_id_a;
	unsigned int quad_id_b;
} sl_simulator_collider_pair;

typedef struct {
	vul_vector_t *quads; // Vector of sl_simulator_quads
	vul_hash_map_t *collision_callbacks; // Hashmap of < sl_simulator_collider_pair, sl_simulator_collider_pair_callback >.
	vul_list_element_t *collission_callback_keys; // List if sl_simulator_collider_pair
	ui32_t scene_id;
	vul_timer_t *clock;
	unsigned long long last_time;
} sl_simulator;

/**
 * Creates a new simulator for a given scene.
 */
void sl_simulator_create( sl_simulator *sim, sl_scene *scene );

/**
 * Destroys a simulator.
 */
void sl_simulator_destroy( sl_simulator *sim );

/**
 * Adds a quad with the given start velocity to the simulation.
 */
sl_simulator_quad *sl_simulator_add_quad( sl_simulator *sim, unsigned int quad_id, sl_vec *start_velocity );

/**
 * Adds a force on the given quad. The force is in "normalized screen coords per second per second".
 * Returns a pointer to the force vector so it can be changed. We don't support removal atm,
 * so just set it to (0,0) to "remove" it.
 */
sl_vec *sl_simulator_add_force( sl_simulator *sim, unsigned int quad_id, sl_vec *force );

/**
 * Adds an impulse to the given quad. The impulse is given as a raw change in velocity.
 */
void sl_simulator_add_impulse( sl_simulator *sim, unsigned int quad_id, sl_vec *impulse );

/**
 * Adds a collission callback for a pair of quads.
 * @NOTE: Order of quad ids is irrelevant; they are stored as a = min(a,b) 
 * and b = max(a,b) internally.
 */
void sl_simulator_add_callback( sl_simulator *sim, unsigned int quad_id_a, unsigned int quad_id_b, sl_simulator_collider_pair_callback callback );

/**
 * Updates the physics simulation:
 *		-Apply forces
 *		-Update positions
 *		-Move the actual rendering quads.
 *		-Run n2n collission checks
 *			-Call callbacks for the respective collission if registered
 * @NOTE: If no callback exists, collissions aren't handled. If the callback wants to update
 *        opsitions (and it should), it also needs to update the rendering quad!
 */
void sl_simulator_update( sl_simulator *sim );

/**
 * Hash function for the collission callback hash map.
 */
ui32_t sl_simulator_callback_hash( const ui8_t *key, ui32_t keylen );

/**
 * Comparison function for the collission callback hash map.
 */
int sl_simulator_callback_comp( void *a, void *b );

/**
 * Simple callback for quad vs. quad collissions.
 * Find the point of intersection in time, reflects the velocity
 * and advances by the distance of intersection in the new direction.
 */
void sl_simulator_callback_quad_quad( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta );

/**
 * Simple callback for quad vs. sphere collissions.
 * Find the point of intersection in time, reflects the velocity
 * and advances by the distance of intersection in the new direction.
 */
void sl_simulator_callback_quad_sphere( sl_scene *scene, sl_simulator_quad *quad, sl_simulator_quad *sphere, double time_frame_delta );

/**
 * Simple callback for sphere vs. sphere collissions.
 * Find the point of intersection in time, reflects the velocity
 * and advances by the distance of intersection in the new direction.
 */
void sl_simulator_callback_sphere_sphere( sl_scene *scene, sl_simulator_quad *quad, sl_simulator_quad *sphere, double time_frame_delta );

#endif