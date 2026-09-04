#include "handler.h"
#include "TEMPSETS.h"
#include "DEFAULTS.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <GL/freeglut.h>
#include <windows.h>

// Works out the full path to Config.cfg from the running
// executable's own location, rather than the current working
// directory. The working directory depends on how main.exe was
// launched (terminal, double-click, a shortcut, an IDE run
// config, ...), so a path relative to it breaks silently in some
// of those cases. Config.cfg lives next to main.exe at the
// project root, so this always finds it regardless of how the
// program was started.
static void GetConfigPath(char *outPath, size_t outSize)
{
    char exePath[MAX_PATH];

    DWORD length = GetModuleFileNameA(NULL, exePath, MAX_PATH);

    if (length == 0 || length == MAX_PATH)
    {
        snprintf(outPath, outSize, "src\\Handler\\Config.cfg");
        return;
    }

    // Strip the executable filename, leaving the origin directory.
    char *lastSlash = strrchr(exePath, '\\');

    if (lastSlash != NULL)
    {
        *(lastSlash + 1) = '\0';
    }
    else
    {
        exePath[0] = '\0';
    }

    // Add the Handler directory.
    snprintf(outPath, outSize, "%ssrc\\Handler\\Config.cfg", exePath);
}

// This enumurator is to say which possible settings are in the config file
typedef enum {
    CFG_UNKNOWN,

    CFG_RESOLUTION,
    CFG_FULLSCREEN,
    CFG_BORDERLESS,
    CFG_GAMMA,
    CFG_FOV,

    CFG_ENGINESET,
    CFG_AVATARSHOW,
    CFG_TEXTURESHOW,
    CFG_FRAMERATE,
    CFG_VSYNC,
    CFG_ANTIALIASING,
    CFG_MOTIONBLUR,

    CFG_MOUSE_SENSITIVITY,
    CFG_INVERT_MOUSE_X,
    CFG_INVERT_MOUSE_Y,
    CFG_USE_MOUSE,

    CFG_MOVE_FORWARD,
    CFG_MOVE_BACK,
    CFG_MOVE_LEFT,
    CFG_MOVE_RIGHT,

    CFG_LOOK_LEFT,
    CFG_LOOK_RIGHT,
    CFG_LOOK_UP,
    CFG_LOOK_DOWN,

    CFG_MASTER_VOLUME,
    CFG_LOBBY_VOLUME,
    CFG_GAME_VOLUME,
    CFG_SFX_VOLUME
} ConfigKey;

// This converts each line into the enum ConfigKey
// This tells me which setting the computer is looking at
static ConfigKey get_key(const char *key)
{
    if (strcmp(key, "resolution") == 0) return CFG_RESOLUTION;
    if (strcmp(key, "fullscreen") == 0) return CFG_FULLSCREEN;
    if (strcmp(key, "borderless") == 0) return CFG_BORDERLESS;
    if (strcmp(key, "gamma") == 0) return CFG_GAMMA;
    if (strcmp(key, "fov") == 0) return CFG_FOV;

    if (strcmp(key, "engine") == 0) return CFG_ENGINESET;
    if (strcmp(key, "avatarshow") == 0) return CFG_AVATARSHOW;
    if (strcmp(key, "textureshow") == 0) return CFG_TEXTURESHOW;
    if (strcmp(key, "framerate") == 0) return CFG_FRAMERATE;
    if (strcmp(key, "vsync") == 0) return CFG_VSYNC;
    if (strcmp(key, "antialiasing") == 0) return CFG_ANTIALIASING;
    if (strcmp(key, "motionblur") == 0) return CFG_MOTIONBLUR;

    if (strcmp(key, "mouse_sensitivity") == 0) return CFG_MOUSE_SENSITIVITY;
    if (strcmp(key, "invert_mouse_x") == 0) return CFG_INVERT_MOUSE_X;
    if (strcmp(key, "invert_mouse_y") == 0) return CFG_INVERT_MOUSE_Y;
    if (strcmp(key, "use_mouse") == 0) return CFG_USE_MOUSE;

    if (strcmp(key, "move_forward") == 0) return CFG_MOVE_FORWARD;
    if (strcmp(key, "move_back") == 0) return CFG_MOVE_BACK;
    if (strcmp(key, "move_left") == 0) return CFG_MOVE_LEFT;
    if (strcmp(key, "move_right") == 0) return CFG_MOVE_RIGHT;

    if (strcmp(key, "look_left") == 0) return CFG_LOOK_LEFT;
    if (strcmp(key, "look_right") == 0) return CFG_LOOK_RIGHT;
    if (strcmp(key, "look_up") == 0) return CFG_LOOK_UP;
    if (strcmp(key, "look_down") == 0) return CFG_LOOK_DOWN;
    
    if (strcmp(key, "master_volume") == 0) return CFG_MASTER_VOLUME;
    if (strcmp(key, "lobby_volume") == 0) return CFG_LOBBY_VOLUME;
    if (strcmp(key, "game_volume") == 0) return CFG_GAME_VOLUME;
    if (strcmp(key, "effects_volume") == 0) return CFG_SFX_VOLUME;

    return CFG_UNKNOWN; // if no keys are found 
}

