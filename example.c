/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains an example project to demonstrate usage of the renderer.
 * It makes use of the simulator-component, and has plans for making use of the
 * animator as well, but is not yet complete.
 * 
 * @NOTE: Since this is rushed to be out pre-LD30, it dooesn't work. This may
 * be because of the engine, but it's as likely that it's bugs within this example.
 * @KNOWN_BUGS:
 * -Headers trigger scores immediatel
 * -jumping does not work as intended; suspect that we recollide before we adjust,
 * stopping the simulation/getting stuck inside things. 
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

#define VUL_DEBUG
#define VUL_VECTOR_C89_ITERATORS

#include <vul_timer.h>
#include "slenderer.h"
#define STBI_HEADER_FILE_ONLY
#include "stb_image.h"

#define SLE_LAYER_BACKGROUND 0
#define SLE_LAYER_WATER 1
#define SLE_LAYER_WEATHER 2
#define SLE_LAYER_BEACH 3
#define SLE_LAYER_GROUND_EFECTS 4
#define SLE_LAYER_NET 5
#define SLE_LAYER_BALL 6
#define SLE_LAYER_PLAYERS 7
#define SLE_LAYER_UI 8

#define SLE_SPRITE_VISIBLE 0
#define SLE_SPRITE_HIDE 1

const float SLE_JUMP_MOMENTUM = 8.4f; // 1 screen height per second.
const float SLE_MOVE_SPEED = 0.5f; // 1/4 screen width per second, or half your half of the pitch.
const float SLE_GRAVITY = -3.0f; // 1.6 screen heights per second^2; @TODO: this needs tweaking; primary gameplay factor!

const float SLE_FLOOR = -0.95f;

const sl_vec SLE_BALL_SCALE = { 0.1f, 0.1f };
const sl_vec SLE_PLAYER_SCALE = { 0.1f, 0.1f };

const sl_vec SLE_PLAYER_1_START_POSITION = { -0.5f, -0.8f };
const sl_vec SLE_PLAYER_2_START_POSITION = {  0.5f, -0.8f };
const sl_vec SLE_BALL_START_POSITION_1 = { -0.65f, 0.25f };
const sl_vec SLE_BALL_START_POSITION_2 = { -0.65f, 0.25f };

// Quad ids of the movable parts
unsigned int sle_ball_id, sle_player1_id, sle_player2_id;
// Physics objects
sl_simulator_quad *sle_player1_pquad, *sle_player2_pquad, *sle_ball_pquad;
// Scores
unsigned int sle_player1_score, sle_player2_score;
// Number of headers since other player headered.
unsigned int sle_player1_header_count, sle_player2_header_count;
// Whether the ball is frozen or affected by gravity.
int sle_freeze_ball;

int sle_load_image_to_texture( sl_texture *tex_out, const char *path_to_image )
{
	unsigned char *data;
	int width, height, component_count;
	
	data = stbi_load( path_to_image, &width, &height, &component_count, 4 );

	if( data != NULL )
	{
		sl_texture_create( tex_out, data, width, height, GL_RGBA, GL_RGBA8 );
		return 0;
	} else {
		return 1;
	}
}

void sle_move_player1( GLFWwindow *win_handle, int key, int scancode, int modifiers )
{
	float eps;

	// Prevent unused parameter warningsy
	(void)win_handle;
	(void)scancode;
	(void)modifiers;

	eps = 0.01f;
	if( key == GLFW_KEY_W ) {
		if( sle_player1_pquad->pos.y < SLE_FLOOR + SLE_PLAYER_SCALE.y + eps ) {
			// Can only jump from ground!
			sle_player1_pquad->velocity.y = SLE_JUMP_MOMENTUM;
		}
	} else if( key == GLFW_KEY_A ) {
		sle_player1_pquad->velocity.x = -SLE_MOVE_SPEED;
	} else if( key == GLFW_KEY_D ) {
		sle_player1_pquad->velocity.x = SLE_MOVE_SPEED;
	}
	sle_freeze_ball = SL_FALSE;
}

