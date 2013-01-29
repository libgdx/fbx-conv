@echo off
for %%X in (premake4.exe) do (set FOUND=%%~$PATH:X)
if not defined FOUND (
echo 'premake4' not found in your path.
echo Premake can be downloaded from http://industriousone.com/premake/
exit /b
)

premake4 --file=premake.lua vs2010
