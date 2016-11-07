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

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vehicle_model.h>
#include <vehicle_model_3d.h>
#include <vul/vul_file_iterator.h>

void read_box(vcl_istream &s,vgl_box_3d<double>& b_box)
    {
    vcl_string str;
    char ch;
    double min_x,min_y,min_z,max_x,max_y,max_z;

    s >> str; 
    s >> min_x >> ch >> min_y >> ch >> min_z >> str
        >> max_x >> ch >> max_y >> ch >> max_z >> ch;

    b_box.set_min_x(min_x);
    b_box.set_min_y(min_y);
    b_box.set_min_z(min_z);

    b_box.set_max_x(max_x);
    b_box.set_max_y(max_y);
    b_box.set_max_z(max_z);
    }


vcl_vector<vehicle_model_3d> read_3d_models(vcl_string bounding_boxes_info,vcl_string file_ext)
    {
    vcl_string b_box_type_part1,b_box_type_part2,b_box_type_part3,f_name;
    vcl_vector<vehicle_model_3d> vehic_mod_vec;

    vgl_box_3d<double> b_box,b_box_hood,b_box_cab, b_box_bed;

    for (vul_file_iterator fn= bounding_boxes_info + file_ext; fn; ++fn) {
        f_name =  fn();
        vcl_ifstream ifstr(f_name.c_str());

        for (unsigned int i = 0;i<3;i++)
            {
            ifstr >> b_box_type_part1 >> b_box_type_part2 >> b_box_type_part3;
            /* read_box(ifstr,b_box);

            vcl_cout << b_box << vcl_endl; */


            if (strcmp(b_box_type_part3.c_str(),"hood:") == 0)
                read_box(ifstr,b_box_hood);


            if (strcmp(b_box_type_part3.c_str(),"cab:") == 0)
                read_box(ifstr,b_box_cab);

            if (strcmp(b_box_type_part3.c_str(),"bed:") == 0)
                read_box(ifstr,b_box_bed);
            }


        if (!(b_box_hood.is_empty()) && !(b_box_cab.is_empty()) && !(b_box_hood.is_empty()))
            {
            vehicle_model_3d M(b_box_hood,b_box_cab,b_box_bed);
            vehic_mod_vec.push_back(M);
            }
        }
return vehic_mod_vec;
    }


