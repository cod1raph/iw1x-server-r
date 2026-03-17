#include "gsc.hpp"

extern cvar_t *player_sprintTime;

void gsc_player_setvelocity(scr_entref_t ref)
{
    int id = ref.entnum;
    vec3_t velocity;

    if (!stackGetParams("v", &velocity))
    {
        stackError("gsc_player_setvelocity() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setvelocity() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);
    VectorCopy(velocity, ps->velocity);

    Scr_AddBool(qtrue);
}

void gsc_player_getvelocity(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getvelocity() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);

    Scr_AddVector(ps->velocity);
}

void gsc_player_addvelocity(scr_entref_t ref)
{
    int id = ref.entnum;
    vec3_t velocity;

    if (!stackGetParams("v", &velocity))
    {
        stackError("gsc_player_addvelocity() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_addvelocity() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);
    VectorAdd(ps->velocity, velocity, ps->velocity);

    Scr_AddBool(qtrue);
}

void gsc_player_getuserinfokey(scr_entref_t ref)
{
    int id = ref.entnum;
    char *key;

    if (!stackGetParams("s", &key))
    {
        stackError("gsc_player_getuserinfokey() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getuserinfokey() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    char *val = Info_ValueForKey(client->userinfo, key);

    if (strlen(val))
        Scr_AddString(val);
    else
        Scr_AddString("");
}

void gsc_player_setuserinfokey(scr_entref_t ref)
{
    int id = ref.entnum;
    char *key, *value;

    if (!stackGetParams("ss", &key, &value))
    {
        stackError("gsc_player_setuserinfokey() one or more arguments is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setuserinfokey() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    Info_SetValueForKey(client->userinfo, key, value);

    Scr_AddBool(qtrue);
}

void gsc_player_button_ads(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_ads() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.buttons & KEY_MASK_ADS_MODE ? qtrue : qfalse);
}

void gsc_player_button_left(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_left() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.rightmove == KEY_MASK_MOVELEFT ? qtrue : qfalse);
}

void gsc_player_button_right(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_right() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.rightmove == KEY_MASK_MOVERIGHT ? qtrue : qfalse);
}

void gsc_player_button_forward(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_forward() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.forwardmove == KEY_MASK_FORWARD ? qtrue : qfalse);
}

void gsc_player_button_back(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_back() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.forwardmove == KEY_MASK_BACK ? qtrue : qfalse);
}

void gsc_player_button_jump(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_jump() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.upmove == KEY_MASK_JUMP ? qtrue : qfalse);
}

void gsc_player_button_leanleft(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_leanleft() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    
    Scr_AddBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANLEFT ? qtrue : qfalse);
}

void gsc_player_button_leanright(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_leanright() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANRIGHT ? qtrue : qfalse);
}

void gsc_player_button_reload(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_reload() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->lastUsercmd.wbuttons & KEY_MASK_RELOAD ? qtrue : qfalse);
}

void gsc_player_getangles(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getangles() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);

    Scr_AddVector(ps->viewangles);
}

void gsc_player_getstance(scr_entref_t ref)
{
    int id = ref.entnum;
    
    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getstance() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);
    if(ps->eFlags & EF_CROUCHING)
        Scr_AddString("crouch");
    else if(ps->eFlags & EF_PRONE)
        Scr_AddString("prone");
    else
        Scr_AddString("stand");
}

void gsc_player_getip(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getip() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    char ip[16];
    
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
        client->netchan.remoteAddress.ip[0],
        client->netchan.remoteAddress.ip[1],
        client->netchan.remoteAddress.ip[2],
        client->netchan.remoteAddress.ip[3]);

    Scr_AddString(ip);
}

void gsc_player_getping(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getping() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    Scr_AddInt(client->ping);
}

void gsc_player_processclientcommand(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_processclientcommand() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    ClientCommand(id);

    Scr_AddBool(qtrue);
}

