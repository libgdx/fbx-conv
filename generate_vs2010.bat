@echo off
for %%X in (premake5.exe) do (set FOUND=%%~$PATH:X)
if not defined FOUND (
echo 'premake5' not found in your path.
echo Premake can be downloaded from http://industriousone.com/premake/
exit /b
)

premake5 --file=premake.lua vs2013
