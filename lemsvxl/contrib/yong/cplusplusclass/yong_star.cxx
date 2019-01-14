#include "yong_star.h"


std::ostream&  operator<<(std::ostream& s, yong_star &star) {
    s << "<yong_star : "<<std::endl;
    s << "center_x: "<< star.center_x() << "center_y: "<< star.center_y() << std::endl;
        s << "arm number: " << star.arm_number() << ", arm orientation: " << star.orientation() << std::endl;
    return s;
}


