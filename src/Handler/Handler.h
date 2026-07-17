#ifndef HANDLER_H_
#define HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    // Settings Structure
    struct 
    {
        int (*Load)(void);
        int (*Save)(void);
        int (*Reset)(void);
    } Settings;

    // More to come

} Handler;

extern const Handler gHandler;

#ifdef __cplusplus
}
#endif

#endif