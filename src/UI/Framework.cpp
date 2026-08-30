#include "Framework.h"
#include "../Handler/Handler.h"
#include "../../include/SUI/SolarUI.h"
#include "../Game/Game.h"

#include <GL/freeglut.h>
#include <iostream>
#include <cstdlib>
#include <vector>

// ==================================================
// Window sizes
// ==================================================

std::vector<int> WinWidthSizes  = {800, 1280, 1920, 3840};
std::vector<int> WinHeightSizes = {600, 720, 1080, 2160};

int framework::WinWidth;
int framework::WinHeight;


// ==================================================
// Main Menu
// ==================================================

static solLabel  mainMenuTitle;
static solButton startButton;
static solButton settingsButton;
static solButton exitButton;


// ==================================================
// Settings
// ==================================================

framework::Settings Settings;


// Settings menu
static solLabel  settingsTitle;
static solButton displaySettings;
static solButton controlsSettings;
static solButton audioSettings;
static solButton saveButton;
static solButton resetButton;
static solButton helpButton;
static solButton backButton;


// ==================================================
// Display Settings
// ==================================================

static solLabel     displaySettingsTitle;
static solDropdown  resolution;

static solCheckbox  fullscreenCheckbox;
static solCheckbox  borderlessFullscreenCheckbox;

static solSlider    gammaSlider;
static solSlider    fovSlider;

static solDropdown  Engine;

static solLabel     graphicsSettingsTitle;

static solCheckbox  showAvatarsCheckbox;
static solCheckbox  showTexturesCheckbox;

static solSlider    framerateSlider;

static solCheckbox  vsyncCheckbox;
static solCheckbox  antiAliasingCheckbox;
static solCheckbox  motionBlurCheckbox;

// Options

const char* resolutions[] =
{
    "800x600",
    "1280x720",
    "1920x1080",
    "3840x2160"
};

const char* engines[] =
{
    "RCUT2.5-PR1"
};

// ==================================================
// Controls Settings
// ==================================================

static solLabel    controlsSettingsTitle;
static solLabel    Controls_NA;

static solSlider   MouseSensitivitySlider;

static solCheckbox InvertMouseXCheckbox;
static solCheckbox InvertMouseYSlider;
static solCheckbox MouseONCheckbox;


// ==================================================
// Audio Settings
// ==================================================

static solLabel  audioSettingsTitle;

static solSlider masterVolumeSlider;
static solSlider lobbyMusicVolumeSlider;
static solSlider inGameMusicVolumeSlider;
static solSlider sfxVolumeSlider;


// ==================================================
// Help
// ==================================================

static solLabel helpTitle;


// ==================================================
// Internal functions
// ==================================================

