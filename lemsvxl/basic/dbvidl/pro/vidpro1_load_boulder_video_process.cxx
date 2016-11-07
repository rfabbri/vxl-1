// This is brl/vidpro1/process/vidpro1_load_boulder_video_process.cxx

//:
// \file

#include <dbvidl/pro/vidpro1_load_boulder_video_process.h>
#include <dbvidl/dbvidl_boulder_video_codec_sptr.h>
#include <dbvidl/dbvidl_boulder_video_codec.h>
#include <vcl_iostream.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_new.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_crop.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_frame.h>
#include <vidl1/vidl1_clip.h>

#include <vidl1/vidl1_io.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
//: Constructor
vidpro1_load_boulder_video_process::vidpro1_load_boulder_video_process() : bpro1_process(), num_frames_(0)
{
    if( !parameters()->add( "Video file <filename...>" , "-video_filename", bpro1_filepath("","*"))||
        !parameters()->add( "Start Frame" , "-start",int(0) )||
        !parameters()->add( "End Frame" , "-end",int(-1) )||
        !parameters()->add( "Increment" , "-inc",int(1) )||
        !parameters()->add( "Is ROI" , "-isroi",bool(false) )||
        !parameters()->add( "ROI(top x)" , "-topx",int(-1) )||
        !parameters()->add( "ROI(top y)" , "-topy",int(-1) )||
        !parameters()->add( "ROI(Length x)" , "-lenx",int(-1) )||
        !parameters()->add( "ROI(Length y)" , "-leny",int(-1) )
        )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
vidpro1_load_boulder_video_process::~vidpro1_load_boulder_video_process()
{
}


//: Clone the process
bpro1_process*
vidpro1_load_boulder_video_process::clone() const
{
    return new vidpro1_load_boulder_video_process(*this);
}


//: Return the name of the process
vcl_string vidpro1_load_boulder_video_process::name()
{
    return "Load Boulder Video";
}


//: Call the parent function and reset num_frames_
void
vidpro1_load_boulder_video_process::clear_output()
{
    bpro1_process::clear_output();
    num_frames_ = 0;
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
vidpro1_load_boulder_video_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
vidpro1_load_boulder_video_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );

    return to_return;
}


//: Returns the number of input frames to this process
int
vidpro1_load_boulder_video_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro1_load_boulder_video_process::output_frames()
{
    return num_frames_;
}


//: Run the process on the current frame
bool
vidpro1_load_boulder_video_process::execute()
{
    bpro1_filepath video_path;
    static int topx_, topy_;
    static int lenx_,leny_;
    static bool isroi;
    static int start;
    static int end;
    static int inc;

    parameters()->get_value( "-video_filename" , video_path );
    
    
    parameters()->get_value( "-start" , start );
    parameters()->get_value( "-end" , end );
    parameters()->get_value( "-inc" , inc );
    
    parameters()->get_value( "-topx" , topx_ );
    parameters()->get_value( "-topy" , topy_ );
    parameters()->get_value( "-lenx" , lenx_ );
    parameters()->get_value( "-leny" , leny_ );
    parameters()->get_value( "-isroi" , isroi );


    vcl_string video_filename = video_path.path;


    
    dbvidl_boulder_video_codec_sptr cdc = new dbvidl_boulder_video_codec();
    vidl1_codec_sptr codec= cdc->load(video_filename.c_str(),'r');
    if(end<start)
        end=cdc->length();
    vidl1_clip_sptr clip = new vidl1_clip( codec, start, end, inc);
    vidl1_movie_sptr my_movie=new vidl1_movie(clip);
        
   
    
    if (!my_movie) {
        vcl_cerr << "Failed to load movie file: "<< video_filename << vcl_endl;
        return false;
    }

    output_data_.clear();
    vidl1_movie::frame_iterator pframe = my_movie->last();
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    int counter=0;
#endif
    for (; pframe >= my_movie->first(); --pframe){
        vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
        vil_image_resource_sptr img=pframe->get_resource();
        if(isroi)
        {
            int ni=img->ni();
            int nj=img->nj();
            if(topx_<ni)
                topx_=vcl_max(0,topx_);
            else
                topx_=0;

            if(topy_<nj)
                topy_=vcl_max(0,topy_);
            else
                topy_=0;

            if(lenx_>0)
                lenx_=vcl_min(ni-topx_,lenx_);
            else
                lenx_=0;

            if(leny_>0)
                leny_=vcl_min(nj-topy_,leny_);
            else
                leny_=0;
            vil_image_resource_sptr cropimg=vil_crop(img,topx_,lenx_,topy_,leny_);
            image_storage->set_image( cropimg );
        }
        else
            image_storage->set_image( img );

        output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,image_storage));
        }
        
    
    num_frames_ = my_movie->length();

    return true;
   
   
}


//: Finish
bool
vidpro1_load_boulder_video_process::finish() 
{
    return true;
}



