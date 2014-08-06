//This is brcv/seg/dbdet/pro/dbdet_save_edg_seq_process.cxx

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

#include "dbdet_save_edg_seq_process.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>

static vcl_string num2str_frame (int n);

dbdet_save_edg_seq_process::dbdet_save_edg_seq_process() : bpro1_process()
{

  if( !parameters()->add( "Output file <filename...>" , "-edgoutput" , bpro1_filepath("","*.edg")) ||
	  !parameters()->add( "Starting frame" , "-sframe" , (int)1) ||
	  !parameters()->add( "Num of frames to save" , "-numframes" , (int)1) || // it will track back numframes from current frame and put in input_data_
	  !parameters()->add( "Start index of saving files" , "-sindex" , (int)1) 
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
/*
  int sframe = 0;
  parameters()->get_value("-sframe",sframe);
  int numframes=0;
  parameters()->get_value("-numframes",numframes);
  
  vidpro1_repository_sptr repo = bvis1_manager::instance()->repository();
  repo->go_to_frame( sframe + numframes -1 );*/
}

vcl_string dbdet_save_edg_seq_process::name() 
{
  return "Save .EDG seq Files";
}

vcl_vector< vcl_string > dbdet_save_edg_seq_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map");
  return to_return;
}

vcl_vector< vcl_string > dbdet_save_edg_seq_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

//: Returns the number of input frames to this process
int
dbdet_save_edg_seq_process::input_frames()
{
   return 1;
}

//: Clone the process
bpro1_process*
dbdet_save_edg_seq_process::clone() const
{
  return new dbdet_save_edg_seq_process(*this);
}


bool dbdet_save_edg_seq_process::execute()
{  

  int numframes=0;
  parameters()->get_value("-numframes",numframes);
  int sframe = 0;
  parameters()->get_value("-sframe",sframe);
  int sindex = 0;
  parameters()->get_value("-sindex",sindex);

  bpro1_filepath output;
  parameters()->get_value( "-edgoutput" , output );
  vcl_string output_file = output.path;

  vidpro1_repository_sptr repo = bvis1_manager::instance()->repository();

  for (int i = 0 ; i<numframes; i++ )
  {
	  //get the input storage class
	  dbdet_edgemap_storage_sptr input_edgemap;
	  bpro1_storage_sptr repomap = repo->get_data_by_name_at(input_names()[0], sframe + i-1);
	  input_edgemap.vertical_cast(repomap);
	  dbdet_edgemap_sptr edgemap = input_edgemap->get_edgemap();
	  //vcl_ostringstream convert;   // stream used for the conversion
	  //convert << (sindex + i);      // insert the textual representation of 'Number' in the characters in the stream
	  //vcl_string output_file_1 = output_file + "_" + convert.str() + ".edg";
	  vcl_string output_file_1 = output_file + "_" + num2str_frame(sindex+i) + ".edg";
	  vcl_cout<< output_file_1 << vcl_endl;
	  //save this edge map onto a file
	  bool retval = dbdet_save_edg(output_file_1, edgemap);

	  if (!retval) {
		vcl_cerr << "Error while saving file: " << output_file_1 << vcl_endl;
		return false;
	  }

  }
  return true;
}

static vcl_string num2str_frame (int n)
{
	vcl_ostringstream convert;   // stream used for the conversion
	if (n<=9999)
	{
		convert << floor(n/1000) << floor(fmod(n, 1000)/100) << floor(fmod(fmod(n, 1000),100)/10) << fmod(fmod(fmod(n, 1000),100), 10);
	}
	else if(n>9999 && n<99999)
	{
		convert << floor(n/10000) << floor(fmod(n, 10000)/1000) << floor(fmod(fmod(n, 10000),1000)/100) << floor(fmod(fmod(fmod(n, 10000),1000), 100)/10) << fmod(fmod(fmod(fmod(n, 10000),1000), 100), 10);
	}
	else{
		vcl_cerr<< "number of frames exceed 99999!" << vcl_endl;
	}
	return convert.str();
}


