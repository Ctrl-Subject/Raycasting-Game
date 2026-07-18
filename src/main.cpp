#include <GL/freeglut.h>
#include <SolarUI.h>
#include "UI/Framework.h"

void display() {
    framework::resize(framework::WinWidth, framework::WinHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (menuState) {
        case MAIN_MENU:
            framework::drawMainMenu();
            break;
        case SETTINGS_MENU:
            framework::drawSettings();
            break;
        case MULTPLAY_SETTINGS_MENU:
            framework::drawMultiplayerSetttings();
            break;
        case PAUSE_MENU:
            framework::drawPauseMenu();
            break;
        default:
            framework::drawMainMenu();
            break;
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    framework::resize(framework::WinWidth, framework::WinHeight);
}

void idle() {
    framework::update();
    glutPostRedisplay();
}

void MouseMove(int x, int y)
{
    float logicalX = 0.0f;
    float logicalY = 0.0f;
    SolarUI::ScreenToLogical(x, y, logicalX, logicalY);
    SolarUI::MouseX = logicalX;
    SolarUI::MouseY = logicalY;
}

void MouseButton(int button, int state, int x, int y)
{
    float logicalX = 0.0f;
    float logicalY = 0.0f;
    SolarUI::ScreenToLogical(x, y, logicalX, logicalY);

    SolarUI::MouseX = logicalX;
    SolarUI::MouseY = logicalY;

    if (button == GLUT_LEFT_BUTTON)
    {
        SolarUI::MouseDown = (state == GLUT_DOWN);
        if (SolarUI::MouseDown)
        {
            // put input box code here?
            return;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Raycasting Game - Framework Test");

    framework::init();

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);

    glutMainLoop();
    return 0;
}