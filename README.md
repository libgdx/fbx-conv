fbx-conv
========

Command line utility using the FBX SDK to convert FBX/Collada/Obj files
to more runtime friendly formats. The FBX content is parsed into an
in-memory datastructure. Pluggable writers then take this datastructure
to generate the output. Send us a pull request if you want the writer
for your engine/framework/app to be integrated. We'll build the
converter for Windows, Linux and Mac.

The FBX parser is largely based on GamePlay SDK's encoder. We'll try to 
back-port any bug fixes or improvements.

Hangout notes https://docs.google.com/document/d/1nz-RexbymNtA4pW1B5tXays0tjByBvO8BJSKrWeU69g/edit#

Precompiled Binaries
====================
You can download the precompiled binaries from http://libgdx.badlogicgames.com/fbx-conv

The binaries are recompiled on any changes in the Git repository, via our trusty Jenkins instance, see http://libgdx.badlogicgames.com:8080/

On Windows you'll need to install VC 2010 Redistributable Package http://www.microsoft.com/en-us/download/confirmation.aspx?id=5555

On Linux and Mac, we have to link to the dynamic libraries of the FBX SDK (libfbxsdk.so and libfbxsdk.dylib). We recommend copying libfbxsdk.so
to /usr/lib on Linux. Otherwise you can use LD_LIBRARY_PATH and set it to the directory you put the .so file.

Building
========
You'll need premake and an installation of the FBX SDK 2014. Once installed/downloaded, set the
FBX_SDK_ROOT to the directory where you installed the FBX SDK. Then run one of the 
generate_XXX scripts. These will generate a Visual Studio/XCode project, or a Makefile.
