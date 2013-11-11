//this is /contrib/bm/dvrml/dvrml_indexed_lineset.cxx

#include"dvrml_indexed_lineset.h"

void dvrml_indexed_lineset::draw( vcl_ostream& os )
{
    assert(this->lineset_point_list.size() > 0);

    os << '#' << this->shape() << '\n'
       << "Shape {\n"
       << "     geometry IndexedLineSet {\n"
       << "         coord Coordinate {\n"
       << "             point [\n";

    point_list_type::iterator point_list_itr;
    point_list_type::iterator point_list_end = lineset_point_list.end();

    for( point_list_itr = lineset_point_list.begin(); point_list_itr != point_list_end; ++point_list_itr )
    {
        os << "                  " << point_list_itr->x() << ' ' << point_list_itr->y() << ' ' << point_list_itr->z() << ",\t" << "#point index " << point_list_itr - this->lineset_point_list.begin() << '\n';
    }
    os << "                 ]#end point\n"
       << "             }#end coord\n"
       << "     coordIndex [\n";
   
    connection_list_type::iterator connection_list_itr;
    connection_list_type::iterator connection_list_end = this->lineset_connection_list.end();

    for( connection_list_itr = this->lineset_connection_list.begin(); connection_list_itr != connection_list_end; ++connection_list_itr )
    {
        vcl_vector<unsigned>::iterator inner_connection_itr;
        vcl_vector<unsigned>::iterator inner_connection_end = connection_list_itr->end();

        os << "             ";

        for( inner_connection_itr = connection_list_itr->begin(); inner_connection_itr != inner_connection_end; ++inner_connection_itr )
        {
            os <<  *inner_connection_itr << ", ";
        }

        os << "-1,\n";
    }

    os << "         ]#end coordIndex\n";
    if( this->lineset_appearance_list.size() > 0 )
    {
        os  << " colorPerVertex FALSE\n"
            << " color Color {\n"
            << "        color [\n";

        appearance_list_type::iterator appearance_list_itr;
        appearance_list_type::iterator appearance_list_end = this->lineset_appearance_list.end();

        for( appearance_list_itr = this->lineset_appearance_list.begin(); appearance_list_itr != appearance_list_end; ++appearance_list_itr )
        {
            vnl_vector_fixed<double,3> color = (*appearance_list_itr)->diffuseColor();
            os << "         " << color.get(0) << ' ' << color.get(1) << ' ' << color.get(2) << '\n';
        }// end appearance iteration

        os << "         ]#end color\n";
        os << "     }#end Color\n";
    }
    os << "     }#end geometry\n" 
       << "}#end Shape\n";

}//end dvrml_indexed_lineset::draw