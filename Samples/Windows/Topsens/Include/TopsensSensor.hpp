#ifndef _TOPSENS_SENSOR_HPP_
#define _TOPSENS_SENSOR_HPP_

#include "Topsens.hpp"
#include "TopsensSensor.h"
#include <vector>

namespace Topsens
{
    /** @brief The sensor operation interface (Dedicated for TOPSENS own cameras). */
    class Sensor
    {
    public:
        /**
         * @brief Gets the count of available sensors.
         * @param count The count of available sensors.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        static Error Count(uint32_t& count)
        {
            return (Error)TopsensSensorCount(&count);
        }

        /**
         * @brief Preloads resource data to minimize sensor startup delay.
         * @param resources Preloaded resource id array.
         * @param count The count of id array.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        static Error Preload(const uint32_t* resources, uint32_t count)
        {
            return (Error)TopsensSensorPreload((uint32_t*)resources, count);
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
        Sensor() : handle(nullptr)
        {
        }

        /** @brief Destructor. */
        ~Sensor()
        {
            this->Close();
        }

        /**
         * @brief Opens the sensor instance.
         * @param index The index value of the sensor to use (starting from 0).
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        Error Open(uint32_t index)
        {
            if (this->handle)
            {
                this->Close();
            }

            return (Error)TopsensSensorCreate(index, &this->handle);
        }

        /** @brief Closes the sensor instance. */
        void Close()
        {
            TopsensSensorDestroy(this->handle);
            this->handle = nullptr;
        }

        /**
         * @brief Starts the sensor instance.
         * @remarks Color, depth and users streams are generated continuously when the sensor is started.
         * @param colorResolution Color image resolution.
         * @param depthResolution Depth image resolution.
         * @param generateUsers Generates users if true, otherwise doesn't generate users.
         * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
         */
        Error Start(Resolution colorResolution, Resolution depthResolution, bool generateUsers)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorStart(this->handle, (int32_t)colorResolution, (int32_t)depthResolution, generateUsers ? 1 : 0);
        }
        
        /**
         * @brief Stops the sensor instance.
         * @remarks Color, depth and users streams are ended when the sensor is stopped.
         */
        void Stop()
        {
            if (this->handle)
            {
                TopsensSensorStop(this->handle);
            }
        }

        /**
         * @brief Gets a color frame within a certain time limit.
         * @remarks Each pixel is 4 channels with ARGB order.
         * @param frame color frame.
         * @param timeout Waiting time limit.
         * @return #Error::Ok if the call succeeds, #Error::Timeout if the new frame is not successfully obtained within the specified time,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error WaitColor(ColorFrame& frame, uint32_t timeout)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorWaitColor(this->handle, &frame, timeout);
        }

        /**
         * @brief Gets a depth frame within a certain time limit.
         * @param frame Depth frame.
         * @param timeout Waiting time limit.
         * @return #Error::Ok if the call succeeds, #Error::Timeout if the new frame is not successfully obtained within the specified time,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error WaitDepth(DepthFrame& frame, uint32_t timeout)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorWaitDepth(this->handle, &frame, timeout);
        }
        
        /**
         * @brief Gets a users frame with a certain time limit.
         * @param frame Users frame.
         * @param timeout Waiting time limit.
         * @return #Error::Ok if the call succeeds, #Error::Timeout if the new frame is not successfully obtained within the specified time,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error WaitUsers(UsersFrame& frame, uint32_t timeout)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorWaitUsers(this->handle, &frame, timeout);
        }

        /**
         * @brief Sets image orientation.
         * @remarks Default is #Orientation::Landscape. If the setting needs to be changed, call this method before #Start(Resolution, Resolution, bool).
         * @param orientation Image orientation.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetOrientation(Orientation orientation)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorSetOrientation(this->handle, (int32_t)orientation);
        }

        /**
         * @brief Gets image orientation.
         * @remarks Default is #Orientation::Landscape.
         * @return The image orientation set. Returns the default #Orientation::Landscape if sensor isn't opened.
         */
        Orientation GetOrientation()
        {
            if (!this->handle)
            {
                return Orientation::Landscape;
            }

            return (Orientation)TopsensSensorGetOrientation(this->handle);
        }

        /**
         * @brief Sets whether the image is flipped.
         * @remarks The image is flipped by default. If the setting needs to be changed, call this method before #Start(Resolution, Resolution, bool).
         * @param flipped The image is flipped if true, otherwise not flipped.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetImageFlipped(bool flipped)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorSetImageFlipped(this->handle, flipped ? 1 : 0);
        }

        /**
         * @brief Checks whether the image is flipped.
         * @remarks The image is flipped by default.
         * @return True if the image is flipped, otherwise false.
         */
        bool IsImageFlipped()
        {
            if (this->handle)
            {
                return TopsensSensorIsImageFlipped(this->handle) ? 1 : 0;
            }

            return true;
        }

        /**
         * @brief Sets whether the depth image is aligned with the color image.
         * @remarks The depth image is not aligned with the color image by default.
         *          If the setting needs to be changed, call this method before #Start(Resolution, Resolution, bool).
         * @param aligned The depth image is aligned with the color image if true, otherwise not aligned.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetDepthAligned(bool aligned)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorSetDepthAligned(this->handle, aligned ? 1 : 0);
        }

        /**
         * @brief Checks whether the depth image is aligned with the color image.
         * @remarks The depth image is not aligned with the color image by default.
         * @return True if the depth image is aligned with the color image, otherwise false.
         */
        bool IsDepthAligned()
        {
            if (this->handle)
            {
                return TopsensSensorIsDepthAligned(this->handle) ? true : false;
            }

            return false;
        }

        /**
         * @brief Sets whether to enable user mask.
         * @remarks Default is not enabled. If the setting needs to be changed, call this method before #Start(Resolution, Resolution, bool).
         * @param enabled Enabled if true, otherwise not enabled.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetMaskEnabled(bool enabled)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorSetMaskEnabled(this->handle, enabled ? 1 : 0);
        }

        /**
         * @brief Checks whether user mask is enabled.
         * @remarks Default is not enabled.
         * @return True if user mask is enabled, otherwise false.
         */
        bool IsMaskEnabled()
        {
            if (this->handle)
            {
                return TopsensSensorIsMaskEnabled(this->handle) ? true : false;
            }

            return false;
        }

        /**
         * @brief Sets whether records stream to file.
         * @remarks Doesn't record stream to file by default. If the setting needs to be changed, call this method before #Start(Resolution, Resolution, bool).
         *          The recording file format is YYYY_MMDD_HHMMSS.tsr.
         * @param recording Records stream to file if true, otherwise not record.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetRecording(bool recording)
        {
            if (!this->handle)
            {
                return Error::Closed;
            }

            return (Error)TopsensSensorSetRecording(this->handle, recording ? 1 : 0);
        }

        /**
         * @brief Checks whether records stream to file.
         * @remarks Doesn't record stream to file by defaul.
         * @return True if records strem to file, otherwise false.
         */
        bool IsRecording()
        {
            if (this->handle)
            {
                return TopsensSensorIsRecording(this->handle) ? true : false;
            }

            return false;
        }

        /**
         * @brief Sets preconfigured identity.
         * @param config Preconfigured identity.
         * @return #Error::Ok if the call succeeds, #Error::InvalidOperation if the sensor has been started,
         *         otherwise refer to #Error for the specific meaning.
         */
        Error SetConfig(Config config)
        {
            return (Error)TopsensSensorSetConfig(this->handle, (uint32_t)config);
        }

    private:
        TOPSENS_SENSOR handle;
    };
}

#endif