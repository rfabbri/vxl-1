//  This is under contrib\Manifold_extraction\vis
// Brief: An example for vehicle classification based on Lie distances


#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vehicle_model.h>
#include <vrml_print.h>
#include <Lie_group_operations.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_canny_ox.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vul/vul_file_iterator.h>


void draw_box(vgl_box_2d<double> box,vil_image_view<unsigned char> &image)
    {
    int minx = box.min_x();
    int maxx = box.max_x();

    int miny = box.min_y();
    int maxy = box.max_y();

    int i,j;

    //fill the entire box with 255
    for (i =minx;i<=maxx;i++)
        {
        for (j = miny;j<=maxy;j++)
            {
            unsigned char& val =  image(i,j);
            val = 0;
            }
        }

#if 0

    for (int j = miny;j<=maxy;j++)
        {
        unsigned char& val1 =  image(minx,j);
        unsigned char& val2 = image(maxx,j);

        val1 = 0;
        val2 = 0;
        }

    for (int i = minx;i<=maxx;i++)
        {
        unsigned char& val1 =  image(i,miny);
        unsigned char& val2 = image(i,maxy);

        val1 = 0;
        val2 = 0;
        }
#endif
    }

vil_image_view<unsigned char> draw_model(vehicle_model trans_model,unsigned int ni,unsigned int nj)
    {

    vil_image_view<unsigned char> model_image(ni,nj,1,1);

    model_image.fill(unsigned char(255));

    draw_box(trans_model.engine(),model_image);
    draw_box(trans_model.body(),model_image);
    draw_box(trans_model.rear(),model_image);

    return model_image;
    }


