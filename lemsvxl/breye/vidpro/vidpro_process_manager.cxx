// This is brl/bpro/vidpro_process_manager.cxx

//:
// \file

#include "vidpro_process_manager.h"

#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <vul/vul_arg.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

#include <bpro/bpro_storage.h>
#include <bpro/bpro_process.h>
#include <vidpro/vidpro_repository.h>

#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/process/vidpro_open_istream_process.h>
#include <vidpro/storage/vidpro_obj_stream_storage.h>


#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_ostream.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>

#include <vsl/vsl_binary_io.h>


//: Constructor
vidpro_process_manager::vidpro_process_manager()
{
  recording_macro_=false;
  allow_costum_window_ = false;
  repository_sptr = new vidpro_repository();
  window_size_=1;
  absolute_frame_=0;
}


//: Destructor
vidpro_process_manager::~vidpro_process_manager()
{
}

bool
vidpro_process_manager::recording_macro()
{
  return recording_macro_;
}

void
vidpro_process_manager::start_recording_macro()
{
  recording_macro_ = true;
}

void
vidpro_process_manager::stop_recording_macro()
{
  recording_macro_ = false;
}

void
vidpro_process_manager::set_window_size(int size)
{
  window_size_ = size;
}

int
vidpro_process_manager::window_size()
{
  return window_size_;
}


void
vidpro_process_manager::set_costum_window(int size)
{
  allow_costum_window_ = true;
  costum_window_=size;
}

void 
vidpro_process_manager::unset_costum_window()
{
  allow_costum_window_=false;
}

bool
vidpro_process_manager::check_process_ostreams(bpro_process_sptr const &process,
                                                vcl_string const & ostream_name,
                                                vcl_string const & ostream_type) 
{

  //find the ostream corresponding to active process

 // vcl_map<vcl_string, vcl_string> ostreams = repository_sptr->get_process_ostreams(process);

 // for ( vcl_map<vcl_string, vcl_string>::iterator it = ostreams.begin();it != ostreams.end();  ++it)
 // {  
 //   //isa: is this necessary, or closing the streams is enough?
 ///*   if(ostream_name == it->first && ostream_type ==it->second)
 //   {
 //     DATABASE->update_selected_tuple_value(selec, "ostream", ostream_name);
 //     DATABASE->update_selected_tuple_value(selec, "type", ostream_type);

 //   }*/

 //   DATABASE->add_tuple("ostream-process", new brdb_tuple(process->name(),ostream_name,ostream_type));


 // }

  DATABASE->add_tuple("ostream-process", new brdb_tuple(process->name(),ostream_name,ostream_type));
  return false;

}

bool
vidpro_process_manager::set_process_ostreams (const bpro_process_sptr& process, 
                                               vcl_map<vcl_string, vcl_vector<vcl_string> > & output_list,
                                               bool has_ostream)
{

  if (!has_ostream)
  {
    vcl_string name = "NONE";
    vcl_string type = "NONE";
    //add or update in ostream-process relation
    this->check_process_ostreams(process,name,type);
    return true;
  }

  //if the user wants to save the ostream need to:

  //find if the ostream extis in memory 
  for(vcl_map<vcl_string, vcl_vector<vcl_string> >::iterator mit = output_list.begin(); mit!=output_list.end(); mit++)
  {
    vcl_string type =mit->first;

    for(vcl_vector<vcl_string>::iterator it = (mit->second).begin(); it!=(mit->second).end(); it++)
    {

      vcl_string name =(*it);

      vcl_string ostream_name = name + "_os";

      vcl_string ostream_dir;
      vcl_string glob;

      ostream_dir= vul_file::get_cwd() + '/' + ostream_name;
      glob= ostream_dir+ '/'+ '*'; 

      //if the directory exists, old files are remove
      if (vul_file::is_directory(ostream_dir))
      {
        vcl_string current_dir= vul_file::get_cwd();
        vul_file::change_directory (ostream_dir.c_str());
        vcl_string filename;

        for (vul_file_iterator fit=glob.c_str(); fit; ++fit) {

          // check to see if file is a directory.
          if (vul_file::is_directory(fit()))
            continue;

          filename=fit.filename();
          vul_file::delete_file_glob(filename.c_str());
        }

        vul_file::change_directory (current_dir.c_str());

      }

      else
        vul_file::make_directory_path(ostream_dir);


      if(type=="image")
          type= "ostream";
      else
          type = "object stream";

      if(type=="ostream")
      {
          vcl_string ostream_name = name + "_os";
          if (!repository_sptr->exist_in_global(name, type))
          {
              this->new_ostream(ostream_dir,ostream_name,"%05d","tiff",0);
              vcl_cout<<vul_file::get_cwd()<<"\n";
          }

      }

      if(type=="object stream")
      {
          vcl_string ostream_name = name + "_os";
          if (!repository_sptr->exist_in_global(name, type))
              this->new_obj_stream(ostream_dir, ostream_name);

      }

      //add or update in ostream-process relation
      this->check_process_ostreams(process,ostream_name,type);         
      return true;

    }
  }
  return true;
}

