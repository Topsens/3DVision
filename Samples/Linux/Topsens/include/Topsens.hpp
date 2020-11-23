#ifndef _TOPSENS_HPP_
#define _TOPSENS_HPP_

#include "Topsens.h"
#include <vector>

namespace Topsens
{
    typedef TOPSENS_VECTOR2     Vector2;
    typedef TOPSENS_VECTOR3     Vector3;
    typedef TOPSENS_VECTOR4     Vector4;

    typedef TOPSENS_JOINT       Joint;
    typedef TOPSENS_SKELETON    Skeleton;
    typedef TOPSENS_USER_MASK   UserMask;
    typedef TOPSENS_COLOR_FRAME ColorFrame;

    /**
     * @brief The error code.
     * @remarks Method return value, indicating the result of the method execution.
     */
    enum class Error
    {
        Ok               = TOPSENS_ERROR_OK,                /**< Method executed successfully. */
        Timeout          = TOPSENS_ERROR_TIMEOUT,           /**< Invalid parameter. */
        InvalidParameter = TOPSENS_ERROR_INVALID_PARAMETER, /**< Invalid parameter. */
        InvalidOperation = TOPSENS_ERROR_INVALID_OPERATION, /**< Invalid operation. */
        Initialization   = TOPSENS_ERROR_INITIALIZATION,    /**< Initialization failed or not initialized before use. */
        Authentication   = TOPSENS_ERROR_AUTHENTICATION,    /**< Product authentication failed. */
        NotSupported     = TOPSENS_ERROR_NOT_SUPPORTED,     /**< Not supported features. */
        NotFound         = TOPSENS_ERROR_NOT_FOUND,         /**< Object not found. */
        Closed           = TOPSENS_ERROR_CLOSED,            /**< Object closed. */
        DeviceClosed     = TOPSENS_ERROR_DEVICE_CLOSED,     /**< Device is not turned on. */
        DevicePermission = TOPSENS_ERROR_DEVICE_PERMISSION, /**< Device is not granted with permission for access. */
        StreamDisabled   = TOPSENS_ERROR_STREAM_DISABLED,   /**< Stream is disabled. */
        StreamContent    = TOPSENS_ERROR_STREAM_CONTENT,    /**< Stream content error. */
        StreamCodec      = TOPSENS_ERROR_STREAM_CODEC,      /**< Stream codec error. */
        StreamEnd        = TOPSENS_ERROR_STREAM_END,        /**< Stream end reached. */
        StreamIO         = TOPSENS_ERROR_STREAM_IO,         /**< Stream IO error. */
        Timestamp        = TOPSENS_ERROR_TIMESTAMP,         /**< Stream frame timestamp error. */
        Resource         = TOPSENS_ERROR_RESOURCE           /**< Resource loading error. */
    };

    /** @brief Image direction. */
    enum class Orientation
    {
        Landscape             = TOPSENS_ORIENTATION_LANDSCAPE,             /**< Default landscape direction. */
        PortraitClockwise     = TOPSENS_ORIENTATION_PORTRAIT_CLOCKWISE,    /**< Portrait direction, rotate 90 degrees clockwise to get landscape image. */
        PortraitAntiClockwise = TOPSENS_ORIENTATION_PORTRAIT_ANTICLOCKWISE /**< Portrait direction, rotate 90 degrees anticlockwise to get landscape image. */
    };

    /** @brief Image resolution. */
    enum class Resolution
    {
        Disabled = TOPSENS_RESOLUTION_DISABLED, /**< Turn off the image data stream */
        QVGA     = TOPSENS_RESOLUTION_QVGA,     /**< 320x240 */
        VGA      = TOPSENS_RESOLUTION_VGA       /**< 640x480 */
    };

    /**
     * @brief Gets the pixels width of image resolution.
     * @param resolution The enum value of image resolution.
     * @return The pixels number of image width.
     */
    inline uint32_t ResolutionWidth(Resolution resolution)
    {
        return TopsensResolutionWidth((int32_t)resolution);
    }

