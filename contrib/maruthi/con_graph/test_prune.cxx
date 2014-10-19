// dbgl headers
//#include <dbskfg/pro/dbskfg_prune_fragments_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vcl_fstream.h>
#include <dbskfg/pro/dbskfg_load_binary_composite_graph_process.h>

#include <bsta/bsta_spherical_histogram.h>

#include <dbdet/algo/dbdet_load_edg.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vil/vil_transpose.h>
#include <vl/mathop.h>
#include <vl/sift.h>
#include <string.h>
#include <vgl/vgl_box_2d.h>

#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vgl/vgl_polygon.h>

void write_distance_matrix(
    double size,
    double* compressed_distance_matrix,
    vcl_string output_file_path);

void transpose_descriptor (vl_sift_pix* dst, vl_sift_pix* src);

void write_distance_matrix(
    double size,
    double* compressed_distance_matrix,
    vcl_string output_file_path)
{
    vcl_ofstream output_binary_file;
    output_binary_file.open(output_file_path.c_str(),
                            vcl_ios::out |
                            vcl_ios::app |
                            vcl_ios::binary);

    output_binary_file.write(reinterpret_cast<char *>(&size),
                             sizeof(double));
    
    output_binary_file.write(
        reinterpret_cast<char *>(compressed_distance_matrix),
        sizeof(double)*size);

    output_binary_file.close();


}



 void transpose_descriptor (vl_sift_pix* dst, vl_sift_pix* src)
    {
        int const BO = 8 ;  /* number of orientation bins */
        int const BP = 4 ;  /* number of spatial bins     */
        int i, j, t ;
        
        for (j = 0 ; j < BP ; ++j) {
            int jp = BP - 1 - j ;
            for (i = 0 ; i < BP ; ++i) {
                int o  = BO * i + BP*BO * j  ;
                int op = BO * i + BP*BO * jp ;
                dst [op] = src[o] ;
                for (t = 1 ; t < BO ; ++t)
                    dst [BO - t + op] = src [t + o] ;
            }
        }
    }