//: Set the process manager to use a new repository

void
vidpro_process_manager::set_repository(const vidpro_repository_sptr& rep)
{
  this->repository_sptr = rep;
  process_queue.clear();
}

//: Return a smart pointer to the repository
vidpro_repository_sptr
vidpro_process_manager::get_repository_sptr() const
{
  return repository_sptr;
}

//takes care of seeing streams as images for processes
bool
vidpro_process_manager::get_process_input_names(vcl_map<vcl_string,vcl_vector<vcl_string> > &input_names,
                                                 const bpro_process_sptr& process )
{

  vcl_vector<vcl_string> input_types = process->get_input_type();

  if(input_types.empty())
  {
    input_names.clear();
    return false;
  }

  for(vcl_vector<vcl_string>::iterator it = input_types.begin(); it!=input_types.end(); it++)
  {

    if ( (*it)== "image")
    { 
      vcl_vector<vcl_string> images; 
      if(this->recording_macro())
        images= repository_sptr->get_all_storage_class_names("image");

      //remove unprocessed and image_from_istream from the list
      for(unsigned i= 0; i<images.size(); i++)
      {
        if(!((images[i].rfind("_unpro"))==vcl_string::npos) || !(((images[i]).rfind("image_from"))==vcl_string::npos))
        {
          images.erase(images.begin()+i);
          i--;

        }
      }


      vcl_vector<vcl_string> i_streams = repository_sptr->get_all_storage_class_names("istream");

      if (!i_streams.empty())
      {
        for(vcl_vector<vcl_string>::iterator vis_it = i_streams.begin(); vis_it!=i_streams.end(); vis_it++)
        {
          images.push_back(*vis_it);
        }
      }

      input_names.insert(vcl_pair<vcl_string,vcl_vector<vcl_string> >((*it),images));

    }
    else
    {
      vcl_vector<vcl_string> types = repository_sptr->get_all_storage_class_names(*it);
      input_names.insert(vcl_pair<vcl_string,vcl_vector<vcl_string> >((*it),types));
    }
  }
  return true;
}


//: Adds any missing input_names 
//takes care of seeing streams as images for processes
bool
vidpro_process_manager::get_process_output_names(vcl_map<vcl_string,vcl_vector<vcl_string> > &output_names,
                                                  const bpro_process_sptr& process )
{

  vcl_vector<vcl_string> output_types = process->get_output_type();

  if(output_types.empty())
  {
    output_names.clear();
    return false;
  }

  for(vcl_vector<vcl_string>::iterator it = output_types.begin(); it!=output_types.end(); it++)
  {

    if ( (*it)== "image")
    {
      vcl_vector<vcl_string> images = repository_sptr->get_all_storage_class_names("image");
      vcl_vector<vcl_string> o_streams = repository_sptr->get_all_storage_class_names("ostream");

      if (!o_streams.empty())
      {
        for(vcl_vector<vcl_string>::iterator vos_it = o_streams.begin(); vos_it!=o_streams.end(); vos_it++)
        {
          images.push_back(*vos_it);
        }
      }
      output_names.insert(vcl_pair<vcl_string,vcl_vector<vcl_string> >((*it),images));
    }
    else
    {
      vcl_vector<vcl_string> types = repository_sptr->get_all_storage_class_names(*it);
      output_names.insert(vcl_pair<vcl_string,vcl_vector<vcl_string> >((*it),types));
    }
  }
  return true;
}




