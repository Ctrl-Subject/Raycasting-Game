#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include "../Handler/TEMPSETS.h"





namespace framework
{
    extern int WinWidth;
    extern int WinHeight;

    void init();
    void shutdown();
    void update();
    void resize(int width, int height);

    void drawMainMenu();

    void drawSettings();

    void drawDisplaySettings();
    void drawControlsSettings();
    void drawAudioSettings();
    void drawHelpMenu();

    void drawPauseMenu();

    
    void hideUI();

    void pauseGame();
    void resumeGame();

    struct Settings
    {
        bool AreSettingSaved = false;

        bool Load();
        void Save();
        void Reset();
    };

    struct Framework
    {
        Framework();
    };
}

#endif