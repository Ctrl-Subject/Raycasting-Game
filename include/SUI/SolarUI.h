#ifndef SOLARUI_H
#define SOLARUI_H


#include "SolarUI_exp.h"
#include "SolarUI_com.h"

#ifdef __cplusplus
extern "C" {
#endif



// **********************************************
// *              VERSION INFORMATION            *
// **********************************************

#define SOLARUI_VERSION_MAJOR 1
#define SOLARUI_VERSION_MINOR 0
#define SOLARUI_VERSION_PATCH 0



// **********************************************
// *              CORE LIFECYCLE                 *
// **********************************************


SOLARUI_API int solUI_Init(void);


SOLARUI_API void solUI_Shutdown(void);



SOLARUI_API void solUI_Update(void);


SOLARUI_API void solUI_Draw(void);



// **********************************************
// *              ELEMENT MANAGEMENT             *
// **********************************************


SOLARUI_API bool solUI_AddElement(
    solElement* element
);



SOLARUI_API bool solUI_RemoveElement(
    solElement* element
);



// **********************************************
// *              LAYER / INPUT CONTROL          *
// **********************************************


SOLARUI_API void solUI_CaptureInput(
    solElement* element
);



SOLARUI_API void solUI_ReleaseInput(void);



SOLARUI_API void solUI_SetElementLayer(
    solElement* element,
    int layer
);



SOLARUI_API int solUI_GetElementLayer(
    solElement* element
);



// **********************************************
// *              VIEWPORT                      *
// **********************************************


SOLARUI_API void solUI_SetLogicalSize(
    float width,
    float height
);



SOLARUI_API void solUI_UpdateViewport(
    int width,
    int height
);



SOLARUI_API void solUI_ScreenToLogical(
    int screenX,
    int screenY,
    float* logicalX,
    float* logicalY
);



// **********************************************
// *              FONT SYSTEM                   *
// **********************************************


SOLARUI_API void solUI_SetFont(
    solFont font
);



// **********************************************
// *              LABEL                          *
// **********************************************

typedef struct
{
    solElement Element;

    const char* Text;

    solFont Font;

    solLabelStyle Style;

} solLabel;


SOLARUI_API void solLabel_Init(
    solLabel* label,
    const char* text,
    float x,
    float y
);

SOLARUI_API void solLabel_Draw(
    solLabel* label
);

SOLARUI_API void solLabel_SetText(
    solLabel* label,
    const char* text
);

SOLARUI_API const char* solLabel_GetText(
    const solLabel* label
);

SOLARUI_API void solLabel_SetPosition(
    solLabel* label,
    float x,
    float y
);

SOLARUI_API void solLabel_SetColour(
    solLabel* label,
    solColour colour
);

SOLARUI_API void solLabel_SetFont(
    solLabel* label,
    solFont font
);



// **********************************************
// *              BUTTON                         *
// **********************************************

typedef struct
{
    solColour Background;
    solColour Hover;
    solColour Pressed;

    solColour Border;
    solColour Text;

} solButtonStyle;


typedef enum
{
    SOL_BUTTON_NORMAL = 0,
    SOL_BUTTON_HOVERED,
    SOL_BUTTON_PRESSED

} solButtonState;


typedef struct
{
    solElement Element;

    const char* Label;

    solButtonStyle Style;

    solButtonState State;

    solVoidCallback OnClick;

} solButton;


SOLARUI_API void solButton_Init(
    solButton* button,
    const char* text,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solButton_Draw(
    solButton* button
);

SOLARUI_API void solButton_Update(
    solButton* button,
    float mouseX,
    float mouseY,
    bool mouseDown
);

SOLARUI_API bool solButton_Contains(
    solButton* button,
    float x,
    float y
);

SOLARUI_API void solButton_Click(
    solButton* button
);

SOLARUI_API void solButton_SetText(
    solButton* button,
    const char* text
);

SOLARUI_API void solButton_SetStyle(
    solButton* button,
    solButtonStyle style
);

SOLARUI_API void solButton_SetCallback(
    solButton* button,
    solVoidCallback callback
);



// **********************************************
// *              SLIDER                         *
// **********************************************

typedef struct
{
    solColour Track;
    solColour Fill;
    solColour Thumb;

    solColour Text;

} solSliderStyle;


typedef struct
{
    solElement Element;

    const char* Label;

    solSliderStyle Style;

    float MinValue;
    float MaxValue;
    float CurrentValue;

    bool Dragging;

    solFloatCallback OnValueChanged;

} solSlider;


SOLARUI_API void solSlider_Init(
    solSlider* slider,
    const char* label,
    float x,
    float y,
    float width,
    float height,
    float minValue,
    float maxValue,
    float startValue
);

SOLARUI_API void solSlider_Draw(
    solSlider* slider
);

SOLARUI_API void solSlider_Update(
    solSlider* slider,
    float mouseX,
    float mouseY,
    bool mouseDown
);

SOLARUI_API bool solSlider_Contains(
    solSlider* slider,
    float x,
    float y
);

SOLARUI_API void solSlider_SetValue(
    solSlider* slider,
    float value
);

SOLARUI_API float solSlider_GetValue(
    solSlider* slider
);

SOLARUI_API void solSlider_SetCallback(
    solSlider* slider,
    solFloatCallback callback
);

SOLARUI_API void solSlider_SetStyle(
    solSlider* slider,
    solSliderStyle style
);



// **********************************************
// *              CHECKBOX                       *
// **********************************************

typedef struct
{
    solColour Background;
    solColour Checked;

    solColour Hover;
    solColour Border;

    solColour Text;

} solCheckboxStyle;


typedef struct
{
    solElement Element;

    const char* Label;

    solCheckboxStyle Style;

    bool Checked;

    bool Hovered;

    solBoolCallback OnCheckedChanged;

} solCheckbox;


SOLARUI_API void solCheckbox_Init(
    solCheckbox* checkbox,
    const char* label,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solCheckbox_Draw(
    solCheckbox* checkbox
);

SOLARUI_API void solCheckbox_Update(
    solCheckbox* checkbox,
    float mouseX,
    float mouseY,
    bool mousePressed
);

SOLARUI_API bool solCheckbox_Contains(
    solCheckbox* checkbox,
    float x,
    float y
);

SOLARUI_API void solCheckbox_SetChecked(
    solCheckbox* checkbox,
    bool state
);

SOLARUI_API bool solCheckbox_IsChecked(
    solCheckbox* checkbox
);

SOLARUI_API void solCheckbox_SetCallback(
    solCheckbox* checkbox,
    solBoolCallback callback
);

SOLARUI_API void solCheckbox_SetStyle(
    solCheckbox* checkbox,
    solCheckboxStyle style
);



// **********************************************
// *              DROPDOWN                       *
// **********************************************

typedef struct
{
    solColour Background;
    solColour Hover;

    solColour Border;

    solColour Text;

} solDropdownStyle;


typedef struct
{
    solElement Element;


    const char* Label;


    const char** Options;

    int OptionCount;

    int SelectedIndex;


    bool Open;

    int HoverIndex;


    /*
        When open, this becomes a temporary
        overlay element.
    */
    int PopupLayer;


    solDropdownStyle Style;


    solIntCallback OnSelectionChanged;


} solDropdown;


SOLARUI_API void solDropdown_Init(
    solDropdown* dropdown,
    const char* label,
    const char** options,
    int optionCount,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solDropdown_Draw(
    solDropdown* dropdown
);

SOLARUI_API void solDropdown_Update(
    solDropdown* dropdown,
    float mouseX,
    float mouseY,
    bool mousePressed
);

SOLARUI_API bool solDropdown_Contains(
    solDropdown* dropdown,
    float x,
    float y
);

SOLARUI_API void solDropdown_Open(
    solDropdown* dropdown
);

SOLARUI_API void solDropdown_Close(
    solDropdown* dropdown
);

SOLARUI_API int solDropdown_GetIndex(
    solDropdown* dropdown
);

SOLARUI_API void solDropdown_SetSelectedIndex(
    solDropdown* dropdown,
    int index
);

SOLARUI_API const char* solDropdown_GetValue(
    solDropdown* dropdown
);

SOLARUI_API void solDropdown_SetCallback(
    solDropdown* dropdown,
    solIntCallback callback
);

SOLARUI_API void solDropdown_SetStyle(
    solDropdown* dropdown,
    solDropdownStyle style
);



// **********************************************
// *              IMAGE                          *
// **********************************************

typedef struct
{
    solColour Tint;

    float Opacity;

} solImageStyle;


typedef struct
{
    solElement Element;

    const char* FilePath;

    void* MediaHandle;

    unsigned int TextureID;

    bool Loaded;

    solImageStyle Style;

} solImage;


SOLARUI_API void solImage_Init(
    solImage* image,
    const char* filePath,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solImage_Draw(
    solImage* image
);

SOLARUI_API void solImage_Load(
    solImage* image
);

SOLARUI_API void solImage_Unload(
    solImage* image
);

SOLARUI_API void solImage_SetTexture(
    solImage* image,
    unsigned int textureID
);

SOLARUI_API unsigned int solImage_GetTexture(
    solImage* image
);

SOLARUI_API void solImage_SetStyle(
    solImage* image,
    solImageStyle style
);



// **********************************************
// *              VIDEO                          *
// **********************************************

typedef struct
{
    solColour Tint;

    float Opacity;

} solVideoStyle;


typedef enum
{
    SOL_VIDEO_STOPPED = 0,
    SOL_VIDEO_PLAYING,
    SOL_VIDEO_PAUSED

} solVideoState;


typedef struct
{
    solElement Element;

    const char* FilePath;

    void* MediaHandle;

    unsigned int TextureID;

    solVideoState State;

    bool Looping;

    float Volume;

    solVideoStyle Style;

} solVideo;


SOLARUI_API void solVideo_Init(
    solVideo* video,
    const char* filePath,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solVideo_Draw(
    solVideo* video
);

SOLARUI_API void solVideo_Load(
    solVideo* video
);

SOLARUI_API void solVideo_Play(
    solVideo* video
);

SOLARUI_API void solVideo_Update(
    solVideo* video
);

SOLARUI_API void solVideo_Pause(
    solVideo* video
);

SOLARUI_API void solVideo_Stop(
    solVideo* video
);

SOLARUI_API void solVideo_SetLoop(
    solVideo* video,
    bool loop
);

SOLARUI_API void solVideo_SetVolume(
    solVideo* video,
    float volume
);

SOLARUI_API void solVideo_SetStyle(
    solVideo* video,
    solVideoStyle style
);

SOLARUI_API void solVideo_Unload(
    solVideo* video
);



// **********************************************
// *              INPUT BOX                      *
// **********************************************

typedef struct
{
    solColour Background;
    solColour Border;

    solColour Text;
    solColour Placeholder;

    solColour Cursor;

} solInputBoxStyle;


typedef struct
{
    solElement Element;


    const char* Placeholder;


    char* Buffer;

    int BufferSize;

    int Length;


    int CursorPosition;


    bool Focused;

    bool Submitted;


    solInputBoxStyle Style;


    void (*OnSubmit)(const char* text);


} solInputBox;


SOLARUI_API void solInputBox_Init(
    solInputBox* input,
    const char* placeholder,
    char* buffer,
    int bufferSize,
    float x,
    float y,
    float width,
    float height
);

SOLARUI_API void solInputBox_Draw(
    solInputBox* input
);

SOLARUI_API void solInputBox_Update(
    solInputBox* input
);

SOLARUI_API void solInputBox_MouseUpdate(
    solInputBox* input,
    float mouseX,
    float mouseY,
    bool pressed
);

SOLARUI_API void solInputBox_KeyInput(
    solInputBox* input,
    char key
);

SOLARUI_API void solInputBox_Backspace(
    solInputBox* input
);

SOLARUI_API void solInputBox_Clear(
    solInputBox* input
);

SOLARUI_API const char* solInputBox_GetText(
    solInputBox* input
);

SOLARUI_API bool solInputBox_IsSubmitted(
    solInputBox* input
);

SOLARUI_API void solInputBox_SetCallback(
    solInputBox* input,
    void (*callback)(const char*)
);

SOLARUI_API void solInputBox_SetStyle(
    solInputBox* input,
    solInputBoxStyle style
);



// **********************************************
// *              INPUT FEED                     *
// **********************************************
// Call these from your own window/event system;
// they mirror GLUT's callback signatures but do
// not require GLUT itself to call them.


SOLARUI_API void solInputBridge_Init(void);


SOLARUI_API void solInputBridge_MouseMove(
    int x,
    int y
);


SOLARUI_API void solInputBridge_MouseButton(
    int button,
    int state,
    int x,
    int y
);


SOLARUI_API void solInputBridge_KeyDown(
    unsigned char key,
    int x,
    int y
);


SOLARUI_API void solInputBridge_KeyUp(
    unsigned char key,
    int x,
    int y
);


SOLARUI_API void solInputBridge_SpecialKey(
    int key,
    int x,
    int y
);


SOLARUI_API void solInputBridge_SpecialKeyUp(
    int key,
    int x,
    int y
);


#ifdef __cplusplus
}
#endif


#endif
