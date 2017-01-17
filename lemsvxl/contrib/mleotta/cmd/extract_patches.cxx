// This is mleotta/cmd/extract_patches.cxx

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vcl_ios.h>
#include <vcl_iomanip.h>


#include <dbpro/dbpro_executive.h>
#include <dbpro/dbpro_basic_processes.h>
#include <dbpro/dbpro_ios_processes.h>
#include <dbpro/dbpro_vsl_processes.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <bil/algo/bil_scale_image.h>

#include <dbvidl2/pro/dbvidl2_source.h>
#include <dbvidl2/pro/dbvidl2_sink.h>
#include <dbvidl2/pro/dbvidl2_frame_to_resource.h>
#include <dbvidl2/pro/dbvidl2_resource_to_frame.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_project.h>
#include <modrec/modrec_feature_3d.h>

#include <dbdet/dbdet_keypoint_sptr.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_surf_keypoint.h>
#include <vsl/vsl_vector_io.h>






//: Read surf keypoints from an ifstream
class dbpro_surf_ifstream_source : public dbpro_ifstream_source_base
{
  public:
    dbpro_surf_ifstream_source(const vcl_string& filename) { open(filename.c_str()); }

    bool open(const vcl_string& filename)
    {
      ifs.open(filename.c_str());
      return ifs.is_open();
    }

    //: Execute the process
    dbpro_signal execute()
    {
      if(ifs.eof()){
        return DBPRO_EOS;
      }
      unsigned d,n;
      ifs >> d >> n;
      if(ifs.eof()){
        return DBPRO_EOS;
      }
      vcl_cout << "size: " << n<<vcl_endl;
      assert(d == 64);
      vcl_vector<dbdet_keypoint_sptr> keypoints;
      for(unsigned i=0; i<n; ++i){
        dbdet_surf_keypoint* s = new dbdet_surf_keypoint;
        ifs >> *s;
        keypoints.push_back(s);
      }
      output(0, keypoints);
      return DBPRO_VALID;
    }

    vcl_ifstream ifs;
};


class build_scale_space_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vil_image_resource_sptr));
      vil_image_resource_sptr image_sptr = input<vil_image_resource_sptr>(0);

      //vil_image_view<vxl_byte> image = vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view());
      vil_image_view<vxl_byte> image = image_sptr->get_view();

      vil_image_resource_sptr image_rsc = vil_new_image_resource_of_view(image);
      vil_image_view_base_sptr imagef = vil_convert_cast(float(), image_rsc->get_view());

      // determine the number of scale octaves
      int min_size = (image.ni() < image.nj())?image.ni():image.nj();
      unsigned num_octaves = 2;
      while( (min_size/=2) >= 8) ++num_octaves;

      float prior_sigma = 1.6f;
      // build the gaussian scale image
      bil_scale_image<float> simg(imagef,3,num_octaves,prior_sigma,-1);

      output(0, simg);

      return DBPRO_VALID;
    }
};


vcl_vector<vil_image_view<float> >
extract_patches(const bil_scale_image<float>& simg,
                const vcl_vector<dbdet_keypoint_sptr>& keypoints)
{
  vcl_vector<vil_image_view<float> > patches;

  typedef vcl_vector<dbdet_keypoint_sptr>::const_iterator kpt_itr;
  for(kpt_itr k=keypoints.begin(); k!=keypoints.end(); ++k)
  {
    double s = (*k)->scale();

      // extract the gradient images for this scale
    double init_scale = simg.init_scale();
    unsigned int num_lvl = simg.levels();
    int first_oct = simg.first_octave();

    double log2_scale = vcl_log(s/init_scale)/vcl_log(2.0)-first_oct;
    unsigned int index = (unsigned int)(log2_scale*num_lvl +0.5);
    int oct = index/num_lvl;
    unsigned int lvl = index%num_lvl;
    oct += first_oct;

    const vil_image_view<float> & img = simg(oct,lvl);


    // compute position relative to the current image size
    float img_scale = simg.image_scale(oct);
    double key_x = (*k)->x()/img_scale;
    double key_y = (*k)->y()/img_scale;
    double orient = (*k)->orientation();
    double so = vcl_sin(orient);
    double co = vcl_cos(orient);

    // compute scale relative to the current image size
    float rel_scale = float(s/init_scale)/img_scale;

    vil_image_view<float> patch(16,16,img.nplanes());
    patch.fill(0.0f);

    for (int i=0; i<16; ++i){
      for (int j=0; j<16; ++j){
        double x = ( (i-7.5)*co -(j-7.5)*so) * rel_scale;
        double y = ( (i-7.5)*so +(j-7.5)*co) * rel_scale;
        double total_w = 0.0;
        for(int c=0; c<4; ++c){
          int xc = (int)vcl_floor(x+key_x) + c/2;
          int yc = (int)vcl_floor(y+key_y) + c%2;
          double interp_x = 1.0 - vcl_fabs( x+key_x - double(xc) );
          double interp_y = 1.0 - vcl_fabs( y+key_y - double(yc) );
          float weight = interp_x*interp_y;
          total_w += weight;
          if ( xc>=0 && xc<int(img.ni()) &&
              yc>=0 && yc<int(img.nj()) ){
            for(unsigned p=0; p<img.nplanes(); ++p)
              patch(i,j,p) += img(xc,yc,p)*weight;
          }else{
            for(unsigned p=0; p<img.nplanes(); ++p)
              patch(i,j,p) += 128.0f*weight;
          }
        }
        assert(total_w > .99 && total_w < 1.01);

      }
    }

    patches.push_back(patch);
  }

  return patches;
}


