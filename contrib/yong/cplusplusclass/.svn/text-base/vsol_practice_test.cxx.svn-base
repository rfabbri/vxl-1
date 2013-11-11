#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_vector_2d.h>

vsol_line_2d_sptr line;


void print_end_points();

void print_end_points()
{
    vcl_cout<< " p0 x = " << line->p0()->x() << vcl_endl;
    vcl_cout<< " p0 y = " << line->p0()->y() << vcl_endl;
    vcl_cout<< " p1 x = " << line->p1()->x() << vcl_endl;
    vcl_cout<< " p1 y = " << line->p1()->y() << vcl_endl;
}

int main()
{
    line = new vsol_line_2d();
    
    vcl_cout<< " setting endpoints to (50,80) and (90,0) " << vcl_endl;
    line->set_p0(vsol_point_2d_sptr( new vsol_point_2d( 50 , 80 )));
    line->set_p1(vsol_point_2d_sptr( new vsol_point_2d( 90 ,0 )));
    print_end_points();
    line->get_min_x();
    vsol_point_2d_sptr p;
    line->add_vector_to_p0(vgl_vector_2d<double>(10,10));
    line->add_vector_to_p1(vgl_vector_2d<double>(-90,0));
    vcl_cout<< " add (10,10) to (0,90) and (-90,0) to (90,0) " << vcl_endl;
    print_end_points();
    while(1);
}

