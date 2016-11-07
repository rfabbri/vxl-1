//this is /contrib/bm/vlfeat/tests/vlfeat_test_dsift.cxx
#include<testlib/testlib_test.h>
#include<testlib/testlib_root_dir.h>

#include"../vlfeat.h"

#include<vcl_iomanip.h>
#include<vcl_sstream.h>
#include<vcl_cmath.h>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>
#include<vil/vil_convert.h>

#include<vnl/vnl_matrix.h>

static void vlfeat_test_dsift()
{
	vcl_string src_path = testlib_root_dir();
	vcl_cout << src_path << vcl_endl;

	vcl_string filename = src_path + "/contrib/brl/lemsvxlsrc/contrib/bm/vlfeat/tests/kermit000.jpg";
	vcl_string matlab_descr = src_path + "/contrib/brl/lemsvxlsrc/contrib/bm/vlfeat/tests/d.txt";
	vcl_string matlab_frame = src_path + "/contrib/brl/lemsvxlsrc/contrib/bm/vlfeat/tests/f.txt";
	
	vnl_matrix<float> matlab_descr_matrix(1,128);

	vcl_ifstream descrFile(matlab_descr.c_str());
	vcl_ifstream frameFile(matlab_frame.c_str());

	unsigned maxLines = 500;

	vcl_vector<vgl_point_2d<float> > frames;
	if( frameFile.is_open() )
	{
		
		unsigned line_num = 0;
		while( !frameFile.eof() && (line_num<maxLines) )
		{
			++line_num;
			vcl_string line;
			vcl_getline(frameFile,line);
			vcl_stringstream ss(line);

			while(!ss.eof())
			{
				float x,y;
				ss >> vcl_skipws >> x >> y;
				//we subtract 1 from img locations because matlab pixel indexing starts from (1,1) not (0,0) as in vxl/c++.
				frames.push_back(vgl_point_2d<float>((x-1),(y-1)));
			}
		}
	}//end if(frameFile.is_open())
	else
	{	
		vcl_cerr << "ERROR: vlfeat_test_dsift -- FAILED TO OPEN FILE: " << matlab_frame << vcl_flush;
		testlib_test_perform(false);
	}
	
	vcl_map<vgl_point_2d<float>, vcl_vector<float>, vlfeat_vgl_point_2d_coord_compare<float> > matlab_gt_map;
	if( descrFile.is_open() )
	{
		unsigned line_num=0;
		while( !descrFile.eof() && (line_num<maxLines) )//only check the first 500 lines.
		{
			
			vcl_vector<float> descr;
			vcl_string line;
			vcl_getline(descrFile,line);
			vcl_stringstream ss(line);

			while(!ss.eof())
			{
				float val;
				ss >> vcl_skipws >> val;
				descr.push_back(val);
			}
			matlab_gt_map[frames[line_num]] = descr;
			++line_num;
		}//end while(gtfile.good())
	}//end if( gtfile.is_open() )
	else
	{
		vcl_cerr << "ERROR: vlfeat_test_dsift -- FAILED TO OPEN FILE: " << matlab_descr << vcl_flush;
		testlib_test_perform(false);
	}
	
	

	vil_image_view<vxl_byte> grey_img, img =  vil_load(filename.c_str());
	vil_image_view<float> grey_float_img;
	vil_convert_planes_to_grey(img,grey_img);
	vil_convert_cast<vxl_byte,float>(grey_img,grey_float_img);

	vcl_map<vgl_point_2d<float>, vcl_vector<float>, vlfeat_vgl_point_2d_coord_compare<float> > dsift_descriptor_map;

	unsigned step = 5;
	unsigned size = 4;
	bool flat_window = false;

	dsift_descriptor_map = vlfeat::dsift( grey_float_img, step, size, flat_window );

	vcl_map<vgl_point_2d<float>, vcl_vector<float>, vlfeat_vgl_point_2d_coord_compare<float> >::const_iterator m_itr, m_end = matlab_gt_map.end();

	for(m_itr = matlab_gt_map.begin(); m_itr != m_end; ++m_itr)
	{
		//look up the matching frame from the wrapper result
		vcl_vector<float> wrapper_vector = dsift_descriptor_map[m_itr->first];
		vcl_vector<float> matlab_vector = m_itr->second;
		float accum = 0;
		for(unsigned i = 0; i < 128; ++i)
			accum+=((wrapper_vector[i]-matlab_vector[i])*(wrapper_vector[i]-matlab_vector[i]));
		
		//L2 norm error
		accum = vcl_sqrt(accum);
		TEST_NEAR("VLfeat c++ wrapper desriptor Near matlab ground truth: ", accum, 0.0f, .01);
	}

	
	vgl_point_2d<float> pt=  frames[372];
	vcl_vector<float> wtemp = dsift_descriptor_map[frames[372]];
	vcl_vector<float> mtemp = matlab_gt_map[frames[372]];

	vgl_point_2d<float> pt2 = frames[233];
	vcl_vector<float>wtemp2 = dsift_descriptor_map[pt2];
	vcl_vector<float>mtemp2 = matlab_gt_map[pt2];

	vgl_point_2d<float>pt3 = frames[94];
	vcl_vector<float>wtemp3 = dsift_descriptor_map[pt3];
	vcl_vector<float>mtemp3 = matlab_gt_map[pt3];

}//end vlfeat_test_dsift()

TESTMAIN( vlfeat_test_dsift );