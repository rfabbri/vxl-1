
#include <slice/sliceFileManager.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vil/vil_save.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_transform.h>
#include <vil/vil_load.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/file_formats/vil3d_slice_list.h>

void parse_globbed_filenames(const vcl_string & input,
                    vcl_vector<vcl_string> &filenames)  ;

vcl_string imgnum(vcl_string image_filename)
{
          vcl_string path =  vul_file::dirname(image_filename);
          vcl_string extension =  vul_file::extension(image_filename);
          vcl_string prefix = vul_file::basename(image_filename,extension.c_str());
          int i;
          int digitcount = 0;
          for( i = prefix.length()-1; i >= 0; i--){
                  if(prefix[i] < '0' || prefix[i] > '9') break;
                  digitcount++;
          }

          return prefix.substr(prefix.length()-digitcount,digitcount);
}

class vil_math_abs_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return vcl_abs(x); } 
  unsigned operator()(unsigned x) const { return x; } 
  int operator()(int x)           const { return vcl_abs(x); } 
  short operator()(short x)       const { return vcl_abs(x); } 
  float operator()(float x)       const { return vcl_fabs(x); } 
  double operator()(double x)     const { return vcl_fabs(x); } 
};

void subtract(const vcl_vector<vcl_string>& f1,
              const vcl_vector<vcl_string>& f2,
              const vcl_string& out_prefix)
{
        vil_image_view<float> im_view1 ;
        vil_image_view<float> im_view2 ;
        vil_image_view<float> diff ;

  for(int i = 0; i < f1.size(); i++){
          vcl_cerr << f1[i] << " - ";
          vcl_cerr << f2[i] << "\n";
          im_view1 = vil_load(f1[i].c_str());
          im_view2 = vil_load(f2[i].c_str());
          vcl_cerr << "im_view1.ni = " << im_view1.ni() << "\n";
          vcl_cerr << "im_view1.nj = " << im_view1.nj() << "\n";
          vcl_cerr << "im_view1.nplanes = " << im_view1.nplanes() << "\n";
          vcl_cerr << "im_view2.ni = " << im_view2.ni() << "\n";
          vcl_cerr << "im_view2.nj = " << im_view2.nj() << "\n";
          vcl_cerr << "im_view2.nplanes = " << im_view2.nplanes() << "\n";

          vil_math_image_difference(im_view1,im_view2,diff); 
          vil_transform(diff,vil_math_abs_functor());
          vcl_cerr << "diff.ni = " << diff.ni() << "\n";
          vcl_cerr << "diff.nj = " << diff.nj() << "\n";
          vcl_cerr << "diff.nplanes = " << diff.nplanes() << "\n";
          vcl_string num = imgnum(f1[i]);
          vcl_string newname = out_prefix;
          newname += num + ".tif";
          vcl_cerr << newname << "\n";
          vil_save(diff,newname.c_str());
  }
}

void log(const vcl_vector<vcl_string>& filenames, const vcl_string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          vcl_cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_transform(im_float,vil_math_log_functor());
          vcl_string num = imgnum(filenames[i]);
          vcl_string newname = out_prefix;
          newname += num + ".tif";
          vcl_cerr << newname << "\n";
          vil_save(im_float,newname.c_str());
  }
}

class vil_math_invert_from_functor
{
        private: float max_;
 public:
                 vil_math_invert_from_functor(float max):max_(max){}
  vxl_byte operator()(vxl_byte x) const { return vxl_byte(max_-(float)x);}
  unsigned operator()(unsigned x) const { return unsigned(max_-(float)x);}
  int operator()(int x)           const { return int(max_-(float)x);}
  short operator()(short x)       const { return short(max_-(float)x);}
  float operator()(float x)       const { return max_-x;}
  double operator()(double x)     const { return double(max_-x);}
};
void invert_from(const vcl_vector<vcl_string>& filenames, const float& max, const vcl_string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          vcl_cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_transform(im_float,vil_math_invert_from_functor(max));
          vcl_string num = imgnum(filenames[i]);
          vcl_string newname = out_prefix;
          newname += num + ".tif";
          vcl_cerr << newname << "\n";
          vil_save(im_float,newname.c_str());
  }
}

void scale_offset(const vcl_vector<vcl_string>& filenames,
            const float& scale,
            const float& offset,
            const vcl_string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          vcl_cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_math_scale_and_offset_values(im_float,scale,offset);
          vcl_string num = imgnum(filenames[i]);
          vcl_string newname = out_prefix;
          newname += num + ".tif";
          vcl_cerr << newname << "\n";
          vil_save(im_float,newname.c_str());
  }


}

int main(int argc, char *argv[])
{

        if(!vcl_strcmp(argv[1],"log")){
                if(argc == 4){
                        vcl_cerr << "taking log\n";
                        vcl_vector<vcl_string> f1;
                        vcl_cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        vcl_cerr << "f1.size = " << f1.size() << "\n";
                        log(f1,argv[3]);
                }
                else{
                        vcl_cerr << "usage: " << argv[0] << " log [fname pattern] [out prefix]\n";
                }
        }
        if(!vcl_strcmp(argv[1],"invert")){
                if(argc == 5){
                        float max = vcl_atof(argv[3]);
                        vcl_cerr << "subtracting from " << max << "\n";
                        vcl_vector<vcl_string> f1;
                        vcl_cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        vcl_cerr << "f1.size = " << f1.size() << "\n";
                        invert_from(f1,max,argv[4]);
                }
                else{
                        vcl_cerr << "usage: " << argv[0] << " log [fname pattern] [invert_from_max] [out prefix]\n";
                }
        }
        else if(!vcl_strcmp(argv[1],"diff")){
                if(argc == 5){
                        vcl_cerr << "image subtract\n";
                        vcl_vector<vcl_string> f1;
                        vcl_cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        vcl_cerr << "f1.size = " << f1.size() << "\n";

                        vcl_vector<vcl_string> f2;
                        vcl_cerr << "parsing filenames matching pattern " << argv[3] << "\n";
                        parse_globbed_filenames(argv[3],f2);
                        vcl_cerr << "f2.size = " << f2.size() << "\n";

                        subtract(f1,f2,argv[4]);
                }
                else{
                        vcl_cerr << "usage: " << argv[0] << " diff [fname pattern #1] [fname pattern #2] [out prefix]\n";

                }

        }
        else if(!vcl_strcmp(argv[1],"scale_offset")){
                if(argc == 6){
                        vcl_cerr << "scale/offset\n";
                        vcl_vector<vcl_string> filenames;
                        vcl_cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],filenames);
                        float scale = vcl_atof(argv[3]);
                        float offset = vcl_atof(argv[4]);
                        scale_offset(filenames,scale,offset,argv[5]);
                }
                else{
                        vcl_cerr << "usage: " << argv[0] << " scale_offset [fname pattern #1] [scale] [offset] [out prefix]\n";

                }
        }
                return 0;
}
