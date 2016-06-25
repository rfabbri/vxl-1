// This is brcv/shp/dbskfg/dbskr_align_shapes.cxx

//:
// \file

#include <shape_align/dbskr_test_routines.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <dbskr/algo/dbskr_rec_algs.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_diag_matrix.h>

#include <bbas/bil/algo/bil_color_conversions.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_bilin_interp.h>
#include <vil/algo/vil_orientations.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>

#include <vgl/vgl_polygon_scan_iterator.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>

#include <bbas/bsol/bsol_algs.h>

#include <vcl_sstream.h>

extern "C" {
#include <vl/gmm.h>
#include <vl/fisher.h>
}

//: Constructor
dbskr_test_routines::dbskr_test_routines(
    vcl_string query_list,
    vcl_string query_dc_file,
    vcl_string model_list,
    vcl_string gmm_file,
    vcl_string pca_M_file,
    vcl_string pca_mean_file,
    DescriptorType descr_type,
    ColorSpace color_space,
    int stride,
    double powernorm,
    bool write_out
        ):query_ni_(0),query_nj_(0),descr_type_(descr_type),
          color_space_(color_space),means_cg_(0),covariances_cg_(0),
          priors_cg_(0), stride_(stride),powernorm_(powernorm),
          write_out_(write_out)
{

    // Write out centers
    vcl_string d_type="";
    if ( descr_type_ == dbskr_test_routines::GRADIENT )
    {
        d_type="gradient";
    }
    else
    {
        d_type="color";
    }

    vcl_string c_type="";
    if ( color_space_ == dbskr_test_routines::RGB )
    {
        c_type="rgb";
    }
    else if ( color_space_ == dbskr_test_routines::LAB )
    {
        c_type="lab";
    }
    else if ( color_space_ == dbskr_test_routines::OPP )
    {
        c_type="opp";
    }
    else
    {
        c_type="nopp";
    }
    
    // Load query data
    vcl_cout<<"Loading query files"<<vcl_endl;
    load_query_file(query_list);

    // Load query dc data
    vcl_cout<<"Loading query DC data"<<vcl_endl;
    load_dc_file(query_dc_file);

    // Load model file first
    vcl_cout<<"Loading model file"<<vcl_endl;
    load_model_file(model_list);

    // Load gmm data
    vcl_cout<<"Loading gmm data"<<vcl_endl;
    load_gmm_data(gmm_file);

    // Load gmm data
    vcl_cout<<"Loading pca data"<<vcl_endl;
    load_pca_data(pca_M_file,
                  pca_mean_file);

    // Computing gradienets of query 
    compute_query_gradients();

    // Compute query test points
    compute_query_test_points();

    //Test
    test();
}

//: Destructor
dbskr_test_routines::~dbskr_test_routines() 
{
    vl_free(means_cg_);
    means_cg_=0;

    vl_free(covariances_cg_);
    covariances_cg_=0;

    vl_free(priors_cg_);
    priors_cg_=0;
}



