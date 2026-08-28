#include <GL/freeglut.h>
#include "DEFAULTS.h"

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
bool  AVATARSHOW_SETTING         = DEFAULT_AVATARSHOW_SETTING;
bool  TEXTURESHOW_SETTING        = DEFAULT_TEXTURESHOW_SETTING;
int   FRAMERATE_SETTING          = DEFAULT_FRAMERATE_SETTING;
bool  VSYNC_SETTING              = DEFAULT_VSYNC_SETTING;
bool  ANTIALIASING_SETTING       = DEFAULT_ANTIALIASING_SETTING;
bool  MOTIONBLUR_SETTING         = DEFAULT_MOTIONBLUR_SETTING;

// Input Settings
float MOUSE_SENSITIVITY_SETTING  = DEFAULT_MOUSE_SENSITIVITY_SETTING;
bool INVERT_MOUSE_X_SETTING      = DEFAULT_INVERT_MOUSE_X_SETTING;
bool INVERT_MOUSE_Y_SETTING      = DEFAULT_INVERT_MOUSE_Y_SETTING;
bool  USE_MOUSE_SETTING          = DEFAULT_USE_MOUSE_SETTING;

// Controls Keys
int MOVE_FORWARD                 = 'w';
int MOVE_BACK                    = 's';
int MOVE_LEFT                    = 'a';
int MOVE_RIGHT                   = 'd';
int LOOK_LEFT                    = GLUT_KEY_LEFT;
int LOOK_RIGHT                   = GLUT_KEY_RIGHT;
int LOOK_UP                      = GLUT_KEY_UP;
int LOOK_DOWN                    = GLUT_KEY_DOWN;

// Volume Settings
float MASTER_VOL                 = 1.0f;
float GAME_VOL                   = 1.0f;
float LOBBY_VOL                  = 1.0f;
float SFX_VOL                    = 1.0f;