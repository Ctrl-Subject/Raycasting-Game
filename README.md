# Raycasting Game

A 3D raycasting game developed as part of my OCR Computer Science NEA (Non-Exam Assessment).

This repository contains the game's source code, along with a copy of the project documentation. The documentation has been included for reference, with the contacts page omitted to protect personal information.

current compilation command
g++ -Iinclude -Iinclude/SUI src/main.cpp src/UI/Framework.cpp src/Handler/Handler.c src/Handler/TEMPSETS.c -Llibs -lSolarUI -lfreeglut -lopengl32 -lglu32 -o main.exe