bool 
vidpro_process_manager::add_missing_input_names(vcl_vector<vcl_string> &input_names, const bpro_process_sptr& process)
{
  //Find if process selected a video stream as a input

  int i=0;
  for( vcl_vector<vcl_string>::const_iterator it= input_names.begin(); it!=input_names.end(); it++, i++)
  {
    bpro_storage_sptr sto = repository_sptr->get_global_storage_by_name(input_names[i]);

    vcl_string type;
    repository_sptr->get_storage_type(sto, type);

    if(type== "istream")
    {
      vidpro_istream_storage_sptr vis_sto;
      vis_sto.vertical_cast(sto);
      if(!vis_sto)
        return false;

      input_names[i] = "image_from_" + input_names[i];
      continue;
    }
  }

  return true;
}


bool
vidpro_process_manager::go_to_next_frame()
{
  absolute_frame_++;

  int current_frames_ = repository_sptr->current_frame()+2;

  if(repository_sptr->current_frame() < 0)
  {
    vcl_cerr << "ERROR: initializing process inputs, current frame is negative " <<vcl_endl;
    return false;
  }

  if(allow_costum_window_ && window_size_ < costum_window_)
    window_size_ = costum_window_;

  bool advanced= false;   
  bool loading_required = false;

  vcl_set<bpro_storage_sptr> vistreams = repository_sptr->get_all_storage_classes("istream");
  vcl_set<bpro_storage_sptr> obj_streams = repository_sptr->get_all_storage_classes("object stream");

  vcl_map<vcl_string, vcl_set<bpro_storage_sptr> > streams;
 
  vidpro_istream_storage_sptr vistream_sto;
  vidl_istream_sptr vistream;
  vidpro_obj_stream_storage_sptr obj_stream_sto;
  vidpro_object_stream_sptr obj_stream;
  int global_frame =0;

  if (!vistreams.empty())
  streams.insert(vcl_pair<vcl_string, vcl_set<bpro_storage_sptr> >("istream", vistreams));
  
  if (!obj_streams.empty())
  streams.insert(vcl_pair<vcl_string, vcl_set<bpro_storage_sptr> >("object stream", obj_streams));

  for(vcl_map<vcl_string, vcl_set<bpro_storage_sptr> >::iterator mit = streams.begin();
    mit!= streams.end(); mit++)
  {
    vcl_string type = mit->first;
    vcl_set<bpro_storage_sptr> streams = mit->second;

    vcl_set<bpro_storage_sptr>::iterator it = streams.begin();
    for( ; it != streams.end();it++)
    {
      
      if (type=="istream")
      {     
      vistream_sto.vertical_cast(*it);
      vistream = vistream_sto->get_istream();
      
      }
      
      else 
      {
      obj_stream_sto.vertical_cast(*it);
      obj_stream = obj_stream_sto->get_stream();
      }
    
     
      if(vistream)
          global_frame = vistream->frame_number();  
      else
          global_frame ++;

      if (type=="istream")
      { 
          if(!vistream->advance())
              continue;
      }
      else
      {
          if(!obj_stream->seek_frame(global_frame))
          continue;
      }
     
      if(!advanced){
        if(window_size_ < current_frames_)
        {  
          repository_sptr->remove_frame(0);
          repository_sptr->decrement_frame_number();    
          repository_sptr->go_to_prev_frame();
        }


        if(!(repository_sptr->go_to_next_frame()))
          loading_required = true;

        advanced=true;
      }

      if (loading_required){
        if(window_size_==1)
        {
          if (type=="istream")
          load_frame_from_istream(0, vistream_sto);
          
          else
          load_object_from_stream(global_frame, 0, obj_stream_sto);

        }
        else
        {
          if (type=="istream")
          load_frame_from_istream(repository_sptr->current_frame()+1, vistream_sto);
        
        }

      }
    }
  }
  if (advanced)
    repository_sptr->go_to_next_frame();

  return advanced;

}


