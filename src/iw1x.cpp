#include "iw1x.h"

//// Cvars
cvar_t *com_dedicated;
cvar_t *com_sv_running;
cvar_t *fs_game;
cvar_t *net_lanauthorize;
cvar_t *sv_allowAnonymous;
cvar_t *sv_allowDownload;
cvar_t *sv_floodProtect;
cvar_t *sv_fps;
cvar_t *sv_master[MAX_MASTER_SERVERS];
cvar_t *sv_maxclients;
cvar_t *sv_maxRate;
cvar_t *sv_onlyVisibleClients;
cvar_t *sv_pure;
cvar_t *sv_rconPassword;
cvar_t *sv_showAverageBPS;
cvar_t *sv_showCommands;

// Custom
cvar_t *fs_callbacks_additional;
cvar_t *fs_svrPaks;
cvar_t *g_deadChat;
cvar_t *g_debugCallbacks;
cvar_t *g_playerEject;
cvar_t *g_resetSlide;
cvar_t *sv_cracked;
cvar_t *sv_debugRate;
cvar_t *sv_downloadForce;
cvar_t *sv_fastDownload;
cvar_t *sv_heartbeatDelay;
cvar_t *sv_spectatorNoclip;
////

//// Game lib
// Objects
gentity_t *g_entities;
gclient_t *g_clients;
level_locals_t *level;

// Functions
G_Say_t G_Say;
G_RegisterCvars_t G_RegisterCvars;
G_AddEvent_t G_AddEvent;
G_AddPredictableEvent_t G_AddPredictableEvent;
trap_SendServerCommand_t trap_SendServerCommand;
trap_GetConfigstringConst_t trap_GetConfigstringConst;
trap_GetConfigstring_t trap_GetConfigstring;
BG_GetNumWeapons_t BG_GetNumWeapons;
BG_GetInfoForWeapon_t BG_GetInfoForWeapon;
BG_GetWeaponIndexForName_t BG_GetWeaponIndexForName;
BG_AnimationIndexForString_t BG_AnimationIndexForString;
BG_AnimScriptEvent_t BG_AnimScriptEvent;
BG_AddPredictableEventToPlayerstate_t BG_AddPredictableEventToPlayerstate;
Scr_GetFunctionHandle_t Scr_GetFunctionHandle;
Scr_GetNumParam_t Scr_GetNumParam;
Scr_IsSystemActive_t Scr_IsSystemActive;
Scr_GetInt_t Scr_GetInt;
Scr_GetString_t Scr_GetString;
Scr_GetType_t Scr_GetType;
Scr_GetEntity_t Scr_GetEntity;
Scr_AddBool_t Scr_AddBool;
Scr_AddInt_t Scr_AddInt;
Scr_AddFloat_t Scr_AddFloat;
Scr_AddString_t Scr_AddString;
Scr_AddUndefined_t Scr_AddUndefined;
Scr_AddVector_t Scr_AddVector;
Scr_MakeArray_t Scr_MakeArray;
Scr_AddArray_t Scr_AddArray;
Scr_AddObject_t Scr_AddObject;
Scr_LoadScript_t Scr_LoadScript;
Scr_ExecThread_t Scr_ExecThread;
Scr_ExecEntThread_t Scr_ExecEntThread;
Scr_ExecEntThreadNum_t Scr_ExecEntThreadNum;
Scr_FreeThread_t Scr_FreeThread;
Scr_GetFunction_t Scr_GetFunction;
Scr_GetMethod_t Scr_GetMethod;
Scr_Error_t Scr_Error;
Scr_ObjectError_t Scr_ObjectError;
Scr_GetConstString_t Scr_GetConstString;
Scr_ParamError_t Scr_ParamError;
Q_strlwr_t Q_strlwr;
Q_strupr_t Q_strupr;
Q_strcat_t Q_strcat;
Q_strncpyz_t Q_strncpyz;
Q_CleanStr_t Q_CleanStr;
StuckInClient_t StuckInClient;
trap_Argv_t trap_Argv;
ClientCommand_t ClientCommand;
Com_SkipRestOfLine_t Com_SkipRestOfLine;
Com_ParseRestOfLine_t Com_ParseRestOfLine;
Com_ParseInt_t Com_ParseInt;
Jump_Check_t Jump_Check;
PM_GetEffectiveStance_t PM_GetEffectiveStance;
PM_ClipVelocity_t PM_ClipVelocity;
va_t va;
trap_GetUserinfo_t trap_GetUserinfo;
PM_NoclipMove_t PM_NoclipMove;
G_LocalizedStringIndex_t G_LocalizedStringIndex;
trap_SetConfigstring_t trap_SetConfigstring;
trap_GetArchivedPlayerState_t trap_GetArchivedPlayerState;
G_Error_t G_Error;
Scr_GetPointerType_t Scr_GetPointerType;
////

//// Callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;
int codecallback_client_spam = 0;
int codecallback_playercommand = 0;
//int codecallback_playercrashland = 0;
int codecallback_error = 0;
callback_t callbacks[] =
{
    // Stock
    {&codecallback_startgametype, "CodeCallback_StartGameType", false},
    {&codecallback_playerconnect, "CodeCallback_PlayerConnect", false},
    {&codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect", false},
    {&codecallback_playerdamage, "CodeCallback_PlayerDamage", false},
    {&codecallback_playerkilled, "CodeCallback_PlayerKilled", false},
    
    // Custom
    {&codecallback_client_spam, "CodeCallback_CLSpam", true},
    {&codecallback_playercommand, "CodeCallback_PlayerCommand", true},
    {&codecallback_error, "CodeCallback_Error", true},
};
////

// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/sv_client.c#L98
static ucmd_t ucmds[] =
{
    {"userinfo",        SV_UpdateUserinfo_f,     },
    {"disconnect",      SV_Disconnect_f,         },
    {"cp",              SV_VerifyPaks_f,         },
    {"vdr",             SV_ResetPureClient_f,    },
    {"download",        SV_BeginDownload_f,      },
    {"nextdl",          SV_NextDownload_f,       },
    {"stopdl",          SV_StopDownload_f,       },
    {"donedl",          SV_DoneDownload_f,       },
    {"retransdl",       SV_RetransmitDownload_f, },
    {NULL, NULL}
};

customPlayerState_t customPlayerState[MAX_CLIENTS];

cHook *hook_ClientEndFrame;
cHook *hook_Com_Init;
cHook *hook_DeathmatchScoreboardMessage;
cHook *hook_GScr_LoadGameTypeScript;
cHook *hook_PM_FlyMove;
cHook *hook_SV_AddOperatorCommands;
cHook *hook_SV_BeginDownload_f;
cHook *hook_SV_SendClientGameState;
cHook *hook_Sys_LoadDll;
cHook *hook_Touch_Item_Auto;

// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/shared.c#L632
#define MAX_VA_STRING 32000
char *custom_va(const char *format, ...)
{
    va_list argptr;
    static char temp_buffer[MAX_VA_STRING];
    static char string[MAX_VA_STRING];
    static int index = 0;
    char *buf;
    int len;

    va_start(argptr, format);
    vsprintf(temp_buffer, format, argptr);
    va_end(argptr);

    if((len = strlen(temp_buffer)) >= MAX_VA_STRING)
        Com_Error(ERR_DROP, "Attempted to overrun string in call to va()\n");

    if(len + index >= MAX_VA_STRING - 1)
        index = 0;

    buf = &string[index];
    memcpy(buf, temp_buffer, len + 1);
    index += len + 1;
    return buf;
}

void sendMessageToClient_orServerConsole(client_t *cl, std::string message)
{
    std::string finalMessage;
    if (cl)
    {
        finalMessage = "e \"";
        finalMessage.append(message);
        finalMessage.append("\"");
        SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, finalMessage.c_str());
    }
    else
    {
        finalMessage = message;
        finalMessage.append("\n");
        Com_Printf(finalMessage.c_str());
    }
}

qboolean FS_svrPak(const char *base)
{
    if(strstr(base, "_svr_"))
        return qtrue;

    if (*fs_svrPaks->string)
    {
        bool isSvrPak = false;
        
        std::string copy_fs_svrPaks(fs_svrPaks->string);
        const char* separator = ";";
        char* strToken = strtok((char*)copy_fs_svrPaks.c_str(), separator);

        while (strToken != NULL)
        {
            if (!strcmp(base, strToken))
            {
                isSvrPak = true;
                break;
            }
            strToken = strtok(NULL, separator);
        }

        if(isSvrPak)
            return qtrue;
    }

    return qfalse;
}

bool shouldServeFile(const char *requestedFilePath)
{
    static char localFilePath[MAX_OSPATH*2+5];
    searchpath_t* search;

    localFilePath[0] = 0;

    for (search = fs_searchpaths; search; search = search->next)
    {
        if (search->pak)
        {
            snprintf(localFilePath, sizeof(localFilePath), "%s/%s.pk3", search->pak->pakGamename, search->pak->pakBasename);
            if(!strcmp(localFilePath, requestedFilePath))
                if(!FS_svrPak(search->pak->pakBasename))
                    return true;
        }
    }
    return false;
}

void custom_Com_Init(char *commandLine)
{
    hook_Com_Init->unhook();
    void (*Com_Init)(char *commandLine);
    *(int*)&Com_Init = hook_Com_Init->from;
    Com_Init(commandLine);
    hook_Com_Init->hook();

    //// Cvars
    // Create references
    com_dedicated = Cvar_FindVar("dedicated");
    com_sv_running = Cvar_FindVar("sv_running");
    fs_game = Cvar_FindVar("fs_game");
    net_lanauthorize = Cvar_FindVar("net_lanauthorize");
    sv_allowAnonymous = Cvar_FindVar("sv_allowAnonymous");
    sv_allowDownload = Cvar_FindVar("sv_allowDownload");
    sv_floodProtect = Cvar_FindVar("sv_floodProtect");
    sv_fps = Cvar_FindVar("sv_fps");
    sv_master[0] = Cvar_FindVar("sv_master1");
    sv_master[1] = Cvar_FindVar("sv_master2");
    sv_master[2] = Cvar_FindVar("sv_master3");
    sv_master[3] = Cvar_FindVar("sv_master4");
    sv_master[4] = Cvar_FindVar("sv_master5");
    sv_maxclients = Cvar_FindVar("sv_maxclients");
    sv_maxRate = Cvar_FindVar("sv_maxRate");
    sv_onlyVisibleClients = Cvar_FindVar("sv_onlyVisibleClients");
    sv_pure = Cvar_FindVar("sv_pure");
    sv_rconPassword = Cvar_FindVar("rconpassword");
    sv_showAverageBPS = Cvar_FindVar("sv_showAverageBPS");
    sv_showCommands = Cvar_FindVar("sv_showCommands");

    // Register
    Cvar_Get("iw1x", "1", CVAR_SERVERINFO | CVAR_ROM);
    Cvar_Get("iw1x_date", __DATE__, CVAR_SERVERINFO | CVAR_ROM);

    // Register and create references
    fs_callbacks_additional = Cvar_Get("fs_callbacks_additional", "", CVAR_ARCHIVE);
    fs_svrPaks = Cvar_Get("fs_svrPaks", "", CVAR_ARCHIVE);
    g_deadChat = Cvar_Get("g_deadChat", "0", CVAR_ARCHIVE);
    g_debugCallbacks = Cvar_Get("g_debugCallbacks", "0", CVAR_ARCHIVE);
    g_playerEject = Cvar_Get("g_playerEject", "1", CVAR_ARCHIVE);
    g_resetSlide = Cvar_Get("g_resetSlide", "0", CVAR_ARCHIVE);
    sv_cracked = Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);
    sv_debugRate = Cvar_Get("sv_debugRate", "0", CVAR_ARCHIVE);
    sv_downloadForce = Cvar_Get("sv_downloadForce", "0", CVAR_ARCHIVE);
    sv_fastDownload = Cvar_Get("sv_fastDownload", "0", CVAR_ARCHIVE);
    sv_heartbeatDelay = Cvar_Get("sv_heartbeatDelay", "30", CVAR_ARCHIVE);
    sv_spectatorNoclip = Cvar_Get("sv_spectatorNoclip", "0", CVAR_ARCHIVE);
    ////
}

// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/script.c#L944
static int localizedStringIndex = 128;
int custom_G_LocalizedStringIndex(const char *string)
{
    int i;
    int start = 1244;
    char s[MAX_STRINGLENGTH];

    if(localizedStringIndex >= 256)
        localizedStringIndex = 128;

    if(!string || !*string)
        return 0;

    for (i = 1; i < 256; i++)
    {
        trap_GetConfigstring(start + i, s, sizeof(s));
        if(!*s)
            break;
        if(!strcmp(s, string))
            return i;
    }

    if(i == 256)
        i = localizedStringIndex;

    trap_SetConfigstring(i + 1244, string);
    localizedStringIndex++;
    
    return i;
}

void custom_GScr_LoadGameTypeScript()
{
    hook_GScr_LoadGameTypeScript->unhook();
    void (*GScr_LoadGameTypeScript)();
    *(int*)&GScr_LoadGameTypeScript = hook_GScr_LoadGameTypeScript->from;
    GScr_LoadGameTypeScript();
    hook_GScr_LoadGameTypeScript->hook();
    
    if (*fs_callbacks_additional->string)
    {
        if (!Scr_LoadScript(fs_callbacks_additional->string))
        {
            Com_DPrintf("custom_GScr_LoadGameTypeScript: Scr_LoadScript for fs_callbacks_additional failed.\n");
        }
        else
        {
            for(unsigned int i = 0; i < sizeof(callbacks) / sizeof(callbacks[0]); i++)
                if(callbacks[i].custom)
                    *callbacks[i].pos = Scr_GetFunctionHandle(fs_callbacks_additional->string, callbacks[i].name);
        }
    }
}

const char* custom_FS_ReferencedPakNames(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;

    info[0] = 0;
    
    for (search = fs_searchpaths; search; search = search->next)
    {
        if(!search->pak)
            continue;
        if(FS_svrPak(search->pak->pakBasename))
            continue;

        if(*info)
            Q_strcat(info, sizeof(info), " ");
        Q_strcat(info, sizeof(info), search->pak->pakGamename);
        Q_strcat(info, sizeof(info), "/");
        Q_strcat(info, sizeof(info), search->pak->pakBasename);
    }

    return info;
}

const char* custom_FS_ReferencedPakChecksums(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;
    
    info[0] = 0;

    for (search = fs_searchpaths; search; search = search->next)
    {
        if(!search->pak)
            continue;
        if(FS_svrPak(search->pak->pakBasename))
            continue;
        
        Q_strcat(info, sizeof(info), va("%i ", search->pak->checksum));
    }

    return info;
}

void custom_SV_AddOperatorCommands()
{
    hook_SV_AddOperatorCommands->unhook();
    void (*SV_AddOperatorCommands)();
    *(int*)&SV_AddOperatorCommands = hook_SV_AddOperatorCommands->from;
    SV_AddOperatorCommands();
    hook_SV_AddOperatorCommands->hook();

    Cmd_AddCommand("ban", ban);
    Cmd_AddCommand("unban", unban);
}

void custom_SV_PacketEvent(netadr_t from, msg_t *msg)
{
    int qport;
    client_t *cl;
    int i;
    
    int *unknown_var = (int*)0x080e30cc; // Maybe related to scrVmGlob.loading
    
    if (msg->cursize < 4 || *(int *)msg->data != -1)
    {
        //// See https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_game_mp.cpp#L399
        if(sv.skelTimeStamp++ == -1)
            sv.skelTimeStamp = 1;
        ////
        *unknown_var = 1;
        
        MSG_BeginReading(msg);
        MSG_ReadLong(msg);
        qport = MSG_ReadShort(msg);
        cl = svs.clients;

        for (i = 0; i < sv_maxclients->integer; i++, cl++)
        {
            if (cl->state != CS_FREE && NET_CompareBaseAdr(from, (cl->netchan).remoteAddress) && (cl->netchan).qport == (qport & 0xFFFF))
            {
                if (cl->netchan.remoteAddress.port != from.port)
                {
                    Com_Printf("SV_ReadPackets: fixing up a translated port\n");
                    cl->netchan.remoteAddress.port = from.port;
                }

                if(!Netchan_Process(&cl->netchan, msg))
                    return;

                cl->serverId = MSG_ReadByte(msg);
                cl->messageAcknowledge = MSG_ReadLong(msg);
                if (cl->messageAcknowledge < 0)
                {
                    Com_Printf("Invalid reliableAcknowledge message from %s - reliableAcknowledge is %i\n", cl->name, cl->reliableAcknowledge);
                    return;
                }
                
                //// [exploit patch] freeze
                /* See:
                - https://github.com/callofduty4x/CoD4x_Server/pull/407
                - https://github.com/diamante0018/MW3ServerFreezer
                */
                cl->reliableAcknowledge = MSG_ReadLong(msg);
                if ((cl->reliableSequence - cl->reliableAcknowledge) > (MAX_RELIABLE_COMMANDS - 1) || cl->reliableAcknowledge < 0 || (cl->reliableSequence - cl->reliableAcknowledge) < 0)
                {
                    Com_Printf("Out of range reliableAcknowledge message from %s - cl->reliableSequence is %i, reliableAcknowledge is %i\n",
                        cl->name, cl->reliableSequence, cl->reliableAcknowledge
                    );
                    
                    cl->reliableAcknowledge = cl->reliableSequence;
                    return;
                }
                ////
                
                SV_Netchan_Decode(cl, msg->data + msg->readcount, msg->cursize - msg->readcount);
                if(cl->state == CS_ZOMBIE)
                    return;

                cl->lastPacketTime = svs.time;
                SV_ExecuteClientMessage(cl, msg);
                return;
            }
        }

        NET_OutOfBandPrint(NS_SERVER, from, "disconnect");
        unknown_var = 0;
        Hunk_ClearTempMemoryInternal();
    }
    else
    {
        SV_ConnectionlessPacket(from, msg);
    }
}

int custom_SV_CanReplaceServerCommand(client_t *client, const char *command)
{
    //// [exploit patch] UNTESTED
    return -1;
}

////
/*
Purpose:
Prevent server from no longer appearing in client list after masterserver spent some time unreachable.
Using the sv_heartbeatDelay cvar which has a default value lower than HEARTBEAT_MSEC.
See:
- https://github.com/xtnded/codextended/blob/f7b28c8b8ee4cfb03f8d46d6e1df2efe0380cc1b/src/sv_main.c#L337
- https://github.com/id-Software/Enemy-Territory/blob/40342a9e3690cb5b627a433d4d5cbf30e3c57698/src/server/sv_main.c#L244
- https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/libcod/libcod.cpp#L505
- https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_main_pc_mp.cpp#L216

// UNTESTED

TODO: Check if issue wasn't caused by not ignoring SV_AuthorizeIpPacket
See https://github.com/xtnded/codextended/blob/f7b28c8b8ee4cfb03f8d46d6e1df2efe0380cc1b/src/sv_client.c#L234
*/
void custom_SV_MasterHeartbeat(const char *hbname)
{
    static netadr_t adr[MAX_MASTER_SERVERS];
    int i;
    
    if(!com_dedicated || com_dedicated->integer != 2)
        return;
    
    if (svs.time >= svs.nextHeartbeatTime)
    {
        svs.nextHeartbeatTime = svs.time + (sv_heartbeatDelay->integer * 1000); // Original: HEARTBEAT_MSEC
        
        for (i = 0; i < MAX_MASTER_SERVERS; i++)
        {
            if(!*sv_master[i]->string)
                continue;
            
            if (sv_master[i]->modified)
            {
                sv_master[i]->modified = qfalse;

                Com_Printf("Resolving %s\n", sv_master[i]->string);

                if (!NET_StringToAdr(sv_master[i]->string, &adr[i]))
                {
                    Com_Printf("Couldn't resolve address: %s\n", sv_master[i]->string);
                    Cvar_Set(sv_master[i]->name, "");
                    sv_master[i]->modified = qfalse;
                    continue;
                }

                if(!strstr(":", sv_master[i]->string))
                    adr[i].port = BigShort(PORT_MASTER);

                Com_Printf("%s resolved to %i.%i.%i.%i:%i\n",
                    sv_master[i]->string,
                    adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
                    BigShort(adr[i].port)
                );
            }

            Com_Printf("Sending heartbeat to %s\n", sv_master[i]->string);
            NET_OutOfBandPrint(NS_SERVER, adr[i], "heartbeat %s\n", hbname);
        }
    }
}
////

//// ioquake3 rate limit connectionless requests
// See https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c
// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024
static leakyBucket_t buckets[MAX_BUCKETS];
static leakyBucket_t* bucketHashes[MAX_HASHES];
leakyBucket_t outboundLeakyBucket;
time_t sys_timeBase = 0; // Base time in seconds
uint64_t Sys_Milliseconds64(void) // Current time in ms, using sys_timeBase as origin
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    if (!sys_timeBase)
    {
        sys_timeBase = tp.tv_sec;
        return tp.tv_usec / 1000;
    }
    return (tp.tv_sec - sys_timeBase) * 1000 + tp.tv_usec / 1000;
}
static long SVC_HashForAddress(netadr_t address)
{
    unsigned char *ip = address.ip;
    int	i;
    long hash = 0;

    for(i = 0; i < 4; i++)
        hash += (long)(ip[i]) * (i + 119);

    hash = (hash ^ (hash >> 10) ^ (hash >> 20));
    hash &= (MAX_HASHES - 1);

    return hash;
}
static leakyBucket_t * SVC_BucketForAddress(netadr_t address, int burst, int period)
{
    leakyBucket_t *bucket = NULL;
    int i;
    long hash = SVC_HashForAddress(address);
    uint64_t now = Sys_Milliseconds64();

    for(bucket = bucketHashes[hash]; bucket; bucket = bucket->next)
        if(memcmp(bucket->adr, address.ip, 4) == 0)
            return bucket;

    for (i = 0; i < MAX_BUCKETS; i++)
    {
        int interval;

        bucket = &buckets[i];
        interval = now - bucket->lastTime;

        // Reclaim expired buckets
        if (bucket->lastTime > 0 && (interval > (burst * period) ||
                                       interval < 0))
        {
            if(bucket->prev != NULL)
                bucket->prev->next = bucket->next;
            else
                bucketHashes[bucket->hash] = bucket->next;

            if(bucket->next != NULL)
                bucket->next->prev = bucket->prev;

            memset(bucket, 0, sizeof(leakyBucket_t));
        }

        if (bucket->type == 0)
        {
            bucket->type = address.type;
            memcpy(bucket->adr, address.ip, 4);

            bucket->lastTime = now;
            bucket->burst = 0;
            bucket->hash = hash;

            // Add to the head of the relevant hash chain
            bucket->next = bucketHashes[hash];
            if(bucketHashes[hash] != NULL)
                bucketHashes[hash]->prev = bucket;

            bucket->prev = NULL;
            bucketHashes[hash] = bucket;

            return bucket;
        }
    }

    // Couldn't allocate a bucket for this address
    return NULL;
}
bool SVC_RateLimit(leakyBucket_t *bucket, int burst, int period)
{
    if (bucket != NULL)
    {
        uint64_t now = Sys_Milliseconds64();
        int interval = now - bucket->lastTime;
        int expired = interval / period;
        int expiredRemainder = interval % period;

        if (expired > bucket->burst || interval < 0)
        {
            bucket->burst = 0;
            bucket->lastTime = now;
        }
        else
        {
            bucket->burst -= expired;
            bucket->lastTime = now - expiredRemainder;
        }

        if (bucket->burst < burst)
        {
            bucket->burst++;
            return false;
        }
    }
    return true;
}
bool SVC_RateLimitAddress(netadr_t from, int burst, int period)
{
    leakyBucket_t *bucket = SVC_BucketForAddress(from, burst, period);
    return SVC_RateLimit(bucket, burst, period);
}
////

