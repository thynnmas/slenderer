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

#include "slenderer.h"

sl_renderer *sl_renderer_global = NULL;

void sl_renderer_create(  )
{
	// Init glfw
#ifdef SL_DEBUG
	assert( glfwInit( ) );
#else
	if( !glfwInit( ) ) {
		printf( "Could not intialize GLFW. Terminating.\n" );
		exit( EXIT_FAILURE );
	}
#endif
	// Register a simple error handling callback
	glfwSetErrorCallback( sl_renderer_glfw_error_callback );
	
	// Hint at GL version
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	
	// Initialize our context.
	sl_renderer_global = ( sl_renderer* )malloc( sizeof( sl_renderer ) );
	
	sl_renderer_global->windows = vul_vector_create( sizeof( sl_window ), 0 );
	sl_renderer_global->scenes = vul_vector_create( sizeof( sl_scene ), 0 );
	sl_renderer_global->animators = vul_vector_create( sizeof( sl_animator ), 0 );
	sl_renderer_global->simulators = vul_vector_create( sizeof( sl_simulator ), 0 );
	sl_renderer_global->textures = vul_vector_create( sizeof( sl_texture ), 0 );
	sl_renderer_global->programs = vul_vector_create( sizeof( sl_program ), 0 );
	sl_renderer_global->renderables = vul_vector_create( sizeof( sl_renderable ), 0 );
	sl_renderer_global->aurators = vul_vector_create( sizeof( sl_aurator), 0 );
	
	sl_controller_create( );
}

void sl_renderer_destroy( )
{
	sl_program *itp, *lastp;
	sl_texture *itt, *lastt;
	sl_animator *ita, *lasta;
	sl_simulator *itsim, *lastsim;
	sl_scene *its, *lasts;
	sl_window *itw, *lastw;
	sl_renderable *itr, *lastr;
	sl_aurator *itar, *lastar;

	// Clean up
	vul_foreach( sl_renderable, itr, lastr, sl_renderer_global->renderables ) {
		sl_renderable_destroy( itr );
	}
	vul_vector_destroy( sl_renderer_global->renderables );

	vul_foreach( sl_program, itp, lastp, sl_renderer_global->programs ) {
		sl_program_destroy( itp );
	}
	vul_vector_destroy( sl_renderer_global->programs );

	vul_foreach( sl_texture, itt, lastt, sl_renderer_global->textures ) {
		sl_texture_destroy( itt );
	}
	vul_vector_destroy( sl_renderer_global->textures );

	vul_foreach( sl_animator, ita, lasta, sl_renderer_global->animators ) {
		sl_animator_destroy( ita );
	}
	vul_vector_destroy( sl_renderer_global->animators );

	vul_foreach( sl_simulator, itsim, lastsim, sl_renderer_global->simulators ) {
		sl_simulator_destroy( itsim );
	}
	vul_vector_destroy( sl_renderer_global->simulators );

	vul_foreach( sl_scene, its, lasts, sl_renderer_global->scenes ) {
		sl_scene_destroy( its );
	}
	vul_vector_destroy( sl_renderer_global->scenes );

	vul_foreach( sl_window, itw, lastw, sl_renderer_global->windows ) {
		sl_window_destroy( itw );
	}
	vul_vector_destroy( sl_renderer_global->windows );
		
	vul_foreach( sl_aurator, itar, lastar, sl_renderer_global->aurators ) {
		sl_aurator_destroy( itar );
	}
	vul_vector_destroy( sl_renderer_global->aurators );

	// Destroy portaudio system
	sl_aurator_finalize_system( );

	// Destroy the controller
	sl_controller_destroy( );

	free( sl_renderer_global );

	// And shut down glfw
	glfwTerminate( );
}

sl_window *sl_renderer_open_window( unsigned int width, unsigned int height, const char *title, int fullscreen, int vsync )
{
	sl_window *win;
	
	win = ( sl_window* )vul_vector_add_empty( sl_renderer_global->windows );
	win->window_id = vul_vector_size( sl_renderer_global->windows ) - 1;
	sl_window_create( win, width, height, title, fullscreen, vsync, NULL );

	// Need to initalize glew if we haven't
	if( win->window_id == 0 ) {
		glewExperimental = GL_TRUE;
		glewInit( );
	}

	return win;
}

void sl_renderer_close_window( sl_window *win )
{
	sl_window_destroy( win );
	vul_vector_remove_swap( sl_renderer_global->windows, win->window_id );
}

