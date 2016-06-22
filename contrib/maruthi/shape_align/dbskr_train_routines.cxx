// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_train_routines.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/algo/dbskr_rec_algs.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_diag_matrix.h>

#include <bbas/bil/algo/bil_color_conversions.h>
#include <vil/algo/vil_orientations.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>

#include <vnl/vnl_matlab_print.h>

#include <vgl/vgl_polygon_scan_iterator.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>

#include <bbas/bsol/bsol_algs.h>

#include <vcl_sstream.h>

extern "C" {
#include <vl/gmm.h>
}

//: Constructor
dbskr_train_routines::dbskr_train_routines(
    vcl_string model_list,
    DescriptorType descr_type,
    ColorSpace color_space,
    int keywords,
    int pca,
    int stride
    ):descr_type_(descr_type),color_space_(color_space),keywords_(keywords),
      pca_(pca),stride_(stride)
{

    // Write out centers
    vcl_string d_type="";
    if ( descr_type_ == dbskr_train_routines::GRADIENT )
    {
        d_type="gradient";
    }
    else
    {
        d_type="color";
    }

    vcl_string c_type="";
    if ( color_space_ == dbskr_train_routines::RGB )
    {
        c_type="rgb";
    }
    else if ( color_space_ == dbskr_train_routines::LAB )
    {
        c_type="lab";
    }
    else if ( color_space_ == dbskr_train_routines::OPP )
    {
        c_type="opp";
    }
    else
    {
        c_type="nopp";
    }

    vcl_stringstream gmm_filename_stream;
    gmm_filename_stream<<"gmm_"<<d_type<<"_"<<c_type<<"_"<<keywords<<".txt";

    vcl_stringstream pca_filename_stream;
    pca_filename_stream<<"pca_"<<d_type<<"_"<<c_type<<"_dim_"<<pca_;
    
    vcl_string gmm_filename=gmm_filename_stream.str();
    vcl_string M_filename=pca_filename_stream.str()+"_M.txt";
    vcl_string mean_filename=pca_filename_stream.str()+"_mean.txt";
    
    
    // Load model file first
    vcl_cout<<"Loading model file"<<vcl_endl;
    load_model_file(model_list);

    if ( descr_type_ == dbskr_train_routines::GRADIENT)
    {
        vcl_cout<<"Computing Gradient Descriptors"<<vcl_endl;
        
        compute_gradients();
        compute_grad_descriptors();
    }
    else
    {

        
    }

    // compute pca
    compute_pca(M_filename,mean_filename);
    this->train(gmm_filename);
}

//: Destructor
dbskr_train_routines::~dbskr_train_routines() 
{
}

//: Write out files
void dbskr_train_routines::write_out()
{

}

//: Load model files and convert to color space if appropriate
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
            vil_load_image_resource(model_imagename.c_str());

        vil_image_view<double> chan_1,chan_2,chan_3;
        
        convert_to_color_space(model_img_sptr,
                               chan_1,
                               chan_2,
                               chan_3,
                               color_space_);
                               
        model_chan_1_.push_back(chan_1);
        model_chan_2_.push_back(chan_2);
        model_chan_3_.push_back(chan_3);

        
    }
}

