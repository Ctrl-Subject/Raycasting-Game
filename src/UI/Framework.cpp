#include "Framework.h"
#include "../Handler/Handler.h"
#include <GL/glui.h>
#include "../Game/Game.h"

#include <GL/freeglut.h>
#include <iostream>
#include <cstdio>
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
// Settings
// ==================================================

framework::Settings Settings;


// ==================================================
// Screens
// ==================================================
//
// Same screens and the same layout as the SolarUI version: a
// column of 200px wide buttons on the left, and the page for the
// current screen to the right of it (Display and Graphics as two
// columns, then Controls, Audio and Help).
//
// GLUI lays widgets out itself, so each screen is one GLUI window
// docked to the left of the game window, with the widgets in the
// same order and the same columns as before. Only one is shown at
// a time.
// ==================================================

enum UiWindow
{
    WIN_MAIN_MENU,
    WIN_SETTINGS,
    WIN_DISPLAY,
    WIN_CONTROLS,
    WIN_AUDIO,
    WIN_HELP,
    WIN_PAUSE,
    WIN_COUNT
};

// One panel per screen, all inside a single GLUI window. Only the
// panel for the current screen is linked into the window, the others
// are unlinked, so the window is only ever as big as the current screen.
// GLUI's unlink() also forgets the panel's own children, and a node's
// child list is protected. NodeAccess reads and writes it the same way a
// subclass of the node could, so a screen can be unlinked and linked
// back with its controls intact.
struct NodeAccess : public GLUI_Node
{
    static GLUI_Node*& head(GLUI_Node* node) { return static_cast<NodeAccess*>(node)->child_head; }
    static GLUI_Node*& tail(GLUI_Node* node) { return static_cast<NodeAccess*>(node)->child_tail; }
};

static GLUI*       uiWindow = NULL;
static GLUI_Panel* uiRoot = NULL;
static GLUI_Panel* uiScreens[WIN_COUNT];
static GLUI_Node*  uiSavedHead[WIN_COUNT];
static GLUI_Node*  uiSavedTail[WIN_COUNT];
static int         uiShown = -1;
static bool        uiBuilt = false;

// GLUI resizes its window a moment after a screen change and does not
// always redraw it afterwards, so update() keeps asking for a redraw
// for a short while after every change.
static int         uiRedrawUntil = 0;

// Panel that the helpers below add controls to.
static GLUI_Panel*  uiPanel = NULL;


// ==================================================
// Control ids (passed to the GLUI callback)
// ==================================================

enum ControlId
{
    ID_START = 1,
    ID_SETTINGS,
    ID_DISPLAY,
    ID_CONTROLS,
    ID_AUDIO,
    ID_SAVE,
    ID_RESET,
    ID_HELP,
    ID_BACK,
    ID_EXIT,
    ID_RESUME,
    ID_PAUSE_MAIN_MENU,
    ID_PAUSE_EXIT,

    ID_SLIDER_BASE = 100
};


// ==================================================
// Live values
// ==================================================
//
// GLUI controls write straight into these variables, so they
// take the place of the old Checked / CurrentValue /
// SelectedIndex fields.
// ==================================================

// Display Settings
static int   resolutionIndex = 0;
static int   fullscreenChecked = 0;
static int   borderlessFullscreenChecked = 0;
static float gammaValue = 50.0f;
static float fovValue = 90.0f;
static int   engineIndex = 0;

// Graphics
static int   showAvatarsChecked = 0;
static int   showTexturesChecked = 0;
static float framerateValue = 60.0f;
static int   vsyncChecked = 0;
static int   antiAliasingChecked = 0;
static int   motionBlurChecked = 0;

// Controls Settings
static float mouseSensitivityValue = 1.0f;
static int   invertMouseXChecked = 0;
static int   invertMouseYChecked = 0;
static int   mouseOnChecked = 0;

// Audio Settings
static float masterVolumeValue = 100.0f;
static float lobbyMusicVolumeValue = 100.0f;
static float inGameMusicVolumeValue = 100.0f;
static float sfxVolumeValue = 100.0f;


