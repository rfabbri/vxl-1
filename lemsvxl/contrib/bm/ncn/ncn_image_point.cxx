// this is contrib/bm/ncn/ncn_image_point.cxx

#include "ncn_image_point.h"

vcl_ostream& operator << (vcl_ostream& os, const ncn_image_point& p)
{
    os << p.x() << '\t' << p.y();
    return os;
}// end ostream operator <<
