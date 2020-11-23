#ifndef _TOPSENS_PEOPLE_H_
#define _TOPSENS_PEOPLE_H_

#include "Topsens.h"
#include <stdint.h>

#ifdef _MSC_VER
    #ifdef TOPSENS_PEOPLE_EXPORT
        #define _FUNC_ __declspec(dllexport)
    #else
        #define _FUNC_
    #endif
#else
    #ifdef TOPSENS_PEOPLE_EXPORT
        #define _FUNC_ __attribute__ ((visibility ("default")))
    #else
        #define _FUNC_
    #endif
#endif

#if defined(_MSC_VER) && defined(__cplusplus)
    #define _CALL_ __cdecl
#else
    #define _CALL_
#endif

typedef void* TOPSENS_PEOPLE;

#ifdef __cplusplus
extern "C" {
#endif

_FUNC_ int32_t _CALL_ TopsensPeopleCreate(TOPSENS_PEOPLE* handle);
_FUNC_ void    _CALL_ TopsensPeopleDestroy(TOPSENS_PEOPLE handle);
_FUNC_ int32_t _CALL_ TopsensPeopleInitialize(TOPSENS_PEOPLE handle, uint32_t width, uint32_t height, float fx, float fy, float cx, float cy);
_FUNC_ void    _CALL_ TopsensPeopleUninitialize(TOPSENS_PEOPLE handle);
_FUNC_ int32_t _CALL_ TopsensPeopleSetOrientation(TOPSENS_PEOPLE handle, int32_t orientation);
_FUNC_ int32_t _CALL_ TopsensPeopleGetOrientation(TOPSENS_PEOPLE handle);
_FUNC_ int32_t _CALL_ TopsensPeopleSetImageFlipped(TOPSENS_PEOPLE handle, int32_t flipped);
_FUNC_ int32_t _CALL_ TopsensPeopleIsImageFlipped(TOPSENS_PEOPLE handle);
_FUNC_ int32_t _CALL_ TopsensPeopleSetMaskEnabled(TOPSENS_PEOPLE handle, int32_t enabled);
_FUNC_ int32_t _CALL_ TopsensPeopleIsMaskEnabled(TOPSENS_PEOPLE handle);
_FUNC_ int32_t _CALL_ TopsensPeopleDetect(TOPSENS_PEOPLE handle, const uint16_t* depth, uint32_t width, uint32_t height, uint64_t timestamp, TOPSENS_USERS_FRAME* users);
_FUNC_ int32_t _CALL_ TopsensPeoplePreload(uint32_t* resources, uint32_t count);

#ifdef __cplusplus
} // extern "C"
#endif

#undef _FUNC_
#undef _CALL_
#endif