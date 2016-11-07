#pragma once

#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_istream.h>
#include <vul/vul_file.h>
#include <vcl_string.h>
#include <bprb/bprb_func_process.h>
#include <string>
#include <bbgm/bbgm_image_sptr.h>
#include <vector>
using namespace std;
class init_resource
{
public:
	init_resource(char* input_path):_path(input_path){}
    ~init_resource();
	bool create_videostream();
	bool create_process();
	bool set_inputs();
	bool fire();
	bool retrieve_output();
	bool init();
	bool exec();
	bool save();
	bool save_bck(int index,string name);
	bool normalizeMax();
	float max(vil_image_view_base_sptr im);
	bool measure_prob(vil_image_view_base_sptr img_ptr,string name);

private:
	char* _path;
	vidl_istream_sptr _vidstream;
	bprb_func_process* _pro, * _save_pro;
    int _max_components,_win_size, _start_frame,_end_frame, ni,nj;
	float _init_var,_g_tresh,_min_stdev;
	string _save_path;
	bbgm_image_sptr _output;
	vector<vil_image_view_base_sptr> * _outVect;

};