// This is fine/contrib/dbseg_mean_shift_process.cxx

//:
// \file

#include "dbseg_mean_shift_process.h"

//: Constructor
dbseg_mean_shift_process::dbseg_mean_shift_process() : bpro1_process()
{
    if( !parameters()->add( "Greyscale?"    , "-greyscale" , true   ) ||
      !parameters()->add( "Filtering Spatial Radius" , "-fiSpatialR" , (int)7   ) ||
      !parameters()->add( "Filtering Range Radius"   , "-fiRangeR"    , 30.0f ) ||
      //!parameters()->add( "Fusing Range Radius"      , "-fuRangeR"    , 6.5f ) ||
      !parameters()->add( "Fusing Minimum Density"   , "-fuDensity"    , (int)50 )
      
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_mean_shift_process::~dbseg_mean_shift_process()
{
}


//: Clone the process
bpro1_process*
dbseg_mean_shift_process::clone() const
{
  return new dbseg_mean_shift_process(*this);
}


//: Return the name of the process
vcl_string dbseg_mean_shift_process::name()
{
  return "Mean Shift Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_mean_shift_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_mean_shift_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_mean_shift_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_mean_shift_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_mean_shift_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_mean_shift_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_mean_shift_process::execute() - "
             << "not exactly one input image\n";
    return false;
  }
    clear_output();
    vul_timer t;
    t.mark();

    // get image from the storage class
    vidpro1_image_storage_sptr image_stg;
    image_stg.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = image_stg->get_image();

     int fiSpatialR, fuDensity;
    float fiRangeR;
    bool greyscale;
    parameters()->get_value("-greyscale", greyscale);
    parameters()->get_value("-fiSpatialR", fiSpatialR);
    parameters()->get_value("-fiRangeR", fiRangeR);
    parameters()->get_value("-fuDensity", fuDensity);

    dbseg_mean_shift_msImageProcessor* meanShift = new dbseg_mean_shift_msImageProcessor();
    byte* image_representation;
    int ni = image_sptr->ni();
    int nj = image_sptr->nj();
    int lw = ni*nj;
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();
    
    //pass image to the Mean Shift algorithm (requires unsigned char array of RGB vectors)
    if (greyscale) {
        image_representation = new byte[lw+1];
        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {
                image_representation[j*ni+i] = imageView(i,j,0);
            }
        }
        meanShift->DefineImage(image_representation, GRAYSCALE, nj, ni);
    }
    else {
        image_representation = new byte[lw*3+1];
        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {
                for (int k = 0; k < 3; k++) {
                    image_representation[(j*ni+i)*3+k] = imageView(i,j,k);
                }
            }
        }
        meanShift->DefineImage(image_representation, COLOR, nj, ni);
    }
    meanShift->Segment(fiSpatialR, fiRangeR, fuDensity, NO_SPEEDUP);
    
    //get results of Mean Shift
    meanShift->GetResults(image_representation);
    int** labels = new int*[lw];
    int** modePointCounts = new int*[lw];
    float** modes = new float*[lw];
    int regionCount = meanShift->GetRegions(labels, modes, modePointCounts);
    
    //create an image from the Mean Shift results
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(imageView.ni(), imageView.nj(),3);
    if (greyscale) {
        for (int i = 0; i < lw; i++) {
            for (int k = 0; k < 3; k++) {
                (*segmented_view)(i%ni, i / ni, k) = image_representation[i];
            }
        }
    }
    else {
        for (int i = 0; i < lw*3; i++) {
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 0) = image_representation[i];
            i++;
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 1) = image_representation[i];
            i++;
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 2) = image_representation[i];
        }
    }

    vil_image_resource_sptr resource = vil_new_image_resource_of_view(*segmented_view);
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);
    vcl_cout << "Mean Shift segmentation completed. " << regionCount << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
    output_data_[0].push_back(segmented_storage);

    delete image_representation;
    delete labels;
    delete modePointCounts;
    delete modes;
    delete meanShift;
  return true;

}


//: Finish
bool
dbseg_mean_shift_process::finish()
{
  return true;
}




