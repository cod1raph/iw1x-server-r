#include "gsc.hpp"

void gsc_entity_showtoplayer(scr_entref_t ref) //TODO: try like cod2rev instead
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