int main(int argc,char **argv)
    {

    vgui::init(argc, argv);

vgui_dialog load_dlg("load the files");

  static vcl_string car_info = "", ext_car = " ";
  static vcl_string new_car_info = "", ext_new_car = " ";
  static vcl_string truck_info = "", ext_truck = " ";

  static vcl_string intrinsic_mean_info = "", ext_mn = "*.mn";
  static vcl_string generator_coeffs_category_one = "", ext_gc_1 = "*.gc";
  static vcl_string generator_coeffs_category_two = "", ext_gc_2 = "*.gc";
  static vcl_string vrml_file = "", ext_wrl = "*.wrl";

  load_dlg.file("car info",ext_car,car_info);
  load_dlg.file("New cars info",ext_new_car,new_car_info);
  load_dlg.file("Trucks info",ext_truck,truck_info);
  load_dlg.file("intrinsic mean info",ext_mn,intrinsic_mean_info);
  load_dlg.file("generator coefficients",ext_gc_1,generator_coeffs_category_one);
  load_dlg.file("vrml file",ext_wrl, vrml_file);

  if(!load_dlg.ask())
    return 0;


  /*  vcl_string vehicle_model_info = argv[1];
    vcl_string intrinsic_mean_info = argv[2];
    vcl_string generator_coeffs = argv[3];
    vcl_string vrml_file = argv[4];*/


    // vcl_string three_box_geodesics = argv[5];

    // vcl_ifstream ifst_box(three_box_geodesics.c_str());

    //usage: 

    //car_info  is the file containing the bounding boxes fitted to the hood,cab and the
    //bed respectively

    //intrinsic_mean_info is the file holding the transformation matrices which would transform the 
    //reference model to the intrinsic mean

    //generator_coeffs is the file holding the coefficients of the generators obtained by 
    //performing PCA on the algebra elements

    //vrml_file is the output vrml file which has the vehicles plotted as spheres in 3d and the
    //curves in 3d representing the principal geodesics

    vcl_string part;
    double diag_hood_mean,diag_cab_mean,diag_bed_mean;
    double diag_hood,diag_cab,diag_bed;
    double val1,val2,val3,val4,val5,val6,val7,val8,val9,num_geodesics = 10;

    vcl_vector<vehicle_model_3d> car_vec = read_3d_models(car_info,"\\*.txt");
    vcl_vector<vehicle_model_3d> new_car_vec = read_3d_models(new_car_info,"\\*.txt");
    vcl_vector<vehicle_model_3d> truck_vec = read_3d_models(truck_info,"\\*.txt");


    vcl_ifstream ifstr(intrinsic_mean_info.c_str());

    vnl_matrix<double > mean_hood_tr(4,4,0.0),mean_cab_tr(4,4,0.0),mean_bed_tr(4,4,0.0);

    ifstr >> part;
    ifstr >> mean_hood_tr;
    ifstr >> part;
    ifstr >> mean_cab_tr;
    ifstr >> part;
    ifstr >> mean_bed_tr;

    vcl_cout << mean_hood_tr << vcl_endl;
    vcl_cout << mean_cab_tr << vcl_endl;
    vcl_cout << mean_bed_tr << vcl_endl;

    ifstr.close();

    vcl_vector<double>sx_hood,sy_hood,sz_hood,sx_cab,sy_cab,sz_cab,sx_bed,sy_bed,sz_bed;

    vcl_ifstream geo_str(generator_coeffs_category_one.c_str());

    for (unsigned int i = 0;i<num_geodesics;i++)
        {
        geo_str >>val1 >> val2 >> val3 >> val4 >> val5 >> val6 >> val7 >> val8 >> val9;

        sx_hood.push_back(val1);
        sy_hood.push_back(val2);
        sz_hood.push_back(val3);

        sx_cab.push_back(val4);
        sy_cab.push_back(val5);
        sz_cab.push_back(val6);

        sx_bed.push_back(val7);
        sy_bed.push_back(val8);
        sz_bed.push_back(val9);
        }

    vcl_vector<vnl_matrix<double> >gen_hood_vec,gen_cab_vec,gen_bed_vec;

    for (unsigned int i = 0;i<num_geodesics;i++)
        {
        vnl_matrix<double> G1(4,4,0.0),G2(4,4,0.0),G3(4,4,0.0);

        G1.put(0,0,sx_hood[i]);
        G1.put(1,1,sy_hood[i]);
        G1.put(2,2,sz_hood[i]);

        G2.put(0,0,sx_cab[i]);
        G2.put(1,1,sy_cab[i]);
        G2.put(2,2,sz_cab[i]);

        G3.put(0,0,sx_bed[i]);
        G3.put(1,1,sy_bed[i]);
        G3.put(2,2,sz_bed[i]);

        gen_hood_vec.push_back(G1);
        gen_cab_vec.push_back(G2);
        gen_bed_vec.push_back(G3);

        }





    //set the reference model as the intrinsic mean model

    vehicle_model_3d mean_model(car_vec[0].hood(),car_vec[0].cab(),car_vec[0].bed());

    mean_model.transform_model(mean_hood_tr,mean_cab_tr,mean_bed_tr);

    diag_hood_mean = vcl_pow((mean_model.hood().height()),2) + vcl_pow((mean_model.hood().width()),2) + vcl_pow((mean_model.hood().depth()),2);
    diag_cab_mean = vcl_pow((mean_model.cab().height()),2) + vcl_pow((mean_model.cab().width()),2) + vcl_pow((mean_model.cab().depth()),2);
    diag_bed_mean = vcl_pow((mean_model.bed().height()),2) + vcl_pow((mean_model.bed().width()),2) + vcl_pow((mean_model.bed().depth()),2);

    int num_car_models = car_vec.size();
    int num_new_car_models = new_car_vec.size();
    int num_truck_models = truck_vec.size();

    vcl_vector<double> s1,s2,s3,ns1,ns2,ns3,ts1,ts2,ts3;

    for (unsigned int i = 0;i<num_car_models;i++)
        {
        diag_hood = vcl_pow((car_vec[i].hood().height()),2) + vcl_pow((car_vec[i].hood().width()),2)+
            vcl_pow((car_vec[i].hood().depth()),2);

        diag_cab = vcl_pow((car_vec[i].cab().height()),2) + vcl_pow((car_vec[i].cab().width()),2)+
            vcl_pow((car_vec[i].cab().depth()),2);

        diag_bed = vcl_pow((car_vec[i].bed().height()),2) + vcl_pow((car_vec[i].bed().width()),2)+
            vcl_pow((car_vec[i].bed().depth()),2);

        s1.push_back(diag_hood/diag_hood_mean);
        s2.push_back(diag_cab/diag_cab_mean);
        s3.push_back(diag_bed/diag_bed_mean);
        }

       for (unsigned int i = 0;i<num_new_car_models;i++)
        {
        diag_hood = vcl_pow((new_car_vec[i].hood().height()),2) + vcl_pow((new_car_vec[i].hood().width()),2)+
            vcl_pow((new_car_vec[i].hood().depth()),2);

        diag_cab = vcl_pow((new_car_vec[i].cab().height()),2) + vcl_pow((new_car_vec[i].cab().width()),2)+
            vcl_pow((new_car_vec[i].cab().depth()),2);

        diag_bed = vcl_pow((new_car_vec[i].bed().height()),2) + vcl_pow((new_car_vec[i].bed().width()),2)+
            vcl_pow((new_car_vec[i].bed().depth()),2);

        ns1.push_back(diag_hood/diag_hood_mean);
        ns2.push_back(diag_cab/diag_cab_mean);
        ns3.push_back(diag_bed/diag_bed_mean);
        }

       for (unsigned int i = 0;i<num_truck_models;i++)
        {
        diag_hood = vcl_pow((truck_vec[i].hood().height()),2) + vcl_pow((truck_vec[i].hood().width()),2)+
            vcl_pow((truck_vec[i].hood().depth()),2);

        diag_cab = vcl_pow((truck_vec[i].cab().height()),2) + vcl_pow((truck_vec[i].cab().width()),2)+
            vcl_pow((truck_vec[i].cab().depth()),2);

        diag_bed = vcl_pow((truck_vec[i].bed().height()),2) + vcl_pow((truck_vec[i].bed().width()),2)+
            vcl_pow((truck_vec[i].bed().depth()),2);

        ts1.push_back(diag_hood/diag_hood_mean);
        ts2.push_back(diag_cab/diag_cab_mean);
        ts3.push_back(diag_bed/diag_bed_mean);
        }
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
    out <<      "radius 0.02  \n";
    out <<      "}\n";

    //write down all the spheres corresponding to the cars

    for (i = 0;i<num_car_models;i++)
        {
        out <<      " Transform { \n";
        out <<      "translation" << " " <<s1[i] << " " << s2[i] << " " << s3[i] << "\n";
        out << "children [ \n";
        out << "Shape { \n";
        out << " appearance Appearance{ \n";
        out << "   material Material \n";
        out << "    { \n";
        // out << "      diffuseColor 1 " <<double(num_car_models-i)/double(num_car_models)<<" "<<double(i)/double(num_car_models) <<" \n";
        out << "      diffuseColor 1 0 0 \n";
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

    //write down all the spheres corresponding to new cars

 for (i = 0;i<num_new_car_models;i++)
        {
        out <<      " Transform { \n";
        out <<      "translation" << " " <<ns1[i] << " " << ns2[i] << " " << ns3[i] << "\n";
        out << "children [ \n";
        out << "Shape { \n";
        out << " appearance Appearance{ \n";
        out << "   material Material \n";
        out << "    { \n";
        // out << "      diffuseColor 1 " <<double(num_models-i)/double(num_models)<<" "<<double(i)/double(num_models) <<" \n";
        out << "      diffuseColor 0 0 1 \n";
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

    //write down all the spheres corresponding to trucks

 for (i = 0;i<num_truck_models;i++)
        {
        out <<      " Transform { \n";
        out <<      "translation" << " " <<ts1[i] << " " << ts2[i] << " " << ts3[i] << "\n";
        out << "children [ \n";
        out << "Shape { \n";
        out << " appearance Appearance{ \n";
        out << "   material Material \n";
        out << "    { \n";
        // out << "      diffuseColor 1 " <<double(num_models-i)/double(num_models)<<" "<<double(i)/double(num_models) <<" \n";
        out << "      diffuseColor 0 1 0 \n";
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

    //generate sample points along the geodesics and write them down
    for (unsigned int it = 0;it<5;it++)
        {
         vcl_vector<double> pt1,pt2,pt3;
    double diag_hood_M,diag_cab_M,diag_bed_M,t;
    vnl_matrix<double> S_hood,S_cab,S_bed;

        for (t = -3;t <= 3;t = t+0.01)
            {
            S_hood = get_Lie_group_3d(t*gen_hood_vec[it]);
            S_cab = get_Lie_group_3d(t*gen_cab_vec[it]);
            S_bed = get_Lie_group_3d(t*gen_bed_vec[it]);

            vehicle_model_3d M(car_vec[0].hood(),car_vec[0].cab(),car_vec[0].bed());
            M.transform_model(S_hood,S_cab,S_bed);

           // ifst_box << print_vrml(M) << vcl_endl;

            diag_hood_M = vcl_pow((M.hood().height()),2) + vcl_pow((M.hood().width()),2) + vcl_pow((M.hood().depth()),2);
            diag_cab_M = vcl_pow((M.cab().height()),2) + vcl_pow((M.cab().width()),2) + vcl_pow((M.cab().depth()),2);
            diag_bed_M = vcl_pow((M.bed().height()),2) + vcl_pow((M.bed().width()),2) + vcl_pow((M.bed().depth()),2);


            pt1.push_back(diag_hood_M/diag_hood_mean);
            pt2.push_back(diag_cab_M/diag_cab_mean);
            pt3.push_back(diag_bed_M/diag_bed_mean);
            }

        out << "Transform { \n";
        out << "translation 0 0  0 \n";
        out << " children [ \n";
        out << "Shape { \n";
        out << " appearance Appearance{ \n";
        out << " material Material \n";
        out << "    { \n";
        out << "      diffuseColor 1 " <<0.2*it <<" "<< 1-0.2*it <<" \n";
        out << "      emissiveColor 1 " <<0.2*it <<" "<< 1-0.2*it <<" \n";
        // out << "      diffuseColor 0.25 0.25 0.25 \n";
       // out << "      emissiveColor 0.25 0.25 0.25 \n";
        out << "    } \n";
        out << "  } \n";
        out << " geometry IndexedLineSet \n";
        out << "{ \n";
        out << "      coord Coordinate{ \n";
        out << "       point[ \n";


        for (unsigned int k=0;k<pt1.size();k++)
            {   

            out <<" " << pt1[k] <<" " << pt2[k]  << " " <<pt3[k]  << vcl_endl;
            }

        out <<"    ] \n";
        out <<"  }   coordIndex [ \n";

        for (i = 0;i<pt1.size();i++)
            {
            out << i << ",";
            }
        out << "-1  ] \n";
        out << "  } \n";
        out <<"} ] \n";
        out <<"} \n";

        }

    out.close();

    return 0;

    }