bool
vidpro_process_manager::go_to_prev_frame()
{
    absolute_frame_--;

    if(repository_sptr->current_frame() < 0)
    {
        vcl_cerr << "ERROR: initializing process inputs, current frame is negative " <<vcl_endl;
        return false;
    }

    if(allow_costum_window_ && window_size_ < costum_window_)
        window_size_ = costum_window_;

    bool rewound = false;
    bool loading_required = false;

    vcl_set<bpro_storage_sptr> vistreams = repository_sptr->get_all_storage_classes("istream");
    vcl_set<bpro_storage_sptr> obj_streams = repository_sptr->get_all_storage_classes("object stream");

    vcl_map<vcl_string, vcl_set<bpro_storage_sptr> > streams;

    vidpro_istream_storage_sptr vistream_sto;
    vidl_istream_sptr vistream;
    vidpro_obj_stream_storage_sptr obj_stream_sto;
    vidpro_object_stream_sptr obj_stream;
    int global_frame =0;

    if (!vistreams.empty())
        streams.insert(vcl_pair<vcl_string, vcl_set<bpro_storage_sptr> >("istream", vistreams));

    if (!obj_streams.empty())
        streams.insert(vcl_pair<vcl_string, vcl_set<bpro_storage_sptr> >("object stream", obj_streams));


    for(vcl_map<vcl_string, vcl_set<bpro_storage_sptr> >::iterator mit = streams.begin();
        mit!= streams.end(); mit++)
    {
        vcl_string type = mit->first;
        vcl_set<bpro_storage_sptr> streams = mit->second;


        vcl_set<bpro_storage_sptr>::iterator it = streams.begin();
        for( ; it != streams.end();it++)
        {
            if (type=="istream")
            {     
                vistream_sto.vertical_cast(*it);
                vistream = vistream_sto->get_istream();
            }

            else 
            {
                obj_stream_sto.vertical_cast(*it);
                obj_stream = obj_stream_sto->get_stream();
            }

            if(vistream)
            {
                global_frame = vistream->frame_number();  
            }
            else
                global_frame ++;

            if (type=="istream")
            { 
                int valid_frame =vistream->frame_number()-1;
                if (valid_frame ==-1)
                    return true;

                if(!vistream->seek_frame(valid_frame))
                    continue;
            }
            else
            {
                if(!obj_stream->seek_frame(global_frame))
                    continue;
            }

            if (!rewound){
                if (repository_sptr->current_frame()==0 )
                {  
                    if(window_size_ == repository_sptr->num_frames())
                        repository_sptr->remove_frame(repository_sptr->num_frames()-1);

                    repository_sptr->increment_frame_number(); 

                    //repository_sptr->go_to_prev_frame();
                }


                if(!(repository_sptr->go_to_prev_frame()))
                    loading_required = true;

                rewound = true;
            }

            if (loading_required)
            {
                if (type=="istream")
                    load_frame_from_istream(0, vistream_sto);
                else
                    load_object_from_stream(global_frame, 0, obj_stream_sto);
            }

        }
    }

    return rewound;
}


bool
vidpro_process_manager::load_frame_from_istream( int frame, vidpro_istream_storage_sptr& vistream_sto)
{

  vidl_istream_sptr vistream= vistream_sto->get_istream();

  if(!vistream)
    return false;

  if (!vistream->is_open()) {
    vcl_cout << "Failed to open the input stream\n";
    return false;
  }

  vidl_frame_sptr video_frame = vistream->current_frame();
  vil_image_view<vxl_byte> image;   // do i need other types?
  vidl_convert_to_view(*video_frame, image);

  //image= vil_flip_ud(image);
  vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(image);

  // create the storage data structure
  vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
  image_storage->set_image( image_sptr );
  image_storage->set_name("image_from_" + vistream_sto->name());

  repository_sptr->store_data_at(image_storage,frame);


  return true;
}

bool
vidpro_process_manager::load_object_from_stream(int global_frame, int local_frame, 
                                                vidpro_obj_stream_storage_sptr obj_sto)
{

  
  vidpro_object_stream_sptr obj_stream= obj_sto->get_stream();

  if(!obj_stream)
    return false;

  if (!obj_stream->is_open()) {
    vcl_cout << "Failed to open the input stream\n";
    return false;
  }

  bpro_storage_sptr sto = obj_stream->read_frame(global_frame);
 
  if(!sto)
      return false;
  sto->set_name("object_from_" + obj_sto->name());

  repository_sptr->store_data_at(sto,local_frame);

  return true;
}



