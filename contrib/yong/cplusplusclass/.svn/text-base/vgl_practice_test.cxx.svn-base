#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_vector.h>
#include <vcl_vector.txx>
#include <vcl_iostream.h>

//Globals
vcl_vector<vgl_point_2d<double>> p;
vcl_vector<vgl_point_2d<double>> p2;

//template <class Type>
//vcl_ostream&  operator<<(vcl_ostream& s, vcl_vector<Type> const& p);


int main()
{  
    double width =10;
    p.push_back(vgl_point_2d<double>(0,0));
    p.push_back(vgl_point_2d<double>(0,width));
    p.push_back(vgl_point_2d<double>(width,width));
    p.push_back(vgl_point_2d<double>(15,5));
    p.push_back(vgl_point_2d<double>(width,0));
    //p.push_back(vgl_point_2d<double>(10,10));
    
    vgl_polygon<double> poly(p);
    poly.new_sheet();
    
    poly.push_back(vgl_point_2d<double>(5,5));
    poly.push_back(vgl_point_2d<double>(5,7));
    poly.push_back(vgl_point_2d<double>(7,7));
    poly.push_back(vgl_point_2d<double>(7,5));

    //scan polygon and scale its coordinates

    vgl_polygon_scan_iterator<double> psi(poly);
    
    for (psi.reset(); psi.next(); ) {
        int y = psi.scany();
        vcl_cout << vcl_endl;
        vcl_cout<<"y = "<< y<< vcl_endl;
        for (int x = psi.startx(); x <= psi.endx(); ++x){
            vcl_cout<<x;
        }
    }

    vcl_cout <<poly.num_sheets()<< vcl_endl;
    vcl_cout <<poly.num_vertices()<< vcl_endl;
    vcl_cout<<poly<<vcl_endl;
    
    vgl_polygon<double> poly2(poly[0]);
    vcl_cout<<"Poly2"<<vcl_endl<<poly2<<vcl_endl;
    while(1);

}


//#define VGL_VECTOR_INSTANTIATE(vgl_point_2d);


