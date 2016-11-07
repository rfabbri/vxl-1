#include "my_rectangle.h"


vcl_ostream&  operator<<(vcl_ostream& s, my_rectangle &r) {
    s << "<my_rectangle : "<<vcl_endl;
    s << "width: "<< r.width() << ','<<" height: " << r.height() << "> ";
    return s;

}

