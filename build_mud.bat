@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d "D:\mud"
if not exist build mkdir build
cl /nologo /std:c++17 /utf-8 /EHsc /W4 /I. /Fe:build\mud_game.exe *.cpp
