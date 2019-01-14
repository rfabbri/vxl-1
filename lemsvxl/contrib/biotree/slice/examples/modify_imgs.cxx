
#include <slice/sliceFileManager.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
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

void parse_globbed_filenames(const std::string & input,
                    std::vector<std::string> &filenames)  ;

std::string imgnum(std::string image_filename)
{
          std::string path =  vul_file::dirname(image_filename);
          std::string extension =  vul_file::extension(image_filename);
          std::string prefix = vul_file::basename(image_filename,extension.c_str());
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
  vxl_byte operator()(vxl_byte x) const { return std::abs(x); } 
  unsigned operator()(unsigned x) const { return x; } 
  int operator()(int x)           const { return std::abs(x); } 
  short operator()(short x)       const { return std::abs(x); } 
  float operator()(float x)       const { return std::fabs(x); } 
  double operator()(double x)     const { return std::fabs(x); } 
};

void subtract(const std::vector<std::string>& f1,
              const std::vector<std::string>& f2,
              const std::string& out_prefix)
{
        vil_image_view<float> im_view1 ;
        vil_image_view<float> im_view2 ;
        vil_image_view<float> diff ;

  for(int i = 0; i < f1.size(); i++){
          std::cerr << f1[i] << " - ";
          std::cerr << f2[i] << "\n";
          im_view1 = vil_load(f1[i].c_str());
          im_view2 = vil_load(f2[i].c_str());
          std::cerr << "im_view1.ni = " << im_view1.ni() << "\n";
          std::cerr << "im_view1.nj = " << im_view1.nj() << "\n";
          std::cerr << "im_view1.nplanes = " << im_view1.nplanes() << "\n";
          std::cerr << "im_view2.ni = " << im_view2.ni() << "\n";
          std::cerr << "im_view2.nj = " << im_view2.nj() << "\n";
          std::cerr << "im_view2.nplanes = " << im_view2.nplanes() << "\n";

          vil_math_image_difference(im_view1,im_view2,diff); 
          vil_transform(diff,vil_math_abs_functor());
          std::cerr << "diff.ni = " << diff.ni() << "\n";
          std::cerr << "diff.nj = " << diff.nj() << "\n";
          std::cerr << "diff.nplanes = " << diff.nplanes() << "\n";
          std::string num = imgnum(f1[i]);
          std::string newname = out_prefix;
          newname += num + ".tif";
          std::cerr << newname << "\n";
          vil_save(diff,newname.c_str());
  }
}

void log(const std::vector<std::string>& filenames, const std::string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          std::cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_transform(im_float,vil_math_log_functor());
          std::string num = imgnum(filenames[i]);
          std::string newname = out_prefix;
          newname += num + ".tif";
          std::cerr << newname << "\n";
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
void invert_from(const std::vector<std::string>& filenames, const float& max, const std::string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          std::cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_transform(im_float,vil_math_invert_from_functor(max));
          std::string num = imgnum(filenames[i]);
          std::string newname = out_prefix;
          newname += num + ".tif";
          std::cerr << newname << "\n";
          vil_save(im_float,newname.c_str());
  }
}

void scale_offset(const std::vector<std::string>& filenames,
            const float& scale,
            const float& offset,
            const std::string& out_prefix)
{
  for(int i = 0; i < filenames.size(); i++){
          std::cerr << filenames[i] << "\n";
          vil_image_view<vxl_uint_16> im_view = vil_load(filenames[i].c_str());
          vil_image_view<float> im_float;
          vil_convert_cast(im_view,im_float);
          vil_math_scale_and_offset_values(im_float,scale,offset);
          std::string num = imgnum(filenames[i]);
          std::string newname = out_prefix;
          newname += num + ".tif";
          std::cerr << newname << "\n";
          vil_save(im_float,newname.c_str());
  }


}

int main(int argc, char *argv[])
{

        if(!std::strcmp(argv[1],"log")){
                if(argc == 4){
                        std::cerr << "taking log\n";
                        std::vector<std::string> f1;
                        std::cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        std::cerr << "f1.size = " << f1.size() << "\n";
                        log(f1,argv[3]);
                }
                else{
                        std::cerr << "usage: " << argv[0] << " log [fname pattern] [out prefix]\n";
                }
        }
        if(!std::strcmp(argv[1],"invert")){
                if(argc == 5){
                        float max = std::atof(argv[3]);
                        std::cerr << "subtracting from " << max << "\n";
                        std::vector<std::string> f1;
                        std::cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        std::cerr << "f1.size = " << f1.size() << "\n";
                        invert_from(f1,max,argv[4]);
                }
                else{
                        std::cerr << "usage: " << argv[0] << " log [fname pattern] [invert_from_max] [out prefix]\n";
                }
        }
        else if(!std::strcmp(argv[1],"diff")){
                if(argc == 5){
                        std::cerr << "image subtract\n";
                        std::vector<std::string> f1;
                        std::cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],f1);
                        std::cerr << "f1.size = " << f1.size() << "\n";

                        std::vector<std::string> f2;
                        std::cerr << "parsing filenames matching pattern " << argv[3] << "\n";
                        parse_globbed_filenames(argv[3],f2);
                        std::cerr << "f2.size = " << f2.size() << "\n";

                        subtract(f1,f2,argv[4]);
                }
                else{
                        std::cerr << "usage: " << argv[0] << " diff [fname pattern #1] [fname pattern #2] [out prefix]\n";

                }

        }
        else if(!std::strcmp(argv[1],"scale_offset")){
                if(argc == 6){
                        std::cerr << "scale/offset\n";
                        std::vector<std::string> filenames;
                        std::cerr << "parsing filenames matching pattern " << argv[2] << "\n";
                        parse_globbed_filenames(argv[2],filenames);
                        float scale = std::atof(argv[3]);
                        float offset = std::atof(argv[4]);
                        scale_offset(filenames,scale,offset,argv[5]);
                }
                else{
                        std::cerr << "usage: " << argv[0] << " scale_offset [fname pattern #1] [scale] [offset] [out prefix]\n";

                }
        }
                return 0;
}