//: Compute appearance difference
void dbskr_test_routines::test()
{

    vcl_cout<<"Computing distances between "<<
        query_test_points_.size()<<" Querys and "<<
        model_points_.size()<<" Models"<<vcl_endl;

    // Create sift filter object
    VlSiftFilt* filter = vl_sift_new(query_ni_,
                                     query_nj_,
                                     3,3,0);
    vl_sift_set_magnif(filter,1.0);

    int encoding_size = 2 * PCA_M_.cols() * keywords_;

    vcl_vector<vnl_matrix<vl_sift_pix> > output;

    for ( int q=0; q < query_test_points_.size() ; ++q)
    {

        vcl_vector<vcl_pair<float,float> > q_pts=query_points_[q];
        vcl_vector<vgl_point_2d<double> > test_points=query_test_points_[q];

        vnl_matrix<vl_sift_pix> dist_matrix(model_points_.size(),
                                            test_points.size(),
                                            0.0);

        for ( int m=0; m < model_points_.size() ; ++m)
        {
            vcl_cout<<"Distance: Query "<<q<<" vs Model "<<m
                    <<vcl_endl;

            vcl_vector<vcl_pair<float,float> > m_pts=model_points_[m][q];

            
            vil_image_view<double> temp(query_ni_,
                                        query_nj_,
                                        3);
            
            temp.fill(255.0);

            explicit_alignment(q_pts,
                               m_pts,
                               model_chan_1_[m],
                               model_chan_2_[m],
                               model_chan_3_[m],
                               temp);

            if ( write_out_)
            {
                vcl_stringstream name;
                name<<"Model_"<<m<<"_vs_Query_"<<q<<"_warp.png";

                vil_image_view<vxl_byte > map_image;
                vil_convert_cast(temp,map_image);
                vil_save(map_image,name.str().c_str());
            } 

            vil_image_view<double> chan1   = vil_plane(temp,0);
            vil_image_view<double> chan2   = vil_plane(temp,1);
            vil_image_view<double> chan3   = vil_plane(temp,2);
            
            // Compute gradients of channels
            vl_sift_pix* model_chan1_grad_data(0);
            vl_sift_pix* model_chan2_grad_data(0);
            vl_sift_pix* model_chan3_grad_data(0);
            
            vgl_polygon<double> poly;
            
            compute_grad_color_maps(chan1,
                                    &model_chan1_grad_data,
                                    poly,
                                    false);
        
            compute_grad_color_maps(chan2,
                                    &model_chan2_grad_data,
                                    poly,
                                    false);
            
            compute_grad_color_maps(chan3,
                                    &model_chan3_grad_data,
                                    poly,
                                    false);
            
            
            vnl_matrix<vl_sift_pix> query_fvs(encoding_size,
                                              test_points.size(),0);
            vnl_matrix<vl_sift_pix> model_fvs(encoding_size,
                                              test_points.size(),0);

            // Compute query_fvs
            compute_fvs(test_points,
                        query_grad_chan_1_[q],
                        query_grad_chan_2_[q],
                        query_grad_chan_3_[q],
                        filter,
                        query_fvs);

            // Compute model_fvs
            compute_fvs(test_points,
                        model_chan1_grad_data,
                        model_chan2_grad_data,
                        model_chan3_grad_data,
                        filter,
                        model_fvs);

            vnl_matrix<vl_sift_pix> result_final=query_fvs-model_fvs;

            for ( int c=0; c < result_final.cols() ; ++c)
            {
                vl_sift_pix d=result_final.get_column(c).magnitude();
                dist_matrix[m][c]=d;
            }
    
            output.push_back(dist_matrix);

            vl_free(model_chan1_grad_data);
            vl_free(model_chan2_grad_data);
            vl_free(model_chan3_grad_data);

            model_chan1_grad_data=0;
            model_chan2_grad_data=0;
            model_chan3_grad_data=0;

                       
        }

        vl_free(query_grad_chan_1_[q]);
        vl_free(query_grad_chan_2_[q]);
        vl_free(query_grad_chan_3_[q]);
        
        query_grad_chan_1_[q]=0;
        query_grad_chan_2_[q]=0;
        query_grad_chan_3_[q]=0;
    }
 
    vl_sift_delete(filter);
    filter=0;
    
    vcl_cout<<"Writing out Results"<<vcl_endl;

    // Write out output
    vcl_ofstream output_binary_file;
    output_binary_file.open(output_filename_.c_str(),
                            vcl_ios::out | 
                            vcl_ios::binary);
    
    float q = output.size();
    float m = model_points_.size();
    
    // Write out number of model trees vs query trees
    // we are comparing
    output_binary_file.write(reinterpret_cast<char *>(&q),
                             sizeof(float));
    output_binary_file.write(reinterpret_cast<char *>(&m),
                             sizeof(float));



    for ( int q=0; q < query_test_points_.size() ; ++q)
    {

        vcl_vector<vgl_point_2d<double> > test_points=query_test_points_[q];

        float size=test_points.size();

        output_binary_file.write(reinterpret_cast<char *>(&size),
                                 sizeof(float));
        
        vnl_matrix<vl_sift_pix> dist_matrix=output[q];

        for (unsigned int p=0; p < dist_matrix.rows() ; ++p)
        {
            vnl_vector<vl_sift_pix> row=dist_matrix.get_row(p);
        
            for ( unsigned int v=0; v < row.size() ; ++v)
            {

                vl_sift_pix dist=row[v];

                output_binary_file.write(reinterpret_cast<char *>(&dist),
                                         sizeof(float));
            
            }            
        }

    }

    output_binary_file.close();


}


