#include "gsc.hpp"

scr_function_t scriptFunctions[] =
{
#if ENABLE_FILE_HANDLING == 1
    {"file_exists", gsc_utils_file_exists, qfalse},
    {"fopen", gsc_utils_fopen, qfalse},
    {"fwrite", gsc_utils_fwrite, qfalse},
    {"fread", gsc_utils_fread, qfalse},
    {"fclose", gsc_utils_fclose, qfalse},
#endif

    {"sendCommandToClient", gsc_utils_sendcommandtoclient, qfalse},
    {"logPrintConsole", gsc_utils_logprintconsole, qfalse},
    {"makeLocalizedString", gsc_utils_makelocalizedstring, qfalse},
    {"toUpper", gsc_utils_toupper, qfalse},
    {"toLower", gsc_utils_tolower, qfalse},
    {"strTok", gsc_utils_strtok, qfalse},
    {"isSubStr", gsc_utils_issubstr, qfalse},
    {"getSubStr", gsc_utils_getsubstr, qfalse},
    {"getSystemTime", gsc_utils_getsystemtime, qfalse},
    {"getConfigString", gsc_utils_getconfigstring, qfalse},
    
    //// Weapons
    {"setWeaponCookable", gsc_weapons_setweaponcookable, qfalse},
    {"setWeaponFuseTime", gsc_weapons_setweaponfusetime, qfalse},
    ////

    {"testFunction", gsc_testfunction, qfalse},
    {NULL, NULL, 0} // Terminator
};

xfunction_t Scr_GetCustomFunction(const char **fname, int *fdev)
{
    xfunction_t m = Scr_GetFunction(fname, fdev);
    if(m)
        return m;

    for (int i = 0; scriptFunctions[i].name; i++)
    {
        if(strcasecmp(*fname, scriptFunctions[i].name))
            continue;

        scr_function_t func = scriptFunctions[i];
        *fname = func.name;
        *fdev = func.developer;
        return func.call;
    }
    return NULL;
}

scr_method_t scriptMethods[] =
{
    //// Entity
    {"showToPlayer", gsc_entity_showtoplayer, qfalse},
    ////

    //// Player
    {"isOnLadder", gsc_player_isonladder, qfalse},
    {"isBot", gsc_player_isbot, qfalse},

    {"aimButtonPressed", gsc_player_button_ads, qfalse},
    {"leftButtonPressed", gsc_player_button_left, qfalse},
    {"rightButtonPressed", gsc_player_button_right, qfalse},
    {"forwardButtonPressed", gsc_player_button_forward, qfalse},
    {"backButtonPressed", gsc_player_button_back, qfalse},
    {"jumpButtonPressed", gsc_player_button_jump, qfalse},
    {"leanleftButtonPressed", gsc_player_button_leanleft, qfalse},
    {"leanRightButtonPressed", gsc_player_button_leanright, qfalse},
    {"reloadButtonPressed", gsc_player_button_reload, qfalse},

    {"getIP", gsc_player_getip, qfalse},
    {"getPing", gsc_player_getping, qfalse},
    {"getUserinfo", gsc_player_getuserinfo, qfalse},
    {"getVelocity", gsc_player_getvelocity, qfalse},
    {"getPlayerAngles", gsc_player_getangles, qfalse},
    {"getStance", gsc_player_getstance, qfalse},
    {"getViewOrigin", gsc_player_getvieworigin, qfalse},
    
    {"setStance", gsc_player_setstance, qfalse},
    {"setVelocity", gsc_player_setvelocity, qfalse},
    {"addVelocity", gsc_player_addvelocity, qfalse},
    {"setUserinfo", gsc_player_setuserinfo, qfalse},
    {"setConfigstringForPlayer", gsc_player_setconfigstringforplayer, qfalse},
    {"setAnimation", gsc_player_setanimation, qfalse},
    {"setWeaponAnimation", gsc_player_setweaponanimation, qfalse},
    {"setExpFogForPlayer", gsc_player_setexpfogforplayer, qfalse},
    {"ufo", gsc_player_ufo, qfalse},
    
    {"dropClient", gsc_player_dropclient, qfalse},
    {"processClientCommand", gsc_player_processclientcommand, qfalse},
    {"connectionlessPacketToClient", gsc_player_connectionlesspackettoclient, qfalse},
    ////

    {"testMethod", gsc_testmethod, 0},
    {NULL, NULL, 0} // Terminator
};

xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev)
{
    xmethod_t m = Scr_GetMethod(fname, fdev);
    if(m)
        return m;

    for (int i = 0; scriptMethods[i].name; i++)
    {
        if(strcasecmp(*fname, scriptMethods[i].name))
            continue;
        
        scr_method_t func = scriptMethods[i];
        *fname = func.name;
        *fdev = func.developer;
        return func.call;
    }
    return NULL;
}

void stackError(const char *format, ...)
{
    char s[MAX_STRINGLENGTH];
    int len = 0;
    va_list va;

    va_start(va, format);
    vsnprintf(s, sizeof(s) - 1, format, va);
    va_end(va);

    len = strlen(s);
    s[len] = '\n';
    s[len + 1] = '\0';
    Com_PrintMessage(0, s);
    Scr_CodeCallback_Error(qfalse, qfalse, "stackError", s);
}

int stackGetParams(const char *params, ...)
{
    va_list args;
    va_start(args, params);

    int errors = 0;

    for (size_t i = 0; i < strlen(params); i++)
    {
        switch (params[i])
        {
        case ' ': // Ignore param
            break;

        case 'i':
        {
            int *tmp = va_arg(args, int *);
            if (!stackGetParamInt(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not an int\n", i);
                errors++;
            }
            break;
        }

        case 'v':
        {
            float *tmp = va_arg(args, float *);
            if (!stackGetParamVector(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not a vector\n", i);
                errors++;
            }
            break;
        }

        case 'f':
        {
            float *tmp = va_arg(args, float *);
            if (!stackGetParamFloat(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not a float\n", i);
                errors++;
            }
            break;
        }

        case 's':
        {
            char **tmp = va_arg(args, char **);
            if (!stackGetParamString(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not a string\n", i);
                errors++;
            }
            break;
        }

        case 'c':
        {
            unsigned int *tmp = va_arg(args, unsigned int *);
            if (!stackGetParamConstString(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not a const string\n", i);
                errors++;
            }
            break;
        }

        case 'l':
        {
            char **tmp = va_arg(args, char **);
            if (!stackGetParamLocalizedString(i, tmp))
            {
                Com_DPrintf("\nstackGetParams() Param %i is not a localized string\n", i);
                errors++;
            }
            break;
        }

        default:
            errors++;
            Com_DPrintf("\nUnknown identifier [%c] passed to stackGetParams()\n", params[i]);
            break;
        }
    }

    va_end(args);
    return errors == 0; // Success if no errors
}

int stackGetParamInt(int param, int *value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if (var->type == VAR_FLOAT)
    {
        *value = var->u.floatValue;
        return 1;
    }

    if(var->type != VAR_INTEGER)
        return 0;

    *value = var->u.intValue;

    return 1;
}

int stackGetParamString(int param, char **value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if(var->type != VAR_STRING)
        return 0;

    *value = SL_ConvertToString(var->u.stringValue);

    return 1;
}

int stackGetParamConstString(int param, unsigned int *value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if(var->type != VAR_STRING)
        return 0;

    *value = var->u.stringValue;

    return 1;
}

int stackGetParamLocalizedString(int param, char **value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if(var->type != VAR_ISTRING)
        return 0;

    *value = SL_ConvertToString(var->u.stringValue);

    return 1;
}

int stackGetParamVector(int param, vec3_t value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if(var->type != VAR_VECTOR)
        return 0;

    VectorCopy(var->u.vectorValue, value);

    return 1;
}

int stackGetParamFloat(int param, float *value)
{
    if(param >= Scr_GetNumParam())
        return 0;

    VariableValue *var;
    var = &scrVmPub.top[-param];

    if (var->type == VAR_INTEGER)
    {
        *value = var->u.intValue;
        return 1;
    }

    if(var->type != VAR_FLOAT)
        return 0;

    *value = var->u.floatValue;

    return 1;
}


// For tests
void gsc_testfunction()
{
    printf("##### gsc_testfunction\n");
}
void gsc_testmethod(scr_entref_t ref)
{
    //printf("##### gsc_testmethod\n");
    int id = ref.entnum;
    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_testcommand() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }


    /*int animNumber;
    if (!stackGetParams("i", &animNumber))
    {
        stackError("gsc_x() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }*/
    
    
    
    //client_t* client = &svs.clients[id];
    //playerState_t *ps = SV_GameClientNum(id);
    //gentity_t *gentity = &g_entities[id];
    //gclient_t *gclient = gentity->client;
    //weaponinfo_t *weapon = BG_GetInfoForWeapon(ps->weapon);
    

    
    

}