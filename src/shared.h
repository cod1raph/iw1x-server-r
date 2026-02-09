#ifndef SHARED_H
#define SHARED_H

#include "pch.h"

#include "types.h"
#include "functions.h"

extern customPlayerState_t customPlayerState[MAX_CLIENTS];

xfunction_t Scr_GetCustomFunction(const char **fname, qboolean *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev);

void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message);

#endif