sl_scene *sl_renderer_add_scene( sl_window *win, sl_program *post_program )
{
	sl_scene *s;
	sl_animator *a;
	sl_simulator *sim;
	sl_aurator *ar;

	// Create the scnee
	s = ( sl_scene* )vul_vector_add_empty( sl_renderer_global->scenes );
	sl_scene_create( s, win, vul_vector_size( sl_renderer_global->scenes ) - 1, post_program );

	// Also add the corrisponding animator and the simulator
	a = ( sl_animator* )vul_vector_add_empty( sl_renderer_global->animators );
	sl_animator_create( a, s );

	sim = ( sl_simulator* )vul_vector_add_empty( sl_renderer_global->simulators );
	sl_simulator_create( sim, s );
	
	// If we have sound, create the aurator
	ar = ( sl_aurator* )vul_vector_add_empty( sl_renderer_global->aurators );
	sl_aurator_create( ar, SL_AUDIO_CHANNEL_COUNT, SL_AUDIO_SAMPLE_RATE, SL_AUDIO_FRAME_RATE_GUARANTEE );

	// Return the scene
	return s;
}

void sl_renderer_finalize_scene( unsigned int scene_id )
{
	// @TODO: This doesn't seem to quite work...
	sl_scene *si, *sil;
	sl_animator *a;
	sl_simulator *s;
	sl_aurator *ar;
	ui32_t i;

	i = 0;
	vul_foreach( sl_scene, si, sil, sl_renderer_global->scenes )
	{
		if( si->scene_id == scene_id ) {
			sl_scene_destroy( si );
			vul_vector_remove_cascade( sl_renderer_global->scenes, i );
			break;
		}
		++i;
	}

	a = sl_renderer_get_animator_for_scene( scene_id );
	sl_animator_destroy( a );
	s = sl_renderer_get_simulator_for_scene( scene_id );
	sl_simulator_destroy( s );
	ar = sl_renderer_get_aurator_for_scene( scene_id );
	sl_aurator_destroy( ar );
}

sl_animator *sl_renderer_get_animator_for_scene( unsigned int scene_id )
{
#ifdef SL_DEBUG
	assert( scene_id < vul_vector_size( sl_renderer_global->animators ) );
#endif
	return ( sl_animator* )vul_vector_get( sl_renderer_global->animators, scene_id );
}

sl_aurator *sl_renderer_get_aurator_for_scene( unsigned int scene_id )
{
#ifdef SL_DEBUG
	assert( scene_id < vul_vector_size( sl_renderer_global->aurators ) );
#endif
	return ( sl_aurator* )vul_vector_get( sl_renderer_global->aurators, scene_id );
}

sl_simulator *sl_renderer_get_simulator_for_scene( unsigned int scene_id )
{
#ifdef SL_DEBUG
	assert( scene_id < vul_vector_size( sl_renderer_global->simulators ) );
#endif
	return ( sl_simulator* )vul_vector_get( sl_renderer_global->simulators, scene_id );
}

sl_texture *sl_renderer_allocate_texture( )
{
	sl_texture* t;

	t = ( sl_texture* )vul_vector_add_empty( sl_renderer_global->textures );
	t->texture_id = vul_vector_size( sl_renderer_global->textures ) - 1;

	return t;
}

void sl_renderer_glfw_error_callback( int error, const char *desc )
{
#ifdef SL_DEBUG
	assert( 0 );
#else
	fprintf( stderr, "GLFW error encountered:\n%s\n", descrtiption );
#endif
}

sl_renderable *sl_renderer_allocate_renderable( )
{
	sl_renderable *r;

	r = ( sl_renderable* )vul_vector_add_empty( sl_renderer_global->renderables );
	r->renderable_id = vul_vector_size( sl_renderer_global->renderables ) - 1;

	return r;
}

sl_program *sl_renderer_allocate_program( )
{
	sl_program* p;

	p = ( sl_program* )vul_vector_add_empty( sl_renderer_global->programs );
	p->program_id = vul_vector_size( sl_renderer_global->programs ) - 1;

	return p;
}