// ==================================================
// Options
// ==================================================

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
// Sliders
// ==================================================
//
// GLUI has no slider widget, so a slider is a horizontal scrollbar
// with the name and current value shown above it (the scrollbar's
// callback keeps that text up to date).
// ==================================================

struct SliderInfo
{
    GLUI_StaticText* text;
    const char*      name;
    float*           value;
};

static std::vector<SliderInfo> sliders;


// ==================================================
// Internal functions
// ==================================================

namespace
{
    // Keeps a loaded index inside the option list, same job
    // solDropdown_SetSelectedIndex used to do for us.
    int clampIndex(int index, int count)
    {
        if (index < 0)       return 0;
        if (index >= count)  return count - 1;
        return index;
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


    void pauseGoToMainMenu()
    {
        std::cout << "Main Menu clicked (from pause)\n";
        gHandler.Screen.SetScreen(SCREEN_MAIN_MENU);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }


    // ==================================================
    // Slider text
    // ==================================================

    void updateSliderText(const SliderInfo& slider)
    {
        char buffer[96];
        std::snprintf(buffer, sizeof(buffer), "%s: %d", slider.name, (int)(*slider.value + 0.5f));
        slider.text->set_text(buffer);
    }

    void updateAllSliderText()
    {
        for (const SliderInfo& slider : sliders)
            updateSliderText(slider);
    }


    // ==================================================
    // GLUI callback
    // ==================================================
    //
    // GLUI gives every control one int callback, so each button
    // passes its id here and this forwards to the same functions
    // the SolarUI buttons used to call directly.
    // ==================================================

    void onControl(int id)
    {
        if (id >= ID_SLIDER_BASE)
        {
            size_t index = (size_t)(id - ID_SLIDER_BASE);

            if (index < sliders.size())
                updateSliderText(sliders[index]);

            return;
        }

        switch (id)
        {
            case ID_START:          startGame();                break;
            case ID_SETTINGS:       openSettings();             break;
            case ID_DISPLAY:        openDisplaySettings();      break;
            case ID_CONTROLS:       openControlsSettings();     break;
            case ID_AUDIO:          openAudioSettings();        break;
            case ID_SAVE:           saveSettings();             break;
            case ID_RESET:          resetSettings();            break;
            case ID_HELP:           openHelp();                 break;
            case ID_BACK:           goBack();                   break;
            case ID_EXIT:           exitGame();                 break;

            // pauseGame/resumeGame live in namespace framework
            // (declared in Framework.h, defined further down)
            // rather than in this anonymous namespace, so they
            // are qualified here.
            case ID_RESUME:         framework::resumeGame();    break;
            case ID_PAUSE_MAIN_MENU: pauseGoToMainMenu();       break;
            case ID_PAUSE_EXIT:     exitGame();                 break;

            default: break;
        }
    }


    // ==================================================
    // Building the screens
    // ==================================================

    const int kButtonW = 200;
    const int kButtonH = 50;

    // The old layout had 25px between buttons and 50px between the
    // rows of each page. Static text rows stand in for that spacing.
    void spacer(GLUI* window, int rows = 1)
    {
        for (int i = 0; i < rows; i++)
            window->add_statictext(" ");
    }


    void spacer(int rows = 1)
    {
        for (int i = 0; i < rows; i++)
            uiWindow->add_statictext_to_panel(uiPanel, " ");
    }

    void addLabel(const char* text)
    {
        uiWindow->add_statictext_to_panel(uiPanel, text);
    }

    void addColumn()
    {
        uiWindow->add_column_to_panel(uiPanel, false);
    }

    void addButton(const char* name, int id)
    {
        GLUI_Button* button = uiWindow->add_button_to_panel(uiPanel, name, id, onControl);
        button->set_w(kButtonW);
        spacer();
    }

    void addCheckbox(const char* name, int* value)
    {
        uiWindow->add_checkbox_to_panel(uiPanel, name, value);
        spacer(2);
    }

    void addSlider(const char* name, float* value, float low, float high)
    {
        SliderInfo info;
        info.name  = name;
        info.value = value;
        info.text  = uiWindow->add_statictext_to_panel(uiPanel, name);

        int id = ID_SLIDER_BASE + (int)sliders.size();

        GLUI_Scrollbar* bar = new GLUI_Scrollbar(uiPanel, name, GLUI_SCROLL_HORIZONTAL,
                                                 value, id, onControl);
        bar->set_float_limits(low, high);
        bar->set_w(200);

        sliders.push_back(info);
        updateSliderText(info);

        spacer(2);
    }

    void addListbox(const char* name, int* value, const char** options, int count)
    {
        GLUI_Listbox* listbox = uiWindow->add_listbox_to_panel(uiPanel, name, value);
        listbox->set_w(200);

        for (int i = 0; i < count; i++)
            listbox->add_item(i, options[i]);

        spacer(2);
    }


    // Starts a screen: a panel of its own inside the window.
    void beginScreen(UiWindow which)
    {
        uiScreens[which] = uiWindow->add_panel_to_panel(uiRoot, "", GLUI_PANEL_NONE);
        uiPanel = uiScreens[which];
    }


    // Title label plus the column of buttons every Settings screen has.
    void addSettingsColumn()
    {
        addLabel("3D PacMan - Settings");
        spacer();

        addButton("Display",  ID_DISPLAY);
        addButton("Controls", ID_CONTROLS);
        addButton("Audio",    ID_AUDIO);
        addButton("Save",     ID_SAVE);
        addButton("Reset",    ID_RESET);
        addButton("Help",     ID_HELP);
        addButton("Back",     ID_BACK);
    }


    void detachScreen(int which)
    {
        GLUI_Panel* panel = uiScreens[which];

        uiSavedHead[which] = NodeAccess::head(panel);
        uiSavedTail[which] = NodeAccess::tail(panel);

        panel->unlink();
    }

    void attachScreen(int which)
    {
        GLUI_Panel* panel = uiScreens[which];

        panel->link_this_to_parent_last(uiRoot);

        NodeAccess::head(panel) = uiSavedHead[which];
        NodeAccess::tail(panel) = uiSavedTail[which];
    }


    void buildUI()
    {
        int mainWindow = glutGetWindow();

        uiWindow = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_LEFT);
        uiWindow->set_main_gfx_window(mainWindow);

        uiRoot = uiWindow->add_panel("", GLUI_PANEL_NONE);


        // ==================================================
        // Main menu
        // ==================================================

        beginScreen(WIN_MAIN_MENU);
        addLabel("3D PacMan");
        spacer();
        addButton("Start Game", ID_START);
        addButton("Settings",   ID_SETTINGS);
        addButton("Exit",       ID_EXIT);


        // ==================================================
        // Settings menu
        // ==================================================

        beginScreen(WIN_SETTINGS);
        addSettingsColumn();


        // ==================================================
        // Display settings (Display column and Graphics column)
        // ==================================================

        beginScreen(WIN_DISPLAY);
        addSettingsColumn();

        addColumn();

        addLabel("Display");
        spacer();
        addListbox("Resolution", &resolutionIndex, resolutions, 4);
        addCheckbox("Fullscreen", &fullscreenChecked);
        addCheckbox("Borderless Fullscreen", &borderlessFullscreenChecked);
        addSlider("Gamma", &gammaValue, 0.0f, 100.0f);
        addSlider("Field of View", &fovValue, 0.0f, 180.0f);
        addListbox("Engine", &engineIndex, engines, 1);

        addColumn();

        addLabel("Graphics");
        spacer();
        addCheckbox("Display Avatars", &showAvatarsChecked);
        addCheckbox("Display WallTextures", &showTexturesChecked);
        addSlider("Framerate Limit", &framerateValue, 0.0f, 240.0f);
        addCheckbox("V-Sync", &vsyncChecked);
        addCheckbox("Anti-Aliasing", &antiAliasingChecked);
        addCheckbox("Motion Blur", &motionBlurChecked);


        // ==================================================
        // Controls settings
        // ==================================================

        beginScreen(WIN_CONTROLS);
        addSettingsColumn();

        addColumn();

        addLabel("Controls");
        spacer();
        addSlider("Mouse Sensitivity", &mouseSensitivityValue, 0.0f, 100.0f);
        addCheckbox("Invert Mouse X-Axis", &invertMouseXChecked);
        addCheckbox("Invert Mouse Y-Axis", &invertMouseYChecked);
        addCheckbox("Use Mouse?", &mouseOnChecked);

        addColumn();

        spacer(3);
        addLabel("W = Forward,");
        addLabel("A = Strafe Left,");
        addLabel("S = Back,");
        addLabel("D = Strafe Right,");
        addLabel("Mouse for looking");


        // ==================================================
        // Audio settings
        // ==================================================

        beginScreen(WIN_AUDIO);
        addSettingsColumn();

        addColumn();

        addLabel("Audio");
        spacer();
        addSlider("Master Volume", &masterVolumeValue, 0.0f, 100.0f);
        addSlider("Lobby Music Volume", &lobbyMusicVolumeValue, 0.0f, 100.0f);
        addSlider("Game Music Volume", &inGameMusicVolumeValue, 0.0f, 100.0f);
        addSlider("Sound Effects Volume", &sfxVolumeValue, 0.0f, 100.0f);


        // ==================================================
        // Help
        // ==================================================

        beginScreen(WIN_HELP);
        addSettingsColumn();

        addColumn();
        addLabel("Help");


        // ==================================================
        // Pause menu
        // ==================================================

        beginScreen(WIN_PAUSE);
        addLabel("Paused");
        spacer();
        addButton("Resume",    ID_RESUME);
        addButton("Main Menu", ID_PAUSE_MAIN_MENU);
        addButton("Exit",      ID_PAUSE_EXIT);


        // Everything starts unlinked and the window hidden, the draw
        // functions show the screen that is current.
        for (int i = 0; i < WIN_COUNT; i++)
            detachScreen(i);

        uiWindow->refresh();
        uiWindow->hide();

        uiShown = -1;
        uiBuilt = true;

        glutSetWindow(mainWindow);
    }