struct vehic_mod_vnl_least_squares_function:public vnl_least_squares_function
    {
    vehic_mod_vnl_least_squares_function(vil_image_resource_sptr img_sptr,vehicle_model ref_model)
        :vnl_least_squares_function(6,6,no_gradient),
        img_sptr_(img_sptr),ref_model_(ref_model){}


    //there are 6 parameters in the model and the number of residuals is ni*nj
    //where ni and nj denote the dimensions of the image

    void f(vnl_vector<double> const& params, vnl_vector<double>& residuals) {

        double s1x,t1x,s1y,t1y,s2x,s2y,s3x,s3y,thet;

        int i = 0;

        t1x = params[i++];
        t1y = params[i++];
        s1x = params[i++];
        s2x = params[i++];
        s3x = params[i++];
        thet = params[i++];

        vcl_cout <<"model parameters while in the optimization process: " <<vcl_endl;

        for (int i = 0;i<params.size();i++)
            {
            vcl_cout << params[i]<<vcl_endl;
            }

        s1y = s1x;
        s2y = s2x;
        s3y = s3x;

        vnl_matrix<double>m1(3,3,0.0);
        vnl_matrix<double>m2(3,3,0.0);
        vnl_matrix<double>m3(3,3,0.0);

        set_transformation_matrices(m1,m2,m3,ref_model_,s1x,t1x,s1y,t1y,s2x,s2y,s3x,s3y);

        vcl_cout << "reference model: " << vcl_endl;
        vcl_cout << ref_model_ << vcl_endl;


        vehicle_model trans_model = transform_model(ref_model_,m1,m2,m3);
        vehicle_model rot_model = rotate_model(trans_model,thet);

        vcl_cout << " model after transformation " << vcl_endl;
        vcl_cout << trans_model << vcl_endl;

        vil_image_view< unsigned char > given_image = img_sptr_->get_view();
        vil_image_view< unsigned char > rot_model_image = draw_model(rot_model,img_sptr_->ni(),img_sptr_->nj());



        vcl_string filename = "C:\\pradeep\\Manifold_extraction\\3_box_model\\drawn_images\\drawn_1.png";
        //save the transformed model image for debugging purposes
        vil_save(rot_model_image,filename.c_str());

        get_residuals(given_image,rot_model,params,residuals);
        } 

void get_residuals(vil_image_view<unsigned char> given_image,vehicle_model rot_model,
                   vnl_vector<double> parameters,vnl_vector<double>& residuals)
    {
    unsigned int xdim = given_image.ni();
    unsigned int ydim = given_image.nj();

    vil_image_view<unsigned char> diff_image(xdim,ydim,1); 
    double diff,val1x,val2x,val3x,val1y,val2y,val3y;
    unsigned int count =0;

    vcl_string res_vec_file = "C:\\pradeep\\Manifold_extraction\\3_box_model\\res_vec.txt";

    vcl_ofstream ofst(res_vec_file.c_str());

    val1x = (rot_model.engine().centroid_x() - (ref_model_.engine().centroid_x()+10 ));
    val2x = (rot_model.body().centroid_x() -  (ref_model_.body().centroid_x()+10));
    val3x = (rot_model.rear().centroid_x() - (ref_model_.rear().centroid_x()+10) );

    val1y = (rot_model.engine().centroid_y() - ref_model_.engine().centroid_y() );
    val2y = (rot_model.body().centroid_y() -  ref_model_.body().centroid_y());
    val3y = (rot_model.rear().centroid_y() - ref_model_.rear().centroid_y() );

    residuals[count++] = val1x;
    residuals[count++] = val2x;
    residuals[count++] = val3x;

    residuals[count++] = val1y;
    residuals[count++] = val2y;
    residuals[count++] = val3y;

    //for (int i =0;i<xdim;i++)
    //    {
    //    for (int j = 0;j<ydim;j++)
    //        {
    //        val1 = given_image(i,j);
    //        val2 = trans_model_image(i,j);
    //        diff = abs(val1 - val2);
    //        if (diff > 0)
    //            ofst << " i " << i <<" j " <<j<<vcl_endl;
    //        
    //        diff_image(i,j) = abs(given_image(i,j) - trans_model_image(i,j));
    //        residuals[count] = diff;
    //
    //     //   residuals[count] = diff;
    //     //   ofst << residuals[count] << vcl_endl;
    //     //   ofst << diff_image(i,j) << vcl_endl;
    //        count++;
    //
    //        }   
    //    }

    //for debugging purposes
    //vcl_cout <<" rms value of residual vector: " << vcl_endl;
    //vcl_cout << residuals.rms()<<vcl_endl;


    //save the transformed model image for debugging purposes
    //vcl_string filename = "C:\\pradeep\\Manifold_extraction\\3_box_model\\drawn_images\\diff_image_1.png";
    //vil_save(diff_image,filename.c_str());
    ofst.close();

    }


vil_image_resource_sptr img_sptr_;
vehicle_model ref_model_;
    };


vil_image_view< unsigned char > get_edge_image(vil_image_resource_sptr img_sptr)
    {
    vil_image_view<unsigned char>greyscale_view = img_sptr->get_view();    
    vil_image_view< unsigned char > binary_edge_img(greyscale_view.ni(),greyscale_view.nj(),1); 

    binary_edge_img.fill(255);

    vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( greyscale_view );

    osl_canny_ox_params canny_params;


    // Canny edge detector execution
    osl_canny_ox detector(canny_params);
    vcl_list<osl_edge*>  edges;

    // =========================================
    detector.detect_edges(img, &edges);
    // =========================================

    // put edge information (location and direction) on to-return images
    // `binary_edge_img' and `dir_img'
    while (edges.size() > 0)
        {
        osl_edgel_chain * ec=(osl_edgel_chain *)edges.front();
        for(unsigned i=0;i<ec->size();i++)
            {
            unsigned int x = (unsigned int)vcl_floor(ec->GetY(i));
            unsigned int y = (unsigned int)vcl_floor(ec->GetX(i));

            // location of edge
            binary_edge_img(x,y)=0;                     
            }
        edges.pop_front();
        }
    return binary_edge_img;
    }

