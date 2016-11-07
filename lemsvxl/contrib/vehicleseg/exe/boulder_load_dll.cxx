// boulder_load.cpp : Defines the entry point for the DLL application.
//
#include <dbvidl/dbvidl_boulder_video_codec.h>
#include <dbvidl/dbvidl_boulder_video_codec_sptr.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_movie.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vul/vul_sprintf.h>
#include <vehicleseg/exe/boulder_load_dll.h>
#include <vil/vil_crop.h>


void load_video_roi(unsigned short * roi,char * inputvid, int frameno,int x0,int y0,int ni,int nj)
{
    dbvidl_boulder_video_codec_sptr cdc = new dbvidl_boulder_video_codec();
    vcl_string fname(inputvid);
    vidl1_clip_sptr clip = new vidl1_clip( cdc->load(fname,'r'));
    vidl1_movie_sptr my_movie=new vidl1_movie(clip);
    if(frameno<my_movie->length()){
        vil_image_resource_sptr img=my_movie->get_frame(frameno)->get_resource();
        vil_image_resource_sptr cropimg=vil_crop(img,x0,ni,y0,nj);
        vil_image_view<unsigned short> crpimg=cropimg->get_view();
        //unsigned short *roi=new unsigned short[ni*nj];
        for(unsigned j=0;j<crpimg.nj();j++)
            for(unsigned i=0;i<crpimg.ni();i++)
                 (*roi++)=crpimg(i,j);
    }
}
