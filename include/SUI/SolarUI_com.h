#ifndef SOLARUI_COM_H
#define SOLARUI_COM_H


#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif



// **********************************************
// *              COLOUR SYSTEM                 *
// **********************************************

typedef struct
{
    float r;
    float g;
    float b;
    float a;

} solColour;



// **********************************************
// *              MOUSE                         *
// **********************************************

typedef enum
{
    SOL_MOUSE_LEFT = 0,
    SOL_MOUSE_MIDDLE,
    SOL_MOUSE_RIGHT

} solMouseButton;



// **********************************************
// *              ELEMENT TYPES                 *
// **********************************************

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

// **********************************************
// *              FONT SYSTEM                   *
// **********************************************

typedef struct
{
    void* Handle;

    float Size;

} solFont;

// **********************************************
// *              VISIBILITY                    *
// **********************************************

typedef enum
{
    SOL_HIDDEN = 0,
    SOL_VISIBLE = 1

} solVisibility;



// **********************************************
// *              ELEMENT STATE                 *
// **********************************************

typedef enum
{
    SOL_DISABLED = 0,
    SOL_ENABLED = 1

} solElementState;



// **********************************************
// *              GEOMETRY                      *
// **********************************************

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

// **********************************************
// *              CALLBACK TYPES                *
// **********************************************

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


// **********************************************
// *              BASE ELEMENT                  *
// **********************************************

typedef struct
{
    int id;


    solElementType Type;


    solBounds Bounds;


    /*
        Higher layer = drawn above lower layers.

        Used for:
        - dropdown menus
        - popups
        - modal windows
        - menus
    */
    int Layer;



    solVisibility Visibility;


    solElementState State;



} solElement;



// **********************************************
// *              COMMON STYLES                 *
// **********************************************


typedef struct
{
    solColour Text;

} solLabelStyle;



#ifdef __cplusplus
}
#endif


#endif