#ifndef _TOPSENS_H_
#define _TOPSENS_H_

#include <stdint.h>

/**
 * @brief The error code.
 * @remarks Method return value, indicating the result of the method execution.
 */
typedef enum _TOPSENS_ERROR
{
    TOPSENS_ERROR_OK                 = 0,   /**< Method executed successfully. */
    TOPSENS_ERROR_TIMEOUT            = 1,   /**< Data acquisition timeout. */
    TOPSENS_ERROR_INVALID_PARAMETER  = 2,   /**< Invalid parameter. */
    TOPSENS_ERROR_INVALID_OPERATION  = 3,   /**< Invalid operation. */
    TOPSENS_ERROR_INITIALIZATION     = 4,   /**< Initialization failed or not initialized before use. */
    TOPSENS_ERROR_AUTHENTICATION     = 5,   /**< Product authentication failed. */
    TOPSENS_ERROR_NOT_SUPPORTED      = 6,   /**< Not supported features. */
    TOPSENS_ERROR_NOT_FOUND          = 7,   /**< Object not found. */
    TOPSENS_ERROR_CLOSED             = 8,   /**< Object closed. */
    TOPSENS_ERROR_DEVICE_CLOSED      = 100, /**< Device is not turned on. */
    TOPSENS_ERROR_DEVICE_PERMISSION  = 101, /**< Device is not granted with permission for access. */
    TOPSENS_ERROR_STREAM_DISABLED    = 102, /**< Stream is disabled. */
    TOPSENS_ERROR_STREAM_CONTENT     = 103, /**< Stream content error. */
    TOPSENS_ERROR_STREAM_CODEC       = 104, /**< Stream codec error. */
    TOPSENS_ERROR_STREAM_END         = 105, /**< Stream end reached. */
    TOPSENS_ERROR_STREAM_IO          = 106, /**< Stream IO error. */
    TOPSENS_ERROR_TIMESTAMP          = 200, /**< Stream frame timestamp error. */
    TOPSENS_ERROR_RESOURCE           = 201, /**< Resource loading error. */
    TOPSENS_ERROR_CONFIG             = 202  /**< Config loading error. */

} TOPSENS_ERROR;

/** @brief Preconfigured identity. */
typedef enum _TOPSENS_CONFIG
{
    TOPSENS_CONFIG_RSD415 = 0x1000, /**< Intel RealSense D415 depth camera. */
    TOPSENS_CONFIG_RSD435 = 0x1001  /**< Intel RealSense D435 depth camera. */
} TOPSENS_CONFIG;

/** @brief Image direction. */
typedef enum _TOPSENS_ORIENTATION
{
    TOPSENS_ORIENTATION_LANDSCAPE = 0,         /**< Default landscape direction. */
    TOPSENS_ORIENTATION_PORTRAIT_CLOCKWISE,    /**< Portrait direction, rotate 90 degrees clockwise to get landscape image. */
    TOPSENS_ORIENTATION_PORTRAIT_ANTICLOCKWISE /**< Portrait direction, rotate 90 degrees anticlockwise to get landscape image. */
} TOPSENS_ORIENTATION;

/** @brief Image resolution. */
typedef enum _TOPSENS_RESOLUTION
{
    TOPSENS_RESOLUTION_DISABLED = 0, /**< Turn off the image data stream */
    TOPSENS_RESOLUTION_QVGA,         /**< 320x240 */
    TOPSENS_RESOLUTION_VGA           /**< 640x480 */

} TOPSENS_RESOLUTION;

/**
 * @brief The vector containing 2 float values.
 * @remarks Screen vector.
 */
typedef struct _TOPSENS_VECTOR2
{
    float X; /**< X-axis component. Rightward direction is positive. */
    float Y; /**< Y-axis component. Downward direction is positive. */
} TOPSENS_VECTOR2;

/**
 * @brief The vector containing 3 float values.
 * @remarks Space vector.
 */
typedef struct _TOPSENS_VECTOR3
{
    float X; /**< X-axis component. Rightward direction is positive. */
    float Y; /**< Y-axis component. Upward direction is positive. */
    float Z; /**< Z-axis component. The camera illumination direction is positive. */

} TOPSENS_VECTOR3;

/**
 * @brief The vector containing 4 float values.
 * @remarks Space vector.
 */
typedef struct _TOPSENS_VECTOR4
{
    /**
     * When representing the ground plane equation, X is the coefficient 'A' in equation Ax + By + Cz + D = 0.
     * When representing the projection to the image, X is the focal in horizontal direction.
     * When representing the space rotation, X is the coefficient 'b' in quaternion a + bi + cj + dk.
     */
    float X;
    
    /**
     * When representing the ground plane equation, Y is the coefficient 'B' in equation Ax + By + Cz + D = 0.
     * When representing the projection to the image, Y is the focal in vertical direction.
     * When representing the space rotation, Y is the coefficient 'c' in quaternion a + bi + cj + dk.
     */
    float Y;
    
    /**
     * When representing the ground plane equation, Z is the coefficient 'C' in equation Ax + By + Cz + D = 0.
     * When representing the projection to the image, Z is the center in horizontal direction.
     * When representing the space rotation, Z is the coefficient 'd' in quaternion a + bi + cj + dk.
     */
    float Z;
    
    /**
     * When representing the ground plane equation, W is coefficient 'D' in equation Ax + By + Cz + D = 0.
     * When representing the projection to the image, W is the center in vertical direction.
     * When representing the space rotation, W is the coefficient 'a' in quaternion a + bi + cj + dk.
     */
    float W;

} TOPSENS_VECTOR4;