bool SVC_callback(const char *str, const char *ip)
{
    if (codecallback_client_spam && Scr_IsSystemActive())
    {
        Scr_AddString(ip);
        Scr_AddString(str);
        short ret = Scr_ExecThread(codecallback_client_spam, 2);
        Scr_FreeThread(ret);
        return true;
    }
    return false;
}

bool SVC_ApplyRconLimit(netadr_t from, qboolean badRconPassword)
{
    // Prevent using rcon as an amplifier and make dictionary attacks impractical
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("RCON:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }
    
    if (badRconPassword)
    {
        static leakyBucket_t bucket;

        // Make DoS via rcon impractical
        if (SVC_RateLimit(&bucket, 10, 1000))
        {
            if(!SVC_callback("RCON:GLOBAL", NET_AdrToString(from)))
                Com_DPrintf("SVC_RemoteCommand: rate limit exceeded, dropping request\n");
            return true;
        }
    }
    
    return false;
}
bool SVC_ApplyStatusLimit(netadr_t from)
{
    // Prevent using getstatus as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }

    // Allow getstatus to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("STATUS:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SVC_Status: rate limit exceeded, dropping request\n");
        return true;
    }

    return false;
}

// See https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_client_mp.cpp#L1685
void SV_AuthorizeRequest(netadr_t from, int challenge)
{
    char gameDir[MAX_STRINGLENGTH];

    if(svs.authorizeAddress.type == NA_BAD)
        return;

    gameDir[0] = '\0';
    if(*fs_game->string)
        Q_strncpyz(gameDir, fs_game->string, sizeof(gameDir));

    Com_DPrintf("sending getIpAuthorize for %s\n", NET_AdrToString(from));
    
    NET_OutOfBandPrint(NS_SERVER, svs.authorizeAddress,
        va("getIpAuthorize %i %i.%i.%i.%i %s %i",
            challenge,
            from.ip[0],
            from.ip[1],
            from.ip[2],
            from.ip[3],
            gameDir,
            sv_allowAnonymous->integer
        )
    );
}
void custom_SV_GetChallenge(netadr_t from)
{
    // Prevent using getchallenge as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow getchallenge to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("CHALLENGE:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
        return;
    }
    
    int i;
    int oldest;
    int oldestTime;
    challenge_t *challenge;

    oldest = 0;
    oldestTime = 0x7FFFFFFF;
    challenge = &svs.challenges[0];
    
    for (i = 0; i < MAX_CHALLENGES; i++, challenge++)
    {
        if(!challenge->connected && NET_CompareAdr(from, challenge->adr))
            break;

        if (challenge->time < oldestTime)
        {
            oldestTime = challenge->time;
            oldest = i;
        }
    }
    
    if (i == MAX_CHALLENGES)
    {
        challenge = &svs.challenges[oldest];

        challenge->challenge = ((rand() << 16) ^ rand()) ^ svs.time;
        challenge->adr = from;
        challenge->firstTime = svs.time;
        challenge->firstPing = 0;
        challenge->time = svs.time;
        challenge->connected = qfalse;

        i = oldest;
    }
    
    if (!net_lanauthorize->integer && Sys_IsLANAddress(from))
    {
        challenge->pingTime = svs.time;
        NET_OutOfBandPrint(NS_SERVER, from, va("challengeResponse %i", challenge->challenge));
        return;
    }
    
    if (!svs.authorizeAddress.ip[0] && svs.authorizeAddress.type != NA_BAD)
    {
        Com_Printf("Resolving %s\n", AUTHORIZE_SERVER_NAME);
        if (!NET_StringToAdr(AUTHORIZE_SERVER_NAME, &svs.authorizeAddress))
        {
            Com_Printf("Couldn't resolve address\n");
            return;
        }
        svs.authorizeAddress.port = BigShort(PORT_AUTHORIZE);
        Com_Printf("%s resolved to %i.%i.%i.%i:%i\n",
                    AUTHORIZE_SERVER_NAME,
                    svs.authorizeAddress.ip[0],
                    svs.authorizeAddress.ip[1],
                    svs.authorizeAddress.ip[2],
                    svs.authorizeAddress.ip[3],
                    BigShort(svs.authorizeAddress.port)
                );
    }
    
    if ((AUTHORIZE_TIMEOUT < svs.time - svs.sv_lastTimeMasterServerCommunicated) && (AUTHORIZE_TIMEOUT < svs.time - challenge->firstTime))
    {
        Com_DPrintf("authorize server timed out\n");
        challenge->pingTime = svs.time;
        if(sv_onlyVisibleClients->integer)
            NET_OutOfBandPrint(NS_SERVER, challenge->adr, va("challengeResponse %i %i", challenge->challenge, sv_onlyVisibleClients->integer));
        else
            NET_OutOfBandPrint(NS_SERVER, challenge->adr, va("challengeResponse %i", challenge->challenge));
        return;
    }
    
    SV_AuthorizeRequest(from, svs.challenges[i].challenge);
}

void hook_SV_DirectConnect(netadr_t from)
{
    // Prevent using connect as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        Com_DPrintf("SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow connect to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        Com_DPrintf("SV_DirectConnect: rate limit exceeded, dropping request\n");
        return;
    }
    
    bool unbanned;
    char* userinfo;
    char ip[16];
    std::stringstream ss;
    std::string argBackup;
    
    unbanned = false;
    userinfo = Cmd_Argv(1);
    ss << "connect \"" << userinfo << "\"";
    argBackup = ss.str();
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d", from.ip[0], from.ip[1], from.ip[2], from.ip[3]);

    auto banInfo = getBanInfoForIp(ip);
    if(std::get<0>(banInfo) == true) // banned
    {
        time_t current_time = time(NULL);
        std::string remainingTime;
        
        if(std::get<1>(banInfo) != -1) // duration
        {
            int elapsed_seconds = difftime(current_time, std::get<2>(banInfo)); // ban date
            int remaining_seconds = std::get<1>(banInfo) - elapsed_seconds;
            if (remaining_seconds <= 0)
            {
                Cbuf_ExecuteText(EXEC_APPEND, va("unban %s\n", ip));
                unbanned = true;
            }
            else
            {
                int days = remaining_seconds / (60 * 60 * 24);
                int hours = (remaining_seconds % (60 * 60 * 24)) / (60 * 60);
                int minutes = (remaining_seconds % (60 * 60)) / 60;
                int seconds = remaining_seconds % 60;

                ss.str(std::string());
                ss.clear();

                if (days > 0)
                {
                    ss << days << " day" << (days > 1 ? "s" : "");
                    if(hours > 0)
                    ss << ", " << hours << " hour" << (hours > 1 ? "s" : "");
                }
                else if (hours > 0)
                {
                    ss << hours << " hour" << (hours > 1 ? "s" : "");
                    if(minutes > 0)
                    ss << ", " << minutes << " minute" << (minutes > 1 ? "s" : "");
                }
                else if(minutes > 0)
                    ss << minutes << " minute" << (minutes > 1 ? "s" : "");
                else
                    ss << seconds << " second" << (seconds > 1 ? "s" : "");

                remainingTime = ss.str();
            }
        }

        if (!unbanned)
        {
            std::string banInfoMessage = "error\nBanned IP";
            if (std::get<3>(banInfo) != "none")
            {
                banInfoMessage.append(" - Reason: ");
                banInfoMessage.append(std::get<3>(banInfo));
            }
            if (!remainingTime.empty())
            {
                banInfoMessage.append(" - Remaining: ");
                banInfoMessage.append(remainingTime);
            }
            
            Com_Printf("rejected connection from banned IP %s\n", NET_AdrToString(from));
            NET_OutOfBandPrint(NS_SERVER, from, banInfoMessage.c_str());
            return;
        }
    }

    if(unbanned)
        Cmd_TokenizeString(argBackup.c_str());
    
    SV_DirectConnect(from);
}

void hook_SV_AuthorizeIpPacket(netadr_t from)
{
    // Prevent ipAuthorize log spam DoS
    if (SVC_RateLimitAddress(from, 20, 1000))
    {
        Com_DPrintf("SV_AuthorizeIpPacket: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow ipAuthorize to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        Com_DPrintf("SV_AuthorizeIpPacket: rate limit exceeded, dropping request\n");
        return;
    }

    SV_AuthorizeIpPacket(from);
}

void hook_SVC_Info(netadr_t from)
{
    // Prevent using getinfo as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if (!SVC_callback("INFO:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow getinfo to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("INFO:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SVC_Info: rate limit exceeded, dropping request\n");
        return;
    }

    SVC_Info(from);
}

void custom_SVC_Status(netadr_t from)
{
    char player[MAX_INFO_STRING];
    char infostring[MAX_INFO_STRING];
    char keywords[MAX_INFO_STRING];
    char status[MAX_MSGLEN];
    int statusLength;
    size_t playerLength;
    int i;
    client_t *cl;
    char *g_password;

    if(SVC_ApplyStatusLimit(from))
        return;
    
    strcpy(infostring, Cvar_InfoString(CVAR_SERVERINFO));
    
    Info_SetValueForKey(infostring, "challenge", Cmd_Argv(1));
    
    if (Cvar_VariableValue("fs_restrict"))
    {
        Com_sprintf(keywords, sizeof(keywords), "demo %s", Info_ValueForKey(infostring, "sv_keywords"));
        Info_SetValueForKey(infostring, "sv_keywords", keywords);
    }
    
    status[0] = 0;
    statusLength = 0;
    for (i = 0; i < sv_maxclients->integer; i++)
    {
        cl = &svs.clients[i];
        if (cl->state >= CS_CONNECTED)
        {
            int clientScore = 0;
            
            Com_sprintf(player, sizeof(player), "%i %i \"%s\"\n", clientScore, cl->ping, cl->name);
            
            playerLength = strlen(player);
            if(statusLength + playerLength >= sizeof(status))
                break;

            strcpy(status + statusLength, player);
            statusLength += playerLength;
        }
    }
    
    g_password = Cvar_VariableString("g_password");
    Info_SetValueForKey(infostring, "pswrd", va("%i", *g_password ? 1 : 0));

    // Inform about fs_game usage, by default for player's convenience
    Info_SetValueForKey(infostring, "fs_game", va("%s", *fs_game->string ? fs_game->string : "0"));
    
    NET_OutOfBandPrint(NS_SERVER, from, "statusResponse\n%s\n%s", infostring, status);
}

