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

#include "renderer/program.h"

#ifdef SL_LEGACY_OPENGL
const char *sl_program_default_vp_src =  "#version 120\n"
										 "uniform mat4 mvp;\n"
										 "uniform vec4 color;\n"
										 "uniform vec4 texcoord_offset_scale;\n"
										 "attribute vec2 position;\n"
										 "attribute vec2 texcoord_in;\n"
										 "varying vec2 texcoord_out;\n"
										 "varying vec4 color_out;\n"
										 "void main()	{\n"
										 "	vec2 texscale = texcoord_offset_scale.zw - texcoord_offset_scale.xy;\n"
										 "	texcoord_out.xy = texcoord_in.xy * texscale.xy + texcoord_offset_scale.xy;\n"
										 "	color_out = color;\n"
										 "	gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
										 "}";

const char *sl_program_default_fp_src =	"#version 120\n"
										"varying vec2 texcoord_out;\n"
										"varying vec4 color_out;\n"
										"uniform sampler2D texture;\n"
										"void main() {\n"
										"	gl_FragColor = clamp( color_out * texture2D( texture, texcoord_out.xy ).rgba, 0.0, 1.0 );\n"
										"}";

const char *sl_program_default_post_vp_src =  "#version 120\n"
											  "attribute vec2 position;\n"
											  "attribute vec2 texcoord_in;\n"
											  "varying vec2 texcoord_out;\n"
											  "varying vec4 color_out;\n"
											  "void main()	{\n"
											  "    texcoord_out.xy = texcoord_in.xy;\n"
											  "    gl_Position = vec4(position, 0.0, 1.0);\n"
											  "}";

const char *sl_program_default_post_fp_src =	"#version 120\n"
												"uniform sampler2D texture;\n"
												"varying vec2 texcoord_out;\n"
												"void main() {\n"
												"    gl_FragColor = texture2D( texture, texcoord_out.xy ).rgba;\n"
												"}";
#else 
const char *sl_program_default_vp_src =  "#version 400 core\n"
										 "uniform mat4 mvp;\n"
										 "uniform vec4 color;\n"
										 "uniform vec4 texcoord_offset_scale;\n"
										 "in vec2 position;\n"
										 "in vec2 texcoord_in;\n"
										 "out vec2 texcoord_out;\n"
										 "out vec4 color_out;\n"
										 "void main()	{\n"
										 "	vec2 texscale = texcoord_offset_scale.zw - texcoord_offset_scale.xy;\n"
										 "	texcoord_out.xy = texcoord_in.xy * texscale.xy + texcoord_offset_scale.xy;\n"
										 "	color_out = color;\n"
										 "	gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
										 "}";

const char *sl_program_default_fp_src =	"#version 400 core\n"
										"in vec2 texcoord_out;\n"
										"in vec4 color_out;\n"
										"out vec4 oColor;\n"
										"uniform sampler2D texture;\n"
										"void main() {\n"
										"	oColor = clamp( color_out * texture2D( texture, texcoord_out.xy ).rgba, 0.0, 1.0 );\n"
										"}";

const char *sl_program_default_post_vp_src =  "#version 400 core\n"
											  "in vec2 position;\n"
											  "in vec2 texcoord_in;\n"
											  "out vec2 texcoord_out;\n"
											  "out vec4 color_out;\n"
											  "void main()	{\n"
											  "    texcoord_out.xy = texcoord_in.xy;\n"
											  "    gl_Position = vec4(position, 0.0, 1.0);\n"
											  "}";

const char *sl_program_default_post_fp_src =	"#version 400 core\n"
												"in vec2 texcoord_out;\n"
												"out vec4 oColor;\n"
												"uniform sampler2D texture;\n"
												"void main() {\n"
												"    oColor = texture2D( texture, texcoord_out.xy ).rgba;\n"
												"}";
#endif


void sl_program_create( sl_program* prog )
{
	GLint src_len;

	// Create the vertex program
	prog->gl_vp_id = glCreateShader( GL_VERTEX_SHADER );
	src_len = ( GLint )strlen( prog->vert_prog_src );

	glShaderSource( prog->gl_vp_id, 1, &prog->vert_prog_src, &src_len );
	glCompileShader( prog->gl_vp_id );
	
	if ( !sl_program_check_shader_compile( prog->gl_vp_id, prog->vert_prog_src ) ) {
		glDeleteShader( prog->gl_vp_id );
		return;
	}
	
	// Create the fragment program
	prog->gl_fp_id = glCreateShader( GL_FRAGMENT_SHADER );
	src_len = ( GLint )strlen( prog->frag_prog_src );

	glShaderSource( prog->gl_fp_id, 1, &prog->frag_prog_src, &src_len );
	glCompileShader( prog->gl_fp_id );
	
	if ( !sl_program_check_shader_compile( prog->gl_fp_id, prog->frag_prog_src ) ) {
		glDeleteShader( prog->gl_vp_id );
		glDeleteShader( prog->gl_fp_id );
		return;
	}
	
	// Create the full prorgam
	prog->gl_prog_id = glCreateProgram( );
	
	glAttachShader( prog->gl_prog_id, prog->gl_vp_id );
	glAttachShader( prog->gl_prog_id, prog->gl_fp_id );
	glLinkProgram( prog->gl_prog_id );
	
	if( !sl_program_check_link( prog->gl_prog_id ) )
	{
		glDeleteShader( prog->gl_vp_id );
		glDeleteShader( prog->gl_fp_id );
		glDeleteProgram( prog->gl_prog_id );
	}
}

void sl_program_destroy( sl_program* prog )
{
	glDeleteShader( prog->gl_vp_id );
	glDeleteShader( prog->gl_fp_id );
	glDeleteProgram( prog->gl_prog_id );
}

void sl_program_bind( sl_program* prog )
{
	glUseProgram( prog->gl_prog_id );
}

void sl_program_unbind( sl_program* prog )
{
	glUseProgram( 0 );
}

int sl_program_check_shader_compile( GLuint prog_id, const char *src )
{
	GLint result, length;
	char *clog;

	glGetShaderiv( prog_id, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE ) {
		// Get shader compile output
		glGetShaderiv( prog_id, GL_INFO_LOG_LENGTH, &length );
		clog = ( char* )malloc( sizeof( char ) * length );
		glGetShaderInfoLog( prog_id, length, &result, clog );

		// Print error
		fprintf( stderr, "Failed to compile shader. Output:\n%s\n\nSource:\n%s\n\n", src, clog );

		// Clean up
		free( clog );

		return SL_FALSE;
	}
	return SL_TRUE;
}

int sl_program_check_link( GLuint prog_id )
{
	GLint result, length;
	char *clog;

	glGetProgramiv( prog_id, GL_LINK_STATUS, &result );

	if ( result == GL_FALSE ) {
		// Get shader compile output
		glGetProgramiv( prog_id, GL_INFO_LOG_LENGTH, &length );
		clog = ( char* )malloc( sizeof( char ) * length );
		glGetShaderInfoLog( prog_id, length, &result, clog );

		// Print error
		fprintf( stderr, "Program failed to link. Output: %s\n", clog );
		
		// Clean up
		free( clog );

		return SL_FALSE;
	}
	return SL_TRUE;
}