/**
 * @brief Color image frame.
 * @remarks Contains a complete frame of color images captured from sensor.
 */
typedef struct _TOPSENS_COLOR_FRAME
{
    uint32_t Width;     /**< Image width in pixels. */
    uint32_t Height;    /**< Image height in pixels. */
    uint32_t FrameId;   /**< Image frame number starting from 0. */
    uint64_t Timestamp; /**< Image acquisition timestamp in milliseconds. */
    
    /**
     * The collection of image pixels. Each pixel containing four bytes of red, green, blue and alpha channels.
     * The length of the array is #Width * #Height.
     */
    const uint32_t* Pixels;

} TOPSENS_COLOR_FRAME;

/**
 * @brief Depth image frame.
 * @remarks Contains a complete frame of depth images captured from sensor.
 */
typedef struct _TOPSENS_DEPTH_FRAME
{
    uint32_t Width;     /**< Image width in pixels. */
    uint32_t Height;    /**< Image height in pixels. */
    uint32_t FrameId;   /**< Image frame number starting from 0. */
    uint64_t Timestamp; /**< Image acquisition timestamp in milliseconds. */

    /**
     * The collection of image pixels.
     * Each element represents the distance from the point on the photographed object to the imaging plane of the sensor, in millimeters.
     * The length of the array is #Width * #Height.
     */
    const uint16_t* Pixels;

    /**
     * Projection parameters.
     * Contains 4 elements: horizontal focal length, vertical focal length, horizontal center coordinate and vertical center coordinate.
     */
    TOPSENS_VECTOR4 Projection;

} TOPSENS_DEPTH_FRAME;

/** @brief Skeleton joint index. */
typedef enum _TOPSENS_JOINT_INDEX
{
    TOPSENS_JOINT_INDEX_HEAD      = 0,  /**< Head */
    TOPSENS_JOINT_INDEX_NECK      = 1,  /**< Neck */
    TOPSENS_JOINT_INDEX_LSHOULDER = 2,  /**< Left shoulder */
    TOPSENS_JOINT_INDEX_RSHOULDER = 3,  /**< Right shoulder */
    TOPSENS_JOINT_INDEX_LELBOW    = 4,  /**< Left elbow */
    TOPSENS_JOINT_INDEX_RELBOW    = 5,  /**< Right elbow */
    TOPSENS_JOINT_INDEX_LHAND     = 6,  /**< Left hand */
    TOPSENS_JOINT_INDEX_RHAND     = 7,  /**< Right hand */
    TOPSENS_JOINT_INDEX_LCHEST    = 8,  /**< Left chest */
    TOPSENS_JOINT_INDEX_RCHEST    = 9,  /**< Right chest */
    TOPSENS_JOINT_INDEX_LWAIST    = 10, /**< Left waist */
    TOPSENS_JOINT_INDEX_RWAIST    = 11, /**< Right waist */
    TOPSENS_JOINT_INDEX_LKNEE     = 12, /**< Left knee */
    TOPSENS_JOINT_INDEX_RKNEE     = 13, /**< Right knee */
    TOPSENS_JOINT_INDEX_LFOOT     = 14, /**< Left foot */
    TOPSENS_JOINT_INDEX_RFOOT     = 15, /**< Right foot */
    TOPSENS_JOINT_INDEX_TORSO     = 16, /**< Torso */
    TOPSENS_JOINT_INDEX_COUNT           /**< The total number of joints */

} TOPSENS_JOINT_INDEX;

/**
 * @brief Skeleton joint.
 * @remarks Contains a three-dimensional spatial information of a joint in the application scenario.
 */
typedef struct _TOPSENS_JOINT
{
    /** Represents the X, Y and Z coordinates in three-dimensional space. */
    TOPSENS_VECTOR3 Position;

    /** The quaternion {x, y, z, w}, representing the spatial rotation of the current joint relative to T-pose in world space coordinates. */
    TOPSENS_VECTOR4 Rotation;

    /**
     * The rotation expression of the joint relative to T-Pose in world space coordinates,
     * and the three elements representing the rotated X, Y and Z axes of the joint.
     */
    TOPSENS_VECTOR3 Orientation[3];

} TOPSENS_JOINT;

/**
 * @brief The skeleton data.
 * @remarks Contains skeleton data for an identified user.
 */
typedef struct _TOPSENS_SKELETON
{
    uint16_t        UserId;   /**< The ID assigned to the user starting from 1 to distinguish multiple users in the scene. */
    uint8_t         Tracked;  /**< The user status indicating whether the user is successfully tracked in the current frame of data. */
    TOPSENS_VECTOR3 Position; /**< The user position indicating the approximate location of the user's body center in the scene. */
    
    /** Joints data. Each element represents the three-dimensional position of a joint on the use's body in the scene. */
    TOPSENS_JOINT Joints[TOPSENS_JOINT_INDEX_COUNT];

} TOPSENS_SKELETON;