// See https://nachtimwald.com/2017/04/02/constant-time-string-comparison-in-c/
bool str_iseq(const char *s1, const char *s2)
{
    int             m = 0;
    volatile size_t i = 0;
    volatile size_t j = 0;
    volatile size_t k = 0;

    if (s1 == NULL || s2 == NULL)
        return false;

    while (1) {
        m |= s1[i]^s2[j];

        if (s1[i] == '\0')
            break;
        i++;

        if (s2[j] != '\0')
            j++;
        if (s2[j] == '\0')
            k++;
    }

    return m == 0;
}

void custom_SVC_RemoteCommand(netadr_t from, msg_t *msg)
{
    char sv_outputbuf[SV_OUTPUTBUF_LENGTH];
    int argc;
    char *argv;
    char *password;
    char remaining[1024];
    int len;
    int max_len;
    int arg;
    bool valid;

    //// [exploit patch] RCON half-second limit
    /* See:
    - https://aluigi.altervista.org/patches/q3rconz.lpatch
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/codextended.c#L291
    - https://github.com/ibuddieat/zk_libcod/blob/dec45a39e3ae8f049cf5d7f4f5b8ec89dea88b3d/code/libcod.cpp#L4865
    */
    // (patch = no more return)

    /*int time = Com_Milliseconds();
    if(rcon_lasttime && time < (rcon_lasttime + 500))
        return;
    rcon_lasttime = time;*/
    ////

    password = Cmd_Argv(1);
    qboolean badRconPassword = !strlen(sv_rconPassword->string) || !str_iseq(password, sv_rconPassword->string);

    if(SVC_ApplyRconLimit(from, badRconPassword))
        return;

    if (badRconPassword)
    {
        valid = false;
        Com_Printf("Bad rcon from %s:\n%s\n", NET_AdrToString(from), Cmd_Argv(2));
    }
    else
    {
        valid = true;
        Com_Printf("Rcon from %s:\n%s\n", NET_AdrToString(from), Cmd_Argv(2));
    }

    svs.redirectAddress = from;

    Com_BeginRedirect(sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect);

    if (!strlen(sv_rconPassword->string))
    {
        Com_Printf("No rconpassword set on the server.\n");
    }
    else if (!valid)
    {
        Com_Printf("Bad rconpassword.\n");
    }
    else
    {
        len = 0;
        max_len = MAX_STRINGLENGTH;
        arg = 2;
        while (argc = Cmd_Argc(), arg < argc)
        {
            argv = Cmd_Argv(arg);
            len = Com_AddToString(argv, remaining, len, max_len, 1);
            len = Com_AddToString(" ", remaining, len, max_len, 0);
            arg++;
        }
        if (len < max_len)
        {
            remaining[len] = 0;
            Cmd_ExecuteString(remaining);
        }
    }

    Com_EndRedirect();
}

const char* hook_AuthorizeState(int arg)
{
    const char* s = Cmd_Argv(arg);
    if(sv_cracked->integer && !strcmp(s, "deny"))
        return "accept";
    return s;
}

void custom_SV_SendClientGameState(client_t *client)
{
    int start;
    entityState_t /**base,*/ nullstate;
    msg_t msg;
    byte msgBuffer[MAX_MSGLEN];
    int clientNum = client - svs.clients;
    
    while(client->state != CS_FREE && client->netchan.unsentFragments)
        SV_Netchan_TransmitNextFragment(&client->netchan);

    Com_DPrintf("SV_SendClientGameState() for %s\n", client->name);
    Com_DPrintf("Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name);

    client->state = CS_PRIMED;
    client->pureAuthentic = 0;
    client->gamestateMessageNum = client->netchan.outgoingSequence;

    // Init/reset custom player state
    customPlayerState[clientNum] = {};

    // Restore user-provided rate and snaps after download
    SV_UserinfoChanged(client);
    
    MSG_Init(&msg, msgBuffer, sizeof(msgBuffer));
    MSG_WriteLong(&msg, client->lastClientCommand);
    SV_UpdateServerCommandsToClient(client, &msg);
    MSG_WriteByte(&msg, svc_gamestate);
    MSG_WriteLong(&msg, client->reliableSequence);
    
    for (start = 0; start < MAX_CONFIGSTRINGS; start++)
    {
        if (sv.configstrings[start][0])
        {
            MSG_WriteByte(&msg, svc_configstring);
            MSG_WriteShort(&msg, start);
            std::string csCopy(sv.configstrings[start]);
            if (start == 1)
            {
                bool clientUsing1_1x = *Info_ValueForKey(client->userinfo, "xtndedbuild");

                if (!sv_allowDownload->integer)
                {
                    /*
                    If client has cl_allowDownload enabled, but sv_allowDownload is disabled, client will attempt to download anyway, and then fail joining.
                    To fix this, if sv_allowDownload is disabled, write cl_allowDownload disabled on client, in case is enabled.
                    */
                    csCopy.append("\\cl_allowDownload\\0");
                }
                else
                {
                    if (sv_downloadForce->integer)
                    {
                        csCopy.append("\\cl_allowDownload\\1");
                    }
                    else
                    {
                        /*
                        1.1x mod requires download forcing, even if player enables cl_allowDownload by himself before joining.
                        See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/cl_main.cpp#L41
                        */
                        if(clientUsing1_1x)
                            csCopy.append("\\cl_allowDownload\\1");
                    }
                }
            }

            MSG_WriteBigString(&msg, csCopy.c_str());
        }
    }
    
    // TODO: clean
    memset(&nullstate, 0, sizeof(nullstate));
    int *base = (int*)(0x08357680);
    for (start = 0; start < MAX_GENTITIES; start++)
    {
        base += 0x5f;
        //base = &sv.svEntities[start].baseline.s;
        if(!*base)
        //if(!base->number)
            continue;
        MSG_WriteByte(&msg, svc_baseline);
        //MSG_WriteDeltaEntity(&msg, &nullstate, base, qtrue);
        ((void(*)(
            msg_t*,
            struct entityState_s*,
            int*,
            qboolean))0x807f698)(&msg, &nullstate, base, qtrue); //MSG_WriteDeltaEntity
    }
    
    MSG_WriteByte(&msg, svc_EOF);
    MSG_WriteLong(&msg, clientNum);
    MSG_WriteLong(&msg, sv.checksumFeed);
    MSG_WriteByte(&msg, svc_EOF);

    Com_DPrintf("Sending %i bytes in gamestate to client: %i\n", msg.cursize, clientNum);

    SV_SendMessageToClient(&msg, client);
}

scr_error_t scr_errors[MAX_ERROR_BUFFER];
int scr_errors_index = 0;
void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message)
{
    if (codecallback_error && Scr_IsSystemActive() && !com_errorEntered)
    {
        if (!strncmp(message, "exceeded maximum number of script variables", 43))
        {
            /* Since we cannot allocate more script variables, further
             execution of scripts or script callbacks could lead to an
             undefined state (in script) or endless error loops, so we stop */
            Com_Error(ERR_DROP, "\x15%s", "exceeded maximum number of script variables");
        }

        if (terminal || emit)
        {
            Scr_AddString(message);
            Scr_AddString(internal_function);
            Scr_AddInt(terminal);
            short ret = Scr_ExecThread(codecallback_error, 3);
            Scr_FreeThread(ret);
        }
        else
        {
            /* If the error is non-critical (not stopping the server), save it
             so we can emit it later at G_RunFrame which is a rather safe
             spot compared to if we emit it directly here within the
             internals of the scripting engine where we risk crashing it
             with a segmentation fault */
            if (scr_errors_index < MAX_ERROR_BUFFER)
            {
                strncpy(scr_errors[scr_errors_index].internal_function, internal_function, sizeof(scr_errors[scr_errors_index].internal_function));
                strncpy(scr_errors[scr_errors_index].message, message, sizeof(scr_errors[scr_errors_index].message));
                scr_errors_index++;
            }
            else
            {
                printf("Warning: Errors buffer full, not calling CodeCallback_Error for '%s'\n", message);
            }
        }
    }
}

////// Custom operator commands
//// ban & unban
std::tuple<bool, int, int, std::string> getBanInfoForIp(char* ip)
{
    char *file;
    std::string token;
    const char *text;
    std::tuple<bool, int, int, std::string> banInfo;

    banInfo = std::make_tuple(false, 0, 0, "");

    if(FS_ReadFile("ban.txt", (void **)&file) < 0)
        return banInfo;

    text = file;
    while (1)
    {
        token = Com_Parse(&text);
        if(token.empty())
            break;

        if (!strcmp(token.c_str(), ip))
        {
            std::get<0>(banInfo) = true;                // banned
            Com_Parse(&text);                           // player name
            std::get<1>(banInfo) = Com_ParseInt(&text); // duration
            std::get<2>(banInfo) = Com_ParseInt(&text); // ban date
            std::get<3>(banInfo) = Com_Parse(&text);    // reason
            break;
        }
        Com_SkipRestOfLine(&text);
    }
    FS_FreeFile(file);
    return banInfo;
}

/*
-i: ip
-n: banned client number
-r: reason
-d: duration
-a: admin client number
*/
const std::array<std::string, 5> banParameters = {"-i", "-n", "-r", "-d", "-a"};
const std::array<std::string, 2> unbanParameters = {"-i", "-a"};

template <std::size_t N>
bool isValidBanParameter(std::string toCheck, std::array<std::string, N> parameters)
{
    for (const std::string&parameter : parameters)
    {
        if(toCheck == parameter)
            return true;
    }
    return false;
}

