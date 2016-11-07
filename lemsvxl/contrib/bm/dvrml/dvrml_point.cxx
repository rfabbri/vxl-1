//this is /contrib/bm/dvrml/dvrml_point.cxx

#include"dvrml_point.h"

void dvrml_point::draw( vcl_ostream& os )
{
    os << '#' << this->shape() << '\n'
       << "Transform{\n"
       << "  translation " << center.x() << ' ' << center.y() << ' ' << center.z() << '\n'
       << "  children [\n"
       << "     Shape{\n"
       << "         appearance Appearance {\n"
       << "             material Material {\n"
       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
       << "                 shininess " << appearance_sptr->shininess() << '\n'
       << "                 transparency " << appearance_sptr->transparency() << '\n'
       << "             }# end Material\n"
       << "         }# end Appearance\n"
       << "         geometry Sphere{\n"
       << "             radius " << radius << '\n'
       << "         } #end geometry\n"
       << "         } #end Shape\n"
       << "   ] #end children\n"
       << "} #end transform\n\n";

}//end dvrml_point::draw( vcl_ostream& os )