namespace
{
    void addAllElements()
    {
        // --------------------------------------------------
        // Main menu
        // --------------------------------------------------

        solUI_AddElement(&mainMenuTitle.Element);
        solUI_AddElement(&startButton.Element);
        solUI_AddElement(&settingsButton.Element);
        solUI_AddElement(&exitButton.Element);


        // --------------------------------------------------
        // Settings menu
        // --------------------------------------------------

        solUI_AddElement(&settingsTitle.Element);
        solUI_AddElement(&displaySettings.Element);
        solUI_AddElement(&controlsSettings.Element);
        solUI_AddElement(&audioSettings.Element);
        solUI_AddElement(&saveButton.Element);
        solUI_AddElement(&resetButton.Element);
        solUI_AddElement(&helpButton.Element);
        solUI_AddElement(&backButton.Element);


        // --------------------------------------------------
        // Display settings
        // --------------------------------------------------

        solUI_AddElement(&displaySettingsTitle.Element);
        solUI_AddElement(&resolution.Element);
        solUI_AddElement(&fullscreenCheckbox.Element);
        solUI_AddElement(&borderlessFullscreenCheckbox.Element);
        solUI_AddElement(&gammaSlider.Element);
        solUI_AddElement(&fovSlider.Element);
        solUI_AddElement(&Engine.Element);

        solUI_AddElement(&graphicsSettingsTitle.Element);
        solUI_AddElement(&showAvatarsCheckbox.Element);
        solUI_AddElement(&showTexturesCheckbox.Element);
        solUI_AddElement(&framerateSlider.Element);
        solUI_AddElement(&vsyncCheckbox.Element);
        solUI_AddElement(&antiAliasingCheckbox.Element);
        solUI_AddElement(&motionBlurCheckbox.Element);


        // --------------------------------------------------
        // Controls settings
        // --------------------------------------------------

        solUI_AddElement(&controlsSettingsTitle.Element);
        solUI_AddElement(&Controls_NA.Element);
        solUI_AddElement(&MouseSensitivitySlider.Element);
        solUI_AddElement(&InvertMouseXCheckbox.Element);
        solUI_AddElement(&InvertMouseYSlider.Element);
        solUI_AddElement(&MouseONCheckbox.Element);


        // --------------------------------------------------
        // Audio settings
        // --------------------------------------------------

        solUI_AddElement(&audioSettingsTitle.Element);
        solUI_AddElement(&masterVolumeSlider.Element);
        solUI_AddElement(&lobbyMusicVolumeSlider.Element);
        solUI_AddElement(&inGameMusicVolumeSlider.Element);
        solUI_AddElement(&sfxVolumeSlider.Element);


        // --------------------------------------------------
        // Help
        // --------------------------------------------------

        solUI_AddElement(&helpTitle.Element);
    }