// It is a helper function to set Keys
static int ParseKey(const char *value)
{
    if (strcmp(value, "GLUT_KEY_LEFT") == 0)      return GLUT_KEY_LEFT;
    if (strcmp(value, "GLUT_KEY_RIGHT") == 0)     return GLUT_KEY_RIGHT;
    if (strcmp(value, "GLUT_KEY_UP") == 0)        return GLUT_KEY_UP;
    if (strcmp(value, "GLUT_KEY_DOWN") == 0)      return GLUT_KEY_DOWN;

    if (strcmp(value, "GLUT_KEY_F1") == 0)        return GLUT_KEY_F1;
    if (strcmp(value, "GLUT_KEY_F2") == 0)        return GLUT_KEY_F2;
    if (strcmp(value, "GLUT_KEY_F3") == 0)        return GLUT_KEY_F3;
    if (strcmp(value, "GLUT_KEY_F4") == 0)        return GLUT_KEY_F4;
    if (strcmp(value, "GLUT_KEY_F5") == 0)        return GLUT_KEY_F5;
    if (strcmp(value, "GLUT_KEY_F6") == 0)        return GLUT_KEY_F6;
    if (strcmp(value, "GLUT_KEY_F7") == 0)        return GLUT_KEY_F7;
    if (strcmp(value, "GLUT_KEY_F8") == 0)        return GLUT_KEY_F8;
    if (strcmp(value, "GLUT_KEY_F9") == 0)        return GLUT_KEY_F9;
    if (strcmp(value, "GLUT_KEY_F10") == 0)       return GLUT_KEY_F10;
    if (strcmp(value, "GLUT_KEY_F11") == 0)       return GLUT_KEY_F11;
    if (strcmp(value, "GLUT_KEY_F12") == 0)       return GLUT_KEY_F12;

    if (strcmp(value, "GLUT_KEY_HOME") == 0)      return GLUT_KEY_HOME;
    if (strcmp(value, "GLUT_KEY_END") == 0)       return GLUT_KEY_END;
    if (strcmp(value, "GLUT_KEY_INSERT") == 0)    return GLUT_KEY_INSERT;
    if (strcmp(value, "GLUT_KEY_PAGE_UP") == 0)   return GLUT_KEY_PAGE_UP;
    if (strcmp(value, "GLUT_KEY_PAGE_DOWN") == 0) return GLUT_KEY_PAGE_DOWN;

    // Normal key (W, A, S, D, J, 1, etc.)
    return (unsigned char)value[0];
}

