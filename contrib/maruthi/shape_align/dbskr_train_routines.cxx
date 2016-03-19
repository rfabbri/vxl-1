// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_train_routines.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/algo/dbskr_rec_algs.h>

#include <vil/algo/vil_orientations.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>

#include <vgl/vgl_polygon_scan_iterator.h>

#include <vul/vul_file.h>

#include <bbas/bsol/bsol_algs.h>

#include <vcl_sstream.h>

//: Constructor
dbskr_train_routines::dbskr_train_routines(
    vcl_string model_list,
    int bag_of_words_sift,
    int bag_of_words_color
    ):sift_words_(bag_of_words_sift),color_words_(bag_of_words_color)
{
    // Load model file first
    vcl_cout<<"Loading model file"<<vcl_endl;
    load_model_file(model_list);

    vcl_cout<<"Computing gradients"<<vcl_endl;
    compute_gradients();

}

//: Destructor
dbskr_train_routines::~dbskr_train_routines() 
{
}


//: Set up bin file
void dbskr_train_routines::load_model_file(vcl_string& filename)
{
    vcl_ifstream esf_file(filename.c_str());

    if ( !esf_file.is_open() )
    {
        vcl_cerr<<"Error opening "<<filename<<vcl_endl;
        return;
    }

    dbsk2d_xshock_graph_fileio loader;

    vcl_string line;
    while ( vcl_getline (esf_file,line) )
    {

        // Load in two of the same one for mirroring one for not
        dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(line);

        // Compute area, and figure out new sample ds
        vgl_polygon<double> polygon=compute_boundary(sg);
        
        masks_.push_back(polygon);

        // Read in image file
        vcl_string model_imagename=vul_file::strip_extension(line);
        
        model_imagename=model_imagename+".jpg";

        vil_image_resource_sptr model_img_sptr = 
            vil_load_image_resource(line.c_str());

        vil_image_view<double> chan_1,chan_2,chan_3;
        
        chan_1.set_size(model_img_sptr->ni(),model_img_sptr->nj());
        chan_2.set_size(model_img_sptr->ni(),model_img_sptr->nj());
        chan_3.set_size(model_img_sptr->ni(),model_img_sptr->nj());
        
        vil_image_view<vxl_byte> temp=model_img_sptr->get_view();
        
        vil_image_view<vxl_byte> red   = vil_plane(temp,0);
        vil_image_view<vxl_byte> green = vil_plane(temp,1);
        vil_image_view<vxl_byte> blue  = vil_plane(temp,2);
        
        vil_convert_cast(red,chan_1);
        vil_convert_cast(green,chan_2);
        vil_convert_cast(blue,chan_3);

        model_chan_1_.push_back(chan_1);
        model_chan_2_.push_back(chan_2);
        model_chan_3_.push_back(chan_3);

        
    }
}

//: Set up bin file
void dbskr_train_routines::train()
{
}


vgl_polygon<double> dbskr_train_routines::compute_boundary(
    dbsk2d_shock_graph_sptr& sg)
{

    float scurve_sample_ds      = 5.0f; 
    float scurve_interpolate_ds = 1.0f; 

    //: required for visualization purposes
    vsol_polygon_2d_sptr poly_temp=trace_boundary_from_graph(
        sg,
        true,
        true,
        vcl_min((float)scurve_sample_ds, scurve_interpolate_ds),
        scurve_sample_ds,
        0);

    return bsol_algs::vgl_from_poly(poly_temp);
 
}


void dbskr_train_routines::compute_descriptors()
{

    double scale_1_radius=16;
    double scale_2_radius=12;
    double scale_3_radius=8;
    double scale_4_radius=4;

    int stride=8;
    double fixed_theta=0.0;

    vcl_vector<double> descriptors;

    VlSiftFilt* filter(0);

    for ( unsigned int i=0; i < masks_.size() ; ++i)
    {
        // Get grad data
        vl_sift_pix* model_chan1_grad_data = grad_chan_1_[i];
        vl_sift_pix* model_chan2_grad_data = grad_chan_2_[i];
        vl_sift_pix* model_chan3_grad_data = grad_chan_3_[i];

        // Create sift filter object
        filter = vl_sift_new(model_chan_1_[i].ni(),
                             model_chan_1_[i].nj(),
                             3,3,0);
        vl_sift_set_magnif(filter,1.0);

        vgl_box_2d<double> bbox;
        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(masks_[i], false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                vgl_point_2d<double> query_pt(x,y);
                
                vcl_pair<int,int> ib(x,y);
                in_bounds.insert(ib);

                bbox.add(query_pt);
            }
        }


        for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
        {
            for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
            {
                vcl_pair<int,int> key(x,y);

                if ( !in_bounds.count(key) )
                {
                    continue;
                }

                vgl_point_2d<double> ps1(x,y);

                vnl_vector<vl_sift_pix> scale_1_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_2_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_3_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_4_descriptor(384,0.0);

                compute_descr(ps1,
                              scale_1_radius,
                              fixed_theta,
                              model_chan1_grad_data,
                              model_chan2_grad_data,
                              model_chan3_grad_data,
                              filter,
                              scale_1_descriptor);

                compute_descr(ps1,
                              scale_2_radius,
                              fixed_theta,
                              model_chan1_grad_data,
                              model_chan2_grad_data,
                              model_chan3_grad_data,
                              filter,
                              scale_2_descriptor);

                compute_descr(ps1,
                              scale_3_radius,
                              fixed_theta,
                              model_chan1_grad_data,
                              model_chan2_grad_data,
                              model_chan3_grad_data,
                              filter,
                              scale_3_descriptor);

                compute_descr(ps1,
                              scale_4_radius,
                              fixed_theta,
                              model_chan1_grad_data,
                              model_chan2_grad_data,
                              model_chan3_grad_data,
                              filter,
                              scale_4_descriptor);

                vl_sift_delete(filter);
                filter=0;

            }
        }
    }
}