//: Set up bin file
void dbskr_train_routines::train(vcl_string& gmm_filename)
{

    // Let time how long this takes
    // Start timer
    vul_timer t;

    // Run gmm
    // Map all descriptors to pca

    vcl_vector<double> descriptors;

    for ( int i =0; i < descriptor_matrix_.rows() ; ++i)
    { 
        vnl_vector<vl_sift_pix> vec = descriptor_matrix_.get_row(i);
        vnl_vector<vl_sift_pix> pca_vec = linear_embed(vec);

        for ( int p=0; p < pca_vec.size() ; ++p)
        {
            descriptors.push_back(pca_vec[p]);
        }
    }

    double* data=descriptors.data();
    int dimension  = pca_;
    int numData    = descriptors.size()/pca_;
    int numCenters = keywords_;

    double * means ;
    double * covariances ;
    double * priors ;
    double * posteriors ;

    vcl_cout<<"GMM "<<numData<<" descriptors "<<vcl_endl;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    // create a new instance of a GMM object for double data
    VlGMM* gmm = vl_gmm_new (VL_TYPE_DOUBLE, dimension, numCenters) ;

    // set verbosity
    vl_gmm_set_verbosity (gmm, 1);

    // set the maximum number of EM iterations to 100
    vl_gmm_set_max_num_iterations (gmm, 100) ;

    // set the initialization to random selection
    vl_gmm_set_initialization (gmm,VlGMMRand);

    // cluster the data, i.e. learn the GMM
    vl_gmm_cluster (gmm, data, numData);

    // get the means, covariances, and priors of the GMM
    means = (double *)vl_gmm_get_means(gmm);
    covariances = (double *)vl_gmm_get_covariances(gmm);
    priors = (double *)vl_gmm_get_priors(gmm);

    double vox_time2 = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"Clustering Time: "<<vox_time2<<" sec"<<vcl_endl;
     
    vcl_ofstream gmm_stream(gmm_filename);

    gmm_stream<<numCenters<<vcl_endl;
    gmm_stream<<dimension<<vcl_endl;

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        gmm_stream<<means[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        gmm_stream<<covariances[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters ; ++c)
    {
        gmm_stream<<priors[c]<<vcl_endl;

    }

    gmm_stream.close();

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"TrainTime: "
            <<vox_time<<" sec"<<vcl_endl;

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

void dbskr_train_routines::compute_grad_descriptors()
{

    double scale_1_radius=16;
    double scale_2_radius=12;
    double scale_3_radius=8;
    double scale_4_radius=4;

    double fixed_theta=0.0;

    vcl_vector<vl_sift_pix> descriptors;

    VlSiftFilt* filter(0);

    for ( unsigned int i=0; i < masks_.size() ; ++i)
    {
        vcl_cout<<"Working on mask: "<<i<<vcl_endl;

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


        for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride_)
        {
            for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride_) 
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

                compute_sift_descr(ps1,
                                   scale_1_radius,
                                   fixed_theta,
                                   model_chan1_grad_data,
                                   model_chan2_grad_data,
                                   model_chan3_grad_data,
                                   filter,
                                   scale_1_descriptor);

                compute_sift_descr(ps1,
                                   scale_2_radius,
                                   fixed_theta,
                                   model_chan1_grad_data,
                                   model_chan2_grad_data,
                                   model_chan3_grad_data,
                                   filter,
                                   scale_2_descriptor);

                compute_sift_descr(ps1,
                                   scale_3_radius,
                                   fixed_theta,
                                   model_chan1_grad_data,
                                   model_chan2_grad_data,
                                   model_chan3_grad_data,
                                   filter,
                                   scale_3_descriptor);

                compute_sift_descr(ps1,
                                   scale_4_radius,
                                   fixed_theta,
                                   model_chan1_grad_data,
                                   model_chan2_grad_data,
                                   model_chan3_grad_data,
                                   filter,
                                   scale_4_descriptor);

                for ( unsigned int c=0; c < scale_1_descriptor.size() ; ++c)
                {
                    descriptors.push_back(scale_1_descriptor[c]);
                }


                for ( unsigned int c=0; c < scale_2_descriptor.size() ; ++c)
                {
                    descriptors.push_back(scale_2_descriptor[c]);
                }
                

                for ( unsigned int c=0; c < scale_3_descriptor.size() ; ++c)
                {
                    descriptors.push_back(scale_3_descriptor[c]);
                }

                for ( unsigned int c=0; c < scale_4_descriptor.size() ; ++c)
                {
                    descriptors.push_back(scale_4_descriptor[c]);
                }

            }
        }
        
        vl_sift_delete(filter);
        filter=0;


    }

    descriptor_matrix_.set_size(descriptors.size()/384,384);
    descriptor_matrix_.copy_in(descriptors.data());
}