static const char *KeyToString(int key)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:      return "GLUT_KEY_LEFT";
        case GLUT_KEY_RIGHT:     return "GLUT_KEY_RIGHT";
        case GLUT_KEY_UP:        return "GLUT_KEY_UP";
        case GLUT_KEY_DOWN:      return "GLUT_KEY_DOWN";

        case GLUT_KEY_F1:        return "GLUT_KEY_F1";
        case GLUT_KEY_F2:        return "GLUT_KEY_F2";
        case GLUT_KEY_F3:        return "GLUT_KEY_F3";
        case GLUT_KEY_F4:        return "GLUT_KEY_F4";
        case GLUT_KEY_F5:        return "GLUT_KEY_F5";
        case GLUT_KEY_F6:        return "GLUT_KEY_F6";
        case GLUT_KEY_F7:        return "GLUT_KEY_F7";
        case GLUT_KEY_F8:        return "GLUT_KEY_F8";
        case GLUT_KEY_F9:        return "GLUT_KEY_F9";
        case GLUT_KEY_F10:       return "GLUT_KEY_F10";
        case GLUT_KEY_F11:       return "GLUT_KEY_F11";
        case GLUT_KEY_F12:       return "GLUT_KEY_F12";
        
        case GLUT_KEY_HOME:      return "GLUT_KEY_HOME";
        case GLUT_KEY_END:       return "GLUT_KEY_END";
        case GLUT_KEY_INSERT:    return "GLUT_KEY_INSERT";
        case GLUT_KEY_PAGE_UP:   return "GLUT_KEY_PAGE_UP";
        case GLUT_KEY_PAGE_DOWN: return "GLUT_KEY_PAGE_DOWN";

        default:
        {
            static char ch[2];
            ch[0] = (char)key;
            ch[1] = '\0';
            return ch;
        }
    }
}

// Load the Settings from the config file
static int Load(void) 
{
    char configPath[MAX_PATH + 32];
    GetConfigPath(configPath, sizeof(configPath));

    FILE *file = fopen(configPath, "r");
    
    // Checker to ensure the file has been opened to read
    if(file == NULL) 
    {
        perror("Could not open config file"); 
        return -1;
    }

    char line[256];

    // Scans the document printing the lines in console, and will return values of each type
    while(fgets(line, sizeof(line), file))
    {
        char key[128];
        char value[128];

        // Ignores comments and blank lines
        if (line[0] == '#' || line[0] == '\n')
            continue;

        // Parse "key=value"
        if (sscanf(line, "%127[^=]=%127s", key, value) != 2)
            continue;
        
        //Sets keys to TEMPSETS.h
        switch (get_key(key))
        {
                
            case CFG_RESOLUTION:
                WINDOW_RESOLUTION = (int)atof(value);
                break;
                
            case CFG_FULLSCREEN:
                FULL_SCREEN_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_BORDERLESS:
                BORDERLESS_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_GAMMA:
                GAMMA_SETTING = (float)atof(value);
                break;

            case CFG_FOV:
                FOV_SETTING = (float)atof(value);
                break;

            case CFG_AVATARSHOW:
                AVATARSHOW_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_ENGINESET:
            {
                int parsedEngine = (int)atof(value);

                // Sanity clamp against an obviously corrupt save
                // (negative, or wildly out of range - like the
                // engine=256 that was crashing the Display
                // screen). Handler.c doesn't know the UI
                // dropdown's real option count, so this is just a
                // first line of defense; solDropdown_SetSelectedIndex
                // clamps again against the dropdown's actual
                // OptionCount when this value is loaded into it.
                if (parsedEngine < 0 || parsedEngine > 63)
                {
                    parsedEngine = DEFAULT_ENGINE_SETTING;
                }

                ENGINE_TYPE = parsedEngine;
                break;
            }

            case CFG_TEXTURESHOW:
                TEXTURESHOW_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_FRAMERATE:
                FRAMERATE_SETTING = atoi(value);
                break;

            case CFG_VSYNC:
                VSYNC_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_ANTIALIASING:
                ANTIALIASING_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_MOTIONBLUR:
                MOTIONBLUR_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_MOUSE_SENSITIVITY:
                MOUSE_SENSITIVITY_SETTING = (float)atof(value);
                break;

            case CFG_INVERT_MOUSE_X:
                INVERT_MOUSE_X_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_INVERT_MOUSE_Y:
                INVERT_MOUSE_Y_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_USE_MOUSE:
                USE_MOUSE_SETTING = strcmp(value, "true") == 0;
                break;

            case CFG_MOVE_FORWARD:
                MOVE_FORWARD = ParseKey(value);
                break;

            case CFG_MOVE_BACK:
                MOVE_BACK = ParseKey(value);
                break;

            case CFG_MOVE_LEFT:
                MOVE_LEFT = ParseKey(value);
                break;

            case CFG_MOVE_RIGHT:
                MOVE_RIGHT = ParseKey(value);
                break;

            case CFG_LOOK_LEFT:
                LOOK_LEFT = ParseKey(value);
                break;

            case CFG_LOOK_RIGHT:
                LOOK_RIGHT = ParseKey(value);
                break;

            case CFG_LOOK_UP:
                LOOK_UP = ParseKey(value);
                break;

            case CFG_LOOK_DOWN:
                LOOK_DOWN = ParseKey(value);
                break;
            
            case CFG_MASTER_VOLUME:
                MASTER_VOL = (float)atof(value);
                break;
            
            case CFG_LOBBY_VOLUME:
                LOBBY_VOL = (float)atof(value);
                break;
            
            case CFG_GAME_VOLUME:
                GAME_VOL = (float)atof(value);
                break;
                
            case CFG_SFX_VOLUME:
                SFX_VOL = (float)atof(value);
                break;

            default:
                printf("Unknown setting: %s\n", key);
                break;
        }
        
    }
    
    // closes file
    fclose(file);
    return 0;
}

