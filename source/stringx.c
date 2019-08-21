
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "stringx.h"

/// @file String handling functions.



//---------------- Private --------------------------//

/// Internal data definition.
struct stringx
{
    char* raw;   ///< The owned string. Can be NULL which means empty.
    //bool valid;  ///< For lifetime management.
};

/// (Re)assign the underlying char pointer. Takes ownership of the string.
/// @param s Source stringx.
/// @param cs The new raw string or NULL for an empty stringx.
static void p_assign(stringx_t* s, char* cs)
{
    if(s->raw != NULL)
    {
        free(s->raw);
    }

    s->raw = cs == NULL ? NULL : cs;
//    s->valid = true;
}

/// Case sensitive char matcher.
/// @param s Source stringx.
/// @param c1 First char.
/// @param c2 Second char.
/// @param csens Case sensitivity.
/// @return True if they match.
static bool p_match(char c1, char c2, csens_t csens)
{
    //A=65 Z=90 a=97 z=122
    bool match = csens == CASE_INSENS ? toupper(c1) == toupper(c2) : c1 == c2;
    return match;
}

/// Copy a const string.
/// @param sinit String to copy. If NULL, a valid empty string is created.
/// @return The new mutable string.
char* p_scopy(const char* sinit)
{
    char* buff;

    // Copy the contents.
    if(sinit != NULL)
    {
        buff = calloc(sizeof(char), strlen(sinit) + 1);
        strcpy(buff, sinit);
    }
    else // make a valid empty string
    {
        buff = malloc(1);
        buff[0] = 0;
    }
    return buff;
}

//---------------- Public API Implementation -------------//

//--------------------------------------------------------//
stringx_t* stringx_create(const char* sinit)
{
    stringx_t* s = malloc(sizeof(stringx_t));
    s->raw = NULL;
    // Copy the contents.
    p_assign(s, p_scopy(sinit));
    return s;
}

//--------------------------------------------------------//
void stringx_destroy(stringx_t* s)
{
    if(s != NULL)
    {
        if(s->raw != NULL)
        {
            free(s->raw);
            s->raw = NULL;
        }
        free(s);
    }
}

//--------------------------------------------------------//
void stringx_set(stringx_t* s, const char* sinit)
{
    // Copy the contents.
    p_assign(s, p_scopy(sinit));
}

//--------------------------------------------------------//
void stringx_append(stringx_t* s, char c)
{
    // TODO This is really crude. Need to make smarter internal buffer to support growing.
    unsigned int slen = stringx_len(s);
    char* snew = calloc(sizeof(char), slen + 2);
    strcpy(snew, s->raw);
    snew[slen] = c;
    p_assign(s, snew);
}

//--------------------------------------------------------//
const char* stringx_content(stringx_t* s)
{
    const char* raw = s != NULL ? s->raw : NULL;
    return raw;
}

//--------------------------------------------------------//
unsigned int stringx_len(stringx_t* s)
{
    unsigned int len = s != NULL ? strlen(s->raw) : 0;
    return len;
}

//--------------------------------------------------------//
bool stringx_compare(stringx_t* s1, const char* s2, csens_t csens)
{
    bool match = stringx_len(s1) == strlen(s2);

    for(unsigned int i = 0; i < strlen(s2) && match; i++)
    {
        match = p_match(s1->raw[i], s2[i], csens);
    }

    return match;
}

//--------------------------------------------------------//
bool stringx_starts(stringx_t* s1, const char* s2, csens_t csens)
{
    bool match = stringx_len(s1) >= strlen(s2);

    for(unsigned int i = 0; i < strlen(s2) && match; i++)
    {
        match = p_match(s1->raw[i], s2[i], csens);
    }

    return match;
}

//--------------------------------------------------------//
bool stringx_ends(stringx_t* s1, const char* s2, csens_t csens)
{
    // s1 = "The Mulberry Bush"
    // s2 = "Bush"

    bool match = stringx_len(s1) >= strlen(s2);
    unsigned int ind1 = stringx_len(s1) - strlen(s2);

    for(unsigned int i = 0; i < strlen(s2) && match; i++)
    {
        match = p_match(s1->raw[ind1++], s2[i], csens);
    }

    return match;
}

//--------------------------------------------------------//
bool stringx_contains(stringx_t* s1, const char* s2, csens_t csens) // TODO
{
    (void)s1;
    (void)s2;
    (void)csens;

    return false;
}

//--------------------------------------------------------//
stringx_t* stringx_copy(stringx_t* s)
{
    stringx_t* copy = stringx_create(s->raw);
    return copy;
}

//--------------------------------------------------------//
stringx_t* stringx_left(stringx_t* s, unsigned int num)
{
    stringx_t* left = stringx_create(NULL);

    if(strlen(s->raw) >= num)
    {
        char* sleft = calloc(sizeof(char), (num + 1));
        char* sresid = calloc(sizeof(char), stringx_len(s) - num + 1);

        strncpy(sleft, s->raw, num);
        strncpy(sresid, s->raw + num, stringx_len(s) - num);

        p_assign(left, sleft);
        p_assign(s, sresid);
    }

    return left;
}

//--------------------------------------------------------//
void stringx_trim(stringx_t* s)
{
    // __123 456___ len=12

    int first = -1;
    int last = - 1;
    int len = (int)strlen(s->raw);

    // Find first.
    for(int i = 0; first < 0 && i < len; i++)
    {
        if(!isspace(s->raw[i]))
        {
            first = i;
        }
    }

    // Find last.
    for(int i = len - 1; last < 0 && i >= 0; i--)
    {
        if(!isspace(s->raw[i]))
        {
            last = i + 1;
        }
    }

    // Is there work to do?
    if(first >= 0 || last >= 0)
    {
        first = first >= 0 ? first : 0;
        last = last >= 0 ? last : len - 1;

        unsigned int slen = (unsigned int)(last - first);
        char* cs = malloc(slen + 1);
        memcpy(cs, s->raw + first, slen);
        cs[slen] = 0;
        p_assign(s, cs);
    }
}

//--------------------------------------------------------//
void stringx_upper(stringx_t* s)
{
    unsigned int len = strlen(s->raw);

    for(unsigned int i = 0; i < len; i++)
    {
        if(isalpha(s->raw[i]))
        {
            s->raw[i] = (char)toupper(s->raw[i]);
        }
    }
}

//--------------------------------------------------------//
void stringx_lower(stringx_t* s)
{
    unsigned int len = strlen(s->raw);

    for(unsigned int i = 0; i < len; i++)
    {
        if(isalpha(s->raw[i]))
        {
            s->raw[i] = (char)tolower(s->raw[i]);
        }
    }
}

//--------------------------------------------------------//
bool stringx_format(stringx_t* s, unsigned int maxlen, const char* format, ...)
{
    bool stat = true;

    char* buff = calloc(sizeof(char), maxlen + 1);
    va_list args;
    va_start(args, format);
    vsnprintf(buff, maxlen, format, args);

    p_assign(s, buff);
  
    return stat;
}

#ifdef HAVE_LIST
//--------------------------------------------------------//
list_t* stringx_split(stringx_t* s, const char* delim)
{
    list_t* parts = list_create();

    // Make writable copy and tokenize it.
    char* cp = calloc(sizeof(char), strlen(s->raw) + 1);
    strcpy(cp, s->raw);

    char* token = strtok(cp, delim);
    while(token != NULL)
    {
        char* ctoken = calloc(sizeof(char), strlen(token) + 1);
        strcpy(ctoken, token);
        list_append(parts, ctoken);
        token = strtok(NULL, delim);
    }

    free(cp);
    return parts;
}
#endif