void dbskr_train_routines::compute_pca(vcl_string& M_filename,
                                       vcl_string& mean_filename)
{
    // Compute mean
    PCA_mean_.set_size(descriptor_matrix_.cols());
    PCA_mean_.fill(0.0);

    for ( int i =0; i < descriptor_matrix_.rows() ; ++i)
    {
        PCA_mean_+=descriptor_matrix_.get_row(i);

    }

    PCA_mean_/=descriptor_matrix_.rows();

    // Subtract pca_mean from descriptor matrix
    for ( int i=0; i < descriptor_matrix_.rows() ; ++i)
    {
        vnl_vector<vl_sift_pix> row=descriptor_matrix_.get_row(i);
        vnl_vector<vl_sift_pix> zero_mean=row-PCA_mean_;
        descriptor_matrix_.set_row(i,zero_mean);
    }

    vnl_matrix<vl_sift_pix> descriptor_matrix_transpose=
        descriptor_matrix_.transpose();

    vnl_matrix<vl_sift_pix> symmetric_matrix = 
        (descriptor_matrix_transpose*descriptor_matrix_)
        /(descriptor_matrix_.rows()-1);


    // Numeric scaling
    vnl_diag_matrix<vl_sift_pix> small_scaling(symmetric_matrix.rows(),0.001);
    symmetric_matrix=symmetric_matrix+small_scaling;

    vnl_matrix<vl_sift_pix> eigen_vectors(symmetric_matrix.rows(),
                                          symmetric_matrix.cols());
    vnl_vector<vl_sift_pix> eigen_values(symmetric_matrix.rows());

    vnl_symmetric_eigensystem_compute(symmetric_matrix,
                                      eigen_vectors,eigen_values);

    // Set pca M
    PCA_M_.set_size(symmetric_matrix.rows(),pca_);
    PCA_M_.fill(0.0);
    
    int index=eigen_vectors.cols()-1;

    for ( unsigned int m=0; m < PCA_M_.cols() ; ++m)
    {
        PCA_M_.set_column(m,eigen_vectors.get_column(index-m));

    }

    {
        vcl_ofstream M_stream(M_filename.c_str());
        M_stream<<PCA_M_.rows()<<vcl_endl;
        M_stream<<PCA_M_.cols()<<vcl_endl;
        for ( int i=0; i < PCA_M_.rows() ; ++i )
        {
            for ( int j=0; j < PCA_M_.cols() ; ++j )
            {
                M_stream<<PCA_M_[i][j]<<vcl_endl;
            }

        }
        M_stream.close();

    }

    {
        vcl_ofstream mean_stream(mean_filename.c_str());
        mean_stream<<PCA_mean_.size()<<vcl_endl;
        for ( int i=0; i < PCA_mean_.size() ; ++i)
        {
            mean_stream<<PCA_mean_[i]<<vcl_endl;
        }
        mean_stream.close();
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


void dbskr_train_routines::compute_sift_descr(
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


void dbskr_train_routines::convert_to_color_space(
    vil_image_resource_sptr& input_image,
    vil_image_view<double>& o1,
    vil_image_view<double>& o2,
    vil_image_view<double>& o3,
    ColorSpace color_space)
{
    vil_image_view<vxl_byte> image = input_image->get_view();
    unsigned int w = image.ni(); 
    unsigned int h = image.nj();
    o1.set_size(w,h);
    o2.set_size(w,h);
    o3.set_size(w,h);

    if ( color_space == LAB )
    {
        convert_RGB_to_Lab(image,
                           o1,
                           o2,
                           o3);
    }
    else if ( color_space == RGB )
    {

        vil_image_view<vxl_byte> red   = vil_plane(image,0);
        vil_image_view<vxl_byte> green = vil_plane(image,1);
        vil_image_view<vxl_byte> blue  = vil_plane(image,2);
        
        vil_convert_cast(red,o1);
        vil_convert_cast(green,o2);
        vil_convert_cast(blue,o3);

    }
    else
    {
        for (unsigned r = 0; r < h; r++)
        {
            for (unsigned c = 0; c < w; c++)
            {
                double red=image(c,r,0);
                double green=image(c,r,1);
                double blue=image(c,r,2);
                o1(c,r) = (red-green)/vcl_sqrt(2);
                o2(c,r) = (red+green-2*blue)/vcl_sqrt(6);
                o3(c,r) = (red+green+blue)/vcl_sqrt(3);
                if ( color_space == NOPP )
                {
                    if ( o3(c,r) > 0.0 )
                    {
                        o1(c,r)=o1(c,r)/o3(c,r);
                        o2(c,r)=o2(c,r)/o3(c,r);
                    }
                }
            }
        }
    }


}
