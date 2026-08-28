#include "../include/GL/freeglut.h"
#include "../include/SUI/SolarUI.h"
#include "UI/Framework.h"
#include "Handler/Handler.h"
#include "Game/Game.h"

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (gHandler.Screen.GetCurrentScreen())
    {
        case SCREEN_MAIN_MENU:
            framework::drawMainMenu();
            break;

        case SCREEN_SETTINGS:
            framework::drawSettings();
            break;

        case SCREEN_DISPLAY_SETTINGS:
            framework::drawDisplaySettings();
            break;

        case SCREEN_CONTROLS_SETTINGS:
            framework::drawControlsSettings();
            break;

        case SCREEN_AUDIO_SETTINGS:
            framework::drawAudioSettings();
            break;

        case SCREEN_HELP:
            framework::drawHelpMenu();
            break;

        case SCREEN_PAUSE_MENU:
            framework::drawPauseMenu();
            break;

        case SCREEN_GAME:
            game::render();
            break;

        default:
            framework::drawMainMenu();
            break;
    }

    // SolarUI draws all registered/visible elements.
    // (Game screen draws its own HUD directly, so it doesn't need
    // SolarUI's overlay - skip it there to avoid drawing stray menu
    // elements over the raycast view. SolarUI's draw call appears to
    // also perform the buffer swap internally, so we do that
    // ourselves here instead when skipping it.)
    if (gHandler.Screen.GetCurrentScreen() != SCREEN_GAME)
        solUI_Draw();
    else
        glutSwapBuffers();
    
}


// ==================================================
// Window Resize
// ==================================================

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, width, height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    solUI_UpdateViewport(width, height);
}


// ==================================================
// Update
// ==================================================

void idle()
{
    static int lastMs = glutGet(GLUT_ELAPSED_TIME);
    int nowMs = glutGet(GLUT_ELAPSED_TIME);
    float dt = (nowMs - lastMs) / 1000.0f;
    lastMs = nowMs;

    if (gHandler.Screen.GetCurrentScreen() == SCREEN_GAME)
        game::update(dt);
    else
        framework::update();

    glutPostRedisplay();
}


// ==================================================
// Mouse Input
// ==================================================

void MouseMove(int x, int y)
{
    solInputBridge_MouseMove(x, y);
}


void MouseButton(int button, int state, int x, int y)
{
    solInputBridge_MouseButton(button, state, x, y);
}


// ==================================================
// Keyboard Input
// ==================================================

void KeyDown(unsigned char key, int x, int y)
{
    solInputBridge_KeyDown(key, x, y);

    // ESC can still be used to quit.
    if (key == 27)
    {
        solUI_Shutdown();
        std::exit(0);
    }
}


void KeyUp(unsigned char key, int x, int y)
{
    solInputBridge_KeyUp(key, x, y);
}


void SpecialKeyDown(int key, int x, int y)
{
    solInputBridge_SpecialKey(key, x, y);
}


void SpecialKeyUp(int key, int x, int y)
{
    solInputBridge_SpecialKeyUp(key, x, y);
}


// ==================================================
// Main
// ==================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_RGBA |
        GLUT_DOUBLE |
        GLUT_DEPTH
    );

    glutInitWindowSize(1280, 720);

    glutCreateWindow("Raycasting Game - Framework Test");


    // Initialise the framework/SolarUI.
    framework::init();


    glClearColor(
        0.08f,
        0.08f,
        0.12f,
        1.0f
    );


    // Display
    glutDisplayFunc(display);

    // Window
    glutReshapeFunc(reshape);

    // Update
    glutIdleFunc(idle);

    // Mouse
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);

    // Keyboard
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialKeyDown);
    glutSpecialUpFunc(SpecialKeyUp);


    // Start application.
    glutMainLoop();


    game::shutdown();
    solUI_Shutdown();
    return 0;
}


