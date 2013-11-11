//This is dbdet/edge/dbdet_third_order_edge_det.cxx

#include <dbdet/algo/dbdet_pb_edge_det.h>
#include <vul/vul_timer.h>
#include <dbdet/algo/dbdet_edge_appearance_util.h>
#include <dbdet/algo/dbdet_edge_uncertainty_measure.h>



#include <nonmax.hh>
#include <image.hh>
#include <pb.hh>
#include <util.hh>
#include <configure.hh>
#include <dbdet/sel/dbdet_sel_utils.h>

//: function to compute generic edges
dbdet_edgemap_sptr dbdet_detect_pb_edges(vil_image_view<vxl_byte> image, 
                                         double sigma, double threshold, 
                                         int N, unsigned parabola_fit, 
                                         unsigned grad_op, unsigned conv_algo,
                                         bool use_adaptive_thresh, 
                                         bool interp_grid, 
                                         bool reduce_tokens)
{
    
    // Use pb data structure
    Util::ImageStack pb_image;

    // Register all namespaces with default configuration
    Util::registerConfig();
    Group::Pb::registerConfig();
 
    // Initialize utilities
    Util::init();
    bool useColor =  true; 
    unsigned int numOrient  = 8; 

    // Resize image stack
    pb_image.resize(image.nplanes(),image.ni(),image.nj());

    // Populate pb data structure
    for (unsigned p = 0; p < image.nplanes(); ++p)
    {
        for (unsigned j = 0; j < image.nj(); ++j)
        {
            for (unsigned i = 0; i < image.ni(); ++i)
            {
                // Normalize Image
                pb_image(p,i,j)=((float) (image(i,j,p))) / 255;
            }
        }
    }

    // Calculate Gradients (TG,CG,BG)
    Group::Pb pbFactory;
    pbFactory.initialize(pb_image,useColor);

    // Perform Pb Processing
    Util::Image boundaries;
    Group::ParabolaSet parabs;
    pbFactory.computePb(numOrient,boundaries,parabs);

    //compute image gradients before performing nonmax suppression
    vil_image_view<double> pb_grad_x, pb_grad_y;

    // Set grad magnitude to pb data
    vil_image_view<float> pbImage(
        boundaries.data(),                      // data
        boundaries.size(0),                     // ni
        boundaries.size(1),                     // nj
        1,                                      // nplanes
        boundaries.size(1),                     // istep
        1,                                      // jstep
        boundaries.size(0)*boundaries.size(1)); // planestep


    // Set up storage for gradients
    vil_image_view<double> grad_x,grad_y,pb_grad_mag;
    //grad_x.set_size(pbImage.ni(),pbImage.nj());
    //grad_y.set_size(pbImage.ni(),pbImage.nj());
    vil_convert_cast(pbImage,pb_grad_mag);

    
    dbdet_edgemap_sptr edge_map = new dbdet_edgemap(
        pbImage.ni(),pbImage.nj());
    
    // Compute grad x and grad y
    for (unsigned i=0; i<pbImage.ni(); i++)
    {
        for (unsigned j=0; j<pbImage.nj(); j++)
        {

            // Amirs approach uncomment for later

            // double orient = parabs.maxo(i,j);
            // grad_x(i,j)=vcl_cos(orient + M_PI_2);
            // grad_y(i,j)=vcl_sin(orient + M_PI_2);
            // //grad_y(i,j)=vcl_sin(vcl_fmod(orient + M_PI_2,M_PI));

            // vcl_cout<<"grad x: "<<grad_x(i,j)<<vcl_endl;
            // vcl_cout<<"grad y: "<<grad_y(i,j)<<vcl_endl;


            if ( parabs.edgemap(i,j) == 1)
            {
                int  ptx = dbdet_round(parabs.X(i,j)+i);
                int  pty = dbdet_round(parabs.Y(i,j)+j);
                double maxo = parabs.maxo(i,j);
                double maxe = parabs.maxe(i,j);
                if ( ptx < 0 || ptx >=  pb_grad_mag.ni())
                {
                    continue;
                }

                if ( pty < 0 || pty >=  pb_grad_mag.nj())
                {
                    continue;
                }


                vgl_point_2d<double> pt(parabs.X(i,j)+i,parabs.Y(i,j)+j);

                edge_map->insert(new dbdet_edgel(pt, 
                                                 parabs.maxo(i,j), 
                                                 parabs.maxe(i,j)));
           }
        }
    } 


    // //Now call the nms code to get the subpixel edge tokens
    // vcl_vector<vgl_point_2d<double> > loc;
    // vcl_vector<double> orientation, mag;

    // dbdet_nms NMS(
    //     dbdet_nms_params(threshold, dbdet_nms_params::PFIT_3_POINTS), 
    //     grad_x, 
    //     grad_y, 
    //     grad_mag);

    // NMS.apply(true, loc, orientation, mag);

    // dbdet_edgemap_sptr edge_map = new dbdet_edgemap(
    //     pbImage.ni(),pbImage.nj());


    // for (unsigned i=0; i<loc.size(); i++){
    //     vgl_point_2d<double> pt(loc[i].x(), loc[i].y());
    //     edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
    // }

    // // dbdet_add_intensity_app(image, edge_map, sigma, 1); //opt: 0: original , 1: smoothed, 2: Half gaussian

    // vcl_cerr<<"dont with stuff"<<vcl_endl;

    return edge_map;
}

