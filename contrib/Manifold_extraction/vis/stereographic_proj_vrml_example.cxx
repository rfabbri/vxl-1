//  This is under contrib\Manifold_extraction\vis
// Brief: An example for generating vrml file for stereographic projections of geodesics from S(3) onto S(2)


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>


vnl_matrix<vcl_complex<double> > matrix_exp_SU2 (double t,vnl_matrix<vcl_complex<double> > g)
    {
vnl_matrix<vcl_complex<double> >I(2,2,0.0);
I.put(0,0,1);
I.put(1,1,1);

vnl_matrix<vcl_complex<double> >resultant(2,2,0.0);

if (t == 0)
    {
resultant.put(0,0,1);
resultant.put(1,1,1);
    }
else
    {
double x1 = t*g.get(0,0).imag();
double x2 = t*g.get(0,1).imag();
double x3 = t*(-g.get(0,1).real());

double phi = vcl_sqrt(vcl_pow(x1,2) + vcl_pow(x2,2) + vcl_pow(x3,2));

resultant = I*vcl_cos(phi) + g*vcl_sin(phi)*t/phi;
    }
return resultant;
    }



int main(int argc,char **argv)
    {
vcl_string vrml_file = argv[1];



int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);

    delete []my_argv;
vcl_ofstream out(vrml_file.c_str());

    out <<      "#VRML V2.0 utf8\n";
    out <<      "Background { \n";
    out <<      "    skyColor [ 1 1 1 ]\n";
    out <<      "    groundColor [ 1 1 1 ]\n";
    out <<      "}\n";
    out <<      "PointLight {\n";
    out <<      "    on FALSE\n";
    out <<      "    intensity 1 \n";
    out <<      "ambientIntensity 0 \n";
    out <<      "color 1 1 1 \n";
    out <<      "location 0 0 0 \n";
    out <<      "attenuation 1 0 0 \n";
    out <<      "radius 100  \n";
    out <<      "}\n";
    out <<      "Shape {\n";
    out <<      "   #make the points white\n";
    out <<      "    appearance Appearance {\n";
    out <<      "       material Material { emissiveColor 1 0 0 }\n";
    out <<      "   } \n";
    out <<      "   geometry PointSet {\n";
    out <<      "      coord Coordinate{\n";
    out <<      "         point[\n";
   
    
    

    // generate a set of elements along a geodesic on the manifold by finding 
    // exp(A*t) where A is a Lie algebra element and t is a continuous valued
    // variable 


        vnl_matrix<double> r1(2,2,0.0);
        vnl_matrix<double> i1(2,2,0.0);
        vnl_matrix<double> r2(2,2,0.0);
        vnl_matrix<double> i2(2,2,0.0);
         vnl_matrix<double> r3(2,2,0.0);
        vnl_matrix<double> i3(2,2,0.0);

        vnl_vector_fixed<double,2> i1_row1(0.0,1);
        vnl_vector_fixed<double,2> i1_row2(1,0.0);

        vnl_vector_fixed<double,2> r2_row1(0.0,1);
        vnl_vector_fixed<double,2> r2_row2(-1,0.0);

         vnl_vector_fixed<double,2> i3_row1(1,0.0);
        vnl_vector_fixed<double,2> i3_row2(0.0,-1);

        i1.set_row(0,i1_row1);
        i1.set_row(1,i1_row2);

        r2.set_row(0,r2_row1);
        r2.set_row(1,r2_row2);

        i3.set_row(0,i3_row1);
        i3.set_row(1,i3_row2);

       
// g1,g2 and g3 represent the generators of the su(2) Lie algebra
        vnl_matrix<vcl_complex<double> > g1 = 
  vnl_complexify(r1,i1);

 //       vcl_cout << g1 << vcl_endl;

        vnl_matrix<vcl_complex<double> > g2 = 
  vnl_complexify(r2,i2);

 //       vcl_cout << g2 << vcl_endl;

        vnl_matrix<vcl_complex<double> > g3 = 
  vnl_complexify(r3,i3);

 //       vcl_cout << g3 << vcl_endl;


// vnl_matrix<vcl_complex<double> > g = g1*1 + g2*(0) + g3*(0);

         double q0,q1,q2,q3,x,y,z,min_x = 1e10,max_x = 1e-10,min_y = 1e10,max_y = 1e-10,min_z = 1e10,max_z = 1e-10;
  vcl_vector<vgl_point_3d<double> >coordinates;

        for (unsigned int k = 0;k<=10;k = k + 1)
            {
        vnl_matrix<vcl_complex<double> > g = g1  + g2*k ;
        double step_size = 0.01,count = 0;

 for (double t = 0;t<=4*vnl_math::pi ;t += step_size)
        {
  count++;

vnl_matrix<vcl_complex<double> >G = matrix_exp_SU2(t,g);



q0 = G.get(0,0).real();
q1 = G.get(0,0).imag();
q2 = G.get(0,1).real();
q3 = G.get(0,1).imag();
//q0 = vcl_cos(t/2);
//q1 = 0;
//q2 = 0;
//q3 = vcl_sin(t/2);



if (q0 + q1 + q2 + q3 != 2)
    {
x = 1+((2*q1-1)/(2-q0-q1-q2-q3));
y = 1+((2*q2-1)/(2-q0-q1-q2-q3));
z = 1+((2*q3-1)/(2-q0-q1-q2-q3));
   /* x = cos(t);
    y = 0;
    z = sin(t);*/
        
out << x << " " << y <<" " << z << vcl_endl;

    }
        }
            }
     out <<      "         ]\n";
    out <<      "      }\n";
    out <<      "      color Color { color [ 1 1 1 ] }\n";
    out <<      "   }\n";
    out <<      "}\n";
  
  return 0;
    }


