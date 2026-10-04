#include "../include/GL/freeglut.h"
#include <GL/glui.h>
#include "UI/Framework.h"
#include "Handler/Handler.h"
#include "Game/Game.h"

#include <cstdlib>

// Id of the main game window. GLUI creates its own windows, and
// glutPostRedisplay() only redraws whichever window is current, so
// idle() switches back to this one first.
static int MainWindow = 0;

// Darkens whatever is already on screen with a see-through black quad,
// used behind the pause menu so the paused game stays visible but the
// menu stands out. alpha is 0 (no change) to 1 (fully black).
static void dimScreen(float alpha)
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f((float)width, 0.0f);
    glVertex2f((float)width, (float)height);
    glVertex2f(0.0f, (float)height);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}


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
            // The paused game is drawn first, then dimmed, then the menu
            // goes on top. game::update() is not called while paused (see
            // idle()), so this is the same frozen frame every time.
            game::render();
            dimScreen(0.6f);
            framework::drawPauseMenu();
            break;

        case SCREEN_GAME:
            game::render();
            break;

        default:
            framework::drawMainMenu();
            break;
    }

    // GLUI draws its own windows, so there is nothing to draw here for
    // the menus. The game screen draws its own HUD directly and has no
    // menu windows, so hide them all there.
    if (gHandler.Screen.GetCurrentScreen() == SCREEN_GAME)
        framework::hideUI();

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

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// ==================================================
// Mouse Input
// ==================================================
//
// GLUI handles the mouse for its own windows. In game the mouse
// is read by Game.cpp directly, so main.cpp has no mouse callbacks.
// ==================================================


// ==================================================
// Keyboard Input
// ==================================================

void KeyDown(unsigned char key, int x, int y)
{
    game::onKeyDown(key, x, y);

    if (key == 27) // Escape
    {
        Screen current = gHandler.Screen.GetCurrentScreen();

        if (current == SCREEN_GAME)
        {
            // Open the pause screen. This alone is enough to pause
            // the game loop too: idle() only calls game::update()
            // while SCREEN_GAME is current, so once we switch away
            // from it, movement/collision/physics simply stop
            // running each frame until we switch back.
            framework::pauseGame();
        }
        else if (current == SCREEN_PAUSE_MENU)
        {
            // Pressing Escape again resumes, same as the Resume button.
            framework::resumeGame();
        }
        else
        {
            // Everywhere else (main menu, settings, etc.), ESC still quits.
            framework::shutdown();
            std::exit(0);
        }
    }
}


void KeyUp(unsigned char key, int x, int y)
{
    game::onKeyUp(key, x, y);
}


void SpecialKeyDown(int key, int x, int y)
{
    game::onSpecialKeyDown(key, x, y);
}


void SpecialKeyUp(int key, int x, int y)
{
    game::onSpecialKeyUp(key, x, y);
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

    MainWindow = glutCreateWindow("Raycasting Game - Framework Test");


    // Initialise the framework/GLUI.
    framework::init();


    glClearColor(
        0.08f,
        0.08f,
        0.12f,
        1.0f
    );


    // Display
    glutDisplayFunc(display);

    // Window, update and keyboard go through GLUI_Master so GLUI can
    // share them with its own windows. Key presses made while a GLUI
    // window has focus are still passed on to KeyDown / SpecialKeyDown.
    GLUI_Master.set_glutReshapeFunc(reshape);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutKeyboardFunc(KeyDown);
    GLUI_Master.set_glutSpecialFunc(SpecialKeyDown);

    // Key releases are only needed by the game, so they stay on the
    // main window.
    glutKeyboardUpFunc(KeyUp);
    glutSpecialUpFunc(SpecialKeyUp);


    // Start application.
    glutMainLoop();


    game::shutdown();
    framework::shutdown();
    return 0;
}