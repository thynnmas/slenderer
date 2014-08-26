/*
 * Slenderer - Thomas Martin Schmid, 2014. Public domain¹
 * 
 * Shaders and programs.Animation manager.
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
#ifndef SLENDERER_PROGRAM_H
#define SLENDERER_PROGRAM_H

#define SL_TRUE 1
#define SL_FALSE 0

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "renderer/window.h"

typedef struct sl_program {
	unsigned int program_id;

	const char *vert_prog_src;
	const char *frag_prog_src;

	GLuint gl_vp_id;
	GLuint gl_fp_id;
	GLuint gl_prog_id;
} sl_program;

/**
 * Default vertex program. The mvp matrix is set automatically, so it must 
 * be present in custom ones as well.
 */
const char *sl_program_default_vp_src;
/**
 * Default fragment program.
 */
const char *sl_program_default_fp_src;
/**
 * Default post-process vertex program. 
 */
const char *sl_program_default_post_vp_src;
/**
 * Default post-process fragment program.
 */
const char *sl_program_default_post_fp_src;

/**
 * Creates a new program with the given sources.
 * Assumes prog->vert_prog_src and prog->frag_prog_src have
 * been set to the correct sources.
 * \note: requires an OpenGL context to pre-exist.
 */
void sl_program_create( sl_program* prog );

/**
 * Destroys a program and its shaders.
 * Sources are NOT freed, as this allows us
 * to use .DATA, stack or heap.
 */
void sl_program_destroy( sl_program* prog );

/**
 * Binds a program for rendering.
 */
void sl_program_bind( sl_program* prog );

/**
 * Unbinds a program after rendering.
 */
void sl_program_unbind( sl_program* prog );

/**
 * Check that a shader compiled fine.
 * Prints errors to stderr in realease, asserts false in debug.
 */
int sl_program_check_shader_compile( GLuint prog_id, const char *src );

/**
 * Check that a program links find.
 * Prints errors to stderr in release, asserts false in debug.
 */
int sl_program_check_link( GLuint prog_id );

#endif
