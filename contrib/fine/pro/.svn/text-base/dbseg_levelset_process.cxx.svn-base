// This is fine/contrib/dbseg_levelset_process.cxx

//:
// \file

#include "dbseg_levelset_process.h"

//: Constructor
dbseg_levelset_process::dbseg_levelset_process() : bpro1_process()
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
dbseg_levelset_process::~dbseg_levelset_process()
{
}


//: Clone the process
bpro1_process*
dbseg_levelset_process::clone() const
{
  return new dbseg_levelset_process(*this);
}


//: Return the name of the process
vcl_string dbseg_levelset_process::name()
{
  return "Level Set Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_levelset_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_levelset_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_levelset_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_levelset_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_levelset_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_levelset_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_levelset_process::execute() - "
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

    
    //char* image_representation;
    int ni = image_sptr->ni();
    int nj = image_sptr->nj();
    int lw = ni*nj;
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();
    vil_image_view<float> image_representation = vil_image_view<float>(imageView.ni(), imageView.nj(), imageView.nplanes());

    for (int i = 0; i < ni; i++) {
        for (int j = 0; j < nj; j++) {
            for (int k = 0; k < imageView.nplanes(); k++) {
                float f = (float)imageView(i,j,k);
                image_representation(i,j,k) = ((float) imageView(i,j,k))/255;
                //vcl_cout << image_representation(i,j,k) << " ";
            }
            
        }
        //vcl_cout << vcl_endl;
    }


    dbacm_standard_levelset_func* levelset = new dbacm_standard_levelset_func();
    levelset->set_levelset_surf(image_representation);
    
    
    levelset->evolve_one_timestep(.2);

    image_representation = levelset->levelset_surf();

    delete levelset;
    //create an image from the Mean Shift results
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(imageView.ni(), imageView.nj(),3);

    
    
    
    for (int i = 0; i < ni; i++) {
        for (int j = 0; j < nj; j++) {
            for (int k = 0; k < imageView.nplanes(); k++) {
                float f = image_representation(i,j,k);
                (*segmented_view)(i,j,k) = (vxl_byte) (image_representation(i,j,k)*255);
            }
        }
    }

    
/*
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
*/
    vil_image_resource_sptr resource = vil_new_image_resource_of_view(*segmented_view);
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);
    vcl_cout << "Level Set segmentation completed. " << 0 << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
    output_data_[0].push_back(segmented_storage);

    /*delete image_representation;
    delete labels;
    delete modePointCounts;
    delete modes;
    delete meanShift;*/
    
  return true;

}


//: Finish
bool
dbseg_levelset_process::finish()
{
  return true;
}