class extract_patches_filter : public dbpro_filter
{
  public:
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<dbdet_keypoint_sptr>));
      vcl_vector<dbdet_keypoint_sptr> keypoints = 
        input<vcl_vector<dbdet_keypoint_sptr> >(0);

      assert(input_type_id(1) == typeid(bil_scale_image<float>));
      bil_scale_image<float> simg = input<bil_scale_image<float> >(1);

      vcl_vector<vil_image_view<float> > p = extract_patches(simg,keypoints);


      output(0, p);
      output(1, vil_new_image_resource_of_view(p[0]));

      return DBPRO_VALID;
    }
};


class save_patches_sink : public dbpro_sink
{
  public:
    save_patches_sink(const vcl_string& path, const vidl_istream_sptr& is)
    : path_(path), count_(0), istream_(is) {}
    //: Execute this process
    dbpro_signal execute()
    {
      assert(input_type_id(0) == typeid(vcl_vector<vil_image_view<float> >));
      vcl_vector<vil_image_view<float> > patches =
        input<vcl_vector<vil_image_view<float> > >(0);

      unsigned sx = static_cast<unsigned>(vcl_ceil(vcl_sqrt(patches.size())));
      unsigned sy = static_cast<unsigned>(vcl_ceil(double(patches.size())/sx));
      unsigned sb = patches[0].ni();
      vcl_cout <<"saving "<<sx<<" by "<<sy<<" grid" << vcl_endl;
      vil_image_view<vxl_byte> image(sx*sb,sy*sb,patches[0].nplanes());
      image.fill(0);
      for(unsigned i=0; i<patches.size(); ++i){
        const vil_image_view<float>& pf = patches[i];
        unsigned x = i%sx, y = i/sx;
        vil_image_view<vxl_byte> pb = vil_crop(image,x*sb,sb,y*sb,sb);
        vil_convert_cast(pf,pb);
      }

      const vidl_image_list_istream* ils = dynamic_cast<const vidl_image_list_istream*>(istream_.ptr());
      assert(ils);
      vcl_string path = ils->current_path();
      path = path_ + '/'+ vul_file::strip_directory(path);

      vil_save(image,path.c_str());
      ++count_;

      return DBPRO_VALID;
    }

    vcl_string path_;
    unsigned count_;
    vidl_istream_sptr istream_;
};


int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_keypoints("-keypoints", "path to keypoints", "");
  vul_arg<unsigned>    a_dim("-dim", "feature descriptor dimension", 128);
  vul_arg<vcl_string>  a_images("-images", "path to images", "");
  vul_arg<vcl_string>  a_patches("-patches", "path to patches output", "");
  vul_arg_parse(argc, argv);


  typedef vcl_vector<dbdet_keypoint_sptr> keypoint_vector;
  vsl_add_to_binary_loader(dbdet_keypoint());
  vsl_add_to_binary_loader(dbdet_lowe_keypoint());

  dbpro_executive graph;
#ifndef NDEBUG
  graph.enable_debug();
#endif

  switch(a_dim()){
    case 64:
    {
      graph["keypoint_src"]= new dbpro_surf_ifstream_source(a_keypoints());
    }
    break;
    case 128:
    {
      graph["keypoint_src"]= new dbpro_b_istream_source<keypoint_vector>(a_keypoints());
    }
    break;
    default:
      vcl_cerr << "features with dimension "<<a_dim()<<" not supported" << vcl_endl;
  }

  vidl_istream_sptr img_str = new vidl_image_list_istream(a_images());
  graph["image_src"]   = new dbvidl2_source(img_str);
  graph["to_resource"] = new dbvidl2_frame_to_resource(dbvidl2_frame_to_resource::PREFER_WRAP);
  graph["scale_img"]   = new build_scale_space_filter();
  graph["extract"]     = new extract_patches_filter();
  graph["save_patch"]  = new save_patches_sink(a_patches(),img_str);

  //vidl_ostream_sptr ostr = new vidl_image_list_ostream("patches");
  //graph["to_frame"]    = new dbvidl2_resource_to_frame();
  //graph["save_sample"] = new dbvidl2_sink(ostr);


  //===========================================================================
  // Make the graph connections
  //===========================================================================


  graph["save_patch"]  ->connect_input(0,graph["extract"],0);
  graph["extract"]     ->connect_input(0,graph["keypoint_src"],0);
  graph["extract"]     ->connect_input(1,graph["scale_img"],0);
  graph["scale_img"]   ->connect_input(0,graph["to_resource"],0);
  graph["to_resource"] ->connect_input(0,graph["image_src"],0);

  //graph["to_frame"]    ->connect_input(0,graph["extract"],1);
  //graph["save_sample"] ->connect_input(0,graph["to_frame"],0);


  //===========================================================================
  // Run the processing graph
  //===========================================================================

  graph.init();
  graph.run_all();

  return 0;
}
