Slenderer
=========

Slenderer is a small 2D rendering library written in C. The goal is to have as small a dependancy-list as possible. At time of writing, that list is:

* GLFW 3
* glew
* Portaudio
* The header-only libraries included in the dependancies-folder.

See the Dependancies section below for more on these.

# Renderer

## Scene structure

We renders a series of layers, each of which is a flat array of quads. Internally in a layer
quads are rendered sorted by program/texture but internally in order, but we make *no guarantees*
of this. If order is important, use the layers to dictate it, since these are always rendered in
the same order.

## Coordinate space

Absolutely everything is in normalized screen space coordinates; even the physics.

# Components

## Input

We handle keyboard and mouse input though callbacks.

## Simulator

Basic 2D physics support is supplied. Registering a quad for simulation means forces, if any are
applied to it, will update its position and velocity every frame, but angular momentum or friction
is supported internally. If a collission-callback is registered for two colliding quads, this will be
called, otherwise collissions are ignored (so you can handle the friction yourself in callbacks).

## Animator

An animation manager that supports both sprite/frame-based animation and linear transforms, however
this component has *not been tested at all* as of this writing. That *will* be bugs here!

## Math

A couple of very, very simple linear math components used internally; the syntax is horrible since this
is C and it was important to avoid cluttering the symbol table with common things such as vec or mat.
It is recommended (since working in screen space is bad for anything but rendering anyway) that you
use your own math library and write conversion-functions that move from your coordinate space and library
to the internal one to ease your work.

## Aurator

The audio component; it takes clips of sound given as samples in a buffer of shorts, channels interleaved if more than one channel is supplied. Similar in function to the animator; once a clip is finished it is removed, restarted or stopped and kept around based on state given in at the beginning of play.
We mix clips together with pure addition into a 32-bit buffer, then clip them down before scaling and passing it to the audio stream, which is very much the wrong thing to do, but it was very rushed. Id love to integrate stb\_audio\_mixer into it, and if anyone feels like trying that, go ahead (and let me know).
We supply a way to load Ogg Vorbis files into the system (through stb\_vorbis), but there is no reason you can't write your own loading code.

# Dependancies

* [GLFW 3](http://www.glfw.org/) is available from their sire or their [github repository](https://github.com/glfw/glfw).
* [GLEW](http://glew.sourceforge.net/) is available from their site or likely in your package manager.
* [Portaudio 1.9](http://portaudio.com/) is available from their site. It was the slimmest audio stream library I found. If anyone know of an even slimmer one, let me know and I might swap it around.
* stb_image.h and stb_vorbis.h are a single-file image and ogg vorbis handling libraries by Sean Barrett. All hail the [stb libraries](https://github.com/nothings/stb)!
* vul_* are a subset of my own header-only libraries. These are previously unrealesed because they have not
  seen the required years of service to make sure they actually work. Using them in this renderer will probably
  help that. In this case they contain a timer, some typedefs, a hash map using a linked list for collissions and a
  resizable array with accompaning sorting functions. These are all released with the same license as Slenderer
  (public domain/MIT).

# Building

There are a few minor defines needed to get the dependancies to work. In your main .c file, define VUL\_DEFINE.
Depending on your platform, define VUL\_LINUX, VUL\_WINDOWS or VUL\_OSX in the compiler toolchain (the included makefile
defines VUL_LINUX). If compiling with a C89 toolchain (so, MSVC), define VUL\_VECTOR\_C89\_ITERATORS. In your debug
build you might want to define VUL\_DEBUG to take advantage of additional checks in vector iterators.

## GLES
Define SL\_OPENGL\_ES to build for an OpenGL ES 2.0 target instead of the stock OpenGL 3.2 Core.

## No audio
If no audio is needed, define SL\_NO\_AUDIO. The dependancy on portaudio neatly goes away if this is defined, and makes this useful for when no audio is needed (non-game applications).

# Notes

* The example gives a good idea of how the engine is used, but has not been tested since the *massive* changes during Ludum Dare, so you're probably better off looking at the code for that if you really want to dive in. Source+binary of that is found [here](http://www.schmidx2.com/Code/LD30.zip).

# Future plans

* Proper audio mixing, integrate stb\_audio\_mixer.
* Font rendering component, probably use stb\_truetype
* Extend the audio component with a synth; pass tracks of "notes" to it with a wave, frequency and duration and mix it.

# License

As is noted in the source, Slenderer is released into the public domain in legal jurisdiction where such a thing exists
Where there is no such thing, it is released under the MIT License, as seen in the LICENSE file.

The stb\_-libraries are lincensed independently. See the files themselves for their licensing (public domain).
