#include "init_resource.h"
#include <bprb/bprb_macros.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_func_process.h>
#include <vcl_iostream.h>
#include <vcl_cstddef.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_measure.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#include <vcl_iostream.h>
#include <vbl/io/vbl_io_smart_ptr.h>

using namespace std;
bool init_resource::create_videostream()
{
	string s(this->_path);
	//vidl_image_list_stream tmp(static_cast<vcl_string>(s));
	this->_vidstream=new vidl_image_list_istream(static_cast<vcl_string>(s));
	//this->_vidstream=tmp;
	//this->_vidstream=static_cast<vidl_istream*>(tmp);
	if(this->_vidstream)
		return true;
	else{
		vcl_cerr << "could not open files !";
		return false;
	}
}

bool init_resource::create_process()
{
	DECLARE_FUNC_CONS_INIT(bbgm_update_dist_image_stream_process);
	DECLARE_FUNC_CONS(bbgm_save_image_of_process);
	_pro=new bprb_func_process((*bbgm_update_dist_image_stream_process),"update_grimson",(*bbgm_update_dist_image_stream_process_cons),
		(*bbgm_update_dist_image_stream_process_init),0);

	_save_pro=new bprb_func_process((*bbgm_save_image_of_process),"save distribution",(*bbgm_save_image_of_process_cons),0,0);
	return true;
}

