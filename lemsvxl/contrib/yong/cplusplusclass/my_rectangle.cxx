#include "my_rectangle.h"


std::ostream&  operator<<(std::ostream& s, my_rectangle &r) {
    s << "<my_rectangle : "<<std::endl;
    s << "width: "<< r.width() << ','<<" height: " << r.height() << "> ";
    return s;

}

