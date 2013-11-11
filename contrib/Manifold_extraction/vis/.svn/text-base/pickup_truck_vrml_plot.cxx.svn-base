//  This is under contrib\Manifold_extraction\vis

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
    vcl_string vehicle_model_info = argv[1];
    vcl_string intrinsic_mean_info = argv[2];
    vcl_string vrml_file = argv[3];
    vcl_string text_file = argv[4];

   /* int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);

    delete []my_argv;*/

    int i,num_models = 7;

    double diag_engine_ref,diag_body_ref,diag_rear_ref;
    double diag_engine,diag_body,diag_rear;

    vcl_vector<vehicle_model> model_vec = read_models(vehicle_model_info,num_models);
    vcl_vector<vehicle_model> intrinsic_mean_model = read_models(intrinsic_mean_info,1);

    //set the reference model as the intrinsic mean model
    //vehicle_model ref_model(model_vec[0].engine(),model_vec[0].body(),model_vec[0].rear());
     vehicle_model ref_model(intrinsic_mean_model[0].engine(),intrinsic_mean_model[0].body(),intrinsic_mean_model[0].rear());

    diag_engine_ref = vcl_pow((ref_model.engine().height()),2) + vcl_pow((ref_model.engine().width()),2);
    diag_body_ref = vcl_pow((ref_model.body().height()),2) + vcl_pow((ref_model.body().width()),2);
    diag_rear_ref = vcl_pow((ref_model.rear().height()),2) + vcl_pow((ref_model.rear().width()),2);


    vcl_vector<double> s1,s2,s3;

    for (i = 0;i<num_models;i++)
        {
        diag_engine = vcl_pow((model_vec[i].engine().height()),2) + vcl_pow((model_vec[i].engine().width()),2);
        diag_body = vcl_pow((model_vec[i].body().height()),2) + vcl_pow((model_vec[i].body().width()),2);
        diag_rear = vcl_pow((model_vec[i].rear().height()),2) + vcl_pow((model_vec[i].rear().width()),2);

        s1.push_back(diag_engine/diag_engine_ref);
        s2.push_back(diag_body/diag_body_ref);
        s3.push_back(diag_rear/diag_rear_ref);
        }

    vcl_ofstream out(vrml_file.c_str());
    vcl_ofstream text_file_info(text_file.c_str());

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
    
//write down all the spheres corresponding to the trucks

for (i = 0;i<num_models;i++)
    {
    out <<      " Transform { \n";
out <<      "translation" << " " <<s1[i] << " " << s2[i] << " " << s3[i] << "\n";
// text_file_info << s1[i] << " " << s2[i] << " " << s3[i] << "\n";
out << "children [ \n";
out << "Shape { \n";
out << " appearance Appearance{ \n";
out << "   material Material \n";
out << "    { \n";
out << "      diffuseColor 1 " <<double(num_models-i)/double(num_models)<<" "<<double(i)/double(num_models) <<" \n";
out << "      transparency 0 \n";
out << "    } \n";
out << "  } \n";
out << " geometry Sphere \n";
out << "{ \n";
out << "  radius 0.02 \n";
out << "   } \n";
out << "  } \n";
out << " ] \n";
out << "} \n";

    }

//out << "Transform { \n";
//out << "translation 0 0  0 \n";
//out << " children [ \n";
//out << "Shape { \n";
//out << " appearance Appearance{ \n";
//out << " material Material \n";
//out << "    { \n";
//out << "      diffuseColor 0.25 0.25 0.25 \n";
//out << "      emissiveColor 0.25 0.25 0.25 \n";
//out << "    } \n";
//out << "  } \n";
//out << " geometry IndexedLineSet \n";
//out << "{ \n";
//out << "      coord Coordinate{ \n";
//out << "       point[ \n";
//
//double s_b1,s_b2,s_b3,k,s = 1;
////write down the principal geodesic 
//double num_points = 0;
//for (k=-3;k<3;k = k+0.01)
//    {   
//s_b1 = k*0.6589;
//s_b2 = k*0.3116;
//s_b3 = k*0.6847;
//out <<" " << s_b1<<" " << s_b2 << " " <<s_b3 << vcl_endl;
//num_points++;
//    }
//
//out <<"    ] \n";
//out <<"  }   coordIndex [ \n";
//
//for (i = 0;i<num_points;i++)
//    {
//out << i << ",";
//    }
//out << "-1  ] \n";
//out << "  } \n";
//out <<"} ] \n";
//out <<"} \n";


    
out.close();
text_file_info.close();

return 0;

    }


