@echo off
g++ -o ..\..\hasilCompile\tugas_akhir\network_viz.exe network_viz.cpp ..\glad.c -I..\..\include -L..\..\lib -lglfw3 -lopengl32 -lgdi32 -std=c++17
if %errorlevel%==0 ( echo Build OK ) else ( echo Build FAILED )
pause
