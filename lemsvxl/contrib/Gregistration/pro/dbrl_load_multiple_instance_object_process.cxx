#include "dbrl_load_multiple_instance_object_process.h"
#include<bseg/brip/brip_vil_float_ops.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<vul/vul_sprintf.h>
#include<vcl_cstdio.h>
#include <vsl/vsl_vector_io.h>
#include <dbru/dbru_label_sptr.h>
#include <dbru/dbru_label.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <dbru/dbru_multiple_instance_object_sptr.h>
dbrl_load_multiple_instance_object_process::dbrl_load_multiple_instance_object_process() : bpro1_process(),num_frames_(0)
{
    if(!parameters()->add( "Input file" ,                    "-name" ,           bpro1_filepath("","*"))
      )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
    
}


/*************************************************************************
* Function Name: dbrl_load_multiple_instance_object_process::~dbrl_load_multiple_instance_object_process
* Parameters: 
* Effects: 
*************************************************************************/

dbrl_load_multiple_instance_object_process::~dbrl_load_multiple_instance_object_process()
{
   
}


//: Clone the process
bpro1_process*
dbrl_load_multiple_instance_object_process::clone() const
{
    return new dbrl_load_multiple_instance_object_process(*this);
}


/*************************************************************************
* Function Name: dbrl_load_multiple_instance_object_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbrl_load_multiple_instance_object_process::name()
{
    return "Load Multiple Instance";
}


/*************************************************************************
* Function Name: ddbrl_load_multiple_instance_object_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbrl_load_multiple_instance_object_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
}

void
dbrl_load_multiple_instance_object_process::clear_output(int resize)
{
  num_frames_ = 0;
  bpro1_process::clear_output(resize);  
}

/*************************************************************************
* Function Name: dbrl_load_multiple_instance_object_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbrl_load_multiple_instance_object_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
    return to_return;
}
//: Returns the number of input frames to this process
int
dbrl_load_multiple_instance_object_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
dbrl_load_multiple_instance_object_process::output_frames()
{
    return num_frames_;
}


/*************************************************************************
* Function Name: ddbrl_load_multiple_instance_object_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_load_multiple_instance_object_process::execute()
{
    bpro1_filepath input_path;
    parameters()->get_value( "-name" , input_path );
    vcl_string path = input_path.path;

    vsl_b_ifstream ifile(path.c_str());
    unsigned numframes=0;
    vsl_b_read(ifile,numframes);

    num_frames_ = numframes;
    for(unsigned i=0;i<numframes;i++)
    {
        vcl_vector<dbru_multiple_instance_object_sptr> temp;
        vsl_b_read(ifile,temp);
        vidpro1_vsol2D_storage * vsols= new vidpro1_vsol2D_storage();
        vidpro1_vsol2D_storage * edgevsols= new vidpro1_vsol2D_storage();
        for(unsigned i=0;i<temp.size();i++)
            vsols->add_object(temp[i]->get_poly()->cast_to_spatial_object());//temp[i]->get_label()->category());

        for(unsigned i=0;i<temp.size();i++)
        {
            vcl_vector<vsol_line_2d_sptr> lines=temp[i]->get_edges();
            for(unsigned j=0;j<lines.size();j++)
            {
                edgevsols->add_object(lines[j]->cast_to_spatial_object());
            }
        }

        vcl_vector< bpro1_storage_sptr > stores;
        stores.push_back(vsols);
        stores.push_back(edgevsols);
        output_data_.push_back(stores);
    }

    vcl_reverse(output_data_.begin(),output_data_.end());
    //int index=-1;
    //num_frames_=0;
    //do
    //{
    //    index=-1;
    //    vsl_b_read(ifile,index);
    //    vcl_vector<dbrl_multiple_instance_object_sptr> temp;
    //    vsl_b_read(ifile,temp);
    //    vidpro1_vsol2D_storage * vsols= new vidpro1_vsol2D_storage();
    //    for(unsigned i=0;i<temp.size();i++)
    //        vsols->add_object(temp[i]->get_poly()->cast_to_spatial_object());//temp[i]->get_label()->category());
    //    vcl_vector< bpro1_storage_sptr > stores;
    //    stores.push_back(vsols);
    //    output_data_.push_back(stores);
    //    num_frames_++;

    //}while(index>-1);


   return true;
}


/*************************************************************************
* Function Name: dbrl_load_multiple_instance_object_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_load_multiple_instance_object_process::finish()
{
    return true;
}