int main( int argc, char *argv[] )
{

    vcl_stringstream stream(argv[1]);
    vcl_string input_img;
    stream>>input_img;

    bool status=true;

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(input_img.c_str());

    // Create vid pro storage
    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(img_sptr);


    // Create output storage for edge detection
    vcl_vector<bpro1_storage_sptr> edge_det_results;


    dbdet_third_order_color_edge_detector_process pro_color_edg;

    // Before we start the process lets clean input output
    pro_color_edg.clear_input();
    pro_color_edg.clear_output();

    pro_color_edg.add_input(inp);
    bool to_c_status = pro_color_edg.execute();
    pro_color_edg.finish();

    // Grab output from color third order edge detection
    // if process did not fail
    if ( to_c_status )
    {
        edge_det_results = pro_color_edg.get_output();
    }

    //Clean up after ourselves
    pro_color_edg.clear_input();
    pro_color_edg.clear_output();




    
    dbdet_save_edg_process save_edg_pro;

    vcl_string output_file="";
    {
        vcl_string final_name=vul_file::strip_extension(
            vul_file::strip_directory(input_img));
        vcl_string orig_directory=vul_file::dirname(input_img);
        
        vcl_string final_directory=orig_directory;
        vcl_string replacer="Cropped";
        final_directory.replace(33,42-33,replacer);
        output_file=final_directory + "/" + final_name + "_to.edg";
        
        //output_file=vul_file::strip_extension(input_img) +"_to.edg";
    }

    vcl_cout<<output_file<<vcl_endl;

    bpro1_filepath output(output_file,"_to.edg");
    vcl_string bbox_file=vul_file::strip_extension(input_img)+"_bbox.txt";

    vcl_ifstream istream(bbox_file.c_str());
    
    double x(0.0),y(0.0),width(0.0),height(0.0);

    istream>>x;
    istream>>y;
    istream>>width;
    istream>>height;

    istream.close();

    double xmin=x; double xmax=x+width;
    double ymin=y; double ymax=y+height;

    vgl_box_2d<double> bbox(xmin,xmax,ymin,ymax);
    vcl_cout<<" x: "<<x<<" y: "<<y<<" width: "<<width<<" height: "
            <<height<<vcl_endl;




    dbdet_edgemap_storage_sptr input_edgemap;
    input_edgemap.vertical_cast(edge_det_results[0]);
    dbdet_edgemap_sptr EM = input_edgemap->get_edgemap();

    //create a new edgemap from the tokens collected from NMS
    dbdet_edgemap_sptr bbox_edge_map = new dbdet_edgemap(width+1, 
                                                         height+1);
    

    vcl_vector<dbdet_edgel*> orig_edges=EM->edgels;
    for  ( unsigned int e=0; e < orig_edges.size() ; ++e)
    {
        vgl_point_2d<double> location=orig_edges[e]->pt;

        if ( bbox.contains(location))
        {
            vgl_point_2d<double> new_location(
                location.x()-xmin,location.y()-ymin);
            
            bbox_edge_map->
                insert(new dbdet_edgel(new_location,
                                       orig_edges[e]->tangent,
                                       orig_edges[e]->strength,
                                       orig_edges[e]->deriv));

            
        }

    }
    
    bool retval = dbdet_save_edg(output_file, bbox_edge_map);

    // save_edg_pro.parameters()->set_value("-edgoutput",output);

    // // Before we start the process lets clean input output
    // save_edg_pro.clear_input();
    // save_edg_pro.clear_output();

    // save_edg_pro.add_input(edge_det_results[0]);
    // status = save_edg_pro.execute();
    // save_edg_pro.finish();

    // //Clean up after ourselves
    // save_edg_pro.clear_input();
    // save_edg_pro.clear_output();


    // vcl_stringstream stream(argv[1]);
    // vcl_string input_img;
    // stream>>input_img;

    //test default constructor
    // bsta_spherical_histogram<double> dsh;
    // //test default units and coordinates
    // bsta_spherical_histogram<double> sh(8, 4, 0.0, 360.0, 0.0, 180.0,
    //                                     bsta_spherical_histogram<double>
    //                                     ::DEG,
    //                                     bsta_spherical_histogram<double>
    //                                     ::B_0_360,
    //                                     bsta_spherical_histogram<double>
    //                                     ::B_0_180);


    // double az(0.0),el(0.0);
    // sh.convert_to_spherical(0,0,1,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,0,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,0,-1,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;


    // sh.convert_to_spherical(1,0,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(-1,0,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,1,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,-1,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,0,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(0,1,1,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(1,0,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(1,0,1,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(1,1,0,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // sh.convert_to_spherical(1,1,1,az, el);
    // sh.upcount(az,el);
    // vcl_cout<<az<<","<<el<<vcl_endl;

    // vcl_ofstream file("sphere.vrml");
    // sh.print_to_vrml(file,0.5);
    // file.close();
    // vil_image_view<vxl_byte> src_image = vil_load("black_white.png");

    // vil_image_view<vxl_byte> temp2=
    //     vil_transpose(src_image);
    
    // vil_image_view<double> image;
    // vil_convert_cast(temp2,image);

    // unsigned int width  = image.ni();
    // unsigned int height = image.nj();

    // double* gradient_magnitude = (double*) 
    //     vl_malloc(width*height*sizeof(double));
    // double* gradient_angle     = (double*) 
    //     vl_malloc(width*height*sizeof(double));

    // double* smoothed_data     = (double*) 
    //     vl_malloc(width*height*sizeof(double));
    // double* image_data=image.top_left_ptr();

    // vl_imsmooth_d(
    //     smoothed_data,      // smoothed image
    //     1,                  // smoothed stride
    //     image_data,         // input image
    //     width,              // input image width
    //     height,             // input image height
    //     width,              // input image width
    //     height,             // input image height
    //     width);             // input image stride
    
    // vl_imgradient_polar_d(
    //     gradient_magnitude, // gradient magnitude 
    //     gradient_angle,     // gradient angle
    //     1,                  // output width
    //     width,              // output height
    //     image_data,         // input image
    //     width,              // input image width
    //     height,             // input image height
    //     width);             // input image stride

    // VlSiftFilt* filter = vl_sift_new(width,height,3,3,0);
    // vl_sift_pix* grad_data=(vl_sift_pix*) 
    //     vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    // unsigned int index=0;
    // for ( unsigned int i=0; i < width*height; ++i)
    // {
    //     double value=gradient_magnitude[i];
    //     (grad_data)[index]=value;
    //     ++index;
    // }

    // for ( unsigned int i=0; i < width*height; ++i)
    // {
    //     double value=gradient_angle[i];
    //     (grad_data)[index]=value;
    //     ++index;
    // }

    // vl_free(gradient_magnitude);
    // vl_free(gradient_angle);

    // vl_sift_pix temp_ps1[128];
    // vl_sift_pix descr_ps1[128];
    // memset(temp_ps1, 0, sizeof(vl_sift_pix)*128);
    // memset(descr_ps1, 0, sizeof(vl_sift_pix)*128);

    // double angle=0;

    // vl_sift_calc_raw_descriptor(filter,
    //                             grad_data,
    //                             temp_ps1,
    //                             filter->width,
    //                             filter->height,
    //                             100, 
    //                             50.5,
    //                             5,
    //                             0);//fmod(angle+M_PI/2,2*M_PI));
    
    // vcl_vector<vl_sift_pix> model_sift;
    // //transpose_descriptor(descr_ps1,temp_ps1);
    // //model_sift.assign(descr_ps1,descr_ps1+128);
    // model_sift.assign(temp_ps1,temp_ps1+128);

    // vcl_ofstream model_stream("Model_sift_app_correspondence.txt");
    // for (unsigned int i=0; i < model_sift.size() ; ++i)
    // {
    //     model_stream<<model_sift[i]<<vcl_endl;
    // }
    // model_stream.close();



    // Grab image
    // vil_image_resource_sptr img_sptr = 
    //     vil_load_image_resource(input_img.c_str());
    // if (!img_sptr) 
    // {
    //     vcl_cerr << "Cannot load image: " << input_img << vcl_endl;
    //     return 1;
    // }

    // // Create vid pro storage
    // vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    // inp->set_image(img_sptr);

    // // Create storage
    // vcl_vector<bpro1_storage_sptr> ct_results;
    // {
    //     vcl_cout<<"************ Contour Tracing  ************"<<vcl_endl;

    //     dbdet_contour_tracer_process ct_pro;
    //     // set_process_parameters_of_bpro1(*params, 
    //     //                                 ct_pro, 
    //     //                                 params->tag_contour_tracing_);
        
    //     // Before we start the process lets clean input output
    //     ct_pro.clear_input();
    //     ct_pro.clear_output();

    //     // Start the process sequence
    //     ct_pro.add_input(inp);
    //     bool ct_status = ct_pro.execute();
    //     ct_pro.finish();

    //     // Grab output from gray scale third order edge detection
    //     // if process did not fail
    //     if ( ct_status )
    //     {
    //         ct_results = ct_pro.get_output();
    //     }

    //     //Clean up after ourselves
    //     ct_pro.clear_input();
    //     ct_pro.clear_output();

    //     if (ct_results.size() != 1 )
    //     {
    //         vcl_cerr<< "Contour tracing failed"<<vcl_endl;
    //         return 1;
    //     }

    // }
    
    // vidpro1_vsol2D_storage_sptr input_vsol;
    // input_vsol.vertical_cast(ct_results[0]);

    // vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
    //     input_vsol->all_data();

    // vsol_polygon_2d_sptr new_poly;

    // for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    // {
    //     if( vsol_list[b]->cast_to_region()->cast_to_polygon())
    //     {
    //         vsol_polygon_2d_sptr poly = 
    //             vsol_list[b]->cast_to_region()->cast_to_polygon();
   
    //         vgl_fit_lines_2d<double> fitter;
    //         fitter.set_min_fit_length(2);
    //         fitter.set_rms_error_tol(0.05f);
    //         for (unsigned int i = 0; i<poly->size(); i++) {
    //             vgl_point_2d<double> p = poly->vertex(i)->get_p();
    //             fitter.add_point(p);
    //         }
    //         fitter.fit();
 
    //         vcl_vector<vgl_line_segment_2d<double> >& segs = 
    //             fitter.get_line_segs();

    //         vcl_vector<vsol_point_2d_sptr > new_pts;
    //         new_pts.push_back(
    //             new 
    //             vsol_point_2d(segs[0]
    //                           .point1().x(),segs[0].point1().y()));
    //         new_pts.push_back(
    //             new 
    //             vsol_point_2d(segs[0]
    //                           .point2().x(),segs[0].point2().y()));
    //         for (unsigned int i = 1; i<segs.size(); i++) 
    //         {
    //             new_pts.push_back(
    //                 new 
    //                 vsol_point_2d(segs[i].point2().x(),segs[i].point2().y()));
    //         }
    //         vcl_cout << "fitted polygon size: " << new_pts.size() << vcl_endl;
    //         new_poly = new vsol_polygon_2d(new_pts);

    //     }
    // }

    // vcl_vector< vsol_spatial_object_2d_sptr > contours;
    // contours.push_back(new_poly->cast_to_spatial_object());
    
    // vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    // output_vsol->add_objects(contours,"trace");

    // {
    //     vcl_string output_file=vul_file::strip_extension(input_img);
    //     output_file=output_file+".cem";
    //     bpro1_filepath output(output_file,".cem");
        
    //     // In this everything else, is .cem, .cemv , .cfg, etc
    //     vidpro1_save_cem_process save_cem_pro;
    //     save_cem_pro.parameters()->set_value("-cemoutput",output);
        
    //     // Before we start the process lets clean input output
    //     save_cem_pro.clear_input();
    //     save_cem_pro.clear_output();
        
    //     // Kick of process
    //     save_cem_pro.add_input(output_vsol);
    //     bool write_status = save_cem_pro.execute();
    //     save_cem_pro.finish();
        
    //     //Clean up after ourselves
    //     save_cem_pro.clear_input();
    //     save_cem_pro.clear_output();
        
    // }
    
    //vcl_stringstream input_file;

    // vcl_stringstream stream(argv[1]);
    // vcl_string input_directory;
    // stream>>input_directory;



    // vcl_string line;
    // vcl_ifstream myfile(input_directory.c_str());
    // if (myfile.is_open())
    // {
    //     while ( getline (myfile,line) )
    //     {
    //         bpro1_filepath input(line);

    //         // Call containment graph process
    //         dbskfg_load_binary_composite_graph_process cgraph_pro;
            

    //         // Before we start the process lets clean input output
    //         cgraph_pro.clear_input();
    //         cgraph_pro.clear_output();
            
    //         cgraph_pro.parameters()->set_value("-cginput",input);

    //         // Pass in input vsol string
    //         bool status = cgraph_pro.execute();
    //         cgraph_pro.finish();
            
    //         //Clean up after ourselves
    //         cgraph_pro.clear_input();
    //         cgraph_pro.clear_output();
            
         
    //     }   
    //     myfile.close();
    // }


    // vcl_string line;

    // unsigned int position = input_directory.find_first_of("_");
    // vcl_string object_name = input_directory.substr(0,position);

    // vcl_ifstream myfile(input_directory.c_str());
    // vcl_getline(myfile,line);
    // vcl_stringstream foo(line);
    // unsigned int numb_polygons=0;
    // foo>>numb_polygons;
    // vcl_cout<<"Working on "<<numb_polygons<<vcl_endl;
    // double* output_distances= new double[numb_polygons];

    // unsigned int index=0;

    // if ( myfile.is_open())
    // {
    //     while(myfile.good())
    //     {
    //         vcl_getline(myfile,line);
    //         unsigned int ind=line.find_last_of(",");
    //         vcl_string distance=line.substr(ind+1);
    //         vcl_stringstream streamer(distance);
    //         double raw_distance(0);
    //         streamer>>raw_distance;
    //         if ( index < numb_polygons )
    //         {
    //             output_distances[index]=raw_distance;
    //         }
    //         index++;
           
    //     }
        
    // }
    
    // vcl_string output_name=object_name + "_distance_mat.bin";
    // vcl_cout<<output_name<<vcl_endl;
    // write_distance_matrix(numb_polygons,output_distances,output_name);
    // delete [] output_distances;

    // vcl_string temp=input_directory;
    // unsigned int position=temp.find("_cgraph_fragments");
    // vcl_string str_scale=temp.substr(position-2,2);
    // vcl_stringstream sstream(str_scale);


    // unsigned int scale;
    // sstream>>scale;    

    // // Determine Object Name
    // vcl_string object_name=input_directory;
    // unsigned int dir_location = object_name.find("image_pyramid");
    // object_name.erase(dir_location-1);
    // dir_location=object_name.find_last_of("/");
    // object_name.erase(0,dir_location+1);
   

    // double step=1.189207115002721;

    // vcl_vector<double> scale_sizes;
    // scale_sizes.push_back(1.000000000000000);
    // scale_sizes.push_back(1.189207115002721);
    // scale_sizes.push_back(1.414213562373095);
    // scale_sizes.push_back(1.681792830507429);
    // scale_sizes.push_back(2.000000000000000);
    // scale_sizes.push_back(2.378414230005442);
    // scale_sizes.push_back(2.828427124746189);
    // scale_sizes.push_back(3.363585661014858);
    // scale_sizes.push_back(3.999999999999999);
    // scale_sizes.push_back(4.756828460010883);
    // scale_sizes.push_back(5.656854249492378);


    // dbskfg_prune_fragments_process pro;

    // pro.clear_input();
    // pro.clear_output();

    // vul_file_iterator bn(input_directory+"/*.bin");
    
    // if ( !bn())
    // {
    //     return 0;
    // }

    // // Lets set the parameters
    // bpro1_filepath input_file(bn(),"");
    // bpro1_filepath output_folder(input_directory,"");

    // vcl_vector<double> thresholds;
    // thresholds.push_back(0.0);
    // thresholds.push_back(0.1);
    // thresholds.push_back(0.2);
    // thresholds.push_back(0.3);
    // thresholds.push_back(0.4);
    // thresholds.push_back(0.5);
    // thresholds.push_back(0.6);
    // thresholds.push_back(0.7);
    // thresholds.push_back(0.8);
    // thresholds.push_back(0.9);
    // thresholds.push_back(1.0);

    // for ( unsigned int i=0; i < thresholds.size() ; ++i)
    // { 
    //     vcl_cout<<"Working on "<<input_directory<<" at threshold "
    //             <<thresholds[i]<<vcl_endl;
    //     pro.clear();

    //     pro.parameters()->set_value( "-output_prefix", object_name);
    //     pro.parameters()->set_value("-output_folder", output_folder);
    //     pro.parameters()->set_value("-input_binary_file", input_file);
    //     pro.parameters()->set_value("-threshold", thresholds[i]);
    //     pro.parameters()->set_value("-scale", scale_sizes[scale]);
    
    //     bool status = pro.execute();

    //     vcl_stringstream streamer;
    //     streamer<<i;

    //     // Create names
    //     vcl_string output_file = input_directory+"/"+
    //         object_name+"_t"+streamer.str()+"_pruned_fragments.bin";
    //     pro.write_out_data(output_file);
    // }

   
    // pro.finish();

    return 0;
}