//: Load model data and convert to color space if appropriate
void dbskr_test_routines::load_pca_data(vcl_string& M_filename,
                                        vcl_string& mean_filename)
{
    // Open M file
    {
        int rows=0;
        int cols=0;

        vcl_ifstream M_stream(M_filename.c_str());
        
        M_stream>>rows;
        M_stream>>cols;

        vcl_cout<<"PCA tranformation is a matrix of "<<rows<<" by "
                <<cols<<vcl_endl;

        PCA_M_.set_size(rows,cols);
        PCA_M_.fill(0.0);
        
        vl_sift_pix M_data[rows*cols];

        for ( int i=0; i < rows*cols ; ++i)
        {
            M_stream>>M_data[i];
        }


        PCA_M_.set(M_data);

        M_stream.close();
    }

    // Open mean file
    {
        vcl_ifstream mean_stream(mean_filename.c_str());

        int dim=0;
        mean_stream>>dim;
        PCA_mean_.set_size(dim);

        vcl_cout<<"Mean file is length "<<dim<<vcl_endl;

        for ( int i =0; i < dim ; ++i)
        {
            mean_stream>>PCA_mean_[i];
        }
        mean_stream.close();
    }

    
}

//: Load model data and convert to color space if appropriate
void dbskr_test_routines::load_gmm_data(vcl_string& filename)
{
    
    int dimension  = 128;
    int numCenters = 0;

    vcl_ifstream myfile (filename.c_str());
    if (myfile.is_open())
    {
        myfile>>numCenters;
        myfile>>dimension;
        
        keywords_=numCenters;

        vcl_cout<<"Num Centers: "<<keywords_<<vcl_endl;
        vcl_cout<<"Dimension:   "<<dimension<<vcl_endl;

        means_cg_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        covariances_cg_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        priors_cg_ = (float*) vl_malloc(
            sizeof(float)*numCenters);
        
        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>means_cg_[c];

        }

        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>covariances_cg_[c];

        }

        for ( unsigned int c=0; c < numCenters ; ++c)
        {
            myfile>>priors_cg_[c];

        }


    }

    myfile.close();


}

//: Load model files and convert to color space if appropriate
void dbskr_test_routines::load_dc_file(vcl_string& filename)
{
    vcl_ifstream fstream(filename.c_str(), 
                         vcl_ios::in|vcl_ios::binary|vcl_ios::ate);
    float* memblock(0);
    if (fstream.is_open())
    {
        vcl_ifstream::pos_type size = fstream.tellg();
        memblock = new float[size/sizeof(float)];
        fstream.seekg (0, vcl_ios::beg);
        fstream.read ((char *) memblock, size);
        fstream.close();
    }


    float models=memblock[0];
    float query=memblock[1];

    query_points_.resize(query);
    model_points_.resize(models);

    int index=2;
    for ( int i=0; i < query_points_.size() ; ++i)
    {
        vcl_vector<vcl_pair<float,float> > query_xy;
        float points=memblock[index];
        ++index;

        for ( int s=0; s < points ; ++s)
        {
            float x(0),y(0);
            
            x=memblock[index];
            ++index;
            y=memblock[index];
            ++index;

            vcl_pair<float,float> coord(x,y);
            query_xy.push_back(coord);
        }

        query_points_[i]=query_xy;
    }

    int numb_xys=query_points_[0].size();

    for ( int i=0; i < model_points_.size() ; ++i)
    {
        model_points_[i].resize(query_points_.size());

        for ( int q=0 ; q < query_points_.size() ; ++q)
        {
            vcl_vector<vcl_pair<float,float> > model_xy;
            for ( int s=0; s < numb_xys ; ++s)
            {
                float x(0),y(0);
                
                x=memblock[index];
                ++index;
                y=memblock[index];
                ++index;
                
                vcl_pair<float,float> coord(x,y);
                model_xy.push_back(coord);
            }

            model_points_[i][q]=model_xy;

        }
    }

    delete memblock;
    memblock=0;
}