// saves settings to the config file
static int Save(void)
{
    char configPath[MAX_PATH + 32];
    GetConfigPath(configPath, sizeof(configPath));

    FILE *file = fopen(configPath, "w");

    if (file == NULL)
    {
        perror("Could not open config file");
        return -1;
    }

    fprintf(file, "# Display Settings\n");
    fprintf(file, "resolution=%d\n", WINDOW_RESOLUTION);
    fprintf(file, "fullscreen=%s\n", FULL_SCREEN_SETTING ? "true" : "false");
    fprintf(file, "borderless=%s\n", BORDERLESS_SETTING ? "true" : "false");
    fprintf(file, "gamma=%f\n", GAMMA_SETTING);
    fprintf(file, "fov=%f\n\n", FOV_SETTING);

    fprintf(file, "# Graphics Settings\n");
    fprintf(file, "engine=%d\n", ENGINE_TYPE);
    fprintf(file, "avatarshow=%s\n", AVATARSHOW_SETTING ? "true" : "false");
    fprintf(file, "textureshow=%s\n", TEXTURESHOW_SETTING ? "true" : "false");
    fprintf(file, "framerate=%d\n", FRAMERATE_SETTING);
    fprintf(file, "vsync=%s\n", VSYNC_SETTING ? "true" : "false");
    fprintf(file, "antialiasing=%s\n", ANTIALIASING_SETTING ? "true" : "false");
    fprintf(file, "motionblur=%s\n\n", MOTIONBLUR_SETTING ? "true" : "false");

    fprintf(file, "# Input Settings\n");
    fprintf(file, "mouse_sensitivity=%f\n", MOUSE_SENSITIVITY_SETTING);
    fprintf(file, "invert_mouse_x=%s\n", INVERT_MOUSE_X_SETTING ? "true" : "false");
    fprintf(file, "invert_mouse_y=%s\n", INVERT_MOUSE_Y_SETTING ? "true" : "false");
    fprintf(file, "use_mouse=%s\n\n", USE_MOUSE_SETTING ? "true" : "false");

    fprintf(file, "# Control Keys\n");
    fprintf(file, "move_forward=%s\n", KeyToString(MOVE_FORWARD));
    fprintf(file, "move_back=%s\n", KeyToString(MOVE_BACK));
    fprintf(file, "move_left=%s\n", KeyToString(MOVE_LEFT));
    fprintf(file, "move_right=%s\n", KeyToString(MOVE_RIGHT));

    fprintf(file, "look_left=%s\n", KeyToString(LOOK_LEFT));
    fprintf(file, "look_right=%s\n", KeyToString(LOOK_RIGHT));
    fprintf(file, "look_up=%s\n", KeyToString(LOOK_UP));
    fprintf(file, "look_down=%s\n\n", KeyToString(LOOK_DOWN));

    fprintf(file, "# Volume Settings\n");
    fprintf(file, "master_volume=%f\n", MASTER_VOL);
    fprintf(file, "lobby_volume=%f\n", LOBBY_VOL);
    fprintf(file, "game_volume=%f\n", GAME_VOL);
    fprintf(file, "effects_volume=%f\n", SFX_VOL);

    fclose(file);

    return 0;
}

