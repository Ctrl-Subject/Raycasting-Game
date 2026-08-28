#ifndef SOLARUI_EXP_H
#define SOLARUI_EXP_H


// **********************************************
// *              DLL EXPORT CONTROL             *
// **********************************************

#ifdef _WIN32

    #ifdef SOLARUI_STATIC

        #define SOLARUI_API

    #elif defined(SOLARUI_BUILD_DLL)

        #define SOLARUI_API __declspec(dllexport)

    #else

        #define SOLARUI_API __declspec(dllimport)

    #endif


#else

    // Linux / other platforms
    #define SOLARUI_API __attribute__((visibility("default")))

#endif



#endif