#include "gsc.hpp"

// See https://github.com/xtnded/codextended/blob/50c43096543b300f1e87dcf5c3c9edf59367e85d/src/scr_method_entity.c#L150
void gsc_entity_showtoplayer(scr_entref_t ref)
{
    int id = ref.entnum;
    gentity_t *clientEnt;
    gentity_t *pEnt;

    pEnt = &g_entities[id];
    if (Scr_GetType(0) == STACK_UNDEFINED)
    {
        pEnt->r.svFlags &= ~SVF_SINGLECLIENT;
        pEnt->r.singleClient = 0;
        Scr_AddBool(qtrue);
        return;
    }
    
    clientEnt = Scr_GetEntity(0);
    if (clientEnt->s.number >= MAX_CLIENTS)
    {
        stackError("gsc_entity_showtoplayer() param must be a client entity");
        return;
    }
    
    pEnt->r.svFlags |= SVF_SINGLECLIENT;
    pEnt->r.singleClient = clientEnt->s.number;

    Scr_AddBool(qtrue);
}