    void initElements()
    {
        // ==================================================
        // Main menu
        // ==================================================

        solLabel_Init(&mainMenuTitle, "3D PacMan", 50, 50);

        solButton_Init(&startButton, "Start Game", 50, 75, 200, 50);
        solButton_Init(&settingsButton, "Settings", 50, 150, 200, 50);
        solButton_Init(&exitButton, "Exit", 50, 225, 200, 50);


        // ==================================================
        // Settings menu
        // ==================================================

        solLabel_Init(&settingsTitle, "3D PacMan - Settings", 50, 50);

        solButton_Init(&displaySettings, "Display", 50, 75, 200, 50);
        solButton_Init(&controlsSettings, "Controls", 50, 150, 200, 50);
        solButton_Init(&audioSettings, "Audio", 50, 225, 200, 50);
        solButton_Init(&saveButton, "Save", 50, 300, 200, 50);
        solButton_Init(&resetButton, "Reset", 50, 375, 200, 50);
        solButton_Init(&helpButton, "Help", 50, 450, 200, 50);
        solButton_Init(&backButton, "Back", 50, 525, 200, 50);


        // ==================================================
        // Display settings
        // ==================================================

        solLabel_Init(&displaySettingsTitle, "Display", 300, 100);
        solDropdown_Init(&resolution, "Resolution", resolutions, 4, 300, 150, 200, 25);
        solCheckbox_Init(&fullscreenCheckbox, "Fullscreen", 300, 200, 20, 20);
        solCheckbox_Init(&borderlessFullscreenCheckbox, "Borderless Fullscreen", 300, 250, 20, 20);
        solSlider_Init(&gammaSlider, "Gamma", 300, 300, 200, 20, 0.0f, 100.0f, 50.0f);
        solSlider_Init(&fovSlider, "Field of View", 300, 350, 200, 20, 0.0f, 180.0f, 90.0f);
        solDropdown_Init(&Engine, "Engine", engines, 1, 300, 400, 200, 25);

        solLabel_Init(&graphicsSettingsTitle, "Graphics", 600, 100);
        solCheckbox_Init(&showAvatarsCheckbox, "Display Avatars", 600, 200, 20, 20);
        solCheckbox_Init(&showTexturesCheckbox, "Display WallTextures", 600, 250, 20, 20);
        solSlider_Init(&framerateSlider, "Framerate Limit", 600, 300, 200, 20, 0.0f, 240.0f, 60.0f);
        solCheckbox_Init(&vsyncCheckbox, "V-Sync", 600, 350, 20, 20);
        solCheckbox_Init(&antiAliasingCheckbox, "Anti-Aliasing", 600, 400, 20, 20);
        solCheckbox_Init(&motionBlurCheckbox, "Motion Blur", 600, 450, 20, 20);

        // ==================================================
        // Controls settings
        // ==================================================

        solLabel_Init(&controlsSettingsTitle, "Controls", 300, 100);

        solLabel_Init(
            &Controls_NA,
            "W = Forward, \nA = Strafe Left, \nS = Back, \nD = Strafe Right, \nMouse for looking",
            500,
            250
        );

        solSlider_Init(&MouseSensitivitySlider, "Mouse Sensitivity", 300, 150, 200, 20, 0.0f, 100.0f, 1.0f);

        solCheckbox_Init(&InvertMouseXCheckbox, "Invert Mouse X-Axis", 300, 200, 20, 20);
        solCheckbox_Init(&InvertMouseYSlider, "Invert Mouse Y-Axis", 300, 250, 20, 20);
        solCheckbox_Init(&MouseONCheckbox, "Use Mouse?", 300,300, 20, 20);


        // ==================================================
        // Audio settings
        // ==================================================

        solLabel_Init(&audioSettingsTitle, "Audio", 300, 100);

        solSlider_Init(&masterVolumeSlider, "Master Volume", 300, 150, 200, 20, 0.0f, 100.0f, 100.0f);
        solSlider_Init(&lobbyMusicVolumeSlider, "Lobby Music Volume", 300, 200, 200, 20, 0.0f, 100.0f, 100.0f);
        solSlider_Init(&inGameMusicVolumeSlider, "Game Music Volume", 300, 250, 200, 20, 0.0f, 100.0f, 100.0f);
        solSlider_Init(&sfxVolumeSlider, "Sound Effects Volume", 300, 300, 200, 20, 0.0f, 100.0f, 100.0f);


        // ==================================================
        // Help
        // ==================================================

        solLabel_Init(&helpTitle, "Help", 300, 100);
    }

    
    void hideAllElements()
    {
        // ==================================================
        // Main menu
        // ==================================================

        mainMenuTitle.Element.Visibility = SOL_HIDDEN;
        startButton.Element.Visibility = SOL_HIDDEN;
        settingsButton.Element.Visibility = SOL_HIDDEN;
        exitButton.Element.Visibility = SOL_HIDDEN;


        // ==================================================
        // Settings menu
        // ==================================================

        settingsTitle.Element.Visibility = SOL_HIDDEN;
        displaySettings.Element.Visibility = SOL_HIDDEN;
        controlsSettings.Element.Visibility = SOL_HIDDEN;
        audioSettings.Element.Visibility = SOL_HIDDEN;
        saveButton.Element.Visibility = SOL_HIDDEN;
        resetButton.Element.Visibility = SOL_HIDDEN;    
        helpButton.Element.Visibility = SOL_HIDDEN;
        backButton.Element.Visibility = SOL_HIDDEN;


        // ==================================================
        // Display settings
        // ==================================================

        displaySettingsTitle.Element.Visibility = SOL_HIDDEN;

        resolution.Element.Visibility = SOL_HIDDEN;

        fullscreenCheckbox.Element.Visibility = SOL_HIDDEN;
        borderlessFullscreenCheckbox.Element.Visibility = SOL_HIDDEN;

        gammaSlider.Element.Visibility = SOL_HIDDEN;
        fovSlider.Element.Visibility = SOL_HIDDEN;

        Engine.Element.Visibility = SOL_HIDDEN;

        graphicsSettingsTitle.Element.Visibility = SOL_HIDDEN;

        showAvatarsCheckbox.Element.Visibility = SOL_HIDDEN;
        showTexturesCheckbox.Element.Visibility = SOL_HIDDEN;

        framerateSlider.Element.Visibility = SOL_HIDDEN;

        vsyncCheckbox.Element.Visibility = SOL_HIDDEN;
        antiAliasingCheckbox.Element.Visibility = SOL_HIDDEN;
        motionBlurCheckbox.Element.Visibility = SOL_HIDDEN;


        // ==================================================
        // Controls settings
        // ==================================================

        controlsSettingsTitle.Element.Visibility = SOL_HIDDEN;

        Controls_NA.Element.Visibility = SOL_HIDDEN;

        MouseSensitivitySlider.Element.Visibility = SOL_HIDDEN;

        InvertMouseXCheckbox.Element.Visibility = SOL_HIDDEN;
        InvertMouseYSlider.Element.Visibility = SOL_HIDDEN;
        MouseONCheckbox.Element.Visibility = SOL_HIDDEN;


        // ==================================================
        // Audio settings
        // ==================================================

        audioSettingsTitle.Element.Visibility = SOL_HIDDEN;

        masterVolumeSlider.Element.Visibility = SOL_HIDDEN;
        lobbyMusicVolumeSlider.Element.Visibility = SOL_HIDDEN;
        inGameMusicVolumeSlider.Element.Visibility = SOL_HIDDEN;
        sfxVolumeSlider.Element.Visibility = SOL_HIDDEN;


        // ==================================================
        // Help
        // ==================================================

        helpTitle.Element.Visibility = SOL_HIDDEN;
    }