bool 
vidpro_process_manager::new_ostream(const vcl_string& directory,
                                     const vcl_string& storage_name,
                                     const vcl_string& name_format,
                                     const vcl_string& file_format,
                                     const unsigned int init_index)
{

  vidl_image_list_ostream *vos = new vidl_image_list_ostream(directory,name_format,file_format, init_index);

  // create the storage data structure
  vidpro_ostream_storage_sptr vos_storage = vidpro_ostream_storage_new();
  vos_storage->set_ostream(vos); 
  vos_storage->set_name(storage_name);
  bpro_storage_sptr sto = vos_storage;

  //add to database
  DATABASE->add_tuple("global_data", new brdb_tuple(storage_name, sto->type(), sto));

  return true;
}

bool 
vidpro_process_manager::new_obj_stream(const vcl_string& directory,
                                     const vcl_string& storage_name)
{

    bpro_storage_sptr sto;

    vidpro_object_stream_sptr obj = new vidpro_object_stream(directory.c_str());

    // create the storage data structure
    vidpro_obj_stream_storage_sptr obj_storage = vidpro_obj_stream_storage_new();

    obj_storage->set_stream(obj);
    obj_storage->set_name(storage_name);
    sto = obj_storage;

    DATABASE->add_tuple("global_data", new brdb_tuple(storage_name,vcl_string("object stream"),sto));

  return true;
}

bool 
vidpro_process_manager::write_to_object_ostream(const bpro_storage_sptr& sto,
                                                 vcl_string const & ostream_name)

{
 
    bpro_storage_sptr storage =repository_sptr->get_global_storage_by_name(ostream_name);
    vidpro_obj_stream_storage_sptr obj_sto;
    obj_sto.vertical_cast(storage);
    if(!obj_sto)
      return false;
    vidpro_object_stream_sptr obj_stream = obj_sto->get_stream();

    if (!obj_stream->is_open()) {
      vcl_cout << "Failed to open the output stream\n";
      return false;
    }

    obj_stream->write_frame(sto,absolute_frame_);
    return true;

}
bool 
vidpro_process_manager::write_to_ostream(const bpro_process_sptr& process,
                                         const bpro_storage_sptr& sto)
{

  if(!DATABASE->exists("ostream-process"))
    return false;

  vcl_map<vcl_string, vcl_string> ostreams = repository_sptr->get_process_ostreams(process);


  for ( vcl_map<vcl_string, vcl_string>::iterator it = ostreams.begin();it != ostreams.end();  ++it)
  {
    vcl_string type= it->first;
    vcl_string ostream_name = it->second;

    //careful check
    if(type!=sto->type())
      vcl_cerr<<"Warning: ostream type does not agree with its database type\n";

    if(ostream_name == "NONE")
    {
      vcl_cerr<<"Warning: Currently not writing to output stream by user request\n";
      return false;
    }

    if(!(sto->type() =="image")){
      return this->write_to_object_ostream(sto, ostream_name);
    }
    // get image view from the storage class

    vidpro_image_storage_sptr img_sto = vidpro_image_storage_new();
    img_sto.vertical_cast(sto);

    if(!img_sto)
      return false;
    vidl_memory_chunk_frame *output_frame = new vidl_memory_chunk_frame(*(img_sto->get_image()->get_view()));

    bpro_storage_sptr storage =repository_sptr->get_global_storage_by_name(ostream_name);

    vidpro_ostream_storage_sptr vos_storage;
    vos_storage.vertical_cast(storage);
    if(!vos_storage)
      return false;
    vidl_ostream_sptr vos = vos_storage->get_ostream();

    if (!vos->is_open()) {
      vcl_cout << "Failed to open the output stream\n";
      return false;
    }

    vos->write_frame(output_frame);


    return true;
  }
  return false;
}