/**
 * @brief User mask image.
 * @remarks Contains mask information of the identified user in the depth image or color image.
 */
typedef struct _TOPSENS_USER_MASK
{
    uint32_t Width;  /**< Mask image width in pixels. It is equal to the corresponding depth image width. */
    uint32_t Height; /**< Mask image height in pixels. It is equal to the corresponding depth image height. */
    
    /**
     * Mask image pixel collection, array length is #Width * #Height. 
     * Each pixel values is equal to the user ID to which the corresponding pixel in the depth image belongs.
     * For example, if the mask pixel value of x=100 and y=100 is 1,
     * the depth pixel corresponding to x=100 and y=100 in depth image belongs to the user body with ID=1.
     */
    uint16_t* Pixels;

} TOPSENS_USER_MASK;

/**
 * @brief Users frame.
 * @remarks Contains user data generated from the input depth frame calculation.
 */
typedef struct _TOPSENS_USERS_FRAME
{
    /** The count of identified users. */
    uint32_t UserCount;
    
    /**
     * The collection of user skeleton data. Each element corresponds to all identified skeleton joints of a user.
     * The effective length of this array is #UserCount.
     */
    TOPSENS_SKELETON* Skeletons;

    /** User mask data. Represents the relationship between each pixel in the depth image and the user. */
    TOPSENS_USER_MASK UserMask;

    /**
     * The plane equation of the ground detected in the scene.
     * The four elements represent the four coefficients A, B, C and D in the plane equation Ax+By+Cz+D=0.
     */
    TOPSENS_VECTOR4 GroundPlane;

    /**
     * Projection parameters.
     * Contains 4 elements: horizontal focal length, vertical focal length, horizontal center coordinate and vertical center coordinate.
     */
    TOPSENS_VECTOR4 Projection;

    
    /**
     * The data timestamp in milliseconds. It is the same as the timestamp of the corresponding input depth image,
     * and is used to synchronize the depth image.
     */
    uint64_t Timestamp;

} TOPSENS_USERS_FRAME;

/**
 * @brief Obtains a projection of a skeleton joint in a depth camera space on the two-dimensional depth image.
 * @remarks The screen coordinate origin point (0, 0) is located at the upper-left corner of the screen.
 * @param projection projection parameters of users frame.
 * @param position3D The coordinates of the skeleton joint in 3D space.
 * @param position2D Two-dimensional coordinates obtained after projection calculation of skeleton joint.
 * @return #TOPSENS_ERROR_OK if the call succeeds, otherwise refer to #_TOPSENS_ERROR for the specific meaning.
 */
static int32_t TopsensMapTo2D(const TOPSENS_VECTOR4* projection, const TOPSENS_VECTOR3* position3D, TOPSENS_VECTOR2* position2D)
{
    if (!projection || !position2D || !position3D)
    {
        return TOPSENS_ERROR_INVALID_PARAMETER;
    }

    position2D->X =  position3D->X / position3D->Z * projection->X + projection->Z;
    position2D->Y = -position3D->Y / position3D->Z * projection->Y + projection->W;

    return TOPSENS_ERROR_OK;
}

/**
 * @brief Gets the pixels width of image resolution.
 * @param resolution The enum value of image resolution.
 * @return The pixels number of image width.
 */
static uint32_t TopsensResolutionWidth(uint32_t resolution)
{
    switch (resolution)
    {
    case TOPSENS_RESOLUTION_QVGA:
        return 320;

    case TOPSENS_RESOLUTION_VGA:
        return 640;

    default:
        return 0;
    }
}

/**
 * @brief Gets the pixels height of image resolution.
 * @param resolution The enum value of image resolution.
 * @return The pixels number of image height.
 */
static uint32_t TopsensResolutionHeight(uint32_t resolution)
{
    switch (resolution)
    {
    case TOPSENS_RESOLUTION_QVGA:
        return 240;

    case TOPSENS_RESOLUTION_VGA:
        return 480;

    default:
        return 0;
    }
}

#ifdef _MSC_VER
    #if defined(TOPSENS_SENSOR_EXPORT) || defined(TOPSENS_CAMERA_EXPORT)
        #define _FUNC_ __declspec(dllexport)
    #else
        #define _FUNC_
    #endif
#else
    #if defined(TOPSENS_SENSOR_EXPORT) || defined(TOPSENS_CAMERA_EXPORT)
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

#ifdef __cplusplus
extern "C" {
#endif

_FUNC_ int32_t _CALL_ TopsensToPointCloud(const TOPSENS_DEPTH_FRAME* frame, TOPSENS_VECTOR3* cloud);
_FUNC_ int32_t _CALL_ TopsensToPointCloudOnlyValid(const TOPSENS_DEPTH_FRAME* frame, TOPSENS_VECTOR3* cloud, uint32_t* validCount);

#ifdef __cplusplus
} // extern "C"
#endif

#undef _CALL_
#undef _FUNC_

#endif