void dbskr_train_routines::compute_gradients()
{
    
    for ( unsigned int i=0; i < masks_.size() ; ++i)
    {
        vl_sift_pix* model_chan1_grad_data(0);
        vl_sift_pix* model_chan2_grad_data(0);
        vl_sift_pix* model_chan3_grad_data(0);
        
        vgl_polygon<double> poly=masks_[i];

        compute_grad_color_maps(model_chan_1_[i],
                                &model_chan1_grad_data,
                                poly);
        
        compute_grad_color_maps(model_chan_2_[i],
                                &model_chan2_grad_data,
                                poly);
        
        compute_grad_color_maps(model_chan_3_[i],
                                &model_chan3_grad_data,
                                poly);
        
        grad_chan_1_.push_back(model_chan1_grad_data);
        grad_chan_2_.push_back(model_chan2_grad_data);
        grad_chan_3_.push_back(model_chan3_grad_data);
           
    }


}

void dbskr_train_routines::compute_grad_color_maps(
    vil_image_view<double>& orig_image,
    vl_sift_pix** grad_data,
    vgl_polygon<double>& poly,
    bool mask_grad,
    bool fliplr)
{

    vil_image_view<double> flipped_image(orig_image.ni(),
                                         orig_image.nj());
    if ( mask_grad )
    {
        flipped_image.fill(0.0);

        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                if ( fliplr )
                {
                    flipped_image(flipped_image.ni()-1-x,y)=
                        orig_image(
                            x,
                            y);
                }
                else
                {
                    flipped_image(x,y)=orig_image(x,y);
                }

                in_bounds.insert(vcl_make_pair(x,y));
                
            }
        }


        for ( int cols=0; cols < orig_image.nj() ; ++cols)
        {
            for ( int rows=0; rows < orig_image.ni() ; ++rows)
            {
                vcl_pair<int,int> key(rows,cols);

                if ( !in_bounds.count(key) )
                {
                    orig_image(rows,cols)=0;
                }
            }
        }

    }
    else
    {
        if ( fliplr )
        {
            for ( unsigned int cols=0; cols < flipped_image.nj() ; ++cols)
            {
                for ( unsigned int rows=0; rows < flipped_image.ni() ; ++rows)
                {
                    flipped_image(rows,cols)=orig_image(
                        flipped_image.ni()-1-rows,
                        cols);

                }
            }
        }
        else
        {
            flipped_image=orig_image;
        }
    }

    unsigned int width  = flipped_image.ni();
    unsigned int height = flipped_image.nj();

    vil_image_view<vl_sift_pix> grad_mag;
    vil_image_view<vl_sift_pix> grad_angle;
    
    vil_orientations_from_sobel(flipped_image,grad_angle,grad_mag);

    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    vl_sift_pix* gradient_magnitude=grad_mag.top_left_ptr();
    vl_sift_pix* gradient_angle=grad_angle.top_left_ptr();

    unsigned int index=0;
    for ( unsigned int row=0; row < grad_mag.nj() ; ++row)
    {
        for ( unsigned int col=0; col < grad_mag.ni() ; ++col)
        {
            vl_sift_pix mag  = grad_mag(col,row);
            vl_sift_pix angle= angle0To2Pi(grad_angle(col,row));
       
            (*grad_data)[index]=mag;
            ++index;
            (*grad_data)[index]=angle;
            ++index;
            
        }
    }
}


void dbskr_train_routines::compute_descr(
    vgl_point_2d<double>& pt,
    double& radius,
    double& theta,
    vl_sift_pix* red_grad_data,
    vl_sift_pix* green_grad_data,
    vl_sift_pix* blue_grad_data,
    VlSiftFilt* sift_filter,
    vnl_vector<vl_sift_pix>& descriptor)
{

    vl_sift_pix descr_ps1_red[128];
    memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_green[128];
    memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_blue[128];
    memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
        

    vl_sift_calc_raw_descriptor(sift_filter,
                                red_grad_data,
                                descr_ps1_red,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);

    vl_sift_calc_raw_descriptor(sift_filter,
                                green_grad_data,
                                descr_ps1_green,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);

    vl_sift_calc_raw_descriptor(sift_filter,
                                blue_grad_data,
                                descr_ps1_blue,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);


    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descriptor.put(d,descr_ps1_red[d]);
        descriptor.put(d+128,descr_ps1_green[d]);
        descriptor.put(d+256,descr_ps1_blue[d]);
        
    }

    // descriptor.normalize();

}
