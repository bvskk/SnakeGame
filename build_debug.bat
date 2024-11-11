@echo off

setlocal
set cflags=/EHsc /std:c11 /Od /Zi /utf-8 /validate-charset /W3 /sdl /MD /I%USERPROFILE%\source\raylib-5.0_win64_msvc16\include
set linkflags=/link /DEBUG /LIBPATH:%USERPROFILE%\source\raylib-5.0_win64_msvc16\lib raylib.lib user32.lib gdi32.lib shell32.lib winmm.lib
set srcfiles=..\snake.c

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64

if not exist build mkdir build
pushd build

call cl %cflags% %srcfiles% %linkflags%

popd