#include <dbvidl/dbvidl_boulder_video_codec.h>
#include <dbvidl/dbvidl_boulder_video_codec_sptr.h>
#include <vidl1/vidl1_clip_sptr.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_movie.h>
#include <vil/vil_save.h>
#include <vul/vul_sprintf.h>
#include <dbvidl/dbvidl_parse_meta_file.h>
#include <vul/vul_file.h>

int main(int argc,char * argv[])
{
    if(argc!=4)
    {
        vcl_cout<<"Usage : .exe bmffile outputpath increment\n";
        return -1;
    }
    else
    {
        vcl_string fname(argv[1]);
        vcl_string outname(argv[2]);
        int inc=atoi(argv[3]);
        if(inc<1)
            inc=1;
        dbvidl_parse_meta_file pmf;
        pmf.parse(fname);

        vcl_list<vcl_string> filenames=pmf.getlist();
        vcl_list<vcl_string>::iterator iter;
        vul_file fileprocess;
        vcl_string dirname=fileprocess.dirname(fname);
        int count=0;    
        int outercount=0;
        for(iter=filenames.begin();iter!=filenames.end();iter++)
        {
            vcl_string currvideo=dirname+"\\\\"+(*iter);
            dbvidl_boulder_video_codec_sptr cdc = new dbvidl_boulder_video_codec();
            vidl1_clip_sptr clip = new vidl1_clip( cdc->load(currvideo.c_str(),'r'));
            vidl1_movie_sptr my_movie=new vidl1_movie(clip);
            for (int i=0;i<my_movie->length();i++){
                ++count;
                if(count==inc)
                {    
                    count=0;
                    vil_image_view<unsigned short> img=my_movie->get_view(i);
                    vcl_string filename = vul_sprintf("%s%05d.%s", outname.c_str(),outercount,"tif");
                    vil_save(img,filename.c_str(),"tiff");
                }
                outercount++;
            }

        }
        return 0; 
    }
    return 0;
}
