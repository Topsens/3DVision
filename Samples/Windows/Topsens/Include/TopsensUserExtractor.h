#ifndef _TOPSENS_USER_EXTRACTOR_H_
#define _TOPSENS_USER_EXTRACTOR_H_

#include "Topsens.h"
#include <stdint.h>

#ifdef _MSC_VER
    #ifdef TOPSENS_TOOLKIT_EXPORT
        #define _FUNC_ __declspec(dllexport)
    #else
        #define _FUNC_
    #endif
#else
    #ifdef TOPSENS_TOOLKIT_EXPORT
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

typedef void* TOPSENS_USER_EXTRACTOR;

#ifdef __cplusplus
extern "C" {
#endif

_FUNC_ int32_t _CALL_ TopsensUserExtractorCreate(TOPSENS_USER_EXTRACTOR* handle);
_FUNC_ void    _CALL_ TopsensUserExtractorDestroy(TOPSENS_USER_EXTRACTOR handle);
_FUNC_ int32_t _CALL_ TopsensUserExtractorInitialize(TOPSENS_USER_EXTRACTOR handle, int resolution);
_FUNC_ int32_t _CALL_ TopsensUserExtractorExtract(TOPSENS_USER_EXTRACTOR handle, const TOPSENS_COLOR_FRAME* cframe, const TOPSENS_USERS_FRAME* uframe, uint32_t* extracted);

#ifdef __cplusplus
} // extern "C"
#endif

#undef _FUNC_
#undef _CALL_
#endif