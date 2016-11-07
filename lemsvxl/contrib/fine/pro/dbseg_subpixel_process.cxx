// This is fine/contrib/dbseg_subpixel_process.cxx

//:
// \file

#include "dbseg_subpixel_process.h"

//: Constructor
dbseg_subpixel_process::dbseg_subpixel_process() : bpro1_process()
{
    if( //!parameters()->add( "Greyscale?"    , "-greyscale" , true   ) ||
      !parameters()->add( "Number of iterations" , "-numitr" , (int)10   ) 
      //!parameters()->add( "Filtering Range Radius"   , "-fiRangeR"    , 30.0f ) ||
      //!parameters()->add( "Fusing Range Radius"      , "-fuRangeR"    , 6.5f ) ||
      //!parameters()->add( "Fusing Minimum Density"   , "-fuDensity"    , (int)50 )
      
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_subpixel_process::~dbseg_subpixel_process()
{
}


//: Clone the process
bpro1_process*
dbseg_subpixel_process::clone() const
{
  return new dbseg_subpixel_process(*this);
}


//: Return the name of the process
vcl_string dbseg_subpixel_process::name()
{
  return "Subpixel Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_subpixel_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_subpixel_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_subpixel_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_subpixel_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_subpixel_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_subpixel_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_subpixel_process::execute() - "
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

    vidpro1_image_storage_sptr seed_stg;
    seed_stg.vertical_cast(input_data_[0][1]);
    vil_image_resource_sptr seed_sptr = seed_stg->get_image();


     int numitr;
    //float fiRangeR;
    //bool greyscale;
    parameters()->get_value("-numitr", numitr);
//    parameters()->get_value("-fispatialr", fispatialr);
//    parameters()->get_value("-firanger", firanger);
//    parameters()->get_value("-fudensity", fudensity);

    SP_Bubble_Deform spbub_deform;
    double* image_representation;
    unsigned char* seed_representation;
    int ni = image_sptr->ni();
    int nj = image_sptr->nj();
    int lw = ni*nj;
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();
    vil_image_view<vxl_byte> seedView = seed_sptr->get_copy_view();
    
    //pass image to the Mean Shift algorithm (requires unsigned char array of RGB vectors)
    //if (greyscale) {
        image_representation = new double[lw+1];
        seed_representation = new unsigned char[lw+1];
        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {
                image_representation[j*ni+i] = imageView(i,j,0);
                seed_representation[j*ni+i] = seedView(i,j,0);
            }
        }
        initialize_sp_bub_deform_args(image_representation,seed_representation,nj,ni,numitr,&spbub_deform);
        //initialize_sp_bub_deform(image_representation,seed_representation,nj,ni,0,numitr,-0.1,0.0,&spbub_deform);
    //}
        subpixel_bubble_evolution(&spbub_deform); 

        image_representation = spbub_deform.bubbles.surface;
    /*else {
        image_representation = new byte[lw*3+1];
        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {
                for (int k = 0; k < 3; k++) {
                    image_representation[(j*ni+i)*3+k] = imageView(i,j,k);
                }
            }
        }
        meanShift->DefineImage(image_representation, COLOR, nj, ni);
    }*/
    
    
    //get results of Mean Shift
    
    //create an image from the Mean Shift results
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(imageView.ni(), imageView.nj(),3);
    //if (greyscale) {
        for (int i = 0; i < lw; i++) {
            for (int k = 0; k < 3; k++) {
                (*segmented_view)(i%ni, i / ni, k) = image_representation[i];
            }
        }
    //}
    /*else {
        for (int i = 0; i < lw*3; i++) {
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 0) = image_representation[i];
            i++;
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 1) = image_representation[i];
            i++;
            (*segmented_view)( (i/3)%ni, (i/3) / ni, 2) = image_representation[i];
        }
    }*/

    vil_image_resource_sptr resource = vil_new_image_resource_of_view(*segmented_view);
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);
    //vcl_cout << "Mean Shift segmentation completed. " << regionCount << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
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
dbseg_subpixel_process::finish()
{
  return true;
}




