@echo off
for %%X in (premake5.exe) do (set FOUND=%%~$PATH:X)
if not defined FOUND (
echo 'premake5' not found in your path.
echo Premake can be downloaded from https://premake.github.io/download.html
exit /b
)

premake5 --file=premake.lua vs2015
