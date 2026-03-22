#include "gsc.hpp"

void gsc_utils_sendcommandtoclient()
{
    int clientNum;
    char *message;

    if (!stackGetParams("is", &clientNum, &message))
    {
        stackError("gsc_utils_sendcommandtoclient() one or more arguments is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    trap_SendServerCommand(clientNum, SV_CMD_CAN_IGNORE, message);
    Scr_AddBool(qtrue);
}

void gsc_utils_logprintconsole()
{
    char *str;

    if (!stackGetParams("s", &str))
    {
        stackError("gsc_utils_logprintconsole() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (!strlen(str) || strlen(str) > MAX_STRINGLENGTH)
    {
        stackError("gsc_utils_logprintconsole() invalid string length");
        Scr_AddUndefined();
        return;
    }

    Com_Printf("%s", str);

    Scr_AddBool(qtrue);
}

void gsc_utils_toupper()
{
    char *str;

    if (!stackGetParams("s", &str))
    {
        stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (!strlen(str))
    {
        stackError("gsc_utils_toupper() string length is 0");
        Scr_AddUndefined();
        return;
    }

    Scr_AddString(Q_strupr(str));
}

void gsc_utils_tolower() // From cod2rev
{
    char c;
    int i;
    const char *string;
    char tempString[MAX_STRINGLENGTH];

    if (!stackGetParams("s", &string))
    {
        stackError("gsc_utils_tolower() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    for (i = 0; i < MAX_STRINGLENGTH; ++i)
    {
        c = tolower(*string);
        tempString[i] = c;

        if (!c)
        {
            Scr_AddString(tempString);
            return;
        }

        ++string;
    }

    stackError("gsc_utils_tolower() string too long");
}

void gsc_utils_strtok() // From cod2rev
{
    signed int j;
    char c;
    int dest;
    int i;
    signed int len;
    const char *tok;
    const char *delim;
    char tempString[1024];

    delim = Scr_GetString(0);
    tok = Scr_GetString(1u);
    len = strlen(tok);
    dest = 0;

    Scr_MakeArray();

    for (i = 0; ; ++i)
    {
        c = delim[i];

        if (!c)
            break;

        for (j = 0; j < len; ++j)
        {
            if (c == tok[j])
            {
                if (dest)
                {
                    tempString[dest] = 0;
                    Scr_AddString(tempString);
                    Scr_AddArray();
                    dest = 0;
                }

                goto skip;
            }
        }

        tempString[dest] = c;

        if (++dest > 1023)
            stackError("gsc_utils_strtok() string too long");
skip:
        ;
    }

    if (dest)
    {
        tempString[dest] = 0;
        Scr_AddString(tempString);
        Scr_AddArray();
    }
}

// This function is here only for MiscMod users
void gsc_utils_replace()
{
    char* orig;
    char* rep;
    char* with;

    if (!stackGetParams("sss", &orig, &rep, &with))
    {
        stackError("gsc_utils_replace() one or more arguments is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }
    
    char *result; // the return string
    char *ins; // the next insert point
    char *tmp; // varies
    int len_rep; // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count; // number of replacements

    if (!orig)
        return;
    if (!rep || !(len_rep = strlen(rep)))
        return;
    if (!(ins = strstr(orig, rep))) 
        return;
    if (!with)
        with = (char*)"";
    len_with = strlen(with);

    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = (char*)malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    
    Scr_AddString(result);
}

void gsc_utils_file_exists()
{
    char *filename;

    if (!stackGetParams("s", &filename))
    {
        stackError("gsc_utils_file_exists() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    int file_exists = access(filename, F_OK) != -1;
    Scr_AddInt(file_exists);
}

void gsc_utils_fopen()
{
    FILE *file;
    char *filename, *mode;

    if (!stackGetParams("ss", &filename, &mode))
    {
        stackError("gsc_utils_fopen() one or more arguments is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    file = fopen(filename, mode);

    if (!file)
    {
        stackError("gsc_utils_fopen() returned an error");
        Scr_AddUndefined();
        return;
    }

    Scr_AddInt((int)file);
}

void gsc_utils_fread()
{
    FILE *file;

    if (!stackGetParams("i", &file))
    {
        stackError("gsc_utils_fread() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (!file)
    {
        stackError("gsc_utils_fread() returned an error");
        Scr_AddUndefined();
        return;
    }

    char buffer[256];
    int ret = fread(buffer, 1, 255, file);

    if (!ret)
    {
        Scr_AddUndefined();
        return;
    }

    buffer[ret] = '\0';
    Scr_AddString(buffer);
}

void gsc_utils_fwrite()
{
    FILE *file;
    char *buffer;

    if (!stackGetParams("is", &file, &buffer))
    {
        stackError("gsc_utils_fwrite() one or more arguments is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (!file)
    {
        stackError("gsc_utils_fwrite() returned an error");
        Scr_AddUndefined();
        return;
    }

    Scr_AddInt(fwrite(buffer, 1, strlen(buffer), file));
}

void gsc_utils_fclose()
{
    FILE *file;

    if (!stackGetParams("i", &file))
    {
        stackError("gsc_utils_fclose() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    if (!file)
    {
        stackError("gsc_utils_fclose() returned an error");
        Scr_AddUndefined();
        return;
    }

    Scr_AddInt(fclose(file));
}

void gsc_utils_getsystemtime()
{
    time_t timer;
    Scr_AddInt(time(&timer));
}

void gsc_utils_getconfigstring()
{
    int index;

    if (!stackGetParams("i", &index))
    {
        stackError("gsc_utils_getconfigstring() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }
    
    if (index < 0 || index >= MAX_CONFIGSTRINGS)
    {
        stackError("gsc_utils_getconfigstring() configstring index is out of range");
        Scr_AddUndefined();
        return;
    }

    const char *string = trap_GetConfigstringConst(index);

    if (!*string)
        Scr_AddUndefined();
    else
        Scr_AddString(string);
}

void gsc_utils_makelocalizedstring()
{
    char *str;

    if (!stackGetParams("s", &str))
    {
        stackError("gsc_utils_makelocalizedstring() argument is undefined or has a wrong type");
        Scr_AddUndefined();
        return;
    }

    Scr_AddString(str);

    VariableValue *var;
    int param = 0;

    var = &scrVmPub.top[-param];
    var->type = STACK_LOCALIZED_STRING;
}

void gsc_utils_issubstr()
{
    Scr_AddBool(strstr(Scr_GetString(0), Scr_GetString(1)) != NULL);
}

// See https://github.com/voron00/CoD2rev_Server/blob/79850694857ddd6af909b375a8310a8c1d7e752f/src/game/g_scr_main_mp.cpp#L3189
void gsc_utils_getsubstr()
{
    size_t i;
    int c;
    char tempString[MAX_STRINGLENGTH];

    const char *s = Scr_GetString(0);

    int start = Scr_GetInt(1);
    int end = INT_MAX;

    if(Scr_GetNumParam() > 2)
        end = Scr_GetInt(2);

    for (i = 0; start < end; i++, start++)
    {
        if (i >= sizeof(tempString))
            Scr_Error("string too long");

        c = s[start];

        if(!c)
            break;
        
        tempString[i] = c;
    }

    tempString[i] = 0;
    Scr_AddString(tempString);
}