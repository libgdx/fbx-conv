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
