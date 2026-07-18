#pragma once

#include "../Handler/TEMPSETS.h"
// MenuStatum `
enum STATE {
    MAIN_MENU,
    SETTINGS_MENU,
    MULTPLAY_SETTINGS_MENU,
    PAUSE_MENU,
    LOBBY_MENU
};

enum SETSTATE { //substate when in settings menu. preset to NONE
    NONE,
    DISPLAY_SETTINGS_MENU,
    CONTROLS_SETTINGS_MENU,
    AUDIO_SETTINGS_MENU,
    HELP_MENU
};

extern STATE menuState;
extern SETSTATE settingState;

// namespace
namespace framework {

    extern int WinWidth;
    extern int WinHeight;

    void init();
    void shutdown();
    void update();
    void resize(int width, int height);

    void drawSettings();
    void drawMultiplayerSetttings();
    void drawDisplaySettings();
    void drawControlsSettings();
    void drawAudioSettings();
    void drawHelpMenu();

    void drawMainMenu();
    void drawPauseMenu();
    void drawLobby();

    struct Framework {
        Framework();
    };

    struct Settings {
        bool AreSettingSaved = false;
        bool Load();
        void Save();
        void Reset();
    };
}