// resets the config file
static int Reset(void)
{
    char configPath[MAX_PATH + 32];
    GetConfigPath(configPath, sizeof(configPath));

    FILE *file = fopen(configPath, "w");

    if (file == NULL)
    {
        perror("Could not open config file");
        return -1;
    }

    fprintf(file, "# Display Settings\n");
    fprintf(file, "resolution=%d\n", DEFAULT_RESOLUTION_SETTING);
    fprintf(file, "fullscreen=%s\n", DEFAULT_FULL_SCREEN_SETTING);
    fprintf(file, "borderless=%s\n", DEFAULT_BORDERLESS_SETTING);
    fprintf(file, "gamma=%f\n", DEFAULT_GAMMA_SETTING);
    fprintf(file, "fov=%f\n\n", DEFAULT_FOV_SETTING);

    fprintf(file, "# Graphics Settings\n");
    fprintf(file, "engine=%d\n", DEFAULT_ENGINE_SETTING);
    fprintf(file, "avatarshow=%s\n", DEFAULT_AVATARSHOW_SETTING);
    fprintf(file, "textureshow=%s\n", DEFAULT_TEXTURESHOW_SETTING);
    fprintf(file, "framerate=%d\n", DEFAULT_FRAMERATE_SETTING);
    fprintf(file, "vsync=%s\n", DEFAULT_VSYNC_SETTING);
    fprintf(file, "antialiasing=%s\n", DEFAULT_ANTIALIASING_SETTING);
    fprintf(file, "motionblur=%s\n\n", DEFAULT_MOTIONBLUR_SETTING);

    fprintf(file, "# Input Settings\n");
    fprintf(file, "mouse_sensitivity=%f\n", DEFAULT_MOUSE_SENSITIVITY_SETTING);
    fprintf(file, "invert_mouse_x=%s\n", DEFAULT_INVERT_MOUSE_X_SETTING);
    fprintf(file, "invert_mouse_y=%s\n", DEFAULT_INVERT_MOUSE_Y_SETTING);
    fprintf(file, "use_mouse=%s\n\n", DEFAULT_USE_MOUSE_SETTING);

    fprintf(file, "# Control Keys\n");
    fprintf(file, "move_forward=%s\n", DEFAULT_MOVE_FORWARD);
    fprintf(file, "move_back=%s\n", DEFAULT_MOVE_BACK);
    fprintf(file, "move_left=%s\n", DEFAULT_MOVE_LEFT);
    fprintf(file, "move_right=%s\n", DEFAULT_MOVE_RIGHT);

    fprintf(file, "look_left=%s\n", DEFAULT_LOOK_LEFT);
    fprintf(file, "look_right=%s\n", DEFAULT_LOOK_RIGHT);
    fprintf(file, "look_up=%s\n", DEFAULT_LOOK_UP);
    fprintf(file, "look_down=%s\n\n", DEFAULT_LOOK_DOWN);

    fprintf(file, "# Volume Settings\n");
    fprintf(file, "master_volume=%f\n", DEFAULT_MAS_VOL);
    fprintf(file, "lobby_volume=%f\n", DEFAULT_LOB_VOL);
    fprintf(file, "game_volume=%f\n", DEFAULT_GAME_VOL);
    fprintf(file, "effects_volume=%f\n", DEFAULT_SFX_VOL);

    fclose(file);

    return 0;
}

static Screen currentScreen = SCREEN_MAIN_MENU;
static Screen previousScreen = SCREEN_MAIN_MENU;

static void Screen_Init(void)
{
    currentScreen = SCREEN_MAIN_MENU;
    previousScreen = SCREEN_MAIN_MENU;
}

static void Screen_SetScreen(Screen screen)
{
    previousScreen = currentScreen;
    currentScreen = screen;
}

static void Screen_GoBack(void)
{
    currentScreen = previousScreen;
}

static Screen Screen_GetCurrentScreen(void)
{
    return currentScreen;
}

// Defines each part of the handler to the C files
const Handler gHandler = 
{
    .Screen = {
        .SetScreen = Screen_SetScreen,
        .GoBack = Screen_GoBack,
        .GetCurrentScreen = Screen_GetCurrentScreen,
        .Init = Screen_Init
    },
    .Settings = {
        .Load  = Load,
        .Save  = Save,
        .Reset = Reset
    }
};