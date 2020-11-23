#ifndef _TOPSENS_USER_EXTRACTOR_HPP_
#define _TOPSENS_USER_EXTRACTOR_HPP_

#include "TopsensUserExtractor.h"
#include "Topsens.hpp"

namespace Topsens
{
    namespace Toolkit
    {
        /** @brief User pixels extractor. */
        class UserExtractor
        {
        public:
            /** @brief Default constructor. */
            UserExtractor()
            {
                TopsensUserExtractorCreate(&this->handle);
            }

            /** @brief Destructor */
            ~UserExtractor()
            {
                TopsensUserExtractorDestroy(this->handle);
            }
            
            /**
             * @brief Initializes user pixels extractor instance.
             * @param resolution The resolution of depth image corresponding to the input users frame.
             * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
             */
            Error Initialize(Resolution resolution)
            {
                return (Error)TopsensUserExtractorInitialize(this->handle, (int)resolution);
            }
            
            /**
             * @brief Extracts user pixels.
             * @param cframe Color image frame.
             * @param uframe Users frame.
             * @param extracted The collection of extracted user pixels.
             * @return #Error::Ok if the call succeeds, otherwise refer to #Error for the specific meaning.
             */
            Error Extract(const ColorFrame& cframe, const UsersFrame& uframe, std::vector<uint32_t>& extracted)
            {
                if (cframe.Width <= 0 || cframe.Height <= 0)
                {
                    return Error::InvalidParameter;
                }

                extracted.resize(cframe.Width * cframe.Height);

                auto err = (Error)TopsensUserExtractorExtract(this->handle, &cframe, &uframe, &extracted[0]);
                if (Error::Ok != err)
                {
                    extracted.clear();
                }

                return err;
            }

        private:
            TOPSENS_USER_EXTRACTOR handle;
        };
    }
}

#endif