//: Load query files and convert to color space if appropriate
void dbskr_test_routines::load_query_file(vcl_string& filename)
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
        
        query_masks_.push_back(polygon);

        // Read in image file
        vcl_string query_imagename=vul_file::strip_extension(line);
        vcl_string filename=vul_file::strip_directory(query_imagename);
        
        output_filename_=filename+"_dist_matrix.bin";

        query_imagename=query_imagename+".jpg";

        vil_image_resource_sptr query_img_sptr = 
            vil_load_image_resource(query_imagename.c_str());

        vil_image_view<double> chan_1,chan_2,chan_3;

        query_ni_=query_img_sptr->ni();
        query_nj_=query_img_sptr->nj();

        convert_to_color_space(query_img_sptr,
                               chan_1,
                               chan_2,
                               chan_3,
                               color_space_);
                               
        query_chan_1_.push_back(chan_1);
        query_chan_2_.push_back(chan_2);
        query_chan_3_.push_back(chan_3);
        
    }

}

//: Load model files and convert to color space if appropriate
void dbskr_test_routines::load_model_file(vcl_string& filename)
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
        
        model_masks_.push_back(polygon);

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

vgl_polygon<double> dbskr_test_routines::compute_boundary(
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

void dbskr_test_routines::compute_fvs(
    vcl_vector<vgl_point_2d<double> >& stride_points,
    vl_sift_pix* chan1_grad_data,
    vl_sift_pix* chan2_grad_data,
    vl_sift_pix* chan3_grad_data,
    VlSiftFilt* filter,
    vnl_matrix<vl_sift_pix>& descriptor_matrix)
{

    double scale_1_radius=16;
    double scale_2_radius=12;
    double scale_3_radius=8;
    double scale_4_radius=4;

    double fixed_theta=0.0;

    vcl_vector<vl_sift_pix> descriptors;

    for ( int g=0; g < stride_points.size() ; ++g)
    {
        vgl_point_2d<double> ps1=stride_points[g];
        
        vnl_vector<vl_sift_pix> scale_1_descriptor(PCA_M_.rows(),0.0);
        vnl_vector<vl_sift_pix> scale_2_descriptor(PCA_M_.rows(),0.0);
        vnl_vector<vl_sift_pix> scale_3_descriptor(PCA_M_.rows(),0.0);
        vnl_vector<vl_sift_pix> scale_4_descriptor(PCA_M_.rows(),0.0);
        
        compute_sift_descr(ps1,
                           scale_1_radius,
                           fixed_theta,
                           chan1_grad_data,
                           chan2_grad_data,
                           chan3_grad_data,
                           filter,
                           scale_1_descriptor);
        
        compute_sift_descr(ps1,
                           scale_2_radius,
                           fixed_theta,
                           chan1_grad_data,
                           chan2_grad_data,
                           chan3_grad_data,
                           filter,
                           scale_2_descriptor);
        
        compute_sift_descr(ps1,
                           scale_3_radius,
                           fixed_theta,
                           chan1_grad_data,
                           chan2_grad_data,
                           chan3_grad_data,
                           filter,
                           scale_3_descriptor);
        
        compute_sift_descr(ps1,
                           scale_4_radius,
                           fixed_theta,
                           chan1_grad_data,
                           chan2_grad_data,
                           chan3_grad_data,
                           filter,
                           scale_4_descriptor);
        


        vnl_vector<vl_sift_pix> ldr_1_descriptor =
            linear_embed(scale_1_descriptor);
        vnl_vector<vl_sift_pix> ldr_2_descriptor =
            linear_embed(scale_2_descriptor);
        vnl_vector<vl_sift_pix> ldr_3_descriptor =
            linear_embed(scale_3_descriptor);
        vnl_vector<vl_sift_pix> ldr_4_descriptor =
            linear_embed(scale_4_descriptor);

        vnl_vector<vl_sift_pix> sift_block(4*ldr_1_descriptor.size(),0.0);
        
        for ( unsigned int s=0; s <ldr_1_descriptor.size() ; ++s)
        {
            sift_block.put(s,ldr_1_descriptor[s]);
            sift_block.put(s+ldr_1_descriptor.size(),ldr_2_descriptor[s]);
            sift_block.put(s+2*ldr_1_descriptor.size(),ldr_3_descriptor[s]);
            sift_block.put(s+3*ldr_1_descriptor.size(),ldr_4_descriptor[s]);
            
        }

        int encoding_size = 2 * ldr_1_descriptor.size() * keywords_;
        
        vnl_vector<vl_sift_pix> fv_descriptor;
        fv_descriptor.clear();
        fv_descriptor.set_size(encoding_size);
                
        // run fisher encoding
        vl_fisher_encode
            (fv_descriptor.data_block(), VL_TYPE_FLOAT,
             means_cg_, ldr_1_descriptor.size(), keywords_,
             covariances_cg_,
             priors_cg_,
             sift_block.data_block(), 4,0);
     
        // Apply power normalization
        for ( int f=0; f < fv_descriptor.size() ; ++f)
        {

            fv_descriptor[f]= vnl_math::sgn0(fv_descriptor[f])*(
                vcl_pow(vcl_fabs(fv_descriptor[f]),powernorm_));
        }
        descriptor_matrix.set_column(g,fv_descriptor);
    }
    
}




void dbskr_test_routines::explicit_alignment(
    vcl_vector<vcl_pair<float,float> >& q_pts, 
    vcl_vector<vcl_pair<float,float> >& m_pts,
    vil_image_view<double>& model_chan_1,
    vil_image_view<double>& model_chan_2,
    vil_image_view<double>& model_chan_3,
    vil_image_view<double >& mapped_img)
{

    for ( int i=0; i < q_pts.size() ; ++i)
    {
        vcl_pair<float,float> q_coords=q_pts[i];
        vcl_pair<float,float> m_coords=m_pts[i];
        
        double xx=m_coords.first;
        double yy=m_coords.second;

        if ( xx>= 0 && yy >= 0 )
        {
            double red   = vil_bilin_interp(model_chan_1,xx,yy);
            double green = vil_bilin_interp(model_chan_2,xx,yy);
            double blue  = vil_bilin_interp(model_chan_3,xx,yy);
            
            int x=static_cast<int>(q_coords.first);
            int y=static_cast<int>(q_coords.second);
            
            mapped_img(x,y,0)=red;
            mapped_img(x,y,1)=green;
            mapped_img(x,y,2)=blue;
        }
    }
    



}
void dbskr_test_routines::compute_query_gradients()
{
    
    for ( unsigned int i=0; i < query_masks_.size() ; ++i)
    {
        vl_sift_pix* query_chan1_grad_data(0);
        vl_sift_pix* query_chan2_grad_data(0);
        vl_sift_pix* query_chan3_grad_data(0);
        
        vgl_polygon<double> poly=query_masks_[i];
        
        compute_grad_color_maps(query_chan_1_[i],
                                &query_chan1_grad_data,
                                poly);
        
        compute_grad_color_maps(query_chan_2_[i],
                                &query_chan2_grad_data,
                                poly);
        
        compute_grad_color_maps(query_chan_3_[i],
                                &query_chan3_grad_data,
                                poly);
        
        query_grad_chan_1_.push_back(query_chan1_grad_data);
        query_grad_chan_2_.push_back(query_chan2_grad_data);
        query_grad_chan_3_.push_back(query_chan3_grad_data);
        
    }
    
}

void dbskr_test_routines::compute_query_test_points()
{
    
    query_test_points_.resize(query_masks_.size());

    for ( unsigned int i=0; i < query_masks_.size() ; ++i)
    {
        vgl_box_2d<double> bbox;
        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(query_masks_[i], false);  
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


        vcl_vector<vgl_point_2d<double> > points;
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
    
                points.push_back(ps1);
            }
        }

        query_test_points_[i]=points;
    }
    
}

void dbskr_test_routines::compute_grad_color_maps(
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


void dbskr_test_routines::compute_sift_descr(
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


void dbskr_test_routines::convert_to_color_space(
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