    // ==================================================
    // Button callbacks
    // ==================================================

    void startGame()
    {
        std::cout << "Start Game clicked\n";
        game::init();
        gHandler.Screen.SetScreen(SCREEN_GAME);
        // Hide the OS cursor during gameplay - RCUT used to do this
        // implicitly via RCUT_Input_Init(), which we no longer call.
        glutSetCursor(GLUT_CURSOR_NONE);
    }


    void openSettings()
    {
        std::cout << "Settings clicked\n";
        gHandler.Screen.SetScreen(SCREEN_SETTINGS);
    }


    void openDisplaySettings()
    {
        std::cout << "Display Settings clicked\n";
        gHandler.Screen.SetScreen(SCREEN_DISPLAY_SETTINGS);
    }


    void openControlsSettings()
    {
        std::cout << "Controls Settings clicked\n";
        gHandler.Screen.SetScreen(SCREEN_CONTROLS_SETTINGS);
    }


    void openAudioSettings()
    {
        std::cout << "Audio Settings clicked\n";
        gHandler.Screen.SetScreen(SCREEN_AUDIO_SETTINGS);
    }


    void openHelp()
    {
        std::cout << "Help clicked\n";
        gHandler.Screen.SetScreen(SCREEN_HELP);
    }


    void goBack()
    {
        std::cout << "Back clicked\n";
        gHandler.Screen.SetScreen(SCREEN_MAIN_MENU);
    }


    void saveSettings()
    {
        std::cout << "Save clicked\n";
        Settings.Save();
    }

    void resetSettings()
    {
        std::cout << "Reset clicked\n";
        Settings.Reset();
    }

    void exitGame()
    {
        std::cout << "Exit clicked\n";
        std::exit(0);
    }


    // ==================================================
    // Assign callbacks
    // ==================================================

    void setupCallbacks()
    {
        solButton_SetCallback(&startButton, startGame);
        solButton_SetCallback(&settingsButton, openSettings);
        solButton_SetCallback(&displaySettings, openDisplaySettings);
        solButton_SetCallback(&controlsSettings, openControlsSettings);
        solButton_SetCallback(&audioSettings, openAudioSettings);
        solButton_SetCallback(&helpButton, openHelp);
        solButton_SetCallback(&backButton, goBack);
        solButton_SetCallback(&resetButton, resetSettings);
        solButton_SetCallback(&saveButton, saveSettings);
        solButton_SetCallback(&exitButton, exitGame);
    }


    // ==================================================
    // Load settings into UI
    // ==================================================

