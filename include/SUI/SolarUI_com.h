#ifndef SOLARUI_COM_H
#define SOLARUI_COM_H


#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif







typedef struct
{
    float r;
    float g;
    float b;
    float a;

} solColour;







typedef enum
{
    SOL_MOUSE_LEFT = 0,
    SOL_MOUSE_MIDDLE,
    SOL_MOUSE_RIGHT

} solMouseButton;







typedef enum
{
    SOL_ELEMENT_NONE = 0,

    SOL_ELEMENT_LABEL,
    SOL_ELEMENT_BUTTON,
    SOL_ELEMENT_SLIDER,
    SOL_ELEMENT_CHECKBOX,
    SOL_ELEMENT_IMAGE,
    SOL_ELEMENT_VIDEO,
    SOL_ELEMENT_DROPDOWN,
    SOL_ELEMENT_INPUTBOX

} solElementType;





typedef struct
{
    void* Handle;

    float Size;

} solFont;





typedef enum
{
    SOL_HIDDEN = 0,
    SOL_VISIBLE = 1

} solVisibility;







typedef enum
{
    SOL_DISABLED = 0,
    SOL_ENABLED = 1

} solElementState;







typedef struct
{
    float X;
    float Y;

} solPosition;



typedef struct
{
    float Width;
    float Height;

} solSize;



typedef struct
{
    solPosition Position;
    solSize Size;

} solBounds;





typedef void (*solVoidCallback)(void);

typedef void (*solFloatCallback)(
    float value
);

typedef void (*solBoolCallback)(
    bool value
);

typedef void (*solIntCallback)(
    int value
);

typedef void (*solStringCallback)(
    const char* value
);






typedef struct
{
    int id;


    solElementType Type;


    solBounds Bounds;


    








    int Layer;



    solVisibility Visibility;


    solElementState State;



} solElement;








typedef struct
{
    solColour Text;

} solLabelStyle;



#ifdef __cplusplus
}
#endif


#endif