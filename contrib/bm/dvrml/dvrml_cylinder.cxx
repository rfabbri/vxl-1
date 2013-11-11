//this is /contrib/bm/dvrml/dvrml_cylinder.cxx

#include"dvrml_cylinder.h"

void dvrml_cylinder::draw( vcl_ostream& os )
{
    os << '#' << this->shape() << '\n'
       << "Transform{\n"
       << "  translation " << center.x() << ' ' << center.y() << ' ' << center.z() << '\n'
       << "  rotation " << xyz_rotation.get(0) << ' ' << xyz_rotation.get(1) << ' ' << xyz_rotation.get(2) << ' ' << xyz_rotation.get(3) << '\n'
       << "  scale " << xyz_scale.get(0) << ' ' << xyz_scale.get(1) << ' ' << xyz_scale.get(2) << '\n'
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
       << "         geometry Cylinder{\n"
       << "             radius " << radius << '\n' 
       << "             height " << height << '\n' 
       << "         } #end geometry\n"
       << "         } #end Shape\n"
       << "   ] #end children\n"
       << "} #end transform\n\n";
}//end dvrml_cylinder::draw