Trillek Core
==============

Requires CMake and one of the following compilers:

* GCC 4.8 or newer;
* Clang 3.3 or newer;
* Visual Studio 2010 or newer;
e
Installing
===

Sigma, and so Trillek, requires the following dependencies:
* [GLEW](http://glew.sourceforge.net) 1.5.2 or newer, not required on OS X;
* [GLFW](http://www.glfw.org) 3.0.0 or newer;
* [GLM](http://glm.g-truc.net);
* [SOIL](http://www.lonesock.net/soil.html);
* [Bullet](http://www.bulletphysics.org);
* [Chromium Embedded Framework](http://code.google.com/p/chromiumembedded) 3;
* An OpenAL API implementation;
* [libogg](https://www.xiph.org/ogg/);
* [libvorbis](https://www.xiph.org/ogg/);

Sigma on Linux also requires [GTK+ 2](http://www.gtk.org), due to usage of Chromium Embedded Framework.


You'll also need a [package of assets](http://wiki.trillek.org/wiki/Assets).  Unpack it in the build/bin/ directory.

## Before Building ##

You must execute :

    git submodule init
    git submodule update
    
This will grab latest Sigma and Virtual Computer as git submodules
Also, temporary, you need to replace line 21 of /modules/sigma/CMakeLists.txt by :

    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")
    
If not, cmake will give an error when process Sigma.

## Building ##

Use CMake to create makefiles or projects. The resulting executable will be saved in build/bin/.

Running
===

__(Unix/Linux/OS X)__

The binary `**Trillek**` is written to `build/bin`, `build/bin/debug`, or `build/debug/bin`

On OS X, you must launch **Trillek** from a shell in the build directory.  Launching **Trillek** through the Finder will not set the working directory correctly.

__Visual Studio__

You must change the startup project via right-clicking on the project in the solution explorer in order for Debugging to launch the correct program.
Also you must go into the project's properties, and under the 'Debugging' category change the 'Working Directory' to '$(OutDir)'.

__Xcode__

On OS X, CMake can generate an Xcode project for Trillek.

```sh
mkdir build/
cd build/
cmake .. -G Xcode
```

You must change the current scheme to Sigma by clicking on the scheme popup menu and selecting Sigma.  You can also change the working directory used when Sigma is started by Xcode.  Select Edit Scheme from the scheme popup menu, switch to the Options tab, check the box next to Working Directory, and enter the path to the Sigma assets in the text field.


Directory structure
===

* **/modules/** -> Here resides all modules that will be used by Trillek as git submodules or in directories. This includes the Virtual Computer and Sigma
* **/modules/core/** -> Trillek core module from where the main executable will be build and some glue code will be live

