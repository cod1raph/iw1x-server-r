#ifndef GSC_H
#define GSC_H

#include "shared.h"

#define STACK_UNDEFINED 0
#define STACK_STRING 1
#define STACK_LOCALIZED_STRING 2
#define STACK_VECTOR 3
#define STACK_FLOAT 4
#define STACK_INT 5
#define STACK_OBJECT 7
#define STACK_FUNCTION 9

const char * stackGetParamTypeAsString(int param);
int stackGetParams(const char *params, ...);
void stackError(const char *format, ...);

int stackGetParamInt(int param, int *value);
int stackGetParamFunction(int param, int *value);
int stackGetParamString(int param, char **value);
int stackGetParamConstString(int param, unsigned int *value);
int stackGetParamLocalizedString(int param, char **value);
int stackGetParamVector(int param, vec3_t value);
int stackGetParamFloat(int param, float *value);
int stackGetParamObject(int param, unsigned int *value);

//// For tests
void gsc_testfunction();
void gsc_testmethod(scr_entref_t ref);
////

//// Entity
void gsc_entity_setbounds(scr_entref_t ref);
void gsc_entity_setclipmask(scr_entref_t ref);
void gsc_entity_getclipmask(scr_entref_t ref);
void gsc_entity_showtoplayer(scr_entref_t ref);
////

//// Player
void gsc_player_setvelocity(scr_entref_t ref);
void gsc_player_getvelocity(scr_entref_t ref);
void gsc_player_addvelocity(scr_entref_t ref);
void gsc_player_button_ads(scr_entref_t ref);
void gsc_player_button_left(scr_entref_t ref);
void gsc_player_button_right(scr_entref_t ref);
void gsc_player_button_forward(scr_entref_t ref);
void gsc_player_button_back(scr_entref_t ref);
void gsc_player_button_jump(scr_entref_t ref);
void gsc_player_button_leanleft(scr_entref_t ref);
void gsc_player_button_leanright(scr_entref_t ref);
void gsc_player_button_reload(scr_entref_t ref);
void gsc_player_getangles(scr_entref_t ref);
void gsc_player_getstance(scr_entref_t ref);
void gsc_player_getuserinfokey(scr_entref_t ref);
void gsc_player_setuserinfokey(scr_entref_t ref);
void gsc_player_getip(scr_entref_t ref);
void gsc_player_getping(scr_entref_t ref);
void gsc_player_processclientcommand(scr_entref_t ref);
void gsc_player_dropclient(scr_entref_t ref);
void gsc_player_getfps(scr_entref_t ref);
void gsc_player_isonladder(scr_entref_t ref);
void gsc_player_noclip(scr_entref_t ref);
void gsc_player_ufo(scr_entref_t ref);
void gsc_player_connectionlesspackettoclient(scr_entref_t ref);
void gsc_player_disableitemautopickup(scr_entref_t ref);
void gsc_player_enableitemautopickup(scr_entref_t ref);
void gsc_player_playscriptanimation(scr_entref_t ref);
void gsc_player_isbot(scr_entref_t ref);
void gsc_player_sethiddenfromscoreboard(scr_entref_t ref);
void gsc_player_ishiddenfromscoreboard(scr_entref_t ref);
////

//// Weapons
void gsc_weapons_setweaponcookable();
void gsc_weapons_setweaponfusetime();
////

//// Utils
void gsc_utils_file_exists();
void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();

void gsc_utils_sendcommandtoclient();
void gsc_utils_logprintconsole();
void gsc_utils_getsubstr();
void gsc_utils_getascii();
void gsc_utils_toupper();
void gsc_utils_tolower();
void gsc_utils_strtok();
void gsc_utils_replace();
void gsc_utils_getsystemtime();
void gsc_utils_getconfigstring();
void gsc_utils_makelocalizedstring();
void gsc_utils_ban();
void gsc_utils_unban();
void gsc_utils_strip();
void gsc_utils_strstr();
void gsc_utils_monotone();
void gsc_utils_gettype();
////

#endif