void gsc_player_dropclient(scr_entref_t ref)
{
    int id = ref.entnum;
    char *reason;

    if (Scr_GetNumParam() > 0 && !stackGetParams("s", &reason))
    {
        stackError("gsc_player_dropclient() argument has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_dropclient() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    if(Scr_GetNumParam() > 0)
        SV_DropClient(client, reason);
    else
        SV_DropClient(client, NULL);

    Scr_AddBool(qtrue);
}

void gsc_player_isonladder(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_isonladder() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);

    Scr_AddBool(ps->pm_flags & PMF_LADDER ? qtrue : qfalse);
}

void gsc_player_ufo(scr_entref_t ref)
{
    int id = ref.entnum;
    char *ufo;

    if (!stackGetParams("s", &ufo))
    {
        stackError("gsc_player_ufo() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_ufo() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];

    if (!Q_stricmp(ufo, "on") || atoi(ufo))
    {
        entity->client->ufo = qtrue;
    }
    else if (!Q_stricmp(ufo, "off") || !Q_stricmp(ufo, "0"))
    {
        entity->client->ufo = qfalse;
    }
    else
    {
        entity->client->ufo = !entity->client->ufo;
    }

    Scr_AddBool(qtrue);
}

void gsc_player_connectionlesspackettoclient(scr_entref_t ref)
{
    int id = ref.entnum;
    char *cmd;

    if (!stackGetParams("s", &cmd))
    {
        stackError("gsc_player_connectionlesspackettoclient() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_connectionlesspackettoclient() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, cmd);

    Scr_AddBool(qtrue);
}

void gsc_player_isbot(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_isbot() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    Scr_AddBool(client->bIsTestClient);
}
void gsc_player_sethiddenfromscoreboard(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_sethiddenfromscoreboard() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    bool hidden = Scr_GetInt(0);
    customPlayerState[id].hiddenFromScoreboard = hidden;

    Scr_AddBool(true);
}

void Scr_SetFogForPlayer(const char *cmd, float start, float density, float heightDensity, float r, float g, float b, float time, int clientNum)
{
    if(start < 0.0)
        Scr_Error(va("%s: near distance must be >= 0", cmd));

    if(start >= density)
        Scr_Error(va("%s: near distance must be less than far distance", cmd));

    if(r < 0.0 || r > 1.0 || g < 0.0 || g > 1.0 || b < 0.0 || b > 1.0)
        Scr_Error(va("%s: red/green/blue color components must be in the range [0, 1]", cmd));

    if(time < 0.0)
        Scr_Error(va("%s: transition time must be >= 0 seconds", cmd));

    client_t *client = &svs.clients[clientNum];
    char configstring[MAX_STRINGLENGTH];

    Com_sprintf(configstring, MAX_STRINGLENGTH, "d %i %s", CS_FOGVARS, va("%g %g %g %g %g %g %.0f", start, density, heightDensity, r, g, b, (float)(time * 1000.0)));
    SV_SendServerCommand(client, SV_CMD_RELIABLE, configstring);
}
void gsc_player_setexpfogforplayer(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setexpfogforplayer() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    if(Scr_GetNumParam() != 5)
        Scr_Error("USAGE: setExpFogForPlayer(density, red, green, blue, transition time);\nDensity must be greater than 0 and less than 1, and typically less than .001. For example, .0002 means the fog gets .02%% more dense for every 1 unit of distance (about 1%% thicker every 50 units of distance)\n");

    float density = Scr_GetFloat(0);
    float r = Scr_GetFloat(1);
    float g = Scr_GetFloat(2);
    float b = Scr_GetFloat(3);
    float transitionTime = Scr_GetFloat(4);

    if(density <= 0.0 || 1.0 <= density)
        Scr_Error("setExpFogForPlayer: density must be greater than 0 and less than 1");
    
    Scr_SetFogForPlayer("setExpFogForPlayer", 0.0, 1.0, density, r, g, b, transitionTime, id);

    Scr_AddBool(qtrue);
}

long BG_StringHashValue(const char *fname)
{
    int i;
    long hash;

    hash = 0;
    i = 0;
    while (fname[i] != '\0')
    {
        hash += (long)(tolower(fname[i])) * (i + 119);
        i++;
    }
    if(hash == -1)
        hash = 0;

    return hash;
}
int GetAnimationId(const char *string)
{
    int i;
    int hash;
    animation_t *anim;

    hash = BG_StringHashValue(string);
    for (i = 0; i < (*globalScriptData)->numAnimations; i++)
    {
        anim = &(*globalScriptData)->animations[i];
        if((hash == anim->nameHash ) && !Q_stricmp(string, anim->name))
            return i;
    }

    return -1;
}
void gsc_player_setanimation(scr_entref_t ref)
{
    int id = ref.entnum;
    char *animation;
    int animationId;

    if (!stackGetParams("s", &animation))
    {
        stackError("gsc_player_setanimation() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if(!strncmp(animation, "none", 4))
        animationId = 0;
    else
        animationId = GetAnimationId(animation);

    if (animationId == -1)
    {
        stackError("gsc_player_setanimation() invalid animation string");
        Scr_AddUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];
    if (entity->s.eType == ET_PLAYER_CORPSE)
    {
        entity->s.legsAnim = animationId;
        Scr_AddBool(qtrue);
        return;
    }

    if (entity->client == NULL)
    {
        stackError("gsc_player_setanimation() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }

    customPlayerState[id].animation = animationId;

    Scr_AddBool(qtrue);
}

void gsc_player_setweaponanimation(scr_entref_t ref)
{
    int id = ref.entnum;
    int animNumber;
    
    if (!stackGetParams("i", &animNumber))
    {
        stackError("gsc_player_setweaponanimation() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);

    if(ps->pm_type >= PM_DEAD)
        return;
    
    ps->weapAnim = (animNumber | (( ps->weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT));
    
    Scr_AddBool(qtrue);
}

void gsc_player_getvieworigin(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getvieworigin() entity %i is not a player", id);
        Scr_AddUndefined();
        return;
    }
    
    gentity_t *gentity = &g_entities[id];
    gclient_t *gclient = gentity->client;
    
    vec3_t viewOrigin;
    VectorCopy(gclient->ps.origin, viewOrigin);
    viewOrigin[2] = viewOrigin[2] + gclient->ps.viewHeightCurrent;
    G_AddLean(gentity, viewOrigin);

    Scr_AddVector(viewOrigin);
}