    // ==================================================
    // Visibility
    // ==================================================
    //
    // The display callback asks for a screen every frame, so
    // this only changes anything when the screen changes.
    // ==================================================

    void showOnly(int which)
    {
        if (!uiBuilt || which == uiShown)
            return;

        int current = glutGetWindow();

        if (uiShown >= 0)
            detachScreen(uiShown);

        if (which >= 0)
        {
            attachScreen(which);

            // Controls of unlinked screens miss loadValues(), so read the
            // live values in again.
            uiWindow->sync_live();
            updateAllSliderText();

            uiWindow->refresh();
            uiWindow->show();

            // Redraw the window at its new size.
            uiRedrawUntil = glutGet(GLUT_ELAPSED_TIME) + 600;
            glutSetWindow(uiWindow->get_glut_window_id());
            glutPostRedisplay();
        }
        else
        {
            uiWindow->hide();
        }

        uiShown = which;

        glutSetWindow(current);
    }


    // ==================================================
    // Load settings into UI
    // ==================================================

    int loadValues()
    {
        resolutionIndex = clampIndex(WINDOW_RESOLUTION, 4);

        fullscreenChecked = FULL_SCREEN_SETTING;
        borderlessFullscreenChecked = BORDERLESS_SETTING;

        gammaValue = GAMMA_SETTING;
        fovValue = FOV_SETTING;

        engineIndex = clampIndex(ENGINE_TYPE, 1);

        showAvatarsChecked = AVATARSHOW_SETTING;
        showTexturesChecked = TEXTURESHOW_SETTING;

        framerateValue = (float)FRAMERATE_SETTING;

        vsyncChecked = VSYNC_SETTING;
        antiAliasingChecked = ANTIALIASING_SETTING;
        motionBlurChecked = MOTIONBLUR_SETTING;

        mouseSensitivityValue = MOUSE_SENSITIVITY_SETTING;
        invertMouseXChecked = INVERT_MOUSE_X_SETTING;
        invertMouseYChecked = INVERT_MOUSE_Y_SETTING;
        mouseOnChecked = USE_MOUSE_SETTING;

        masterVolumeValue = MASTER_VOL;
        lobbyMusicVolumeValue = LOBBY_VOL;
        inGameMusicVolumeValue = GAME_VOL;
        sfxVolumeValue = SFX_VOL;

        // Push the new values into the controls.
        if (uiBuilt)
        {
            GLUI_Master.sync_live_all();
            updateAllSliderText();
        }

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
        gHandler.Settings.Load();

        // The windows are only built once. Reset() calls init()
        // again, and that just reloads the values.
        if (!uiBuilt)
            buildUI();

        loadValues();

        // Index via resolutionIndex (clamped in loadValues())
        // rather than the raw WINDOW_RESOLUTION straight from the
        // config file, same out-of-bounds risk as the engine
        // crash, just on WinWidthSizes/WinHeightSizes instead.
        WinWidth = WinWidthSizes[resolutionIndex];
        WinHeight = WinHeightSizes[resolutionIndex];

        gHandler.Screen.Init();
        gHandler.Screen.SetScreen(SCREEN_MAIN_MENU);
    }