vehicle_model get_optimal_model(vil_image_resource_sptr img_sptr,vehicle_model ref_model,vnl_vector<double> & parameters)
    {

    vil_image_view<unsigned char> edge_img = get_edge_image(img_sptr);

    vcl_string filename = "C:\\pradeep\\Manifold_extraction\\3_box_model\\edge_images\\edge_1.png";
    //save the edge image for debugging purposes
    vil_save(edge_img,filename.c_str());

    vil_image_resource_sptr edge_img_sptr = vil_new_image_resource(edge_img.ni(),edge_img.nj(),1,VIL_PIXEL_FORMAT_BYTE);

    //vehic_mod_vnl_least_squares_function lsqr_fn(edge_img_sptr,ref_model);

    //for finding out the residuals,use the original image itself instead of edge image
    vehic_mod_vnl_least_squares_function lsqr_fn(img_sptr,ref_model);
    vnl_levenberg_marquardt lm (lsqr_fn);

    lm.set_x_tolerance(lm.get_x_tolerance());
    lm.set_trace(true);
    lm.minimize_without_gradient(parameters);

    vcl_cout <<"converged parameter values: " <<vcl_endl;

    for (int i = 0;i<parameters.size();i++)
        {
        vcl_cout << parameters[i]<<vcl_endl;
        }

    lm.diagnose_outcome(vcl_cout);

    vehicle_model M;
    return M;
    }

int main(int argc,char **argv)
    {
    vcl_string class_A_mask_images = argv[1];
    vcl_string class_B_mask_images = argv[2];
    vcl_string debug_info_file = argv[3];


    vcl_ofstream debug_info(debug_info_file.c_str());

    int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);

    delete []my_argv;



    vgl_box_2d<double>starting_box1(60,80,100,130);
    vgl_box_2d<double>starting_box2(80,120,90,140);
    vgl_box_2d<double>starting_box3(120,150,95,125);

    vehicle_model ref_model(starting_box1,starting_box2,starting_box3);

    vcl_vector<vil_image_resource_sptr> class_A;
    vcl_vector<vil_image_resource_sptr> class_B;
    vcl_string image_name;

    for (vul_file_iterator fn=class_A_mask_images+"/*.png"; fn; ++fn) {
        image_name = fn();
        vil_image_resource_sptr img_sptr = vil_load_image_resource(image_name.c_str());
        class_A.push_back(img_sptr);
        }

for (vul_file_iterator fn=class_B_mask_images+"/*.png"; fn; ++fn) {
    image_name = fn();
    vil_image_resource_sptr img_sptr = vil_load_image_resource(image_name.c_str());
    class_B.push_back(img_sptr);
    }

vnl_matrix<double>m1(3,3,0.0);
vnl_matrix<double>m2(3,3,0.0);
vnl_matrix<double>m3(3,3,0.0);

//scaling the first box along x direction by 3,translating along x direction by 10
//scaling along y direction by 2,translating along y direction by 20

double t1x = 0,t1y = 0,s1x = 1,s1y = s1x,s2x = 1,s2y = s2x,s3x =1,s3y = s3x;//thet = -0.125*vnl_math::pi;
double thet = 0;
int i = 0;

//six parameters for the model

vnl_vector<double> parameters;
parameters.set_size(6);

parameters[i++] = t1x;
parameters[i++] = t1y;
parameters[i++] = s1x;
parameters[i++] = s2x;
parameters[i++] = s3x;
parameters[i++] = thet;

set_transformation_matrices(m1,m2,m3,ref_model,s1x,t1x,s1y,t1y,s2x,s2y,s3x,s3y);

vehicle_model OM = get_optimal_model(class_A[0],ref_model,parameters);

debug_info <<"model parameters: " <<vcl_endl;

for (i = 0;i<parameters.size();i++)
    {
    debug_info << parameters[i]<<vcl_endl;
    }

debug_info << "optimal model :" << vcl_endl;
debug_info << OM << vcl_endl;


return 0;
    }

