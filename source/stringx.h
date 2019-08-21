
#ifndef STRINGX_H
#define STRINGX_H

#include <stdbool.h>
#include <string.h>


/// @file


//---------------- Public API ----------------------//

/// Case sensitivity for ops.
typedef enum
{
    CASE_SENS,
    CASE_INSENS
} csens_t;

/// Opaque string object.
typedef struct stringx stringx_t;

/// Create an empty string.
/// @param sinit Optional initial value. If NULL, content will be "".
/// @return The opaque pointer used in all functions.
stringx_t* stringx_create(const char* sinit);

/// Frees all data pointers, and the string struct.
/// @param s Source stringx. After this returns it is no longer valid.
void stringx_destroy(stringx_t* s);

/// Get the contained data.
/// @param s Source stringx.
/// @return The char pointer.
const char* stringx_content(stringx_t* s);

/// Size of the string.
/// @param s Source stringx.
/// @return The size. 
unsigned int stringx_len(stringx_t* s);

/// Set s to new value.
/// @param s Source stringx.
/// @param sinit The new value. If NULL, content will be "".
void stringx_set(stringx_t* s, const char* sinit);

/// Convert to upper case in place.
/// @param s Source stringx.
void stringx_upper(stringx_t* s);

/// Convert to lower case in place.
/// @param s Source stringx.
void stringx_lower(stringx_t* s);

/// Compare strings.
/// @param s1 Source stringx.
/// @param s2 The test value.
/// @param csens Case sensitivity.
/// @return True if equal.
bool stringx_compare(stringx_t* s1, const char* s2, csens_t csens);

/// Test if string starts with.
/// @param s1 Source stringx.
/// @param s2 The test value.
/// @param csens Case sensitivity.
/// @return True if starts with.
bool stringx_starts(stringx_t* s1, const char* s2, csens_t csens);

/// Test if string ends with.
/// @param s1 Source stringx.
/// @param s2 The test value.
/// @param csens Case sensitivity.
/// @return True if ends with.
bool stringx_ends(stringx_t* s1, const char* s2, csens_t csens);

/// Test if string contains.
/// @param s Source stringx.
/// @param s1 Source stringx.
/// @param s2 The test value.
/// @param csens Case sensitivity.
/// @return True if contains.
bool stringx_contains(stringx_t* s1, const char* s2, csens_t csens);

/// Copy a stringx.
/// @param s Source stringx.
/// @return The copied string.
stringx_t* stringx_copy(stringx_t* s);

/// Removes left chars as a new stringx.
/// @param s Source stringx.
/// @param num Number to remove.
/// @return The left part or empty if num > len(s).
stringx_t* stringx_left(stringx_t* s, unsigned int num);

/// Trim whitespace from both ends in place.
/// @param s Source stringx.
void stringx_trim(stringx_t* s);

/// Append a char to the stringx.
/// @param s Source stringx.
/// @param c Char to append.
void stringx_append(stringx_t* s, char c);

/// Format the string in place.
/// @param s Source stringx.
/// @param maxlen Client must give us a clue.
/// @param format Standard format string and args.
/// @return True - Not used right now but in future could check arg validity.
bool stringx_format(stringx_t* s, unsigned int maxlen, const char* format, ...);

#ifdef HAVE_LIST
#include "list.h"
/// Split the string into parts by token.
/// @param s Source stringx.
/// @param delim Like strtok.
/// @return List of string parts. Client is responsible for this now.
list_t* stringx_split(stringx_t* s, const char* delim);
#endif

#endif // STRINGX_H
