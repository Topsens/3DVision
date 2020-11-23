#ifndef _TOPSENS_PEOPLE_HPP_
#define _TOPSENS_POEPLE_HPP_

#include "TopsensPeople.h"
#include "Topsens.hpp"
#include <vector>

namespace Topsens
{
    /** @brief The people detector operation interface (Dedicated for all third-party cameras). */
    class People
    {
    public:
        /**
         * @brief Preloads resource data to minimize sensor startup delay.
         * @param resources Preloaded resource id array.
         * @param count The count of id array.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        static Error Preload(const uint32_t* resources, uint32_t count)
        {
            return (Error)TopsensPeoplePreload((uint32_t*)resources, count);
        }

        /**
         * @brief Preloads resource data to minimize sensor startup delay.
         * @param list Preloaded resource id list.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        static Error Preload(const std::initializer_list<uint32_t>& list)
        {
            std::vector<uint32_t> resources(list);
            return Preload(resources.data(), (uint32_t)resources.size());
        }
        
        /** @brief Default constructor. */
        People() : handle(nullptr)
        {
            TopsensPeopleCreate(&this->handle);
        }

        /** @brief Destructor. */
        ~People()
        {
            this->Uninitialize();
            TopsensPeopleDestroy(this->handle);
        }
        
        /**
         * @brief Initializes people detector.
         * @param width Depth frame width.
         * @param height Depth frame hedith.
         * @param fx Horizontal focal length.
         * @param fy Vertical focal length.
         * @param cx Horizontal center coordinate.
         * @param cy Vertical center coordinate.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        Error Initialize(uint32_t width, uint32_t height, float fx, float fy, float cx, float cy)
        {
            this->Uninitialize();

            auto err = TopsensPeopleInitialize(this->handle, width, height, fx, fy, cx, cy);
            if (err)
            {
                this->Uninitialize();
                return (Error)err;
            }

            return Error::Ok;
        }

        /** @brief Uninitializes people detector instance. */
        void Uninitialize()
        {
            TopsensPeopleUninitialize(this->handle);
        }

        /**
         * @brief Sets depth image orientation.
         * @remarks Default is #Orientation::Landscape. If the setting needs to be changed, call this method before #Initialize(uint32_t, uint32_t, float, float, float, float).
         * @param orientation Depth image orientation.
         * @return #Error.Ok if the call succeeds, #Error::InvalidOperation if the people detector has been initialized,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetOrientation(Orientation orientation)
        {
            return (Error)TopsensPeopleSetOrientation(this->handle, (int32_t)orientation);
        }

        /**
         * @brief Gets depth image orientation.
         * @remarks Default is #Orientation::Landscape.
         * @return The depth image orientation set. Returns the default #Orientation::Landscape if people detector is closed.
         */
        Orientation GetOrientation() const
        {
            return (Orientation)TopsensPeopleGetOrientation(this->handle);
        }

        /**
         * @brief Sets whether the depth image is flipped.
         * @remarks The depth image is flipped by default. If the setting needs to be changed, call this method before #Initialize(uint32_t, uint32_t, float, float, float, float).
         * @param flipped The depth image is flipped if true, otherwise not flipped.
         * @return #Error.Ok if the call succeeds, #Error::InvalidOperation if the people detector has been initialized,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetImageFlipped(bool flipped)
        {
            return (Error)TopsensPeopleSetImageFlipped(this->handle, flipped ? 1 : 0);
        }

        /**
         * @brief Checks whether the depth image is flipped.
         * @remarks The depth image is flipped by default.
         * @return True if the depth image is flipped, otherwise false.
         */
        bool IsImageFlipped() const
        {
            return TopsensPeopleIsImageFlipped(this->handle) ? true : false;
        }

        /**
        * @brief Sets whether to enable user mask.
        * @remarks Default is not enabled. If the setting needs to be changed, call this method before #Initialize(uint32_t, uint32_t, float, float, float, float).
        * @param enabled Enabled if true, otherwise not enabled.
        * @return #Error.Ok if the call succeeds, #Error::InvalidOperation if the people detector has been initialized,
         *         otherwise refer to #Error for the specific meaning.
        */
        Error SetMaskEnabled(bool enabled)
        {
            return (Error)TopsensPeopleSetMaskEnabled(this->handle, enabled ? 1 : 0);
        }

        /**
        * @brief Checks whether user mask is enabled.
        * @remarks Default is not enabled.
        * @return True if user mask is enabled, otherwise false.
        */
        bool IsMaskEnabled() const
        {
            return TopsensPeopleIsMaskEnabled(this->handle) ? true : false;
        }
        
        /**
         * @brief Processes depth data.
         * @param depth Depth frame data.
         * @param width Depth frame width.
         * @param height Depth frame height.
         * @param timestamp Depth frame timestamp.
         * @param users The generated users frame.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        Error Detect(const uint16_t* depth, uint32_t width, uint32_t height, int64_t timestamp, UsersFrame& users)
        {
            return (Error)TopsensPeopleDetect(this->handle, depth, width, height, timestamp, &users);
        }

    private:
        TOPSENS_PEOPLE handle;
    };
}

#endif