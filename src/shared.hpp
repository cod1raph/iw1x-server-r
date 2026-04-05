#ifndef SHARED_HPP
#define SHARED_HPP

#include "pch.h"

#include "types.hpp"
#include "functions.hpp"

extern bool playerCommand;
extern customPlayerState_t customPlayerState[MAX_CLIENTS];
extern scr_function_t customScriptFunctions[];
extern scr_method_t customScriptMethods[];

#endif