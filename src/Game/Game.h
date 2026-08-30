#pragma once

// ======================================================================
// Game.h
//
// Wraps RCUT's low-level Raycaster API (NOT RCUT_Engine_*, which owns
// its own window/GLUT loop and would clash with the freeglut window
// SolarUI already created in main.cpp). This module assumes an OpenGL
// context + freeglut window already exist and just draws into them.
// ======================================================================

namespace game
{
    // Call once, when the player presses "Start Game" (transition INTO
    // SCREEN_GAME). Loads the map, textures, spawns ghost/orb/fruit
    // sprites, and resets lives to full.
    void init();

    // Call every frame while SCREEN_GAME is active, before render().
    // Handles movement input and collision against ghosts/orbs/fruit.
    void update(float dt);

    // Call every frame while SCREEN_GAME is active, from display().
    // Renders the raycast view + a simple lives HUD.
    void render();

    // Call once when leaving SCREEN_GAME for good (e.g. app shutdown).
    void shutdown();

    int  getLives();
    bool isGameOver();

    // Re-rolls the fruit's position using the given seed. Called
    // automatically by init() with a time-based seed; exposed here so
    // you can pass a fixed seed for repeatable testing.
    void setFruitSeed(unsigned int seed);
}
