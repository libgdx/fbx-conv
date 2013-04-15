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
You can download the precompiled binaries from http://libgdx.badlogicgames.com/downloads/fbx-conv

On Windows you'll need to install VC 2010 Redistributable Package http://www.microsoft.com/en-us/download/confirmation.aspx?id=5555

Building
========
You'll need premake and an installation of the FBX SDK. Once installed/downloaded, set the
FBX_SDK_ROOT to the directory where you installed the FBX SDK. Then run one of the 
generate_XXX scripts. These will generate a Visual Studio/XCode project, or a Makefile.