//: Run a process on the current frame
bool
vidpro_process_manager::run_process_on_current_frame( const bpro_process_sptr& process,
                                                      vcl_set<bpro_storage_sptr>* modified )
{

  bool to_return = false;

  process->clear_input();
  process->clear_output();

  // SET INPUTS ////////////////////////////////////////////

  vcl_vector< vcl_string > input_type_list = process->get_input_type();
  vcl_vector< vcl_string > input_names = process->input_names();

  for( unsigned int i = 0 ;
    i < input_names.size();
    i++ ) {

      //Get the right storage class from the repository from the previous N frames the process needs
      for(  int a = 0 ;
        a < process->input_frames();
        a++ ) {

          //use the storage names in the process that maps to the exact input to be used
          bpro_storage_sptr input_storage_sptr = repository_sptr->get_data_by_name( input_names[i], -a );
          //assert(input_type_list[i] == input_storage_sptr->type());
          vcl_cout<< input_storage_sptr->name() << "vs" <<input_names[i]<< "\n";
          process->add_input(input_storage_sptr, a);
      }
  }

  vcl_vector<vcl_vector<bpro_storage_sptr> >  initial_inputs;
  for(int a = 0; a < process->input_frames(); ++a)
    initial_inputs.push_back(process->get_input(a));

  //////////////////////////////////////////////////////////
  // initial number of output frames
  int num_outputs = process->output_frames();


  // EXECUTE ///////////////////////////////////////////////
  to_return = process->execute();
  //////////////////////////////////////////////////////////

  // See if any missing input storage classes have been created
  if( to_return ){
    for(int a = 0; a < process->input_frames(); ++a){
      vcl_vector<bpro_storage_sptr> final_inputs = process->get_input(a);
      for(unsigned int i = 0; i < final_inputs.size(); ++i){
        if( final_inputs[i] && !initial_inputs[a][i] ){
          final_inputs[i]->set_name(input_names[i]);
          to_return = repository_sptr->store_data( final_inputs[i], -a );
          if (modified)
            modified->insert(final_inputs[i]);
        }
      }
    }
  }

  int frame_offset = process->output_frames() - num_outputs - 1;
  int curr_frame = repository_sptr->current_frame();

  // GET OUTPUTS ///////////////////////////////////////////

  vcl_vector<vcl_vector<bool> > is_global = process->get_global();
  bool global_is_present = !is_global.empty();

  if( to_return ) {
    for( int a = 0 ;
      a < process->output_frames();
      a++ ) {

        vcl_vector < bpro_storage_sptr > output_storage_classes = process->get_output(a);
        vcl_vector< vcl_string > output_names = process->output_names();

        // Any outputs beyond those specified with output_names() are 
        // added to the modified set but not added to the repository
        // it is assumed that these are preexisting but modified data
        for( unsigned int i = 0 ;
          i < output_storage_classes.size();
          i++ )
        {
          if(!output_storage_classes[i])
            continue;                    


          if(i < output_names.size()){
            if(output_storage_classes[i]->name()=="defaultname")
              //assign the chosen names for the output classes
              output_storage_classes[i]->set_name( output_names[i]);

            //storage class types are automatically determined by the repository
            if (!global_is_present) {
              to_return = repository_sptr->store_data( output_storage_classes[i], -a );
            } else {
              if(is_global[a][i]) {
                to_return = repository_sptr->store_global_data(output_storage_classes[i]);
              } else {
                to_return = repository_sptr->store_data( output_storage_classes[i], -a );
              }
            }
            this->write_to_ostream(process, output_storage_classes[i]);
          }
          if (modified)
            modified->insert(output_storage_classes[i]);
        }
    }
  }

  //Amir: The processes should not keep the input and output storage classes
  //      once the process has been executed. This is preventing those storage
  //      classes from being destructed.
  process->clear_input();
  process->clear_output();

  //////////////////////////////////////////////////////////
  // return to the original frame
  repository_sptr->go_to_frame(curr_frame);

  // RETURN VALUE //////////////////////////////////////////
  return to_return;
  //////////////////////////////////////////////////////////
}


bool
vidpro_process_manager::close_ostreams()
{
  vcl_vector< bpro_process_sptr >::iterator it = process_queue.begin();
  vcl_map<vcl_string, vcl_string> ostream_names;
  bpro_process_sptr process;
  unsigned i = 0;
  for (; it != process_queue.end(); ++it, i++){
    process=(*it);
    ostream_names = repository_sptr->get_process_ostreams(process);
    for(vcl_map<vcl_string, vcl_string>::iterator mit = ostream_names.begin();
      mit!=ostream_names.end(); mit++)
    {
      vcl_string type =mit->first;
      vcl_string name =mit->second;
      vcl_string os_path = vul_file::get_cwd() + '/' + name;

      if(type=="ostream")
      {
        //get ostream and close it
        vcl_map<vcl_string, vcl_string> ostream_names;
        bpro_storage_sptr sto = repository_sptr->get_global_storage_by_name(name);
        vidpro_ostream_storage_sptr os_sto;
        os_sto.vertical_cast(sto);
        os_sto->get_ostream()->close();
        //remove ostream from global data
        repository_sptr->remove_global_storage_by_name(name);
        //load ostream as an istream

        bpro_storage_sptr is_sto;
        is_sto = vidpro_open_istream_process::image_list_istream(os_path.c_str());
        DATABASE->add_tuple("global_data", new brdb_tuple(mit->second,vcl_string("istream"),is_sto));

      }
 
    }

    repository_sptr->clear_process_ostreams(process);

  }

  return true;
}


