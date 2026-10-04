#include "../include/GL/freeglut.h"
#include <GL/glui.h>
#include "UI/Framework.h"
#include "Handler/Handler.h"
#include "Game/Game.h"

#include <cstdlib>

static int MainWindow = 0;

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

    if (gHandler.Screen.GetCurrentScreen() == SCREEN_GAME)
        framework::hideUI();

    glutSwapBuffers();
}






void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, width, height, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}






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





void KeyDown(unsigned char key, int x, int y)
{
    game::onKeyDown(key, x, y);

    if (key == 27) 
    {
        Screen current = gHandler.Screen.GetCurrentScreen();

        if (current == SCREEN_GAME)
        {
            framework::pauseGame();
        }
        else if (current == SCREEN_PAUSE_MENU)
        {
            framework::resumeGame();
        }
        else
        {
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

    framework::init();
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glutDisplayFunc(display);

    GLUI_Master.set_glutReshapeFunc(reshape);
    GLUI_Master.set_glutIdleFunc(idle);
    GLUI_Master.set_glutKeyboardFunc(KeyDown);
    GLUI_Master.set_glutSpecialFunc(SpecialKeyDown);

    glutKeyboardUpFunc(KeyUp);
    glutSpecialUpFunc(SpecialKeyUp);
    glutMainLoop();

    game::shutdown();
    framework::shutdown();
    return 0;
}