void sle_stop_player1( GLFWwindow *win_handle, int key, int scancode, int modifiers )
{

	// Prevent unused parameter warningsy
	(void)win_handle;
	(void)scancode;
	(void)modifiers;
	(void)key;

	sle_player1_pquad->velocity.x = 0.0f;
}

void sle_move_player2( GLFWwindow *win_handle, int key, int scancode, int modifiers )
{
	float eps;

	// Prevent unused parameter warningsy
	(void)win_handle;
	(void)scancode;
	(void)modifiers;

	eps = 0.01f;
	if( key == GLFW_KEY_UP ) {
		if( sle_player2_pquad->pos.y < SLE_FLOOR + ( SLE_PLAYER_SCALE.y * 0.5f ) + eps ) {
			// Can only jump from ground!
			sle_player2_pquad->velocity.y = SLE_JUMP_MOMENTUM;
		}
	} else if( key == GLFW_KEY_LEFT ) {
		sle_player2_pquad->velocity.x = -SLE_MOVE_SPEED;
	} else if( key == GLFW_KEY_RIGHT ) {
		sle_player2_pquad->velocity.x = SLE_MOVE_SPEED;
	}
	sle_freeze_ball = SL_FALSE;
}

void sle_stop_player2( GLFWwindow *win_handle, int key, int scancode, int modifiers )
{
	// Prevent unused parameter warningsy
	(void)win_handle;
	(void)scancode;
	(void)modifiers;
	(void)key;

	sle_player2_pquad->velocity.x = 0.0f;
}

void sle_score( sl_scene *scene, int player )
{
	sl_quad *p1_quad, *p2_quad, *ball_quad;

	// Update scores
	if( player == 1 ) {
		sle_player1_score++;
	} else if ( player == 2 ) {
		sle_player2_score++;
	} else {
#ifdef SL_DEBUG
		assert( 0 );
#else
		fprintf( stderr, "Unknoiwn player just scored.\n" );
		exit( EXIT_FAILURE );
#endif
	}
	// Reset players and ball
	p1_quad = sl_scene_get_volitile_quad( scene, sle_player1_id, SLE_LAYER_PLAYERS );
	p2_quad = sl_scene_get_volitile_quad( scene, sle_player2_id, SLE_LAYER_PLAYERS );
	ball_quad = sl_scene_get_volitile_quad( scene, sle_ball_id, SLE_LAYER_BALL );

	sl_quad_create_world_matrix( p1_quad, &SLE_PLAYER_1_START_POSITION, &SLE_PLAYER_SCALE, 0.0f );
	sl_quad_create_world_matrix( p2_quad, &SLE_PLAYER_2_START_POSITION, &SLE_PLAYER_SCALE, 0.0f );
	if( player == 1 ) {
		sl_quad_create_world_matrix( ball_quad, &SLE_BALL_START_POSITION_2, &SLE_BALL_SCALE, 0.0f );
		sl_vset( &sle_ball_pquad->pos, SLE_BALL_START_POSITION_2.x, SLE_BALL_START_POSITION_2.y );
	} else {
		sl_quad_create_world_matrix( ball_quad, &SLE_BALL_START_POSITION_1, &SLE_BALL_SCALE, 0.0f );
		sl_vset( &sle_ball_pquad->pos, SLE_BALL_START_POSITION_1.x, SLE_BALL_START_POSITION_1.y );
	}
	
	// Reset momentums
	sl_vset( &sle_player1_pquad->velocity, 0.0f, 0.0f );
	sl_vset( &sle_player2_pquad->velocity, 0.0f, 0.0f );
	sl_vset( &sle_ball_pquad->velocity, 0.0f, 0.0f );

	// Reset physics positions
	sl_vset( &sle_player1_pquad->pos, SLE_PLAYER_1_START_POSITION.x, SLE_PLAYER_1_START_POSITION.y );
	sl_vset( &sle_player2_pquad->pos, SLE_PLAYER_2_START_POSITION.x, SLE_PLAYER_2_START_POSITION.y );

	sle_freeze_ball = SL_TRUE;

	// Reset header count
	sle_player1_header_count = 0;
	sle_player2_header_count = 0;
}

