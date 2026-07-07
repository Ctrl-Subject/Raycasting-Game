#include "Framework.h"

#include <SolarUI.h>
#include <iostream>
#include <cstdlib>

STATE menuState = MAIN_MENU;
SETSTATE settingState = NONE;

// Main Menu stuff
SolarUI::Label mainMenuTitle                    ("Main Menu", 50, 50);
SolarUI::Button startButton                     (50, 75, 200, 50, "Start Game");
SolarUI::Button settingsButton                  (50, 150, 200, 50, "Settings");
SolarUI::Button MultiplayerSettings             (50, 225, 200, 50, "Multiplayer Settings");
SolarUI::Button exitButton                      (50, 300, 200, 50, "Exit");


// ===================================
// =          settings stuff         =
// ===================================


// Settings Menu stuff
SolarUI::Label settingsTitle                    ("Settings", 50, 100);
SolarUI::Button displaySettings                 (50, 75, 200, 50, "Display");
SolarUI::Button controlsSettings                (50, 150, 200, 50, "Controls");
SolarUI::Button audioSettings                   (50, 225, 200, 50, "Audio");
SolarUI::Button saveButton                      (50, 300, 200, 50, "Save");
SolarUI::Button backButton                      (50, 375, 200, 50, "Back");
SolarUI::Button helpButton                      (50, 450, 200, 50, "Help");


// Display Settings stuff ********************************************************
SolarUI::Label displaySettingsTitle             ("Display", 300, 100);

/* Resolution Dropdown */

SolarUI::Checkbox fullscreenCheckbox            (300, 200, 20, 20, "Fullscreen");
SolarUI::Checkbox borderlessFullscreenCheckbox  (300, 225, 20, 20, "Borderless Fullscreen");
SolarUI::Slider gammaSlider                     (300, 250, 200, 20, "Gamma");
SolarUI::Slider fovSlider                       (300, 500, 200, 20, "Field of View");

/* Renderer Engine Dropdown */

SolarUI::Label graphicsSettingsTitle            ("Graphics", 500, 100);

/* Graphics Quality Dropdown */

SolarUI::Checkbox AvatarsCheckbox               (500, 300, 20, 20, "Display Avatars");
SolarUI::Checkbox TexturesCheckbox              (500, 350, 20, 20, "Display WallTextures");
SolarUI::Slider framerateSlider                 (500, 400, 200, 20, "Framerate Limit");
SolarUI::Checkbox vsyncCheckbox                 (500, 450, 20, 20, "V-Sync");
SolarUI::Checkbox antiAliasingCheckbox          (500, 500, 20, 20, "Anti-Aliasing");
SolarUI::Checkbox motionBlurCheckbox            (500, 550, 20, 20, "Motion Blur");

// *******************************************************************************


// Controls Settings stuff *******************************************************

// *******************************************************************************


// Audio Settings stuff

SolarUI::Slider masterVolumeSlider              (100, 200, 200, 20, "Master Volume");
SolarUI::Slider lobbyMusicVolumeSlider          (100, 250, 200, 20, "Lobby Music Volume");
SolarUI::Slider inGameMusicVolumeSlider         (100, 300, 200, 20, "Game Music Volume");
SolarUI::Slider sfxVolumeSlider                 (100, 350, 200, 20, "Sound Effects Volume");

// ***************************************************************************


// Help Menu stuff ***************************************************************

SolarUI::Label helpTitle                        ("Help", 100, 100);

/* Help Content Text Box
    Contents:
    How to setup multiplayer, basic controls, and other information. 
    also the License information for the game and the libraries used in the game.
*/

// *******************************************************************************


// Implementation of the framework functions and methods

namespace {
    void handleMainMenuClicks() {
        switch(menuState) {
            case MAIN_MENU:
                if (SolarUI::MousePressed && startButton.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Start Game clicked\n";
                    menuState = LOBBY_MENU;
                }

                if (SolarUI::MousePressed && settingsButton.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Settings clicked\n";
                    menuState = SETTINGS_MENU;
                }

                if (SolarUI::MousePressed && exitButton.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Exit clicked\n";
                    std::exit(0);
                }
                break;

            case SETTINGS_MENU:
                if (SolarUI::MousePressed && backButton.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Back clicked\n";
                    menuState = MAIN_MENU;
                    settingState = NONE;
                }

                if (SolarUI::MousePressed && displaySettings.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Display Settings clicked\n";
                    settingState = DISPLAY_SETTINGS_MENU;
                }

                if (SolarUI::MousePressed && controlsSettings.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Controls Settings clicked\n";
                    settingState = CONTROLS_SETTINGS_MENU;
                }

                if (SolarUI::MousePressed && audioSettings.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Audio Settings clicked\n";
                    settingState = AUDIO_SETTINGS_MENU;
                }

                if (SolarUI::MousePressed && helpButton.Contains(static_cast<int>(SolarUI::MouseX), static_cast<int>(SolarUI::MouseY))) {
                    std::cout << "Help clicked\n";
                    settingState = HELP_MENU;
                }

                break;
            case PAUSE_MENU:
                break;
            default:
                break;
        }
    }
}

namespace framework {

    void init() {
        // Sets defaults for the UI framework, initializes necessary components
        SolarUI::Init();
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
    }

    void update() {
        // Update the UI framework state, handle input, etc.
        SolarUI::Update();
        handleMainMenuClicks();
        SolarUI::MousePressed = false;
    }

    void shutdown() {
        // Cleanup code here
        return;
    }

    void save() {
        // Save state code here
        return;
    }

    Framework::Framework() {
        // Constructor implementation
        init();
    }

    void drawSettings() {
        // Code to draw the settings menu
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
        settingsTitle.Draw();
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_10);
        displaySettings.Draw();
        controlsSettings.Draw();
        audioSettings.Draw();
        saveButton.Draw();
        backButton.Draw();
        helpButton.Draw();

        switch (settingState) {
            case DISPLAY_SETTINGS_MENU:
                drawDisplaySettings();
                break;
            case CONTROLS_SETTINGS_MENU:
                drawControlsSettings();
                break;
            case AUDIO_SETTINGS_MENU:
                drawAudioSettings();
                break;
            case HELP_MENU:
                drawHelpMenu();
                break;
            default:
                break;
        }
    }

    void drawDisplaySettings() {
        // Code to draw the display settings
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
        displaySettingsTitle.Draw();
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_10);
        fullscreenCheckbox.Draw();
        borderlessFullscreenCheckbox.Draw();
        gammaSlider.Draw();
        fovSlider.Draw();
        
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
        graphicsSettingsTitle.Draw();
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_10);
        AvatarsCheckbox.Draw();
        TexturesCheckbox.Draw();
        framerateSlider.Draw();
        vsyncCheckbox.Draw();
        antiAliasingCheckbox.Draw();
        motionBlurCheckbox.Draw();

    }

    void drawControlsSettings() {
    }

    void drawAudioSettings() {
        // Code to draw the audio settings
        masterVolumeSlider.Draw();
        lobbyMusicVolumeSlider.Draw();
        inGameMusicVolumeSlider.Draw();
        sfxVolumeSlider.Draw();
    }

    void drawHelpMenu() {
        // Code to draw the help menu
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
        helpTitle.Draw();
    }

    void drawMainMenu() {
        // Code to draw the main menu
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_24);
        mainMenuTitle.Draw();
        SolarUI::SetFont(GLUT_BITMAP_TIMES_ROMAN_10);
        startButton.Draw();
        settingsButton.Draw();
        MultiplayerSettings.Draw();
        exitButton.Draw();
    }

    void drawPauseMenu() {
        // Code to draw the pause menu
    }
}