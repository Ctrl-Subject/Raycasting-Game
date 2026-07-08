#pragma once

// MenuStatum `
enum STATE {
    MAIN_MENU,
    SETTINGS_MENU,
    PAUSE_MENU,
    LOBBY_MENU
};

enum SETSTATE {
    NONE,
    DISPLAY_SETTINGS_MENU,
    CONTROLS_SETTINGS_MENU,
    AUDIO_SETTINGS_MENU,
    HELP_MENU
};

extern STATE menuState;
extern SETSTATE settingState;

namespace framework {

    void init();
    void shutdown();
    void save();
    void update();

    void drawSettings();
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
}