void sle_quit( GLFWwindow *win_handle, int key, int scancode, int modifiers )
{
	// Prevent unused parameter warningsy
	(void)win_handle;
	(void)scancode;
	(void)modifiers;
	(void)key;

	glfwSetWindowShouldClose( win_handle, GL_TRUE );
}

void sle_ball_dropped( sl_scene *scene, sl_simulator_quad *quad, sl_simulator_quad *sphere, double time_frame_delta )
{
	(void)quad;
	(void)time_frame_delta;

	// Since we know the ground is flat and the sphere reaches the bottom of the quad containing it,
	// we know this is a collission. Up the score and reset.
	// Left half of the screen is player 1's court, so 2 scored. Screen coords are normalized.
	if( sphere->pos.x < 0.0f ) {
		sle_score( scene, 2 );
	} else {
		sle_score( scene, 1 );
	}
}

void sle_header( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	// Just call sphere, sphere callback.
	sl_simulator_callback_sphere_sphere( scene, a, b, time_frame_delta );
}

void sle_player1_header( sl_scene *scene, sl_simulator_quad *player, sl_simulator_quad *ball, double time_frame_delta )
{
	sle_player2_header_count = 0;
	if( ++sle_player1_header_count > 3 ) {
		// Too many headers, score for the other dude.
		sle_score( scene, 2 );
	}
	// Then handle the header
	sle_header( scene, player, ball, time_frame_delta );
}

void sle_player2_header( sl_scene *scene, sl_simulator_quad *player, sl_simulator_quad *ball, double time_frame_delta )
{
	sle_player1_header_count = 0;
	if( ++sle_player2_header_count > 3 ) {
		// Too many headers, score for the other dude.
		sle_score( scene, 1 );
	}
	// Then handle the header
	sle_header( scene, player, ball, time_frame_delta );
}

void sle_player1_clamp_horizontal( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	float minx, maxx;

	(void)a;
	(void)b;
	(void)time_frame_delta;

	sle_player1_pquad->velocity.x = 0.f;
	minx = -1.f + ( SLE_PLAYER_SCALE.x * 0.5f );
	maxx = -( SLE_PLAYER_SCALE.x * 0.5f );
	if( sle_player1_pquad->pos.x < minx ) {
		sle_player1_pquad->pos.x = minx;
	} else if ( sle_player1_pquad->pos.x > maxx ) {
		sle_player1_pquad->pos.x = maxx;
	}
	
	sl_mseti4( &sl_scene_get_volitile_quad( scene, sle_player1_pquad->quad->quad_id, SLE_LAYER_PLAYERS )->world_matrix, 
			   3, 0, sle_player1_pquad->pos.x );
}

void sle_player2_clamp_horizontal( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	float minx, maxx;

	(void)a;
	(void)b;
	(void)time_frame_delta;

	sle_player2_pquad->velocity.x = 0.f;
	minx = SLE_PLAYER_SCALE.x * 0.5f;
	maxx = 1.f - ( SLE_PLAYER_SCALE.x * 0.5f );
	if( sle_player2_pquad->pos.x < minx ) {
		sle_player2_pquad->pos.x = minx;
	} else if ( sle_player2_pquad->pos.x > maxx ) {
		sle_player2_pquad->pos.x = maxx;
	}
	
	sl_mseti4( &sl_scene_get_volitile_quad( scene, sle_player2_pquad->quad->quad_id, SLE_LAYER_PLAYERS )->world_matrix, 
			   3, 0, sle_player2_pquad->pos.x );
}

void sle_player1_clamp_vertical( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	float miny;

	(void)a;
	(void)b;
	(void)time_frame_delta;

	sle_player1_pquad->velocity.y = 0.f;
	miny = SLE_FLOOR + SLE_PLAYER_SCALE.y;
	if( sle_player1_pquad->pos.y < miny && sle_player1_pquad->velocity.y < 0.f ) {
		sle_player1_pquad->velocity.y = 0.f;
		sle_player1_pquad->pos.y = miny;
	}
	
	sl_mseti4( &sl_scene_get_volitile_quad( scene, sle_player1_pquad->quad->quad_id, SLE_LAYER_PLAYERS )->world_matrix,
			   3, 1, sle_player1_pquad->pos.y );
}

