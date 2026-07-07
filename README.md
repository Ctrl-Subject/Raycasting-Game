# Raycasting Game

A 3D raycasting game developed as part of my OCR Computer Science NEA (Non-Exam Assessment).

This repository contains the game's source code, along with a copy of the project documentation. The documentation has been included for reference, with the contacts page omitted to protect personal information.

current compilation command
g++ -std=c++17 -Iinclude -Iextern/freeglut/include -Lextern/freeglut/lib src/main.cpp src/UI/Framework.cpp -lfreeglut -lSolarUI -lopengl32 -lgdi32 -luser32 -lshell32 -lwinmm -o RaycastingGame.exe