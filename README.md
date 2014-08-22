Slenderer
=========

Slenderer is a tiny 2D rendering library written in C. The goal is to have as small a dependancy-list as possible. At time of writing, that list is:

* GLFW 3
* The header-only libraries included in the dependancies-folder.

See the Dependancies section below for more on these.

# Warning
While this is released at this point, it is not stable, tested or advised for people who aren't either me or very adventurous. It has been released in time for Ludum Dare and will be a companion-work for whatever game I make there, so it is likely to have a lot its faults fixed over the weekend. If anyone wants to try using it in this time, let me know (@thynnmas on twitter/#ludumdare for the weekend) and I'll try to help out as much as I can.

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

# Dependancies

* [GLFW](http://www.glfw.org/) is available from their sire or their [github repository](https://github.com/glfw/glfw).
* stb_image.c is a single-file image handling library by Sean Barrett. All hail the [stb libraries](https://github.com/nothings/stb)!
* vul_* are a subset of my own header-only libraries. These are previously unrealesed because they have not
  seen the required years of service to make sure they actually work. Using them in this renderer will probably
  help that. In this case they contain a timer, some typedefs, a hash map using a linked list for collissions and a
  resizable array with accompaning sorting functions. These are all released with the same license as Slenderer
  (public domain/MIT).

# Notes

This was released in haste to be able to use it for Ludum Dare 30. Improving the renderer as a side-efect of using
it there was a goal of the premature release, but it means the state of it is not quite where it should be for release.

* The animator has not been tested. At all. It will be, but use at own risk.
* The code is not well commented; for the most part header-files contain comments descriping in somtimes way too obious
  ways what the functions do, but the meat of things are not guaranteed to be well commented.
* There ARE bugs. Many will be squashed over the weekend. Some won't. If anyone uses this, let me know if you find any
  and they will be looked at when I have time.
* The example renders, handles input applies forces to the ball and players and detects collissions, but jumping does
  not work, touching the ball with the player causes everything to freeze and it's generally not a playable game. I
  suspect these bugs are mostly in the example itself (although the collission detection handling seems off on one of
  the players causing drift) and not in the renderer/components though, so it's not as bad as it looks. If they are
  indeed in the components, they are liekyl to get squashed over the weekend. The example still shows a relatively
  complete usage of the engine, even if the animation-component is not used; the boilerplate is there, but it is not
  written into it yet. At some point it will be.

# Future plans

* Fix all the things that aren't ready yet due to the rushed release
* A small audio playing component. This is likely to appear during the weekend.
* OpenGL ES support; I have an ARM-based laptop, so this will happen at some point.

# License

As is noted in the source, Slenderer is released into the public domain in legal jurisdiction where such a thing exists
Where there is no such thing, it is released under the MIT License, as seen in the LICENSE file.
