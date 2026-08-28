#ifndef DEFAULT_SETTING_H_
#define DEFAULT_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

// settings values
// Display Settings
extern int   WINDOW_RESOLUTION;
extern bool  FULL_SCREEN_SETTING;
extern bool  BORDERLESS_SETTING;
extern float GAMMA_SETTING;
extern float FOV_SETTING;

// Graphics Settings
extern int  ENGINE_TYPE;
extern bool AVATARSHOW_SETTING;
extern bool TEXTURESHOW_SETTING;
extern int  FRAMERATE_SETTING;
extern bool VSYNC_SETTING;
extern bool ANTIALIASING_SETTING;
extern bool MOTIONBLUR_SETTING;

// Input Settings
extern float MOUSE_SENSITIVITY_SETTING;
extern bool INVERT_MOUSE_X_SETTING;
extern bool INVERT_MOUSE_Y_SETTING;
extern bool USE_MOUSE_SETTING;

// Controls Keys
extern int MOVE_FORWARD;
extern int MOVE_BACK;
extern int MOVE_LEFT;
extern int MOVE_RIGHT;

extern int LOOK_LEFT;
extern int LOOK_RIGHT;
extern int LOOK_UP;
extern int LOOK_DOWN;

// Audio Settings
extern float MASTER_VOL;
extern float GAME_VOL;
extern float LOBBY_VOL;
extern float SFX_VOL;

#ifdef __cplusplus
}
#endif

#endif