static void ban()
{
    if (!com_sv_running->integer)
    {
        Com_Printf("Server is not running.\n");
        return;
    }

    if (Cmd_Argc() < 3)
    {
        Com_Printf("Usage: ban (-i <IP address> | -n <client number>) [-r reason] [-d duration]\n");
        Com_Printf("Notes: Use h for hours or d for days\n");
        return;
    }

    std::vector<std::string> argvList;
    std::map<std::string, std::string> parsedParameters;
    std::string infoMessage;
    bool useIp = false;
    bool useClientnum = false;
    int file;
    bool clAdmin_searched = false;
    client_t *clToBan;
    client_t *clAdmin;
    char ip[16];
    char cleanName[64] = "n/a";
    time_t current_time;
    int duration = -1;
    std::string duration_drop;
    std::string reason_log = "none";
    std::string reason_drop;

    // Directly store all the argv to be able to use Cmd_TokenizeString before the end of the parse
    for (int i = 1; i < Cmd_Argc(); i++)
    {
        std::string argv = Cmd_Argv(i);
        argvList.push_back(argv);
    }
    
    //// Parse and store the parameters
    for (std::size_t i = 0; i < argvList.size(); i++)
    {
        std::string argv = argvList[i];
        if (isValidBanParameter(argv, banParameters)) // Found an option
        {
            if (parsedParameters.find(argv) == parsedParameters.end())
            {
                // Parse the argument
                std::string value;
                for (std::size_t j = i+1; j < argvList.size(); j++)
                {
                    std::string argv_next = argvList[j];
                    if (!isValidBanParameter(argv_next, banParameters))
                    {
                        if(j != i+1)
                            value.append(" ");
                        value.append(argv_next);
                    }
                    else
                        break;
                }
                // Store the pair
                if(!value.empty())
                    parsedParameters[argv] = value;

                /*
                Check if got admin client after first storage and only once
                because it should be passed as first parameter from gsc
                so you can redirect the error messages since the beginning
                */
                if (!clAdmin_searched)
                {
                    auto adminParam = parsedParameters.find("-a");
                    if (adminParam != parsedParameters.end())
                    {
                        clAdmin = &svs.clients[std::stoi(adminParam->second)];
                    }
                    clAdmin_searched = true;
                }
            }
            else
            {
                infoMessage = "Duplicated option " + argv;
                sendMessageToClient_orServerConsole(clAdmin, infoMessage);
                return;
            }
        }
        else if (argv[0] == '-' && !isValidBanParameter(argv, banParameters))
        {
            infoMessage = "Unrecognized option " + argv;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
    }
    ////
    
    //// Check the parameters
    // Client number
    if (parsedParameters.find("-n") != parsedParameters.end())
    {
        // Check if specified both an IP and a client number
        if (parsedParameters.find("-i") != parsedParameters.end())
        {
            infoMessage = "Don't use both an IP and a client number";
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
        useClientnum = true;
    }

    // IP
    auto ipParam = parsedParameters.find("-i");
    if (ipParam != parsedParameters.end())
    {
        struct sockaddr_in sa;
        if (!inet_pton(AF_INET, ipParam->second.c_str(), &(sa.sin_addr)))
        {
            infoMessage = "Invalid IP address " + ipParam->second;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
        useIp = true;
        std::strcpy(ip, ipParam->second.c_str());
    }

    if (!useClientnum && !useIp)
    {
        infoMessage = "Use an IP or a client number";
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
        return;
    }

    // Duration
    auto durationParam = parsedParameters.find("-d");
    if (durationParam != parsedParameters.end())
    {
        char durationParam_lastChar = durationParam->second.back();
        if (durationParam_lastChar != 'h' && durationParam_lastChar != 'd')
        {
            infoMessage = "Invalid duration parameter " + durationParam->second;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
        else
        {
            durationParam->second.pop_back(); // Remove unit indicator
            if (durationParam->second.empty())
            {
                infoMessage = "Invalid duration parameter " + durationParam->second;
                sendMessageToClient_orServerConsole(clAdmin, infoMessage);
                return;
            }
            else
            {
                for (int i = 0; durationParam->second[i]; i++)
                {
                    if (durationParam->second[i] < '0' || durationParam->second[i] > '9')
                    {
                        infoMessage = "Invalid duration parameter " + durationParam->second;
                        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
                        return;
                    }
                }
                duration = std::stoi(durationParam->second);
            }
        }
        if(durationParam_lastChar == 'h')
        {
            duration_drop = durationParam->second + " hour";
            if(duration > 1)
                duration_drop.append("s");
            duration *= 3600;
        }
        else if(durationParam_lastChar == 'd')
        {
            duration_drop = durationParam->second + " day";
            if(duration > 1)
                duration_drop.append("s");
            duration *= 86400;
        }
    }

    // Reason
    auto reasonParam = parsedParameters.find("-r");
    if (reasonParam != parsedParameters.end())
    {
        reason_log = reasonParam->second.c_str();
        reason_drop = "Ban reason: " + reasonParam->second;
    }

    // Add duration to drop message after reason
    if (!duration_drop.empty())
    {
        if(reason_drop.empty())
        {
            reason_drop = "Ban duration: " + duration_drop;
        }
        else
        {
            reason_drop.append(" - ");
            reason_drop.append("Duration: ");
            reason_drop.append(duration_drop);
        }
    }
    else if (reason_drop.empty())
    {
        reason_drop = "EXE_PLAYERKICKED";
    }
    ////

    // Find the player
    if (useClientnum)
    {
        clToBan = &svs.clients[std::stoi(parsedParameters.find("-n")->second)];
        if (!clToBan)
        {
            infoMessage = "Couldn't find player by num " + parsedParameters.find("-n")->second;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
        else
        {
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
                clToBan->netchan.remoteAddress.ip[0],
                clToBan->netchan.remoteAddress.ip[1],
                clToBan->netchan.remoteAddress.ip[2],
                clToBan->netchan.remoteAddress.ip[3]
            );
        }
    }
    else if (useIp)
    {
        int i;
        client_t *clCheck;
        for (i = 0, clCheck = svs.clients; i < sv_maxclients->integer; i++, clCheck++)
        {
            if (clCheck->state > CS_CONNECTED)
            {
                char ip_check[16];
                snprintf(ip_check, sizeof(ip_check), "%d.%d.%d.%d",
                    clCheck->netchan.remoteAddress.ip[0],
                    clCheck->netchan.remoteAddress.ip[1],
                    clCheck->netchan.remoteAddress.ip[2],
                    clCheck->netchan.remoteAddress.ip[3]
                );
                
                if (!strcmp(ip_check, ip))
                {
                    clToBan = clCheck;
                    break;
                }
            }
        }
    }
    
    auto banInfo = getBanInfoForIp(ip);
    if(std::get<0>(banInfo) == true) // banned
    {
        std::ostringstream oss;
        oss << "This IP (" << ip << ") is already banned";
        infoMessage = oss.str();
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
        return;
    }
    
    if (clToBan)
        Q_strncpyz(cleanName, clToBan->name, sizeof(cleanName));
    
    // Add IP to ban.txt
    if (FS_FOpenFileByMode("ban.txt", &file, FS_APPEND) < 0)
    {
        infoMessage = "Couldn't open ban.txt";
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
        return;
    }
    else
    {
        current_time = time(NULL);
        FS_Write(file, "\"%s\" \"%s\" \"%i\" \"%i\" \"%s\"\r\n", ip, cleanName, duration, current_time, reason_log.c_str());
        FS_FCloseFile(file);
    }

    // Disconnect the player
    if (clToBan)
    {
        SV_DropClient(clToBan, reason_drop.c_str());
        clToBan->lastPacketTime = svs.time;
    }
}
static void unban()
{
    if (!com_sv_running->integer)
    {
        Com_Printf("Server is not running.\n");
        return;
    }

    if (Cmd_Argc() < 2)
    {
        Com_Printf("Usage: unban -i <IP address>\n");
        return;
    }
    
    std::vector<std::string> argvList;
    std::map<std::string, std::string> parsedParameters;
    std::string infoMessage;
    bool clAdmin_searched = false;
    client_t *clAdmin;
    char *file;
    int fileSize;
    char *line;
    std::string token;
    bool found = false;
    char *text;
    std::string ip;
    
    // Directly store all the argv to be able to use Cmd_TokenizeString before the end of the parse
    for (int i = 1; i < Cmd_Argc(); i++)
    {
        std::string argv = Cmd_Argv(i);
        argvList.push_back(argv);
    }

    //// Parse and store the parameters
    for (std::size_t i = 0; i < argvList.size(); i++)
    {
        std::string argv = argvList[i];
        if (isValidBanParameter(argv, unbanParameters)) // Found an option
        {
            if (parsedParameters.find(argv) == parsedParameters.end())
            {
                // Parse the argument
                std::string value;
                for (std::size_t j = i+1; j < argvList.size(); j++)
                {
                    std::string argv_next = argvList[j];
                    if (!isValidBanParameter(argv_next, unbanParameters))
                    {
                        if(j != i+1)
                            value.append(" ");
                        value.append(argv_next);
                    }
                    else
                        break;
                }
                // Store the pair
                if(!value.empty())
                    parsedParameters[argv] = value;

                /*
                Check if got admin client after first storage and only once
                because it should be passed as first parameter from gsc
                so you can redirect the error messages since the beginning
                */
                if (!clAdmin_searched)
                {
                    auto adminParam = parsedParameters.find("-a");
                    if (adminParam != parsedParameters.end())
                    {
                        clAdmin = &svs.clients[std::stoi(adminParam->second)];
                    }
                    clAdmin_searched = true;
                }
            }
            else
            {
                infoMessage = "Duplicated option " + argv;
                sendMessageToClient_orServerConsole(clAdmin, infoMessage);
                return;
            }
        }
        else if (argv[0] == '-' && !isValidBanParameter(argv, unbanParameters))
        {
            infoMessage = "Unrecognized option " + argv;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
    }
    ////

    //// Check the parameters
    // IP
    auto ipParam = parsedParameters.find("-i");
    if (ipParam != parsedParameters.end())
    {
        struct sockaddr_in sa;
        if(!inet_pton(AF_INET, ipParam->second.c_str(), &(sa.sin_addr)))
        {
            infoMessage = "Invalid IP address " + ipParam->second;
            sendMessageToClient_orServerConsole(clAdmin, infoMessage);
            return;
        }
        ip = ipParam->second;
    }
    else
    {
        infoMessage = "Specify an IP address";
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
        return;
    }
    ////
    
    // Remove IP from ban.txt
    fileSize = FS_ReadFile("ban.txt", (void **)&file);
    if (fileSize < 0)
    {
        infoMessage = "Couldn't read ban.txt";
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
        return;        
    }
    
    text = file;
    while (1)
    {
        line = text;
        token = Com_Parse((const char **)&text);
        if(token.empty())
            break;

        if(token == ip)
            found = true;

        Com_SkipRestOfLine((const char **)&text);

        if (found)
        {
            memmove((unsigned char *)line, (unsigned char *)text, fileSize - (text - file) + 1);
            fileSize -= text - line;
            text = line;
            break;
        }
    }

    FS_WriteFile("ban.txt", file, fileSize);
    FS_FreeFile(file);

    if (found)
    {
        infoMessage = "Unbanned IP " + ip;
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
    }
    else
    {
        std::stringstream ss;
        ss << "IP " << ip << " not found";
        infoMessage = ss.str();
        sendMessageToClient_orServerConsole(clAdmin, infoMessage);
    }
}
////
//////

int custom_MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize, int outputBufSize)
{
    readsize = readsize * 8;
    byte *outptr = outputBuf;

    int get;
    int offset;
    int i;

    if(readsize <= 0)
        return 0;

    for (offset = 0, i = 0; offset < readsize && i < outputBufSize; i++)
    {
        Huff_offsetReceive(msgHuff.decompressor.tree, &get, (byte*)input, &offset); // See https://github.com/callofduty4x/CoD4x_Server/pull/396
        *outptr = (byte)get;
        outptr++;
    }

    return i;
}

void custom_SV_ExecuteClientMessage(client_t *cl, msg_t *msg)
{
    byte outputBuf[MAX_MSGLEN];
    msg_t decompressMsg;
    int c;

    MSG_Init(&decompressMsg, outputBuf, sizeof(outputBuf));

    //// [exploit patch] CVE-2018-10718 // UNTESTED
    // See https://github.com/momo5502/cod-exploits/tree/master/huffman
    decompressMsg.cursize = custom_MSG_ReadBitsCompress(msg->data + msg->readcount, outputBuf, msg->cursize - msg->readcount, decompressMsg.maxsize);
    if (decompressMsg.cursize == decompressMsg.maxsize)
    {
        SV_DropClient(cl, "SV_ExecuteClientMessage: Client sent oversize message");
        return;
    }
    ////
    
    /*
    Check "nextdl" to prevent client getting stuck downloading after a map_restart occurred.
    See https://github.com/id-Software/RTCW-MP/blob/937b209a3c14857bea09a692545c59ac1a241275/src/server/sv_client.c#L1578
    */
    if ((cl->serverId == sv_serverId_value || cl->downloadName[0])
        || (!cl->downloadName[0] && strstr(cl->lastClientCommandString, "nextdl")))
    {
        do
        {
            c = MSG_ReadBits(&decompressMsg, 2);
            if (c == clc_EOF || c != clc_clientCommand)
            {
                if (sv_pure->integer && cl->pureAuthentic == 2)
                {
                    cl->nextSnapshotTime = -1;
                    SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
                    cl->state = CS_ACTIVE;
                    SV_SendClientSnapshot(cl);
                    cl->state = CS_ZOMBIE;
                }
                if(c == clc_move)
                    SV_UserMove(cl, &decompressMsg, 1);
                else if(c == clc_moveNoDelta)
                    SV_UserMove(cl, &decompressMsg, 0);
                else if(c != clc_EOF)
                    Com_Printf("WARNING: bad command byte %i for client %i\n", c, cl - svs.clients);
                return;
            }

            if(!SV_ClientCommand(cl, &decompressMsg))
                return;
        } while (cl->state != CS_ZOMBIE);
    }
    else if ((cl->serverId & 0xF0) == (sv_serverId_value & 0xF0))
    {
        if(cl->state == CS_PRIMED)
            SV_ClientEnterWorld(cl, &cl->lastUsercmd);
    }
    else
    {
        if (cl->gamestateMessageNum < cl->messageAcknowledge)
        {
            Com_DPrintf("%s : dropped gamestate, resending\n", cl->name);
            SV_SendClientGameState(cl);
        }
    }
}

// See https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_client_mp.cpp#L2128
void custom_SV_ExecuteClientCommand(client_t *cl, const char *s, qboolean clientOK)
{
    ucmd_t *u;

    ((void(*)(int))0x080bfea0)(1); // Unknown function, seems related to scrAnimGlob
    Cmd_TokenizeString(s);

    for (u = ucmds; u->name; u++)
    {
        if (!strcmp(Cmd_Argv(0), u->name))
        {
            u->func(cl);
            break;
        }
    }

    if(clientOK)
        if(!u->name && sv.state == SS_GAME)
            VM_Call(gvm, GAME_CLIENT_COMMAND, cl - svs.clients);
}

void custom_SV_BeginDownload_f(client_t *cl)
{
    //// [exploit patch] q3dirtrav
    /* See:
    - https://aluigi.altervista.org/poc/q3dirtrav.zip
    - https://oldforum.aluigi.org/post3479.html#p3479
    */
    int args = Cmd_Argc();
    if (args > 1)
    {
        const char* arg1 = Cmd_Argv(1);
        if (!shouldServeFile(arg1))
        {
            char ip[16];
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
                cl->netchan.remoteAddress.ip[0],
                cl->netchan.remoteAddress.ip[1],
                cl->netchan.remoteAddress.ip[2],
                cl->netchan.remoteAddress.ip[3]);
            Com_Printf("WARNING: %s (%s) tried to download %s\n", cl->name, ip, arg1);
            return;
        }
    }
    ////

    hook_SV_BeginDownload_f->unhook();
    void (*SV_BeginDownload_f)(client_t *cl);
    *(int*)&SV_BeginDownload_f = hook_SV_BeginDownload_f->from;
    SV_BeginDownload_f(cl);
    hook_SV_BeginDownload_f->hook();
}

// See https://github.com/ibuddieat/zk_libcod/blob/dff123fad25d7b46d65685e9bca2111c8946a36e/code/libcod.cpp#L3600
void SV_WriteDownloadErrorToClient(client_t *cl, msg_t *msg, char *errorMessage)
{
    MSG_WriteByte(msg, svc_download);
    MSG_WriteShort(msg, 0);
    MSG_WriteLong(msg, -1);
    MSG_WriteString(msg, errorMessage);
    *cl->downloadName = 0;
}
void custom_SV_WriteDownloadToClient(client_t *cl, msg_t *msg)
{
    int curindex;
    int blksize;
    char downloadNameNoExt[MAX_QPATH];
    char errorMessage[MAX_STRINGLENGTH];

    if(!*cl->downloadName)
        return;
    
    cl->state = CS_CONNECTED;
    cl->rate = 25000;
    cl->snapshotMsec = 50;

    if (!cl->download)
    {
        if (!sv_allowDownload->integer)
        {
            Com_Printf("clientDownload: %d : \"%s\" download disabled\n", cl - svs.clients, cl->downloadName);

            if(sv_pure->integer)
                Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
            else
                Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);

            SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
            return;
        }

        Q_strncpyz(downloadNameNoExt, cl->downloadName, strlen(cl->downloadName) - 3);
        if (FS_iwPak(downloadNameNoExt, "main"))
        {
            Com_Printf("clientDownload: %d : \"%s\" cannot download id pk3 files\n", cl - svs.clients, cl->downloadName);
            Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEPAK\x15%s", cl->downloadName);
            SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
            return;
        }

        if ((cl->downloadSize = FS_SV_FOpenFileRead(cl->downloadName, &cl->download)) <= 0)
        {
            Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
            Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
            SV_WriteDownloadErrorToClient(cl, msg, errorMessage);
            return;
        }

        // Init download
        Com_Printf("clientDownload: %d : beginning \"%s\"\n", cl - svs.clients, cl->downloadName);
        cl->downloadCurrentBlock = cl->downloadClientBlock = cl->downloadXmitBlock = 0;
        cl->downloadCount = 0;
        cl->downloadEOF = qfalse;
    }
    
    while (cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW && cl->downloadSize != cl->downloadCount)
    {
        curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

        blksize = MAX_DOWNLOAD_BLKSIZE;
        if (sv_fastDownload->integer)
            blksize = MAX_DOWNLOAD_BLKSIZE_FAST;
        
        if (!cl->downloadBlocks[curindex])
        {
            // See https://github.com/ibuddieat/zk_libcod/blob/dfdd4ef17508ff8ffbaacb0353a6b736a9707cba/code/libcod.cpp#L3761
            cl->downloadBlocks[curindex] = (unsigned char *)Z_MallocInternal(MAX_DOWNLOAD_BLKSIZE_FAST);
        }
        cl->downloadBlockSize[curindex] = FS_Read(cl->downloadBlocks[curindex], blksize, cl->download);

        if (cl->downloadBlockSize[curindex] < 0)
        {
            // EOF
            cl->downloadCount = cl->downloadSize;
            break;
        }

        cl->downloadCount += cl->downloadBlockSize[curindex];
        // Load in next block
        cl->downloadCurrentBlock++;
    }

    // Check to see if we have eof condition and add the EOF block
    if (cl->downloadCount == cl->downloadSize && !cl->downloadEOF && cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW)
    {
        cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
        cl->downloadCurrentBlock++;
        cl->downloadEOF = qtrue;
    }

    if(cl->downloadClientBlock == cl->downloadCurrentBlock)
        return; // Nothing to transmit

    if (cl->downloadXmitBlock == cl->downloadCurrentBlock)
    {
        // FIXME: See https://github.com/id-Software/RTCW-MP/blob/937b209a3c14857bea09a692545c59ac1a241275/src/server/sv_client.c#L962
        if(svs.time - cl->downloadSendTime > 1000)
            cl->downloadXmitBlock = cl->downloadClientBlock;
        else
            return;
    }

    // Send current block
    curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

    MSG_WriteByte(msg, svc_download);
    MSG_WriteShort(msg, cl->downloadXmitBlock);

    // Block zero contains file size
    if(cl->downloadXmitBlock == 0)
        MSG_WriteLong(msg, cl->downloadSize);

    MSG_WriteShort(msg, cl->downloadBlockSize[curindex]);

    // Write the block
    if(cl->downloadBlockSize[curindex])
        MSG_WriteData(msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex]);

    Com_DPrintf("clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock);

    // Move on to the next block
    // It will get sent with next snapshot
    cl->downloadXmitBlock++;
    cl->downloadSendTime = svs.time;
}

// See https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_snapshot_mp.cpp#L686
static int SV_RateMsec(client_t *client, int messageSize)
{
    int rate;
    int rateMsec;
    
    if(messageSize > 1500)
        messageSize = 1500;

    rate = client->rate;
    if (sv_maxRate->integer)
    {
        if(sv_maxRate->integer < 1000)
            Cvar_Set("sv_MaxRate", "1000");

        if(sv_maxRate->integer < rate)
            rate = sv_maxRate->integer;
    }

    rateMsec = ((messageSize + HEADER_RATE_BYTES) * 1000) / rate;
    if(sv_debugRate->integer)
        Com_Printf("It would take %ims to send %i bytes to client %s (rate %i)\n", rateMsec, messageSize, client->name, client->rate);

    return rateMsec;
}

void custom_SV_SendClientMessages(void)
{
    int i;
    client_t *cl;
    int numclients = 0;

    sv.bpsTotalBytes = 0;
    sv.ubpsTotalBytes = 0;

    for (i = 0; i < sv_maxclients->integer; i++)
    {
        cl = &svs.clients[i];

        if(!cl->state)
            continue;
        if(svs.time < cl->nextSnapshotTime)
            continue;

        numclients++;

        if (sv_fastDownload->integer && cl->download)
        {
            for (int j = 0; j < 1 + ((sv_fps->integer / 20) * MAX_DOWNLOAD_WINDOW); j++)
            {
                while (cl->netchan.unsentFragments)
                {
                    cl->nextSnapshotTime = svs.time + SV_RateMsec(cl, cl->netchan.unsentLength - cl->netchan.unsentFragmentStart);
                    SV_Netchan_TransmitNextFragment(&cl->netchan);
                }
                SV_SendClientSnapshot(cl);
            }
        }
        else
        {
            if (cl->netchan.unsentFragments)
            {
                cl->nextSnapshotTime = svs.time + SV_RateMsec(cl, cl->netchan.unsentLength - cl->netchan.unsentFragmentStart);
                SV_Netchan_TransmitNextFragment(&cl->netchan);
                continue;
            }
            SV_SendClientSnapshot(cl);
        }
    }

    if (sv_showAverageBPS->integer && numclients > 0)
    {
        float ave = 0, uave = 0;

        for (i = 0; i < MAX_BPS_WINDOW - 1; i++)
        {
            sv.bpsWindow[i] = sv.bpsWindow[i + 1];
            ave += sv.bpsWindow[i];

            sv.ubpsWindow[i] = sv.ubpsWindow[i + 1];
            uave += sv.ubpsWindow[i];
        }

        sv.bpsWindow[MAX_BPS_WINDOW - 1] = sv.bpsTotalBytes;
        ave += sv.bpsTotalBytes;

        sv.ubpsWindow[MAX_BPS_WINDOW - 1] = sv.ubpsTotalBytes;
        uave += sv.ubpsTotalBytes;

        if(sv.bpsTotalBytes >= sv.bpsMaxBytes)
            sv.bpsMaxBytes = sv.bpsTotalBytes;

        if(sv.ubpsTotalBytes >= sv.ubpsMaxBytes)
            sv.ubpsMaxBytes = sv.ubpsTotalBytes;

        sv.bpsWindowSteps++;

        if (sv.bpsWindowSteps >= MAX_BPS_WINDOW)
        {
            float comp_ratio;

            sv.bpsWindowSteps = 0;

            ave = ave / (float)MAX_BPS_WINDOW;
            uave = uave / (float)MAX_BPS_WINDOW;

            comp_ratio = (1 - ave / uave) * 100.f;
            sv.ucompAve += comp_ratio;
            sv.ucompNum++;

            Com_DPrintf("bpspc(%2.0f) bps(%2.0f) pk(%i) ubps(%2.0f) upk(%i) cr(%2.2f) acr(%2.2f)\n",
                        ave / (float)numclients,
                        ave,
                        sv.bpsMaxBytes,
                        uave,
                        sv.ubpsMaxBytes,
                        comp_ratio,
                        sv.ucompAve / sv.ucompNum);
        }
    }
}

void custom_SV_SendMessageToClient(msg_t *msg, client_t *client)
{
    byte svCompressBuf[MAX_MSGLEN];
    int compressedSize;
    int rateMsec;
    
    memcpy(svCompressBuf, msg->data, 4);
    compressedSize = MSG_WriteBitsCompress(msg->data + 4, svCompressBuf + 4, msg->cursize - 4) + 4;
    if (client->dropReason)
    {
        SV_DropClient(client, client->dropReason);
    }
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSize = compressedSize;
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSent = svs.time;
    client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageAcked = -1;
    SV_Netchan_Transmit(client, svCompressBuf, compressedSize);

    if (client->netchan.remoteAddress.type == NA_LOOPBACK || Sys_IsLANAddress(client->netchan.remoteAddress)
        || (sv_fastDownload->integer && client->download))
    {
        client->nextSnapshotTime = svs.time - 1;
        return;
    }

    rateMsec = SV_RateMsec(client, compressedSize);
    if (rateMsec < client->snapshotMsec)
    {
        rateMsec = client->snapshotMsec;
        client->rateDelayed = qfalse;
    }
    else
    {
        client->rateDelayed = qtrue;
    }
    client->nextSnapshotTime = svs.time + rateMsec;
    if (client->state != CS_ACTIVE)
    {
        if (!*client->downloadName && client->nextSnapshotTime < svs.time + 1000)
        {
            client->nextSnapshotTime = svs.time + 1000;
        }
    }
    sv.bpsTotalBytes += compressedSize;
}

qboolean custom_SV_ClientCommand(client_t *cl, msg_t *msg)
{
    int seq;
    char *s;
    qboolean clientOk;
    qboolean floodprotect;

    clientOk = qtrue;
    floodprotect = qtrue;
    seq = MSG_ReadLong(msg);
    s = MSG_ReadCommandString(msg);

    if(seq <= cl->lastClientCommand)
        return qtrue;

    if(sv_showCommands->integer)
        Com_Printf("clientCommand: %i : %s\n", seq, s);
    
    if (cl->lastClientCommand + 1 < seq)
    {
        Com_Printf("Client %s lost %i clientCommands\n", cl->name, (seq - cl->lastClientCommand) + 1);
        SV_DropClient(cl, "EXE_LOSTRELIABLECOMMANDS");
        return qfalse;
    }
    
    if (!I_strncmp("score", s, 5) || !I_strncmp("mr ", s, 3) || !I_strncmp("userinfo ", s, 9) || !I_strncmp("ufo", s, 3))
    {
        floodprotect = qfalse;
    }
    
    if (CS_PRIMED < cl->state && sv_floodProtect->integer && svs.time < cl->nextReliableTime && floodprotect)
    {
        clientOk = qfalse;
        Com_DPrintf("client text ignored for %s: %s\n", cl->name, Cmd_Argv(0));
    }

    if(floodprotect)
        cl->nextReliableTime = svs.time + 800;

    SV_ExecuteClientCommand(cl, s, clientOk);
    cl->lastClientCommand = seq;
    Com_sprintf(cl->lastClientCommandString, MAX_STRINGLENGTH, "%s", s);

    return qtrue;
}

void hook_ClientCommand(int clientNum)
{
    if(!Scr_IsSystemActive())
        return;

    char* cmd = Cmd_Argv(0);

    // [exploit patch] gc
    if(!strcmp(cmd, "gc"))
        return;

    // [glitch patch] follow while alive
    if(!strcmp(cmd, "follownext") || !strcmp(cmd, "followprev")) // Not checking if alive, client doesn't call these commands when clicking as spectator
        return;

    if (!codecallback_playercommand)
    {
        ClientCommand(clientNum);
        return;
    }

    Scr_MakeArray();
    int args = Cmd_Argc();
    for (int i = 0; i < args; i++)
    {
        char tmp[MAX_STRINGLENGTH];
        trap_Argv(i, tmp, sizeof(tmp));
        if (i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
        {
            char *part = strtok(tmp + 1, " ");
            while (part != NULL)
            {
                Scr_AddString(part);
                Scr_AddArray();
                part = strtok(NULL, " ");
            }
        }
        else
        {
            Scr_AddString(tmp);
            Scr_AddArray();
        }
    }
    
    short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
    Scr_FreeThread(ret);
}

//// 1.1 deadchat support
// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/sv_client.c#L940
void hook_G_Say(gentity_s *ent, gentity_s *target, int mode, const char *chatText)
{
    int unknown_var = *(int*)((int)ent->client + 8400);
    if(unknown_var && !g_deadChat->integer)
        return;

    G_Say(ent, NULL, mode, chatText);
}
////

void custom_ClientEndFrame(gentity_t *ent)
{
    hook_ClientEndFrame->unhook();
    void (*ClientEndFrame)(gentity_t *ent);
    *(int*)&ClientEndFrame = hook_ClientEndFrame->from;
    ClientEndFrame(ent);
    hook_ClientEndFrame->hook();

    if (ent->client->sess.sessionState == STATE_PLAYING)
    {
        // Stop slide after fall damage
        if(g_resetSlide->integer)
            if(ent->client->ps.pm_flags & PMF_SLIDING)
                ent->client->ps.pm_flags &= ~PMF_SLIDING;
    }
}

void custom_PM_FlyMove()
{
    if (sv_spectatorNoclip->integer)
    {
        PM_NoclipMove();
        return;
    }

    hook_PM_FlyMove->unhook();
    void (*PM_FlyMove)();
    *(int*)&PM_FlyMove = hook_PM_FlyMove->from;
    PM_FlyMove();
    hook_PM_FlyMove->hook();
}

qboolean hook_StuckInClient(gentity_s *self)
{
    if(!g_playerEject->integer)
        return qfalse;
    return StuckInClient(self);
}

void custom_Touch_Item_Auto(gentity_t *item, gentity_t *entity, int touch)
{
    int clientNum = entity->client->ps.clientNum;
    client_t *client = &svs.clients[clientNum];

    if(customPlayerState[clientNum].noAutoPickup && !(client->lastUsercmd.buttons & KEY_MASK_USE))
        return;

    hook_Touch_Item_Auto->unhook();
    void (*Touch_Item_Auto)(gentity_t * item, gentity_t * entity, int touch);
    *(int*)&Touch_Item_Auto = hook_Touch_Item_Auto->from;
    Touch_Item_Auto(item, entity, touch);
    hook_Touch_Item_Auto->hook();
}

void custom_DeathmatchScoreboardMessage(gentity_t *ent)
{
    int ping;
    int clientNum;
    int numSorted;
    gclient_t *client;
    int len;
    int i;
    int stringlength;
    char string[1400];
    char entry[1024];
    int visiblePlayers;

    string[0] = 0;
    stringlength = 0;
    visiblePlayers = 0;

    numSorted = level->numConnectedClients;

    if(level->numConnectedClients > MAX_CLIENTS)
        numSorted = MAX_CLIENTS;

    for (i = 0; i < numSorted; i++)
    {
        clientNum = level->sortedClients[i];
        client = &level->clients[clientNum];
        if(customPlayerState[clientNum].hiddenFromScoreboard)
            continue;
        
        if (client->sess.connected == CON_CONNECTING)
        {
            Com_sprintf(
                entry,
                0x400u,
                " %i %i %i %i %i",
                level->sortedClients[i],
                client->sess.score,
                -1,
                client->sess.deaths,
                client->sess.statusIcon);
        }
        else
        {
            /*
            Send cl->ping instead of cl->ps.ping,
            to fix the scoreboard showing the ping of the player your are spectating as being your own ping.
            */
            ping = svs.clients[clientNum].ping;

            Com_sprintf(
                entry,
                0x400u,
                " %i %i %i %i %i",
                level->sortedClients[i],
                client->sess.score,
                ping,
                client->sess.deaths,
                client->sess.statusIcon);
        }

        len = strlen(entry);

        if(stringlength + len > 1024)
            break;

        strcpy(&string[stringlength], entry);
        stringlength += len;
        visiblePlayers++;
    }

    trap_SendServerCommand(ent - g_entities, SV_CMD_RELIABLE, va("b %i %i %i%s", visiblePlayers, level->teamScores[1], level->teamScores[2], string));
}

void CrashLogger(int sig)
{
    int fd;
    FILE *fp;
    void *array[20];
    size_t size = backtrace(array, 20);
    
    std::filesystem::path fs_lnxded_path;
    char lnxded_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", lnxded_path, sizeof(lnxded_path) - 1);
    if (len == -1)
    {
        printf("Failed writing to crash log\n");
        system("stty sane");
        exit(1);
    }
    lnxded_path[len] = '\0';
    fs_lnxded_path = lnxded_path;
    fs_lnxded_path.replace_filename("iw1x_crash.log");

    // Write to crash log
    fp = fopen(fs_lnxded_path.c_str(), "a");
    if (fp)
    {
        fd = fileno(fp);
        fseek(fp, 0, SEEK_END);
        fprintf(fp, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
        fflush(fp);
        backtrace_symbols_fd(array, size, fd);
    }
    
    // Write to stderr
    fprintf(stderr, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    system("stty sane");
    exit(1);
}

void *custom_Sys_LoadDll(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
    hook_Sys_LoadDll->unhook();
    void *(*Sys_LoadDll)(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
    *(int*)&Sys_LoadDll = hook_Sys_LoadDll->from;
    void* libHandle = Sys_LoadDll(name, fqpath, entryPoint, systemcalls);
    hook_Sys_LoadDll->hook();

    //// Unprotect game.mp.i386.so
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L55
    char libPath[512];
    char buf[512];
    char flags[4];
    void *low, *high;
    FILE *fp;
    
    if(*fs_game->string)
        sprintf(libPath, "%s/game.mp.i386.so", fs_game->string);
    else
        sprintf(libPath, "main/game.mp.i386.so");
    
    fp = fopen("/proc/self/maps", "r");
    if(!fp)
        return 0;

    while (fgets(buf, sizeof(buf), fp))
    {
        if(!strstr(buf, libPath))
            continue;
        if(sscanf (buf, "%p-%p %4c", &low, &high, flags) != 3)
            continue;
        mprotect((void *)low, (int)high-(int)low, PROT_READ | PROT_WRITE | PROT_EXEC);
    }
    fclose(fp);
    ////

    // Objects
    g_clients = (gclient_t*)dlsym(libHandle, "g_clients");
    g_entities = (gentity_t*)dlsym(libHandle, "g_entities");
    level = (level_locals_t*)dlsym(libHandle, "level");

    //// Functions
    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)dlsym(libHandle, "Scr_GetFunctionHandle");
    Scr_GetNumParam = (Scr_GetNumParam_t)dlsym(libHandle, "Scr_GetNumParam");
    trap_Argv = (trap_Argv_t)dlsym(libHandle, "trap_Argv");
    ClientCommand = (ClientCommand_t)dlsym(libHandle, "ClientCommand");
    Com_SkipRestOfLine = (Com_SkipRestOfLine_t)dlsym(libHandle, "Com_SkipRestOfLine");
    Com_ParseRestOfLine = (Com_ParseRestOfLine_t)dlsym(libHandle, "Com_ParseRestOfLine");
    Com_ParseInt = (Com_ParseInt_t)dlsym(libHandle, "Com_ParseInt");
    Scr_GetFunction = (Scr_GetFunction_t)dlsym(libHandle, "Scr_GetFunction");
    Scr_GetMethod = (Scr_GetMethod_t)dlsym(libHandle, "Scr_GetMethod");
    trap_SendServerCommand = (trap_SendServerCommand_t)dlsym(libHandle, "trap_SendServerCommand");
    Scr_ExecThread = (Scr_ExecThread_t)dlsym(libHandle, "Scr_ExecThread");
    Scr_ExecEntThread = (Scr_ExecEntThread_t)dlsym(libHandle, "Scr_ExecEntThread");
    Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)dlsym(libHandle, "Scr_ExecEntThreadNum");
    Scr_FreeThread = (Scr_FreeThread_t)dlsym(libHandle, "Scr_FreeThread");
    Scr_Error = (Scr_Error_t)dlsym(libHandle, "Scr_Error");
    Scr_ObjectError = (Scr_ObjectError_t)dlsym(libHandle, "Scr_ObjectError");
    Scr_GetConstString = (Scr_GetConstString_t)dlsym(libHandle, "Scr_GetConstString");
    Scr_ParamError = (Scr_ParamError_t)dlsym(libHandle, "Scr_ParamError");
    G_Say = (G_Say_t)dlsym(libHandle, "G_Say");
    G_RegisterCvars = (G_RegisterCvars_t)dlsym(libHandle, "G_RegisterCvars");
    G_AddEvent = (G_AddEvent_t)dlsym(libHandle, "G_AddEvent");
    G_AddPredictableEvent = (G_AddPredictableEvent_t)dlsym(libHandle, "G_AddPredictableEvent");
    trap_GetConfigstringConst = (trap_GetConfigstringConst_t)dlsym(libHandle, "trap_GetConfigstringConst");
    trap_GetConfigstring = (trap_GetConfigstring_t)dlsym(libHandle, "trap_GetConfigstring");
    BG_GetNumWeapons = (BG_GetNumWeapons_t)dlsym(libHandle, "BG_GetNumWeapons");
    BG_GetInfoForWeapon = (BG_GetInfoForWeapon_t)dlsym(libHandle, "BG_GetInfoForWeapon");
    BG_GetWeaponIndexForName = (BG_GetWeaponIndexForName_t)dlsym(libHandle, "BG_GetWeaponIndexForName");
    BG_AnimationIndexForString = (BG_AnimationIndexForString_t)dlsym(libHandle, "BG_AnimationIndexForString");
    BG_AnimScriptEvent = (BG_AnimScriptEvent_t)dlsym(libHandle, "BG_AnimScriptEvent");
    BG_AddPredictableEventToPlayerstate = (BG_AddPredictableEventToPlayerstate_t)dlsym(libHandle, "BG_AddPredictableEventToPlayerstate");
    Scr_IsSystemActive = (Scr_IsSystemActive_t)dlsym(libHandle, "Scr_IsSystemActive");
    Scr_GetInt = (Scr_GetInt_t)dlsym(libHandle, "Scr_GetInt");
    Scr_GetString = (Scr_GetString_t)dlsym(libHandle, "Scr_GetString");
    Scr_GetType = (Scr_GetType_t)dlsym(libHandle, "Scr_GetType");
    Scr_GetEntity = (Scr_GetEntity_t)dlsym(libHandle, "Scr_GetEntity");
    Scr_AddBool = (Scr_AddBool_t)dlsym(libHandle, "Scr_AddBool");
    Scr_AddInt = (Scr_AddInt_t)dlsym(libHandle, "Scr_AddInt");
    Scr_AddFloat = (Scr_AddFloat_t)dlsym(libHandle, "Scr_AddFloat");
    Scr_AddString = (Scr_AddString_t)dlsym(libHandle, "Scr_AddString");
    Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(libHandle, "Scr_AddUndefined");
    Scr_AddVector = (Scr_AddVector_t)dlsym(libHandle, "Scr_AddVector");
    Scr_MakeArray = (Scr_MakeArray_t)dlsym(libHandle, "Scr_MakeArray");
    Scr_AddArray = (Scr_AddArray_t)dlsym(libHandle, "Scr_AddArray");
    Scr_AddObject = (Scr_AddObject_t)dlsym(libHandle, "Scr_AddObject");
    Scr_LoadScript = (Scr_LoadScript_t)dlsym(libHandle, "Scr_LoadScript");
    StuckInClient = (StuckInClient_t)dlsym(libHandle, "StuckInClient");
    Q_strlwr = (Q_strlwr_t)dlsym(libHandle, "Q_strlwr");
    Q_strupr = (Q_strupr_t)dlsym(libHandle, "Q_strupr");
    Q_strcat = (Q_strcat_t)dlsym(libHandle, "Q_strcat");
    Q_strncpyz = (Q_strncpyz_t)dlsym(libHandle, "Q_strncpyz");
    Q_CleanStr = (Q_CleanStr_t)dlsym(libHandle, "Q_CleanStr");
    Jump_Check = (Jump_Check_t)((int)dlsym(libHandle, "_init") + 0x76F4);
    PM_GetEffectiveStance = (PM_GetEffectiveStance_t)dlsym(libHandle, "PM_GetEffectiveStance");
    PM_ClipVelocity = (PM_ClipVelocity_t)dlsym(libHandle, "PM_ClipVelocity");
    va = (va_t)dlsym(libHandle, "va");
    trap_GetUserinfo = (trap_GetUserinfo_t)dlsym(libHandle, "trap_GetUserinfo");
    PM_NoclipMove = (PM_NoclipMove_t)((int)dlsym(libHandle, "_init") + 0x8300);
    G_LocalizedStringIndex = (G_LocalizedStringIndex_t)dlsym(libHandle, "G_LocalizedStringIndex");
    trap_SetConfigstring = (trap_SetConfigstring_t)dlsym(libHandle, "trap_SetConfigstring");
    trap_GetArchivedPlayerState = (trap_GetArchivedPlayerState_t)dlsym(libHandle, "trap_GetArchivedPlayerState");
    G_Error = (G_Error_t)dlsym(libHandle, "G_Error");
    Scr_GetPointerType = (Scr_GetPointerType_t)dlsym(libHandle, "Scr_GetPointerType");
    ////

    //// [exploit patch] codmsgboom
    /* See:
    - https://aluigi.altervista.org/adv/codmsgboom-adv.txt
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L146
    */
    *(int*)((int)dlsym(libHandle, "G_Say") + 0x50e) = 0x37f;
    *(int*)((int)dlsym(libHandle, "G_Say") + 0x5ca) = 0x37f;
    ////

    //// 1.1 deadchat support
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L161
    *(byte*)((int)dlsym(libHandle, "G_Say") + 0x2B3) = 0xeb;
    *(byte*)((int)dlsym(libHandle, "G_Say") + 0x3B6) = 0xeb;
    hook_call((int)dlsym(libHandle, "G_Say") + 0x5EA, (int)hook_G_Say);
    hook_call((int)dlsym(libHandle, "G_Say") + 0x77D, (int)hook_G_Say);
    hook_call((int)dlsym(libHandle, "G_Say") + 0x791, (int)hook_G_Say);
    ////
    
    hook_call((int)dlsym(libHandle, "vmMain") + 0xB0, (int)hook_ClientCommand);
    hook_call((int)dlsym(libHandle, "ClientEndFrame") + 0x311, (int)hook_StuckInClient);

    hook_jmp((int)dlsym(libHandle, "G_LocalizedStringIndex"), (int)custom_G_LocalizedStringIndex);
    hook_jmp((int)dlsym(libHandle, "va"), (int)custom_va);
    
    hook_GScr_LoadGameTypeScript = new cHook((int)dlsym(libHandle, "GScr_LoadGameTypeScript"), (int)custom_GScr_LoadGameTypeScript);
    hook_GScr_LoadGameTypeScript->hook();
    hook_ClientEndFrame = new cHook((int)dlsym(libHandle, "ClientEndFrame"), (int)custom_ClientEndFrame);
    hook_ClientEndFrame->hook();
    hook_Touch_Item_Auto = new cHook((int)dlsym(libHandle, "Touch_Item_Auto"), (int)custom_Touch_Item_Auto);
    hook_Touch_Item_Auto->hook();
    hook_DeathmatchScoreboardMessage = new cHook((int)dlsym(libHandle, "DeathmatchScoreboardMessage"), (int)custom_DeathmatchScoreboardMessage);
    hook_DeathmatchScoreboardMessage->hook();
    hook_PM_FlyMove = new cHook((int)dlsym(libHandle, "_init") + 0x79C8, (int)custom_PM_FlyMove);
    hook_PM_FlyMove->hook();
    
    return libHandle;
}

class iw1x
{
    public:
    iw1x()
    {
        printf("-------- iw1x --------\n");
        printf("Compiled on %s %s using g++ %s and glibc %i.%i\n", __DATE__, __TIME__, __VERSION__, __GLIBC__, __GLIBC_MINOR__);

        // Don't inherit lib of parent
        unsetenv("LD_PRELOAD");

        // Crash handlers for debugging
        signal(SIGSEGV, CrashLogger);
        signal(SIGABRT, CrashLogger);
        
        // Otherwise the printf()'s are printed at crash/end on older os/compiler versions
        // See https://github.com/M-itch/libcod/blob/e58d6a01b11c911fbf886659b6ea67795776cf4a/libcod.cpp#L1346
        setbuf(stdout, NULL);

        // Allow to write in executable memory
        mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

#if 0
        // Crash test
        * (int*)nullptr = 1;
#endif

        // [exploit patch] q3infoboom
        /* See:
        - https://aluigi.altervista.org/adv/q3infoboom-adv.txt
        - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/codextended.c#L295
        */
        *(byte*)0x807f459 = 1;
        
        hook_call(0x08085213, (int)hook_AuthorizeState);
        hook_call(0x08094c54, (int)Scr_GetCustomFunction);
        hook_call(0x080951c4, (int)Scr_GetCustomMethod);
        hook_call(0x0808c7b8, (int)hook_SV_DirectConnect);
        hook_call(0x0808c7ea, (int)hook_SV_AuthorizeIpPacket);
        hook_call(0x0808c74e, (int)hook_SVC_Info);

        hook_jmp(0x080717a4, (int)custom_FS_ReferencedPakChecksums);
        hook_jmp(0x080716cc, (int)custom_FS_ReferencedPakNames);
        hook_jmp(0x080872ec, (int)custom_SV_ExecuteClientMessage);
        hook_jmp(0x08086d58, (int)custom_SV_ExecuteClientCommand);
        hook_jmp(0x0809045c, (int)custom_SV_SendClientMessages);
        hook_jmp(0x08086290, (int)custom_SV_WriteDownloadToClient);
        hook_jmp(0x0808f680, (int)custom_SV_SendMessageToClient);
        hook_jmp(0x0808ba0c, (int)custom_SV_MasterHeartbeat);
        hook_jmp(0x0808c870, (int)custom_SV_PacketEvent);
        hook_jmp(0x08085eec, (int)custom_SV_SendClientGameState);
        hook_jmp(0x08084d90, (int)custom_SV_GetChallenge);
        hook_jmp(0x0808b580, (int)custom_SV_CanReplaceServerCommand);
        hook_jmp(0x08086e08, (int)custom_SV_ClientCommand);
        hook_jmp(0x0808c404, (int)custom_SVC_RemoteCommand);
        hook_jmp(0x0808bd58, (int)custom_SVC_Status);
        
        hook_Sys_LoadDll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
        hook_Sys_LoadDll->hook();
        hook_Com_Init = new cHook(0x0806c654, (int)custom_Com_Init);
        hook_Com_Init->hook();
        hook_SV_BeginDownload_f = new cHook(0x08087a64, (int)custom_SV_BeginDownload_f);
        hook_SV_BeginDownload_f->hook();
        hook_SV_AddOperatorCommands = new cHook(0x08084a3c, (int)custom_SV_AddOperatorCommands);
        hook_SV_AddOperatorCommands->hook();

        printf("----------------------\n");
    }

    ~iw1x()
    {
        printf("iw1x destructor called\n");
        system("stty sane");
    }
};

iw1x *_iw1x;
void __attribute__ ((constructor)) lib_load(void)
{
    _iw1x = new iw1x;
}
void __attribute__ ((destructor)) lib_unload(void)
{
    delete _iw1x;
}