    /**
     * @brief Gets the pixels height of image resolution.
     * @param resolution The enum value of image resolution.
     * @return The pixels number of image height.
     */
    inline uint32_t ResolutionHeight(Resolution resolution)
    {
        return TopsensResolutionHeight((int32_t)resolution);
    }

    /**
     * @brief Depth image frame.
     * @remarks Contains a complete frame of depth images captured from sensor.
     */
    struct DepthFrame : public TOPSENS_DEPTH_FRAME
    {
        /**
         * @brief Converts depth frame to cloud frame.
         * @param cloud Cloud frame object.
         * @param onlyValid Whether to convert only valid depth values. 
         *                  If this parameter is true, only depth pixels with a depth value other than 0 will be converted to point cloud pixels.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        Error ToPointCloud(std::vector<Vector3>& cloud, bool onlyValid = false) const
        {
            if (!this->Width || !this->Height || !this->Pixels)
            {
                return Error::InvalidParameter;
            }

            cloud.resize(this->Width * this->Height);

            if (onlyValid)
            {
                uint32_t validCount;

                auto err = (Error)TopsensToPointCloudOnlyValid(this, &cloud[0], &validCount);
                if (Error::Ok != err)
                {
                    cloud.clear();
                    return err;
                }

                cloud.resize(validCount);
                return Error::Ok;
            }
            else
            {
                return (Error)TopsensToPointCloud(this, &cloud[0]);
            }
        }
    };
    
    /**
     * @brief Users frame.
     * @remarks Contains user data generated from the input depth frame calculation.
     */
    struct UsersFrame : public TOPSENS_USERS_FRAME
    {
        /**
         * @brief Obtains a projection of a skeleton joint in a depth camera space on a two-dimensional plane.
         * @remarks The screen coordinate origin point (0, 0) is located at the top left of the screen.
         * @param position The coordinates of the skeleton joint in 3D space.
         * @return Two-dimensional coordinates obtained after projection calculation of skeleton joint.
         */
        Vector2 MapTo2D(const Vector3 position) const
        {
            Vector2 pos2d;
            TopsensMapTo2D(&this->Projection, &position, &pos2d);
            return pos2d;
        }
    };

    /** @brief Skeleton joint index. */
    class JointIndex
    {
    public:
        enum
        {
            Head      = TOPSENS_JOINT_INDEX_HEAD,      /**< Head */
            Neck      = TOPSENS_JOINT_INDEX_NECK,      /**< Neck */
            LShoulder = TOPSENS_JOINT_INDEX_LSHOULDER, /**< Left shoulder */
            RShoulder = TOPSENS_JOINT_INDEX_RSHOULDER, /**< Right shoulder */
            LElbow    = TOPSENS_JOINT_INDEX_LELBOW,    /**< Left elbow */
            RElbow    = TOPSENS_JOINT_INDEX_RELBOW,    /**< Right elbow */
            LHand     = TOPSENS_JOINT_INDEX_LHAND,     /**< Left hand */
            RHand     = TOPSENS_JOINT_INDEX_RHAND,     /**< Right hand */
            LChest    = TOPSENS_JOINT_INDEX_LCHEST,    /**< Left chest */
            RChest    = TOPSENS_JOINT_INDEX_RCHEST,    /**< Right chest */
            LWaist    = TOPSENS_JOINT_INDEX_LWAIST,    /**< Left waist */
            RWaist    = TOPSENS_JOINT_INDEX_RWAIST,    /**< Right waist */
            LKnee     = TOPSENS_JOINT_INDEX_LKNEE,     /**< Left knee */
            RKnee     = TOPSENS_JOINT_INDEX_RKNEE,     /**< Right knee */
            LFoot     = TOPSENS_JOINT_INDEX_LFOOT,     /**< Left foot */
            RFoot     = TOPSENS_JOINT_INDEX_RFOOT,     /**< Right foot */
            Torso     = TOPSENS_JOINT_INDEX_TORSO,     /**< Torso */
            Count     = TOPSENS_JOINT_INDEX_COUNT      /**< The total number of joints */
        };
    };
}

#endif