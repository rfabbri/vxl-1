#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_point_2d.h>
#include <vector>
#include <vectorxx>
#include <iostream>

//Globals
std::vector<vgl_point_2d<double>> p;
std::vector<vgl_point_2d<double>> p2;

//template <class Type>
//std::ostream&  operator<<(std::ostream& s, std::vector<Type> const& p);


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
        std::cout << std::endl;
        std::cout<<"y = "<< y<< std::endl;
        for (int x = psi.startx(); x <= psi.endx(); ++x){
            std::cout<<x;
        }
    }

    std::cout <<poly.num_sheets()<< std::endl;
    std::cout <<poly.num_vertices()<< std::endl;
    std::cout<<poly<<std::endl;
    
    vgl_polygon<double> poly2(poly[0]);
    std::cout<<"Poly2"<<std::endl<<poly2<<std::endl;
    while(1);

}


//#define VGL_VECTOR_INSTANTIATE(vgl_point_2d);


