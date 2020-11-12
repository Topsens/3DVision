#ifndef _TOPSENS_SENSOR_H_
#define _TOPSENS_SENSOR_H_

#include "Topsens.h"
#include <stdint.h>

#ifdef _MSC_VER
    #ifdef TOPSENS_SENSOR_EXPORT
        #define _FUNC_ __declspec(dllexport)
    #else
        #define _FUNC_
    #endif
#else
    #ifdef TOPSENS_SENSOR_EXPORT
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

typedef void* TOPSENS_SENSOR;

#ifdef __cplusplus
extern "C" {
#endif

_FUNC_ int32_t _CALL_ TopsensSensorCount(uint32_t* count);
_FUNC_ int32_t _CALL_ TopsensSensorCreate(uint32_t index, TOPSENS_SENSOR* handle);
_FUNC_ void    _CALL_ TopsensSensorDestroy(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorStart(TOPSENS_SENSOR handle, int32_t colorResolution, int32_t depthResolution, int32_t generateUsers);
_FUNC_ void    _CALL_ TopsensSensorStop(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorWaitColor(TOPSENS_SENSOR handle, TOPSENS_COLOR_FRAME* frame, uint32_t timeout);
_FUNC_ int32_t _CALL_ TopsensSensorWaitDepth(TOPSENS_SENSOR handle, TOPSENS_DEPTH_FRAME* frame, uint32_t timeout);
_FUNC_ int32_t _CALL_ TopsensSensorWaitUsers(TOPSENS_SENSOR handle, TOPSENS_USERS_FRAME* frame, uint32_t timeout);
_FUNC_ int32_t _CALL_ TopsensSensorSetOrientation(TOPSENS_SENSOR handle, int32_t orientation);
_FUNC_ int32_t _CALL_ TopsensSensorGetOrientation(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorSetImageFlipped(TOPSENS_SENSOR handle, int32_t flipped);
_FUNC_ int32_t _CALL_ TopsensSensorIsImageFlipped(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorSetDepthAligned(TOPSENS_SENSOR handle, int32_t aligned);
_FUNC_ int32_t _CALL_ TopsensSensorIsDepthAligned(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorSetMaskEnabled(TOPSENS_SENSOR handle, int32_t enabled);
_FUNC_ int32_t _CALL_ TopsensSensorIsMaskEnabled(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorSetRecording(TOPSENS_SENSOR handle, int32_t recording);
_FUNC_ int32_t _CALL_ TopsensSensorIsRecording(TOPSENS_SENSOR handle);
_FUNC_ int32_t _CALL_ TopsensSensorPreload(uint32_t* resources, uint32_t count);

#ifdef __cplusplus
} // extern "C"
#endif

#undef _FUNC_
#undef _CALL_
#endif