bool
vidpro_process_manager::run_process_queue_on_current_frame( vcl_set<bpro_storage_sptr>* modified )
{
  vcl_vector< bpro_process_sptr >::iterator it = process_queue.begin();
  unsigned i = 0;
  for (; it != process_queue.end(); ++it, i++){

    bool success = run_process_on_current_frame( (*it), modified );
    if( !success )
      return false;
  }

  return true;
}


//: Call the finish function on the process
bool
vidpro_process_manager::finish_process( int first_frame, int last_frame,
                                        const bpro_process_sptr& process,
                                        vcl_set<bpro_storage_sptr>* modified )
{

  process->clear_input(last_frame+1);
  process->clear_output(last_frame+1);

  // SET INPUTS ////////////////////////////////////////////

  vcl_vector< vcl_string > input_type_list = process->get_input_type();
  vcl_vector< vcl_string > input_names = process->input_names();

  vcl_vector< vcl_string > output_type_list = process->get_output_type();
  vcl_vector< vcl_string > output_names = process->output_names();

  //Get the right storage classes from the repository
  for( int f = first_frame; f <= last_frame; ++f) {
    for( unsigned i = 0; i < input_names.size(); ++i ) {
      //use the storage names in the process that maps to the exact input to be used
      bpro_storage_sptr input_storage_sptr = repository_sptr->get_data_by_name_at( input_names[i], f );
      //assert(input_storage_sptr && input_type_list[i] == input_storage_sptr->type());
      process->add_input(input_storage_sptr, f);
    }
    for( unsigned i = 0; i < output_names.size(); ++i ) {
      //use the storage names in the process that maps to the exact input to be used
      bpro_storage_sptr output_storage_sptr = repository_sptr->get_data_by_name_at( output_names[i], f );
      if(output_storage_sptr){
        //assert(output_type_list[i] == output_storage_sptr->type());
        process->add_input(output_storage_sptr, f);
      }
    }
  }

  if(!process->finish())
    return false;

  // GET OUTPUTS ///////////////////////////////////////////

  vcl_vector<vcl_vector<bool> > is_global = process->get_global();
  bool global_is_present = !is_global.empty();

  for( int f = 0; f <= last_frame; ++f)
  {
    vcl_vector < bpro_storage_sptr > output_storage_classes = process->get_output(f);
    vcl_vector< vcl_string > output_names = process->output_names();

    // Any outputs beyond those specified with output_names() are 
    // added to the modified set but not added to the repository
    // it is assumed that these are preexisting but modified data
    for(unsigned int i = 0; i < output_storage_classes.size(); ++i )
    {
      if(!output_storage_classes[i])
        continue;

      if(i < output_names.size()){
        if(output_storage_classes[i]->name()=="defaultname")
          //assign the chosen names for the output classes
          output_storage_classes[i]->set_name( output_names[i]);

        if (!global_is_present) {
          repository_sptr->store_data_at( output_storage_classes[i], f );
        } else {
          if(is_global[f][i]) {
            repository_sptr->store_global_data(output_storage_classes[i]);
          } else {
            repository_sptr->store_data_at( output_storage_classes[i], f );
          }
        }
      }
      if (modified)
        modified->insert(output_storage_classes[i]);

    }
  }
  return true;
}


//: Call the finish function on the process queue
bool
vidpro_process_manager::finish_process_queue( int first_frame, int last_frame,
                                              vcl_set<bpro_storage_sptr>* modified )
{
  vcl_vector< bpro_process_sptr >::iterator it = process_queue.begin();
  for (; it != process_queue.end(); ++it){
    bool success = finish_process( first_frame, last_frame, (*it), modified );
    if( !success )
      return false;
  }

  return true;
}

