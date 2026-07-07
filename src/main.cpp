#include <GL/freeglut.h>
#include <SolarUI.h>
#include "UI/Framework.h"

namespace {
    void display() {
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
    }

    void idle() {
        framework::update();
        glutPostRedisplay();
    }

    void mouseMotion(int x, int y) {
        SolarUI::MouseX = static_cast<float>(x);
        SolarUI::MouseY = static_cast<float>(y);
    }

    void mouseButton(int button, int state, int x, int y) {
        mouseMotion(x, y);

        if (button == GLUT_LEFT_BUTTON) {
            SolarUI::MouseDown = (state == GLUT_DOWN);
            SolarUI::MousePressed = (state == GLUT_DOWN);
        } else {
            SolarUI::MouseDown = false;
        }
    }

    void keyboard(unsigned char key, int, int) {
        if (key == 27) {
            framework::shutdown();
            exit(0);
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
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}