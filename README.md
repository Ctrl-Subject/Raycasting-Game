# Raycasting Game

A 3D raycasting game developed as part of my OCR Computer Science NEA (Non-Exam Assessment).

This repository contains the game's source code, along with a copy of the project documentation. The documentation has been included for reference, with the contacts page omitted to protect personal information.

current compilation command
g++ "src/main.cpp" "src/UI/Framework.cpp" -std=c++17 -o main.exe -I "src" -I "extern/SolarUI/include" -L "extern/SolarUI/lib/x64" -lSolarUI -lfreeglut -lopengl32 -lglu32