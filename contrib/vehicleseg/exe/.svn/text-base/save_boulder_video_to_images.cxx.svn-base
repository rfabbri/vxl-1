#include <dbvidl/dbvidl_boulder_video_codec.h>
#include <dbvidl/dbvidl_boulder_video_codec_sptr.h>
#include <vidl1/vidl1_clip_sptr.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_movie.h>
#include <vil/vil_save.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_arg.h>
#include <brip/brip_vil_float_ops.h>

int main(int argc,char * argv[])
{
    

        vul_arg<vcl_string> fname("-input", "File Name", "");
        vul_arg<vcl_string> outname("-outdir", "OutputDir", "");
        vul_arg<vcl_string> prefix("-prefix", "Prefix", "");
        vul_arg<bool> bit16("-bit", "Is 16 bit", false);
        vul_arg<unsigned> inc("-inc", "Increment", 1);

        vul_arg_parse(argc,argv);

        if(inc()<=0)
            inc()=1;

        dbvidl_boulder_video_codec_sptr cdc = new dbvidl_boulder_video_codec();
        vidl1_clip_sptr clip = new vidl1_clip( cdc->load(fname(),'r'));
        vidl1_movie_sptr my_movie=new vidl1_movie(clip);
        vcl_cout<<"\n saving images";

        for (int i=0;i<my_movie->length();){
            vcl_string filename = vul_sprintf("%s\\%s%05d.%s", outname().c_str(),prefix().c_str(),i,"tif");

            if(bit16())
                {               
                vil_image_view<unsigned short> img=my_movie->get_view(i);
                vil_save(img,filename.c_str(),"tiff");
                }
            else
                {
                vil_image_view<unsigned short> img=my_movie->get_view(i);
                vil_image_view<unsigned char> img8=brip_vil_float_ops::convert_to_byte(img,0,4095);
                vil_save(img8,filename.c_str(),"tiff");
             
            }

            i+=inc();
            }
                return 0; 
   
   
}
