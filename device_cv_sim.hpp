#ifndef DEVICE_CV_SIM_HPP
#define DEVICE_CV_SIM_HPP

#include <stdint.h>
#include <opencv/cv.h>
#include "device.hpp"
#include "types.h"

namespace Device
{
    class CV_sim : public Device::Generic
    {
        public:
            CV_sim();
            CV_sim( const std::string filename );
            /* virtual */ bool move_to(const xy &aPoint);
            /* virtual */ bool cut_to(const xy &aPoint);
            /* virtual */ bool curve_to(const xy &p0, const xy &p1, const xy &p2, const xy &p3);
            /* virtual */ bool start();
            /* virtual */ bool stop();
            /* virtual */ xy   get_dimensions();
			inline IplImage * get_image(){return image;};
        private:
            xy convert_to_internal( const xy &input );
            xy current_position;
            std::string output_filename;
            bool running;
            IplImage * image;
    };
}
#endif
