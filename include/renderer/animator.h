/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * Animation manager. Contains a list of animations and their states. Updates
 * the scene / quads in the update function that should be called before rendering every frame.
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
#ifndef SLENDERER_ANIMATOR_H
#define SLENDERER_ANIMATOR_H

#include "math/mat.h"
#include "math/vec.h"
#include "renderer/scene.h"
#include "renderer/entity.h"

#ifndef SL_BOOL
	#define SL_BOOL int
	#define SL_TRUE 1
	#define SL_FALSE 0
#endif

typedef enum {
	SL_ANIMATION_TRANSFORM,
	SL_ANIMATION_SPRITE,
	SL_ANIMATION_EITHER
} sl_animation_type;

typedef enum {
	SL_ANIMATION_STOPPED,
	SL_ANIMATION_RUNNING,
	SL_ANIMATION_RUNNING_LOOPED,	// Wraps around the end
	SL_ANIMATION_RUNNING_PERIODIC,	// Goes 0->1->0->1->0...Back and forth infinitely.
	SL_ANIMATION_FINISHED,
	SL_ANIMATION_COUNT
} sl_animation_state;

typedef struct {
	unsigned int texture_id;
	sl_box uvs;
} sl_animation_sprite_state;

typedef struct {
	unsigned int animation_id;
	unsigned int entity_id;
	unsigned long long start_time;
	unsigned long long end_time;
	sl_mat4 start_world_mat;
	sl_mat4 end_world_mat;
	sl_animation_state state;
	// @TODO: Add interpolation-type enum, "linear", "quadratic_Accel", "quadratic_decel" etc.
} sl_animation_transform;

typedef struct {
	unsigned int animation_id;
	unsigned int entity_id;
	unsigned long long time_per_frame_in_ms;
	vul_vector_t *frames; // Vector of sl_animation_sprite_state
	sl_animation_state state;
	unsigned int frame_count; // == vul_vector_size( frames ), but stored for speed
	int current_frame; // Signed because looping...
	unsigned long long time_since_current_frame;
	SL_BOOL period_rising; // Increasing frame count if true, decreasing otheriwse
} sl_animation_sprite;


typedef struct {
	vul_vector_t *transforms; // Vector of sl_animation_transform
	vul_vector_t *sprites; // Vector of sl_animation_sprite

	unsigned int next_animation_id; // Unique ID to give next added animation

	vul_timer_t *clock; // Our timer
	unsigned long long last_time; // Last frame's time. We don't reset
								  // between frames because of interpolation
								  // of transforms.

	ui32_t scene_id;
} sl_animator;

/** 
 * Creates a new animator.
 */
void sl_animator_create( sl_animator *animator, sl_scene *scene );
/**
 * Destroys an animator.
 */
void sl_animator_destroy( sl_animator *animator );

/**
 * Add a transform. Returns the unique animation id.
 * State must be either SL_ANIMATION_RUNNING, SL_ANIMATION_RUNNING_LOOPED or 
 * SL_ANIMATION_RUNNING_PERIODIC.
 */
unsigned int sl_animator_add_transform( sl_animator *animator, unsigned int entity_id, const sl_mat4 *end_world_matrix, unsigned long long length_in_ms, sl_animation_state state );

/**
 * Adds a new sprite animation. Returns the unique animation id.
 * The vul_vector_t of frames is destroyed by the animator, and should be created by calling:
 *     vul_vector_create( sizeof( sl_animation_sprite_state ), initial_size );
 * State must be either SL_ANIMATION_RUNNING, SL_ANIMATION_RUNNING_LOOPED or 
 * SL_ANIMATION_RUNNING_PERIODIC.
 */
unsigned int sl_animator_add_sprite( sl_animator *animator, unsigned int entity_id, vul_vector_t *frames, unsigned long long ms_per_frame, sl_animation_state state );

/** 
 * Removes the animation of the given id.
 * Only the given type is considered for deletion (but if EITHER is supplied, obviously either are).
 * @NOTE: Any animation that reaches FINISHED state is removed automatically.
 */
void sl_animator_remove_animation( sl_animator *animator, unsigned int id, sl_animation_type type );

/**
 * Advances all animations, updates their state & removes finished ones.
 */
void sl_animator_update( sl_animator *animator );

#endif
