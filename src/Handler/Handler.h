#ifndef HANDLER_H_
#define HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SCREEN_MAIN_MENU,
    SCREEN_SETTINGS,
    SCREEN_DISPLAY_SETTINGS,
    SCREEN_CONTROLS_SETTINGS,
    SCREEN_AUDIO_SETTINGS,
    SCREEN_HELP,
    SCREEN_MULTIPLAYER_SETTINGS,
    SCREEN_PAUSE_MENU,
    SCREEN_LOBBY,
    SCREEN_GAME

} Screen;


typedef struct
{
    struct
    {
        void (*SetScreen)(Screen screen);
        void (*GoBack)(void);
        Screen (*GetCurrentScreen)(void);
        void (*Init)(void);

    } Screen;


    struct
    {
        int (*Load)(void);
        int (*Save)(void);
        int (*Reset)(void);

    } Settings;

} Handler;


extern const Handler gHandler;

#ifdef __cplusplus
}
#endif

#endif