// This is contrib/fine/pro/dbseg_graph_process.cxx

//:
// \file
#include "dbseg_graph_process.h"

//: Constructor
dbseg_graph_process::dbseg_graph_process() : bpro1_process()
{
    if( !parameters()->add( "Sigma"    , "-sigma" , (float)1   ) ||
      !parameters()->add( "k" , "-k" , (float)3   ) ||
      !parameters()->add( "Min Size"   , "-min_size"    , (int)7 )      
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_graph_process::~dbseg_graph_process()
{
}


//: Clone the process
bpro1_process*
dbseg_graph_process::clone() const
{
  return new dbseg_graph_process(*this);
}


//: Return the name of the process
vcl_string dbseg_graph_process::name()
{
  return "Graph Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_graph_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_graph_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_graph_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_graph_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_graph_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_graph_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
    clear_output();
    vul_timer t;
    t.mark();

    // get image from the storage class
    vidpro1_image_storage_sptr image_stg;
    image_stg.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = image_stg->get_image();

     float sigma, k;
    int min_size, regionCount;
    parameters()->get_value("-sigma", sigma);
    parameters()->get_value("-k", k);
    parameters()->get_value("-min_size", min_size);

    int ni = image_sptr->ni();
    int nj = image_sptr->nj();
    int lw = ni*nj;
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();
    
    //turn the image into something the algorithm will accept
    image<rgb>* rgb_image = new image<rgb>(ni, nj);
    for (int j = 0; j < nj; j++) {
        for (int i = 0; i < ni; i++) {
            imRef(rgb_image, i, j).r = imageView(i,j,0);
            if (imageView.nplanes() > 1) {
                imRef(rgb_image, i, j).g = imageView(i,j,1);
                imRef(rgb_image, i, j).b = imageView(i,j,2);
            }
            else {
                imRef(rgb_image, i, j).g = imageView(i,j,0);
                imRef(rgb_image, i, j).b = imageView(i,j,0);
            }

        }
    }
    
    //run segmentation
    image<rgb>* segmented_rgb = segment_image(rgb_image, sigma, k, min_size, &regionCount);
    
    //convert the output to VXL format
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(ni, nj,3);

    for (int j = 0; j < nj; j++) {
        for (int i = 0; i < ni; i++) {
            (*segmented_view)(i,j,0) = imRef(segmented_rgb, i, j).r;
            (*segmented_view)(i,j,1) = imRef(segmented_rgb, i, j).g;
            (*segmented_view)(i,j,2) = imRef(segmented_rgb, i, j).b;
            
        }
    }

    vil_image_resource_sptr resource = vil_new_image_resource_of_view(*segmented_view);
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);


    vcl_cout << "Graph segmentation completed. " << regionCount << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
    output_data_[0].push_back(segmented_storage);

    return true;

}


//: Finish
bool
dbseg_graph_process::finish()
{
  return true;
}






