#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include "../Handler/TEMPSETS.h"

// ==================================================
// Framework
// ==================================================

namespace framework
{
    extern int WinWidth;
    extern int WinHeight;

    // --------------------------------------------------
    // Lifecycle
    // --------------------------------------------------

    void init();
    void shutdown();
    void update();
    void resize(int width, int height);


    // --------------------------------------------------
    // Screen drawing / visibility
    // --------------------------------------------------

    void drawMainMenu();

    void drawSettings();

    void drawDisplaySettings();
    void drawControlsSettings();
    void drawAudioSettings();
    void drawHelpMenu();

    void drawPauseMenu();


    // --------------------------------------------------
    // Settings
    // --------------------------------------------------

    struct Settings
    {
        bool AreSettingSaved = false;

        bool Load();
        void Save();
        void Reset();
    };


    // --------------------------------------------------
    // Framework object
    // --------------------------------------------------

    struct Framework
    {
        Framework();
    };
}

#endif
