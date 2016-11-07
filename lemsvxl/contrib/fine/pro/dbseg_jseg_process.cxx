// This is contrib/fine/pro/dbseg_jseg_process.cxx

//:
// \file

#include "dbseg_jseg_process.h"

//: Constructor
dbseg_jseg_process::dbseg_jseg_process() : bpro1_process()
{
    if(!parameters()->add( "Color Quantization Threshold (0-600, leave at 0 for automatic, lower value for more regions)" , "-TQUAN" , (float)0   ) ||
      !parameters()->add( "Number of scales (leave at 0 for automatic, lower value for more regions)"   , "-scale"    , (int) 0) ||
      !parameters()->add( "Merge threshold (between 0.0 and 0.7, lower value for more regions)"   , "-mthresh"    , .4f )
      
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_jseg_process::~dbseg_jseg_process()
{
}


//: Clone the process
bpro1_process*
dbseg_jseg_process::clone() const
{
  return new dbseg_jseg_process(*this);
}


//: Return the name of the process
vcl_string dbseg_jseg_process::name()
{
  return "JSEG Segmentation";
}


//: Call the parent function and reset num_frames_
void
dbseg_jseg_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_jseg_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_jseg_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );
  
  return to_return;
}


//: Returns the number of input frames to this process
int
dbseg_jseg_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_jseg_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbseg_jseg_process::execute()
{
    vul_timer t;
    t.mark();
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_jseg_process::execute() - "
             << "not exactly one input image\n";
    return false;
  }
    clear_output();

    // get image from the storage class
    vidpro1_image_storage_sptr image_stg;
    image_stg.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = image_stg->get_image();

     float TQUAN, threshcolor;
    int scale;
    parameters()->get_value("-TQUAN", TQUAN);
    parameters()->get_value("-scale", scale);
    parameters()->get_value("-mthresh", threshcolor);
    vil_image_view<vxl_byte> imageView = image_sptr->get_copy_view();

    




    vil_image_resource_sptr resource = vil_new_image_resource_of_view(static_execute(imageView, TQUAN, threshcolor, scale));
    vidpro1_image_storage_sptr segmented_storage = vidpro1_image_storage_new();
    segmented_storage->set_image(resource);
    vcl_cout << "JSEG segmentation completed. " << "unknown" << " regions created.  Time elapsed: " << t.real()/1000 << " seconds." << vcl_endl; 
    output_data_[0].push_back(segmented_storage);

  return true;

}


//: Finish
bool
dbseg_jseg_process::finish()
{
  return true;
}


//static execute
vil_image_view<vxl_byte> dbseg_jseg_process::static_execute(vil_image_view<vxl_byte> imageView, float TQUAN, float threshcolor, int scale) {
    unsigned char* image_representation1;
    unsigned char* image_representation;
    
    int ni = imageView.ni();
    int nj = imageView.nj();
    int lw = ni*nj;
    
    image_representation1 = new unsigned char[lw*3+1];
    
    //turn VXL image into unsigned char array for the algorithm
    for (int j = 0; j < nj; j++) {
        for (int i = 0; i < ni; i++) {
            for (int k = 0; k < 3; k++) {
                image_representation1[(j*ni+i)*3+k] = imageView(i,j,k);
            }
        }
    }

    //run JSEG algorithm
    float* LUV = new float[lw*3+1];
    float** cb = (float**) fmatrix(256, 3);
    rgb2luv(image_representation1, LUV, lw*3);
    
    if (TQUAN == 0) {
        TQUAN = -1; 
    }
    if (scale == 0) {
        scale = -1;
    }
    int N = quantize(LUV, cb, 1, nj, ni, 3, TQUAN);
    unsigned char* cmap = new unsigned char[lw];
    image_representation = new unsigned char[lw*3+1];
    getcmap(LUV, cmap, cb, lw, 3, N);
    int i,j,k;
    j=0;
    for (i=0;i<lw;i++) 
      for (k=0;k<3;k++) LUV[j++] = cb[cmap[i]][k];


    luv2rgb(image_representation, LUV, lw*3);
    char* blank = new char[3];
    int TR = segment(image_representation, cmap, N, 1, nj, ni,image_representation1, blank, blank, 1, 3, scale, .5, false, 1);

    TR = merge1(image_representation,cmap,N,1,nj,ni,TR,threshcolor);





    //create an image from the JSEG results
    vil_image_view<vxl_byte>* segmented_view = new vil_image_view<vxl_byte>(imageView.ni(), imageView.nj(),3);
    vector<int> r;
    vector<int> g;
    vector<int> b;
    vector<int> count;
    count.resize(TR+1);
    r.resize(TR+1);
    g.resize(TR+1);
    b.resize(TR+1);
    r.assign(TR+1, 0);
    g.assign(TR+1, 0);
    b.assign(TR+1, 0);
    count.assign(TR+1, 0);
    for (i = 0; i < lw; i++) {
        for (k = 0; k < 3; k++) {
            (*segmented_view)(i%ni, i / ni, k) = image_representation[i];
        }
        int a = (int)image_representation[i];
        count[(int)image_representation[i]]++;
        r[(int)image_representation[i]]+=(int)imageView(i%ni, i / ni, 0);
        g[(int)image_representation[i]]+=(int)imageView(i%ni, i / ni, 1);
        b[(int)image_representation[i]]+=(int)imageView(i%ni, i / ni, 2);
    }

    for (i = 0; i <= TR; i++) {
        if (count[i] != 0) {
            r[i] = r[i]/count[i];
            g[i] = g[i]/count[i];
            b[i] = b[i]/count[i];
        }
    }

    for (i = 0; i < ni; i++) {
        for (j = 0; j < nj; j++) {
            (*segmented_view)(i,j,0) = r[(*segmented_view)(i,j,0)];
            (*segmented_view)(i,j,1) = g[(*segmented_view)(i,j,1)];
            (*segmented_view)(i,j,2) = b[(*segmented_view)(i,j,2)];
        }
    }
    
    delete cmap;
    delete image_representation;
    delete image_representation1;
    delete LUV;
    delete cb;
    return *segmented_view;
}