    int loadValues()
    {
        solDropdown_SetSelectedIndex(&resolution, WINDOW_RESOLUTION);

        fullscreenCheckbox.Checked = FULL_SCREEN_SETTING;
        borderlessFullscreenCheckbox.Checked = BORDERLESS_SETTING;

        gammaSlider.CurrentValue = GAMMA_SETTING;
        fovSlider.CurrentValue = FOV_SETTING;

        solDropdown_SetSelectedIndex(&Engine, ENGINE_TYPE);

        showAvatarsCheckbox.Checked = AVATARSHOW_SETTING;
        showTexturesCheckbox.Checked = TEXTURESHOW_SETTING;

        framerateSlider.CurrentValue = FRAMERATE_SETTING;

        vsyncCheckbox.Checked = VSYNC_SETTING;
        antiAliasingCheckbox.Checked = ANTIALIASING_SETTING;
        motionBlurCheckbox.Checked = MOTIONBLUR_SETTING;

        MouseSensitivitySlider.CurrentValue = MOUSE_SENSITIVITY_SETTING;
        InvertMouseXCheckbox.Checked = INVERT_MOUSE_X_SETTING;
        InvertMouseYSlider.Checked = INVERT_MOUSE_Y_SETTING;
        MouseONCheckbox.Checked = USE_MOUSE_SETTING;

        masterVolumeSlider.CurrentValue = MASTER_VOL;
        lobbyMusicVolumeSlider.CurrentValue = LOBBY_VOL;
        inGameMusicVolumeSlider.CurrentValue = GAME_VOL;
        sfxVolumeSlider.CurrentValue = SFX_VOL;

        return 0;
    }
}


// ==================================================
// Framework
// ==================================================

namespace framework
{
    void init()
    {
        solUI_Init();

        solUI_SetLogicalSize(1280.0f, 720.0f);

        solUI_SetFont(solFont{GLUT_BITMAP_TIMES_ROMAN_24,24.0f});

        gHandler.Settings.Load();

        initElements();
        setupCallbacks();
        addAllElements();

        loadValues();

        // Index via resolution.SelectedIndex (clamped by
        // solDropdown_SetSelectedIndex in loadValues()) rather
        // than the raw WINDOW_RESOLUTION straight from the config
        // file - same out-of-bounds risk as the engine crash,
        // just on WinWidthSizes/WinHeightSizes instead.
        WinWidth = WinWidthSizes[resolution.SelectedIndex];
        WinHeight = WinHeightSizes[resolution.SelectedIndex];

        gHandler.Screen.Init();
        gHandler.Screen.SetScreen(SCREEN_MAIN_MENU);
    }


    void update()
    {
        solUI_Update();
    }


    void shutdown()
    {
        solUI_Shutdown();
    }


    void Settings::Save()
    {
        WINDOW_RESOLUTION = resolution.SelectedIndex;

        FULL_SCREEN_SETTING = fullscreenCheckbox.Checked;
        BORDERLESS_SETTING = borderlessFullscreenCheckbox.Checked;

        GAMMA_SETTING = gammaSlider.CurrentValue;
        FOV_SETTING = fovSlider.CurrentValue;

        ENGINE_TYPE = Engine.SelectedIndex;

        AVATARSHOW_SETTING = showAvatarsCheckbox.Checked;
        TEXTURESHOW_SETTING = showTexturesCheckbox.Checked;

        FRAMERATE_SETTING = framerateSlider.CurrentValue;

        VSYNC_SETTING = vsyncCheckbox.Checked;
        ANTIALIASING_SETTING = antiAliasingCheckbox.Checked;
        MOTIONBLUR_SETTING = motionBlurCheckbox.Checked;

        MOUSE_SENSITIVITY_SETTING = MouseSensitivitySlider.CurrentValue;
        INVERT_MOUSE_X_SETTING = InvertMouseXCheckbox.Checked;
        INVERT_MOUSE_Y_SETTING = InvertMouseYSlider.Checked;
        USE_MOUSE_SETTING = MouseONCheckbox.Checked;

        MASTER_VOL = masterVolumeSlider.CurrentValue;
        LOBBY_VOL = lobbyMusicVolumeSlider.CurrentValue;
        GAME_VOL = inGameMusicVolumeSlider.CurrentValue;
        SFX_VOL = sfxVolumeSlider.CurrentValue;

        WinWidth = WinWidthSizes[resolution.SelectedIndex];
        WinHeight = WinHeightSizes[resolution.SelectedIndex];

        gHandler.Settings.Save();
    }