void sl_renderer_render_scene( unsigned int scene_index, unsigned int window_index, SL_BOOL swap_buffers, sl_vec *camera_offset )
{
	sl_scene *scene;
	sl_animator *anim;
	sl_aurator *aur;
	sl_simulator *sim;
	sl_window *win;
	sl_quad *it, *last_it; // iterator
	int i, cpi, cti, cri;
	sl_program *cp; // Current program
	sl_texture *ct; // Current texture
	sl_renderable *cr; // Current renderable

	// Check that the window is still open, and make it current
	win = ( sl_window* )vul_vector_get( sl_renderer_global->windows, window_index );
#ifdef SL_DEBUG
	assert( !glfwWindowShouldClose( win->handle ) );
#else
	if ( glfwWindowShouldClose( win->handle ) ) {
		printf( "GLFW has been asked to close a window. Doing so, render failed.\n" );
		return;
	}
#endif
	sl_window_bind_framebuffer_fbo( win );
	
	// Update the corresponding animator
	anim = ( sl_animator* )vul_vector_get( sl_renderer_global->animators, scene_index );
	sl_animator_update( anim );

	// Update the corresponding simulator
	sim = ( sl_simulator* )vul_vector_get( sl_renderer_global->simulators, scene_index );
	sl_simulator_update( sim );

	// Update the corresponding audio manager
	aur = ( sl_aurator* )vul_vector_get( sl_renderer_global->aurators, scene_index );
	sl_aurator_update( aur );

	// Grab the scene and sort it
	scene = ( sl_scene* )vul_vector_get( sl_renderer_global->scenes, scene_index );
	sl_scene_sort( scene );
	
	// Set GL state
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	cp = NULL;
	ct = NULL;
	cr = NULL;
	cpi = -1;
	cti = -1;
	cri = -1;
	for( i = 0; i < MAX_LAYERS; ++i )
	{
		vul_foreach( sl_quad, it, last_it, scene->layers[ i ] )
		{
			// If the quad is invisible, don't render it
			if( it->hidden ) {
				continue;
			}
			// If new program, rebind it
			if( cpi != it->program_id ) {
				cpi = it->program_id;
				sl_program_unbind( cp );
				cp = ( sl_program* )vul_vector_get( sl_renderer_global->programs, cpi );
				sl_program_bind( cp );
			}
			// If new texture, rebind it
			if( cti != it->texture_id ) {
				cti = it->texture_id;
				sl_texture_unbind( ct );
				ct = ( sl_texture* )vul_vector_get( sl_renderer_global->textures, cti );
				sl_texture_bind( cp, ct );
			}
			// If new renderable, rebind it
			if( cri != it->renderable_id ) {
				cri = it->renderable_id;
				sl_renderable_unbind( cr );
				cr = ( sl_renderable* )vul_vector_get( sl_renderer_global->renderables, cri );
				sl_renderable_bind( cr );
			}
			// Render the quad
			sl_renderer_draw_instance( &scene->camera_pos, cp, it );
		}
	}

	// Render post
	sl_window_bind_framebuffer_post( win );
	{
		// Bind the program
		sl_program_bind( scene->post_program );
		// Bind the FBO as the texture
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, win->fbo_texture );
		glUniform1i( glGetUniformLocation( scene->post_program->gl_prog_id, "texture" ), 0 );
		// Bind the renderable
		sl_renderable_bind( &scene->post_renderable );
		// Bind program parameters
		if( scene->post_program_callback ) {
			scene->post_program_callback( scene->post_program );
		}
		// And draw the instance
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
		// And unbind things
		sl_renderable_unbind( &scene->post_renderable );
		glBindTexture( GL_TEXTURE_2D, 0 );
		sl_program_unbind( scene->post_program );
	}

	// Swap buffers
	if( swap_buffers ) {
		sl_window_swap_buffers( win );
	}
}

void sl_renderer_draw_instance( sl_vec *camera_offset, sl_program *prog, sl_quad *quad )
{
	sl_mat4 mat;
	sl_box uvs;
	f32_t tmp;

	// Calculate offset into matrix
	sl_mcopy4( &mat, &quad->world_matrix );
	mat.data[ 12 ] -= camera_offset->x;
	mat.data[ 13 ] -= camera_offset->y;

	// Calculate the uvs; they may be flipped
	sl_bset( &uvs, &quad->uvs );
	if( quad->flip_uvs.x ) {
		tmp = uvs.min_p.x;
		uvs.min_p.x = uvs.max_p.x;
		uvs.max_p.x = tmp;
	}
	if( quad->flip_uvs.y ) {
		tmp = uvs.min_p.y;
		uvs.min_p.y = uvs.max_p.y;
		uvs.max_p.y = tmp;
	}

	// Set world matrix
	glUniformMatrix4fv( glGetUniformLocation( prog->gl_prog_id, "mvp" ), 1, GL_FALSE, ( ( GLfloat* )&mat.data[ 0 ] ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "color" ), 1, ( ( GLfloat* )&quad->color ) );
	glUniform4fv( glGetUniformLocation( prog->gl_prog_id, "texcoord_offset_scale" ), 1, ( ( GLfloat* )&uvs ) );
	
	// Draw the quad
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

}

void sl_renderer_get_scenes_by_window_handle( vul_vector_t *vec, GLFWwindow *win_handle )
{
	sl_scene *it, *last_it;
	sl_window *win, *itw, *last_itw;

	win = NULL;
	vul_foreach( sl_window, itw, last_itw, sl_renderer_global->windows )
	{
		if( itw->handle == win_handle ) {
			win = itw;
			break;
		}
	}
	if( win == NULL ) {
		return;
	}

	vul_foreach( sl_scene, it, last_it, sl_renderer_global->scenes )
	{
		if( it->window == win ) {
			vul_vector_add( vec, &it );
		}
	}
}

sl_scene *sl_renderer_get_scene_by_id( unsigned int id )
{
	sl_scene *si, *sil;

	vul_foreach( sl_scene, si, sil, sl_renderer_global->scenes )
	{
		if( si->scene_id == id ) {
			return si;
		}
	}
	return NULL;
}

sl_texture *sl_renderer_get_texture_by_id( unsigned int id )
{
	sl_texture *ti, *til;

	vul_foreach( sl_texture, ti, til, sl_renderer_global->textures )
	{
		if( ti->texture_id == id ) {
			return ti;
		}
	}
	return NULL;
}