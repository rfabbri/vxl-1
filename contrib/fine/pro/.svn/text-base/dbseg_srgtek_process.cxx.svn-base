// This is contrib/fine/pro/dbseg_srgtek_process.cxx

//:
// \file

#include "dbseg_srgtek_process.h"

//: Constructor
dbseg_srgtek_process::dbseg_srgtek_process() : bpro1_process()
{
}


//: Destructor
dbseg_srgtek_process::~dbseg_srgtek_process()
{
}


//: Clone the process
bpro1_process*
dbseg_srgtek_process::clone() const
{
  return new dbseg_srgtek_process(*this);
}


//: Return the name of the process
vcl_string dbseg_srgtek_process::name()
{
  return "SRGtek Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_srgtek_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_srgtek_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_srgtek_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_srgtek_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_srgtek_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_srgtek_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_srgtek_process::execute() - "
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


     unsigned char* image_representation;
    unsigned char* seed_representation;
    int numseeds = 0;
    int ni = image_sptr->ni();
    int nj = image_sptr->nj();
    int lw = ni*nj;
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();
    vil_image_view<vxl_byte> seedView = seed_sptr->get_copy_view();
    std::set<int> tempset;
    //pass image to the Mean Shift algorithm (requires unsigned char array of RGB vectors)
    //if (greyscale) {
        image_representation = new unsigned char[lw+1];
        seed_representation = new unsigned char[lw+1];
        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {
                image_representation[j*ni+i] = imageView(i,j,0);
                tempset.insert(seedView(i,j,0));
                if ( ((int)seedView(i,j,0)) > numseeds) {
                    numseeds = seedView(i,j,0);
                }
                seed_representation[j*ni+i] = seedView(i,j,0);
            }
        }
        numseeds=tempset.size();
        int* seed_array = new int[lw+1];
        int* aux_seed_array = new int[lw+1];
        int* contour_array = new int[lw+1];
        unsigned char* image_array = image_representation;
        unsigned char* boundary_array = new unsigned char[lw+1];
        float* mean_averages = new float[3*numseeds+1];
        float* ssl_array = new float[4*lw+1];
        for (int q = 0; q < lw; q++) {
        //seed_array = (int*) seed_representation;
            seed_array[q] = (int) seed_representation[q];
        }

        ssl_array[0] = 9999;
        ssl_array[1] = 0;
        ssl_array[2] = 0;
        ssl_array[3] = 0;
        ssl_array[4] = -1;
    
        find_mean_averages(seed_array, image_array, mean_averages, numseeds, nj, ni);

    for(int ii=1; ii<=numseeds; ii++) {
        printf("ii = %i\n", ii);
    
        /*Find the start of a seed region. It calls find_contour
        and find the contour bounding the seed

        Arguments:
        ii                = index runs from 1 to no_of_seeds
        contour_array     = int array (height*width)
        length_of_contour = height*width
        seed_array        = int array (height*width) containing the seeds */
        find_start(ii, contour_array, lw, seed_array, (ni-1), (nj-1));
        initial_neighbors(contour_array, seed_array, boundary_array, 
              aux_seed_array,image_array, mean_averages, 
              ssl_array, numseeds, nj, ni);
    }


      grow_regions(seed_array, boundary_array, aux_seed_array, 
           image_array, ssl_array, 
           numseeds, mean_averages, nj, ni);

        
      for (int q = 0; q < lw; q++) {    
        image_representation[q] = (unsigned char)seed_array[q];
      }
    
    
    
    //get results of Mean Shift
    
    //create an image from the Mean Shift results
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(imageView.ni(), imageView.nj(),3);
    int regionCount = 0;
    //if (greyscale) {
        for (int i = 0; i < lw; i++) {
            for (int k = 0; k < 3; k++) {
                (*segmented_view)(i%ni, i / ni, k) = image_representation[i];
                if (image_representation[i] > regionCount) {
                    regionCount = image_representation[i];
                }
            }
        }
    //}
    
    //colorize the output image    
    std::vector<vxl_byte> rVals;
    std::vector<vxl_byte> gVals;
    std::vector<vxl_byte> bVals;
    rVals.resize(regionCount+1);
    gVals.resize(regionCount+1);
    bVals.resize(regionCount+1);
    for (int i = 0; i <= regionCount; i++) {
        rVals[i] = (vxl_byte) rand();
        gVals[i] = (vxl_byte) rand();
        bVals[i] = (vxl_byte) rand();
    }
    for (int j = 0; j < segmented_view->nj(); j++) {
        for (int i = 0; i < segmented_view->ni(); i++) {
            int temp = image_representation[j*ni+i];
            (*segmented_view)(i,j,0) = rVals[temp];
            (*segmented_view)(i,j,1) = gVals[temp];
            (*segmented_view)(i,j,2) = bVals[temp];
        }
    }


    vil_image_resource_sptr resource = vil_new_image_resource_of_view(*segmented_view);
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);
    vcl_cout << "SRGtek segmentation completed. " << regionCount << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
    output_data_[0].push_back(segmented_storage);

    delete image_representation;
    delete seed_representation;
    delete seed_array;
    delete aux_seed_array;
    delete contour_array;
    //delete image_array;
    delete boundary_array;
    delete mean_averages;
    delete ssl_array;
    /*delete image_representation;
    delete labels;
    delete modePointCounts;
    delete modes;
    delete meanShift;*/
  return true;

}


//: Finish
bool
dbseg_srgtek_process::finish()
{
  return true;
}