    void update()
    {
        // GLUI updates its own windows from GLUI_Master's idle
        // function. This only asks for redraws just after a screen change.
        if (!uiBuilt || uiShown < 0 || glutGet(GLUT_ELAPSED_TIME) > uiRedrawUntil)
            return;

        int current = glutGetWindow();
        glutSetWindow(uiWindow->get_glut_window_id());
        glutPostRedisplay();
        glutSetWindow(current);
    }


    void shutdown()
    {
        GLUI_Master.close_all();
    }


    void pauseGame()
    {
        std::cout << "Game paused\n";
        gHandler.Screen.SetScreen(SCREEN_PAUSE_MENU);
        // Show the OS cursor again so the pause menu is clickable.
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }


    void resumeGame()
    {
        std::cout << "Game resumed\n";
        gHandler.Screen.SetScreen(SCREEN_GAME);
        // Hide the OS cursor again, same as startGame().
        glutSetCursor(GLUT_CURSOR_NONE);
    }


    void Settings::Save()
    {
        WINDOW_RESOLUTION = resolutionIndex;

        FULL_SCREEN_SETTING = fullscreenChecked != 0;
        BORDERLESS_SETTING = borderlessFullscreenChecked != 0;

        GAMMA_SETTING = gammaValue;
        FOV_SETTING = fovValue;

        ENGINE_TYPE = engineIndex;

        AVATARSHOW_SETTING = showAvatarsChecked != 0;
        TEXTURESHOW_SETTING = showTexturesChecked != 0;

        FRAMERATE_SETTING = (int)framerateValue;

        VSYNC_SETTING = vsyncChecked != 0;
        ANTIALIASING_SETTING = antiAliasingChecked != 0;
        MOTIONBLUR_SETTING = motionBlurChecked != 0;

        MOUSE_SENSITIVITY_SETTING = mouseSensitivityValue;
        INVERT_MOUSE_X_SETTING = invertMouseXChecked != 0;
        INVERT_MOUSE_Y_SETTING = invertMouseYChecked != 0;
        USE_MOUSE_SETTING = mouseOnChecked != 0;

        MASTER_VOL = masterVolumeValue;
        LOBBY_VOL = lobbyMusicVolumeValue;
        GAME_VOL = inGameMusicVolumeValue;
        SFX_VOL = sfxVolumeValue;

        WinWidth = WinWidthSizes[resolutionIndex];
        WinHeight = WinHeightSizes[resolutionIndex];

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
        // GLUI docks its windows to the game window by itself, so
        // they do not need the size. Kept so the Framework API
        // stays the same.
        (void)width;
        (void)height;
    }


    Framework::Framework()
    {
        init();
    }


    // ==================================================
    // Drawing
    // ==================================================
    //
    // GLUI draws its own windows.
    //
    // The Framework only controls which screen is shown.
    // ==================================================

    void drawMainMenu()
    {
        showOnly(WIN_MAIN_MENU);
    }


    void drawSettings()
    {
        showOnly(WIN_SETTINGS);
    }


    void drawDisplaySettings()
    {
        showOnly(WIN_DISPLAY);
    }


    void drawControlsSettings()
    {
        showOnly(WIN_CONTROLS);
    }


    void drawAudioSettings()
    {
        showOnly(WIN_AUDIO);
    }


    void drawHelpMenu()
    {
        showOnly(WIN_HELP);
    }

    void drawPauseMenu()
    {
        showOnly(WIN_PAUSE);
    }


    void hideUI()
    {
        showOnly(-1);
    }
}