void sle_player2_clamp_vertical( sl_scene *scene, sl_simulator_quad *a, sl_simulator_quad *b, double time_frame_delta )
{
	float miny;

	(void)a;
	(void)b;
	(void)time_frame_delta;

	miny = SLE_FLOOR + SLE_PLAYER_SCALE.y;
	if( sle_player2_pquad->pos.y < miny && sle_player2_pquad->velocity.y < 0.f ) {
		sle_player2_pquad->velocity.y = 0.f;
		sle_player2_pquad->pos.y = miny;
	}
	
	sl_mseti4( &sl_scene_get_volitile_quad( scene, sle_player2_pquad->quad->quad_id, SLE_LAYER_PLAYERS )->world_matrix,
			   3, 1, sle_player2_pquad->pos.y );
}

int main( int argc, char **argv )
{
	sl_window *win;
	sl_scene *scene;
	sl_animator *animator;
	sl_simulator *sim;
	sl_program *program,
		   *post_program;;
	sl_texture *bg_tex,
			   *water_tex,
			   *rain_tex, 
			   *beach_tex, 
			   *splash_tex, 
			   *net_tex, 
			   *ball_tex, 
			   *player1_tex,
			   *player2_tex;
	sl_renderable *static_mesh;
	sl_quad *q;

	sl_vec pos, scale, tmp;
	sl_box uvs;
	sl_bvec flip_uvs;
	GLfloat color[ 4 ];

	unsigned int world_bounds_left, 
				 world_bounds_right,
				 world_bounds_sky,
				 world_bounds_ground,
				 net_bounds_id;
	// @TODO: UI

	// Initialize slenderer
	sl_renderer_create( );
	win = sl_renderer_open_window( 512, 512, "Volleyball", SL_FALSE, SL_FALSE );
	
	// Add the textures
	bg_tex = sl_renderer_allocate_texture( );
	water_tex = sl_renderer_allocate_texture( );
	//rain_tex = sl_renderer_allocate_texture( );
	beach_tex = sl_renderer_allocate_texture( );
	//splash_tex = sl_renderer_allocate_texture( );
	net_tex = sl_renderer_allocate_texture( );
	ball_tex = sl_renderer_allocate_texture( );
	player1_tex = sl_renderer_allocate_texture( );
	player2_tex = sl_renderer_allocate_texture( );
	assert( !sle_load_image_to_texture( bg_tex, "../res/sky.png" ) );
	assert( !sle_load_image_to_texture( water_tex, "../res/water.png" ) );
	//assert( !sle_load_image_to_texture( rain_tex, "../res/rain.png" ) );
	assert( !sle_load_image_to_texture( beach_tex, "../res/beach.png" ) );
	//assert( !sle_load_image_to_texture( splash_tex, "../res/splash.png" ) );
	assert( !sle_load_image_to_texture( net_tex, "../res/net.png" ) );
	assert( !sle_load_image_to_texture( ball_tex, "../res/ball.png" ) );
	assert( !sle_load_image_to_texture( player1_tex, "../res/player1.png" ) );
	assert( !sle_load_image_to_texture( player2_tex, "../res/player2.png" ) );
	
	// Create the default renderable
	static_mesh = sl_renderer_allocate_renderable( );
	sl_bset_scalar( &uvs, 0.0f, 0.0f, 1.0f, 1.0f );
	sl_renderable_create( static_mesh, &uvs );

	// Create the default program
	program = sl_renderer_allocate_program( );
	program->vert_prog_src = sl_program_default_vp_src;
	program->frag_prog_src = sl_program_default_fp_src;
	sl_program_create( program );

	// Create the default post process program
	post_program = sl_renderer_allocate_program( );
	post_program->vert_prog_src = sl_program_default_post_vp_src;
	post_program->frag_prog_src = sl_program_default_post_fp_src;
	sl_program_create( post_program );

	// Set up the scene
	scene = sl_renderer_add_scene( win->window_id, post_program->program_id );
	animator = sl_renderer_get_animator_for_scene( scene->scene_id );
	sim = sl_renderer_get_simulator_for_scene( scene->scene_id );

	sl_vset( &pos, 0.0f, 0.0f );
	sl_vset( &scale, 1.0f, 1.0f );
	flip_uvs.x = SL_FALSE; flip_uvs.y = SL_FALSE;
	
	// Add static sprites
	color[ 0 ] = color[ 1 ] = color[ 2 ] = color[ 3 ] = 1.0f;
	sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
						 &pos, &scale, 0.0f,
						 bg_tex->texture_id,
						 program->program_id,
						 static_mesh->renderable_id,
						 &uvs, &flip_uvs,
						 color,
						 SLE_SPRITE_VISIBLE );
	sl_scene_add_sprite( scene, SLE_LAYER_WATER, 
						 &pos, &scale, 0.0f,
						 water_tex->texture_id,
						 program->program_id,
						 static_mesh->renderable_id,
						 &uvs, &flip_uvs,
						 color,
						 SLE_SPRITE_VISIBLE );
	sl_scene_add_sprite( scene, SLE_LAYER_BEACH, 
						 &pos, &scale, 0.0f,
						 beach_tex->texture_id,
						 program->program_id,
						 static_mesh->renderable_id,
						 &uvs, &flip_uvs,
						 color,
						 SLE_SPRITE_VISIBLE );
	sl_vset( &pos, 0.0f, -0.5f ); sl_vset( &scale, 0.5f, 0.5f );
	sl_scene_add_sprite( scene, SLE_LAYER_NET, 
						 &pos, &scale, 0.0f,
						 net_tex->texture_id,
						 program->program_id,
						 static_mesh->renderable_id,
						 &uvs, &flip_uvs,
						 color,
						 SLE_SPRITE_VISIBLE );
	sl_vset( &pos, -2.f, 0.0f ); sl_vset( &scale, 1.f, 1.f );
	world_bounds_left = sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
											 &pos, &scale, 0.0f,
											 SL_INVISIBLE_TEXTURE,
											 program->program_id,
											 static_mesh->renderable_id,
											 &uvs, &flip_uvs,
											 color,
											 SLE_SPRITE_VISIBLE );
	sl_vset( &pos, 2.0f, 0.0f );
	world_bounds_right = sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
											  &pos, &scale, 0.0f,
											  NULL,
											  program->program_id,
											  static_mesh->renderable_id,
											  &uvs, &flip_uvs,
											  color,
											  SLE_SPRITE_VISIBLE );
	sl_vset( &pos, 0.0f, -1.f + SLE_FLOOR );
	world_bounds_ground = sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
											   &pos, &scale, 0.0f,
											   NULL,
											   program->program_id,
											   static_mesh->renderable_id,
											   &uvs, &flip_uvs,
											   color,
											   SLE_SPRITE_VISIBLE );
	sl_vset( &pos, 0.0f, 2.0f );
	world_bounds_sky = sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
											&pos, &scale, 0.0f,
											NULL,
											program->program_id,
											static_mesh->renderable_id,
											&uvs, &flip_uvs,
											color,
											SLE_SPRITE_VISIBLE );
	sl_vset( &pos, 0.0f, -0.5f ); sl_vset( &scale, 0.01f, 0.5f );
	net_bounds_id = sl_scene_add_sprite( scene, SLE_LAYER_BACKGROUND, 
										 &pos, &scale, 0.0f,
										 NULL,
										 program->program_id,
										 static_mesh->renderable_id,
										 &uvs, &flip_uvs,
										 color,
										 SLE_SPRITE_HIDE );

	// @TODO: Add weather effects. Rain that goes top->bottom tranforms on repeat w/callback
	//		  that makes splash visible and play sprite animation. Add splash at endpoints
	//		  of rain right away, but hide until callback.
	sl_vset( &scale, 0.1f, 0.1f );
	sl_vset( &pos, ( rand( ) / RAND_MAX ) * 0.5f > 0.5f ? SLE_BALL_START_POSITION_1.x : SLE_BALL_START_POSITION_2.x, SLE_BALL_START_POSITION_1.y );
	sle_ball_id = sl_scene_add_sprite( scene, SLE_LAYER_BALL,
									   &pos, &scale, 0.0f,
									   ball_tex->texture_id,
									   program->program_id,
									   static_mesh->renderable_id,
									   &uvs, &flip_uvs,
									   color,
									   SLE_SPRITE_VISIBLE );
	sle_player1_id = sl_scene_add_sprite( scene, SLE_LAYER_PLAYERS,
										  &SLE_PLAYER_1_START_POSITION, &SLE_PLAYER_SCALE, 0.0f,
										  player1_tex->texture_id,
										  program->program_id,
										  static_mesh->renderable_id,
										  &uvs, &flip_uvs,
										  color,
										  SLE_SPRITE_VISIBLE );
	sle_player2_id = sl_scene_add_sprite( scene, SLE_LAYER_PLAYERS,
										  &SLE_PLAYER_2_START_POSITION, &SLE_PLAYER_SCALE, 0.0f,
										  player2_tex->texture_id,
										  program->program_id,
										  static_mesh->renderable_id,
										  &uvs, &flip_uvs,
										  color,
										  SLE_SPRITE_VISIBLE );

	// Controls
	sl_controller_create( );
	sl_controller_register_window( win );

	sl_controller_add_key_press_callback( GLFW_KEY_ESCAPE, sle_quit );

	sl_controller_add_key_press_callback( GLFW_KEY_A, sle_move_player1 );
	sl_controller_add_key_press_callback( GLFW_KEY_D, sle_move_player1 );
	sl_controller_add_key_press_callback( GLFW_KEY_W, sle_move_player1 );
	sl_controller_add_key_press_callback( GLFW_KEY_LEFT, sle_move_player2 );
	sl_controller_add_key_press_callback( GLFW_KEY_RIGHT, sle_move_player2 );
	sl_controller_add_key_press_callback( GLFW_KEY_UP, sle_move_player2 );

	sl_controller_add_key_repeat_callback( GLFW_KEY_A, sle_move_player1 );
	sl_controller_add_key_repeat_callback( GLFW_KEY_D, sle_move_player1 );
	sl_controller_add_key_repeat_callback( GLFW_KEY_LEFT, sle_move_player2 );
	sl_controller_add_key_repeat_callback( GLFW_KEY_RIGHT, sle_move_player2 );
	
	sl_controller_add_key_release_callback( GLFW_KEY_A, sle_stop_player1 );
	sl_controller_add_key_release_callback( GLFW_KEY_D, sle_stop_player1 );
	sl_controller_add_key_release_callback( GLFW_KEY_LEFT, sle_stop_player2 );
	sl_controller_add_key_release_callback( GLFW_KEY_RIGHT, sle_stop_player2 );

	// UI things @TODO

	// Physics
	sl_vset( &tmp, 0.0f, 0.0f );
	sle_player1_pquad = sl_simulator_add_quad( sim, sle_player1_id, &tmp );	 // Add players
	sle_player2_pquad = sl_simulator_add_quad( sim, sle_player2_id, &tmp );
	sle_ball_pquad	  = sl_simulator_add_quad( sim, sle_ball_id, &tmp );	 // Add ball
	sl_simulator_add_quad( sim, world_bounds_left, &tmp ); // Add some quads outside the world as bounds for the ball
	sl_simulator_add_quad( sim, world_bounds_right, &tmp );
	sl_simulator_add_quad( sim, world_bounds_sky, &tmp );
	sl_simulator_add_quad( sim, net_bounds_id, &tmp );	// Add the net
	sl_simulator_add_quad( sim, world_bounds_ground, &tmp ); // Add the beach
	
	sl_simulator_add_callback( sim, sle_ball_id, sle_player1_id, sle_player1_header ); // Player vs ball (sphere/elipsoid)
	sl_simulator_add_callback( sim, sle_ball_id, sle_player2_id, sle_player2_header );
	sl_simulator_add_callback( sim, sle_ball_id, net_bounds_id, sl_simulator_callback_quad_sphere ); // Ball vs net (sphere/quad)
	sl_simulator_add_callback( sim, sle_ball_id, world_bounds_ground, sle_ball_dropped ); // Ball vs beach (sphere/quad)
	sl_simulator_add_callback( sim, sle_ball_id, world_bounds_left, sl_simulator_callback_quad_sphere ); // Ball vs world bounds (sphere/quad)
	sl_simulator_add_callback( sim, sle_ball_id, world_bounds_right, sl_simulator_callback_quad_sphere );
	sl_simulator_add_callback( sim, sle_ball_id, world_bounds_sky, sl_simulator_callback_quad_sphere );

	sl_simulator_add_callback( sim, sle_player1_id, net_bounds_id, sle_player1_clamp_horizontal ); // Player vs net (quad/quad)
	sl_simulator_add_callback( sim, sle_player1_id, world_bounds_left, sle_player1_clamp_horizontal ); // Player vs world bounds (quad/quad)
	sl_simulator_add_callback( sim, sle_player1_id, world_bounds_ground, sle_player1_clamp_vertical ); // Player vs world bounds (quad/quad)
	
	sl_simulator_add_callback( sim, sle_player2_id, net_bounds_id, sle_player2_clamp_horizontal ); // Player vs net (quad/quad)
	sl_simulator_add_callback( sim, sle_player2_id, world_bounds_right, sle_player2_clamp_horizontal ); // Note that we only need to check one side here, net is other side.
	sl_simulator_add_callback( sim, sle_player2_id, world_bounds_ground, sle_player2_clamp_vertical ); // Player vs world bounds (quad/quad)
	
	sl_vset( &tmp, 0.0f, SLE_GRAVITY );
	sl_simulator_add_force( sim, sle_ball_id, &tmp ); // Add gravity to ball
	sl_simulator_add_force( sim, sle_player1_id, &tmp ); // Add gravity to players
	sl_simulator_add_force( sim, sle_player2_id, &tmp ); 

	// Logic
	sle_player1_score = 0;
	sle_player2_score = 0;
	sle_freeze_ball = SL_TRUE;

	// Reset to start state
	sle_score( scene, 1 );
	sle_player1_score = 0;
	sle_player2_score = 0;

	// Enter the loop
	while( !glfwWindowShouldClose( win->handle ) )
	{
		// Logic is handled in physics callbacks, see pre-loop physics part.
		if( sle_freeze_ball ) {
			( ( sl_vec* )vul_vector_get( sle_ball_pquad->forces, 0 ) )->y = 0.0;
		} else {
			( ( sl_vec* )vul_vector_get( sle_ball_pquad->forces, 0 ) )->y = SLE_GRAVITY;
		}

		// Move the actual quads
		q = sl_scene_get_volitile_quad( scene, sle_player1_id, SLE_LAYER_PLAYERS );
		sl_quad_create_world_matrix( q, &sle_player1_pquad->pos, &SLE_PLAYER_SCALE, 0.0f );
		q = sl_scene_get_volitile_quad( scene, sle_player2_id, SLE_LAYER_PLAYERS );
		sl_quad_create_world_matrix( q, &sle_player2_pquad->pos, &SLE_PLAYER_SCALE, 0.0f );
		q = sl_scene_get_volitile_quad( scene, sle_ball_id, SLE_LAYER_BALL );
		sl_quad_create_world_matrix( q, &sle_ball_pquad->pos, &SLE_BALL_SCALE, 0.0f );
		
		// Update animator
		sl_animator_update( animator );

		// Render scene
		sl_renderer_render_scene( scene->scene_id, win->window_id );
	}

	// Clean up
	sl_renderer_destroy( );

	return 0;
}
