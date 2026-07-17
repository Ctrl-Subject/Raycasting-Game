#include "TEMPSETS.h"
#include "DEFAULTS.h"

#include <GL/freeglut.h>

// Actual storage for the settings declared `extern` in TEMPSETS.h.
// (TEMPSETS.h only promises these exist somewhere - this is that "somewhere".)

// Display Settings
int   WINDOW_RESOLUTION          = DEFAULT_RESOLUTION_SETTING;
bool  FULL_SCREEN_SETTING        = false;
bool  BORDERLESS_SETTING         = false;
float GAMMA_SETTING              = DEFAULT_GAMMA_SETTING;
float FOV_SETTING                = DEFAULT_FOV_SETTING;

// Graphics Settings
bool  ENGINE_TYPE                = DEFAULT_ENGINE_SETTING;
bool  AVATARSHOW_SETTING         = true;
bool  TEXTURESHOW_SETTING        = true;
int   FRAMERATE_SETTING          = DEFAULT_FRAMERATE_SETTING;
bool  VSYNC_SETTING              = true;
bool  ANTIALIASING_SETTING       = true;
bool  MOTIONBLUR_SETTING         = true;

// Input Settings
float MOUSE_SENSITIVITY_SETTING  = DEFAULT_MOUSE_SENSITIVITY_SETTING;
bool  INVERT_MOUSE_Y_SETTING     = false;

// Controls Keys
int MOVE_FORWARD = 'w';
int MOVE_BACK    = 's';
int MOVE_LEFT    = 'a';
int MOVE_RIGHT   = 'd';

int LOOK_LEFT  = GLUT_KEY_LEFT;
int LOOK_RIGHT = GLUT_KEY_RIGHT;
int LOOK_UP    = GLUT_KEY_UP;
int LOOK_DOWN  = GLUT_KEY_DOWN;