bool init_resource::set_inputs()
{
	this->_win_size=10;
	this->_min_stdev=0.5;
	this->_end_frame=10;
	this->_init_var=0.25;
	this->_max_components=3;
	this->_g_tresh=3;
	string s("C:/tmp_vid/bck.bmp");
	this->_save_path=s;
	this->_outVect=new vector<vil_image_view_base_sptr>();

	if (_pro)
	{
#if defined fix
		
					
		
		_pro->set_input(1,new brdb_value_t<vidl_istream_sptr >(_vidstream););
		_pro->set_input(2,new brdb_value_t<int>(_max_components));
		_pro->set_input(3,brdb_value_t<int>(_win_size));
		_pro->set_input(4,brdb_value_t<float>(_init_var));
		_pro->set_input(5,brdb_value_t<float>(_g_tresh));
		_pro->set_input(6,brdb_value_t<float>(_min_stdev));
		_pro->set_input(7,brdb_value_t<int>(_start_frame));
		_pro->set_input(8,brdb_value_t<int>(_end_frame);*/
#endif		
		return true;

	}
	else
	{
		vcl_cerr<<"process has not been created yet."<<endl;
		return false;
	}
}

bool init_resource::fire()
{
	if((_pro)&&(_pro->n_inputs()!=0))
	{
		_pro->init();
		_pro->execute();
		return true;
	}
	else
		return false;

}
bool init_resource::init()
{
  
  vidl_istream_sptr istr = _vidstream;
  if (!(istr && istr->is_open())){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid input stream\n";
    return false;
  }
  if (istr->is_seekable())
    istr->seek_frame(0);
  vidl_frame_sptr f = istr->current_frame();
  if (!f){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::init() -"
             << " invalid initial frame\n";
    return false;
  }
  ni = f->ni(); nj = f->nj();
  vcl_cout << " initialized, stream frame size: " << ni << ", " << nj << ", stream at frame # " << istr->frame_number() << vcl_endl;
  vcl_cout.flush();

 // pro.set_input(0, new brdb_value_t<bbgm_image_sptr>(0));

  return true;
}

bool init_resource::exec()
{
	if (!_vidstream){
    vcl_cerr << "In bbgm_update_dist_image_stream_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
	vidl_istream_sptr istr=_vidstream;
  // Retrieve background image
  bbgm_image_sptr bgm =0;

  //Retrieve max components
  int max_components = this->_max_components;

  //Retrieve window_size
  int window_size = this->_win_size;

  //Retrieve initial_variance
  float initial_variance = this->_init_var;

  //Retrieve g_thresh
  float g_thresh = this->_g_tresh;

  //Retrieve min_stdev
  float min_stdev = this->_min_stdev;

  //Retrieve start frame number
  int start_frame =0;


  //Retrieve end frame number
  int end_frame = this->_end_frame;

  vcl_cout << " will start at frame # " << start_frame << " will end at frame # " << end_frame << vcl_endl;

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type vector_;
  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture<gauss_type> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  if (!bgm) {
    model_sptr = new bbgm_image_of<obs_mix_gauss_type>(ni, nj, obs_mix_gauss_type());
    vcl_cout << " Initialized the bbgm image\n";
    vcl_cout.flush();

  }
  else model_sptr = bgm;
  bbgm_image_of<obs_mix_gauss_type> *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type>*>(model_sptr.ptr());

  bsta_gauss_t init_gauss(vector_(0.0f), vector_(initial_variance) );

#if 0
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components);
#endif
  bsta_mg_grimson_window_updater<mix_gauss_type> updater( init_gauss,
                                                          max_components,
                                                          g_thresh,
                                                          min_stdev,
                                                          window_size);


  while (istr->advance() && (end_frame<0||(int)(istr->frame_number()) <= end_frame)) {
    // get frame from stream
    if ((int)(istr->frame_number()) >= start_frame) {
      vidl_frame_sptr f = istr->current_frame();
      vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*f);
      if (!fb)
        return false;
      vil_image_view<float> frame = *vil_convert_cast(float(), fb);
      if (fb->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
        vil_math_scale_values(frame,1.0/255.0);

      update(*model,frame,updater);
      vcl_cout << "updated frame # "<< istr->frame_number()
               << " format " << fb->pixel_format() << " nplanes "
               << fb->nplanes()<< '\n';
      vcl_cout.flush();
    }
  }

  _output = model_sptr;
 
  return true;


}
bool init_resource::save()
{
	 if (!_output) {
    vcl_cerr << "In bbgm_save_image_of_process::execute - invalid inputs\n";
    return false;
  }

	 vcl_string binary_filename = this->_save_path;

  vsl_b_ofstream ostr(binary_filename);
  if (!ostr) {
    vcl_cerr << "Failed to load background image from "
             << binary_filename << vcl_endl;
    return false;
  }

  vsl_b_ostream& bos = static_cast<vsl_b_ostream&>(ostr);

  bbgm_image_sptr bgm=_output;
  if (!bgm) {
    vcl_cerr << "Null background image\n";
    return false;
  }

  vsl_b_write(bos, bgm);



}

bool init_resource::measure_prob(vil_image_view_base_sptr img_ptr,string name)
{
 // Sanity check
 
  // Retrieve background image

  bbgm_image_sptr bgm =_output;
  if (!bgm) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null distribution image\n";
    return false;
  }
  vcl_string image_type = bgm->is_a();
  //for now just check for parzen_sphere in the string
  vcl_size_t str_indx = image_type.find("parzen_sphere");
  bool parzen = str_indx != vcl_string::npos;

  if (!img_ptr) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> image = *vil_convert_cast(float(), img_ptr);
  if (img_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(image,1.0/255.0);

  unsigned np = image.nplanes();

  //Retrieve attribute to measure, e.g. probability
  vcl_string attr = "probability";

  //Retrieve measure tolerance
  float tolerance = 0.005;

  vil_image_view<float> result;
  if (np ==1) {
    typedef bsta_gauss_f1 bsta_gauss1_t;
    typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
    typedef bsta_mixture<gauss_type1> mix_gauss_type1;
    typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
    bbgm_image_of<obs_mix_gauss_type1> *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type1>*>(bgm.ptr());

    if (attr=="probability") {
      bsta_probability_functor<mix_gauss_type1> functor_;
      measure(*model, image, result, functor_, tolerance);
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      bsta_prob_density_functor<mix_gauss_type1> functor_;
      measure_bkground(*model, image, result, functor_, tolerance);
    }
#endif // MEASURE_BKGROUND
    else {
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  if (np ==3)
  {
    typedef bsta_gauss_if3 bsta_gauss3_t;
    typedef bsta_gauss3_t::vector_type vector3_;
    typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
    typedef bsta_mixture<gauss_type3> mix_gauss_type3;
    typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
#if 0
    typedef bsta_parzen_sphere<float, 3> parzen_f3_t;
    typedef parzen_f3_t::vector_type pvtype_;
#endif
    if (attr=="probability") {
      if (!parzen) {
        bbgm_image_of<obs_mix_gauss_type3> *model =
          static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_probability_functor<mix_gauss_type3> functor_;
        measure(*model, image, result, functor_, tolerance);
      }
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      if (!parzen) {
        bbgm_image_of<obs_mix_gauss_type3> *model =
          static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_prob_density_functor<mix_gauss_type3> functor_;
        measure_bkground(*model, image, result, functor_, tolerance);
      }
    }
#endif // MEASURE_BKGROUND
    else {
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }

#if defined scale
	vil_image_view_base_sptr tmp =new vil_image_view<float>();
	 _outVect->push_back(result);
#endif
  vil_image_view<vxl_byte> tmp;
  string base("C:/temp_back/");
  string final=base+name;
  vil_convert_stretch_range(result,tmp);
  vil_save(tmp,final.c_str());
  
  return true;


}




bool init_resource::save_bck(int index,string name)
{
#if defined scale
  string base("C:/temp_back/");
  string final=base+name;
  vil_save(_outVect->at(index),final.c_str());
  return true;
#endif
  return false;
}

float init_resource::max(vil_image_view_base_sptr im)
{
#if defined scale
	float max=0;
	for (int i=0;i<im->ni();i++)
		for(int j=0;j<im->nj();j++)
			if( im(i,j)>max)
				max=im(i,j);
#endif
	return 0;
	
}


init_resource::~init_resource()
{
	
}