    void Settings::Reset()
    {
        gHandler.Settings.Reset();
        loadValues();
        init();
    }


    void resize(int width, int height)
    {
        solUI_UpdateViewport(width, height);
    }


    Framework::Framework()
    {
        init();
    }


    // ==================================================
    // Drawing
    // ==================================================
    //
    // SolarUI now owns the element list and handles
    // drawing + layering itself.
    //
    // The Framework only controls visibility.
    // ==================================================

    void drawMainMenu()
    {
        hideAllElements();
        mainMenuTitle.Element.Visibility = SOL_VISIBLE;
        startButton.Element.Visibility = SOL_VISIBLE;
        settingsButton.Element.Visibility = SOL_VISIBLE;
        exitButton.Element.Visibility = SOL_VISIBLE;
    }


    void drawSettings()
    {
        hideAllElements();
        settingsTitle.Element.Visibility = SOL_VISIBLE;

        displaySettings.Element.Visibility = SOL_VISIBLE;
        controlsSettings.Element.Visibility = SOL_VISIBLE;
        audioSettings.Element.Visibility = SOL_VISIBLE;
        saveButton.Element.Visibility = SOL_VISIBLE;
        resetButton.Element.Visibility = SOL_VISIBLE;
        helpButton.Element.Visibility = SOL_VISIBLE;
        backButton.Element.Visibility = SOL_VISIBLE;
    }


    void drawDisplaySettings()
    {
        drawSettings();

        displaySettingsTitle.Element.Visibility = SOL_VISIBLE;
        resolution.Element.Visibility = SOL_VISIBLE;

        fullscreenCheckbox.Element.Visibility = SOL_VISIBLE;
        borderlessFullscreenCheckbox.Element.Visibility = SOL_VISIBLE;

        gammaSlider.Element.Visibility = SOL_VISIBLE;
        fovSlider.Element.Visibility = SOL_VISIBLE;

        Engine.Element.Visibility = SOL_VISIBLE;

        graphicsSettingsTitle.Element.Visibility = SOL_VISIBLE;

        showAvatarsCheckbox.Element.Visibility = SOL_VISIBLE;
        showTexturesCheckbox.Element.Visibility = SOL_VISIBLE;

        framerateSlider.Element.Visibility = SOL_VISIBLE;

        vsyncCheckbox.Element.Visibility = SOL_VISIBLE;
        antiAliasingCheckbox.Element.Visibility = SOL_VISIBLE;
        motionBlurCheckbox.Element.Visibility = SOL_VISIBLE;
    }


    void drawControlsSettings()
    {
        drawSettings();

        controlsSettingsTitle.Element.Visibility = SOL_VISIBLE;
        Controls_NA.Element.Visibility = SOL_VISIBLE;

        MouseSensitivitySlider.Element.Visibility = SOL_VISIBLE;

        InvertMouseXCheckbox.Element.Visibility = SOL_VISIBLE;
        InvertMouseYSlider.Element.Visibility = SOL_VISIBLE;
        MouseONCheckbox.Element.Visibility = SOL_VISIBLE;
    }


    void drawAudioSettings()
    {
        drawSettings();

        audioSettingsTitle.Element.Visibility = SOL_VISIBLE;

        masterVolumeSlider.Element.Visibility = SOL_VISIBLE;
        lobbyMusicVolumeSlider.Element.Visibility = SOL_VISIBLE;
        inGameMusicVolumeSlider.Element.Visibility = SOL_VISIBLE;
        sfxVolumeSlider.Element.Visibility = SOL_VISIBLE;
    }


    void drawHelpMenu()
    {
        drawSettings();

        helpTitle.Element.Visibility = SOL_VISIBLE;
    }

    void drawPauseMenu()
    {
        // Pause menu will be implemented here.
    }
}