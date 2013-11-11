//  This is under contrib\Manifold_extraction\vis
// Brief: this generates three box models along the principal geodesics and 
//stores the moedls in a vrml file so that all the models can be seen in an animation


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
#include <vehicle_model.h>
#include <Lie_group_operations.h>
#include <vrml_print.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

vcl_vector<vehicle_model> read_models(vcl_string vehicle_model_info,int num_models)
    {
    int i,j,k;
    double x,y;
    char ch;
    vcl_ifstream ifst(vehicle_model_info.c_str());
    vcl_vector<vehicle_model> model_vec;
    vcl_vector<vsol_rectangle_2d> box1,box2,box3;

    vsol_point_2d p0,p1,p2,p3;

    // vgl_point_2d<double> pt1box1,pt2box1,pt1box2,pt2box2,pt1box3,pt2box3;

    for (i =0;i<num_models;i++)
        {
        for (j =0;j<3;j++)
            {  
            for (k = 0;k<4;k++)
                {
                // ifst >> x >> ch >> y;
                ifst >> x >> y;
                if (k==0)
                    {
                    p0.set_x(x);
                    p0.set_y(y);
                    }
                if (k==1)
                    {
                    p1.set_x(x);
                    p1.set_y(y);
                    }
                if (k==2)
                    {
                    p2.set_x(x);
                    p2.set_y(y);
                    }
                if (k==3)
                    {
                    p3.set_x(x);
                    p3.set_y(y);
                    }
                }
            vsol_point_2d_sptr p0_sptr = new vsol_point_2d(p0);
            vsol_point_2d_sptr p1_sptr = new vsol_point_2d(p1);
            vsol_point_2d_sptr p2_sptr = new vsol_point_2d(p2);
            vsol_point_2d_sptr p3_sptr = new vsol_point_2d(p3);

            if (j == 0)
                {
                vsol_rectangle_2d_sptr box1_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box1.push_back(*box1_sptr);

                }
            if (j == 1)
                {
                vsol_rectangle_2d_sptr box2_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box2.push_back(*box2_sptr);
                }
            if (j == 2)
                {
                vsol_rectangle_2d_sptr box3_sptr = new vsol_rectangle_2d(p0_sptr,p1_sptr,p2_sptr,p3_sptr);
                box3.push_back(*box3_sptr);
                }


            }
        vehicle_model M(box1[i],box2[i],box3[i]);
        model_vec.push_back(M);
        }
    return model_vec;
    }


int main(int argc,char **argv)
    {
    vcl_string ref_model = argv[1];
    vcl_string generator_info = argv[2];
    vcl_string vrml_file = argv[3];
    vcl_string text_file = argv[4];

    //usage: 

    //ref_model is the three box reference model

    //generator_info is the file holding the elements of the generator for 
    //the principal geodesic

    //vrml_file is the output vrml file which has the three box models generated 
    //along the geodesic

    vcl_ifstream generator_info_read(generator_info.c_str());
    vcl_ofstream out (vrml_file.c_str());
    vcl_ofstream txt_file_out (text_file.c_str());

    vcl_vector<vehicle_model>vehic_model_vec = read_models(ref_model,1);

    vehicle_model geodesic_mod;

    vcl_vector<double>geodesics;
    int i,num_coeffs = 6,count;
    // some hard-coded values ...
    double t,x,k1 = 97.87 ,k2 = 208.54;

    for (i = 0;i<num_coeffs;i++)
        {
        generator_info_read >>  x ;
        geodesics.push_back(x);
        }

    vnl_matrix<double>m1(3,3,0.0);
    vnl_matrix<double>m2(3,3,0.0); 
    vnl_matrix<double>m3(3,3,0.0); 

   out <<" #VRML V2.0 utf8 \n";
   out <<"Background { \n";
   out <<" skyColor [ 1 1 1 ] \n";
   out <<" groundColor [ 1 1 1 ] \n";
   out <<"} \n";
   out << "PointLight { \n";
   out << " on FALSE \n";
   out << " intensity 1 \n";
   out << "ambientIntensity 0 \n";
   out << "color 1 1 1 \n";
   out << "location 0 0 0 \n";
   out << "attenuation 1 0 0 \n";
   out << "radius 100  \n";
    out << "} \n";

    print_vrml_model(out,vehic_model_vec[0],0.5);

      /*out << " DEF pi PositionInterpolator { \n";
      out << "   key [ ";*/

     /* count = 0;
      for (t = -1;t<=1;t = t+0.01)
          {
          out << count++ <<" ";
          }

      out << "]" ;*/

    double sxh,syh,sxc,syc,sxb,syb,tx,ty;
   
    // out << " keyValue [ \n";

    for (t = -1;t<=1;t = t+0.01)
        {
        sxh = vcl_exp(t*geodesics[0]);
        syh = vcl_exp(t*geodesics[1]);
        sxc = vcl_exp(t*geodesics[2]);
        syc = vcl_exp(t*geodesics[3]);
        sxb = vcl_exp(t*geodesics[4]);
        syb = vcl_exp(t*geodesics[5]);

        m1.put(0,0,sxh);
        m1.put(1,1,syh);
        m1.put(2,2,1);
        m1.put(0,2,k1*(sxc - sxh));

        m2.put(0,0,sxc);
        m2.put(1,1,syc);
        m2.put(2,2,1);

        m3.put(0,0,sxb);
        m3.put(1,1,syb);
        m3.put(2,2,1);
        m3.put(0,2,k2*(sxc - sxb));

        vehicle_model geodesic_mod(vehic_model_vec[0]);

        geodesic_mod.transform_model(m1,m2,m3);

        // out << m1.get(0,2) << " " <<  m1.get(0,2) << " " <<  m1.get(0,2) << "," << vcl_endl;
       
txt_file_out << geodesic_mod;
        }
 out << "] \n";
        out << "    } \n";

        out.close();
 
 //out << "     DEF timer TimeSensor { \n";
 //out << "        cycleInterval 10 \n";
 //out << "            loop TRUE \n";
 //out << "        } \n";

 //out << " ROUTE timer.fraction_changed TO pi.set_fraction \n";
 //out << " ROUTE pi.value_changed TO model_shift.set_translation \n";

    return 0;

    }


