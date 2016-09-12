// This is breye/bvis/menu/bvis_command.cxx
//:
// \file

#include "bvis_command.h"
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vgui/vgui_dialog.h>
#include <bvis/bvis_tool_manager.h>
#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_mview_manager.h>
#include <bvis/bvis_manager_traits.h>
#include <bvis/menu/mview/bvis_mview_io_dialog.h>
#include <bpro/bpro_process.h>
#include <bpro/bpro_parameters_sptr.h>
#include <bpro/bpro_parameters.h>
#include <bpro/bpro_mview_dbutils.h>
#include <bpro/bpro_mview_process_manager.h>


#include <vidpro/vidpro_repository_sptr.h>
#include <vidpro/vidpro_repository.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidpro/storage/vidpro_ostream_storage.h>
#include <vidpro/storage/vidpro_ostream_storage_sptr.h>
#include <vidpro/process/vidpro_frame_from_istream_process.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

#include <brdb/brdb_relation.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_database_manager.h>

//
//Uses menus to assign inputs and outputs for both tools and processes
//
static int dummy;


static void 
generate_choice_menu(vidpro_repository_sptr const& repository_sptr,
                     vcl_map<vcl_string, vcl_vector<vcl_string> > &inputs_list,
                     vcl_map<vcl_string, vcl_vector<vcl_string> > &outputs_list,
                     vcl_vector< vcl_string > const& output_suggested_names,
                     vcl_vector< vcl_string >& input_names,
                     vcl_vector< vcl_string >& output_names,
                     bool &has_ostream
                     )
{

    //Prompt the user to select input/output variable
    vgui_dialog io_dialog("Select Inputs and outputs" );

    //display input options
    if (!inputs_list.empty())
        io_dialog.message("Select Input(s) From Available ones:");

    //store the choices
    vcl_vector<int> input_choices(inputs_list.size());

    //set of existing storage classes
    vcl_vector< vcl_vector <vcl_string> > available_storage_classes(inputs_list.size());
    //resize the resulting set of names
    input_names.resize(inputs_list.size());
    vcl_vector <vcl_string> input_type_list(inputs_list.size());


    unsigned i= 0;
    for( vcl_map<vcl_string, vcl_vector<vcl_string> >::iterator mip = inputs_list.begin();
        mip!=inputs_list.end(); mip++,i++)
    {
        //for this input type allow user to select from available storage classes in the repository
        available_storage_classes[i] = mip->second;
        input_type_list[i] = mip->first;
        //always have the choice of none of the existing classes
        available_storage_classes[i].push_back("(NONE)");
        //Multiple choice - with the list of options.
        io_dialog.choice(mip->first.c_str(), available_storage_classes[i], input_choices[i]);
        // i++;

    }

    //display output options
    if (!outputs_list.empty())
        io_dialog.message("Select Output(s) From Available ones:");

    //get the type of storages
    vcl_vector <vcl_string> output_type_list(outputs_list.size());

    //store the choices
    output_names.resize(output_type_list.size());

    //since output storage classes don't exist yet we can only assign names
    //make sure that these names are assigned to the storage classes after the process is executed
    //first build up a collection of all existing names

    vcl_map<vcl_string, vcl_vector<vcl_string> > existing_names;


    i=0;
    for( vcl_map<vcl_string, vcl_vector<vcl_string> >::iterator mip = outputs_list.begin();
        mip!=outputs_list.end(); mip++,i++)
    {
        //generate a default name that can be changed if desired
        vcl_string suggested_name = mip->first;
        vcl_string output_type = mip->first;
        if(i < output_suggested_names.size())
            suggested_name = output_suggested_names[i];
        vcl_string name_str;
        int c = 0;

        vcl_stringstream name_stream;
        name_stream << suggested_name << c++;
        name_str = name_stream.str();
        for( vcl_vector<vcl_string>::iterator vit = (mip->second).begin();
            vit!=(mip->second).end(); vit++)
        {          
            if(!((*vit)==name_str))
                continue;

            vcl_stringstream name_istream;
            name_istream << suggested_name << c++;
            name_str = name_istream.str();
        }

        output_names[i] = name_str;
       
        io_dialog.field( output_type.c_str() , output_names[i] );
        


    }

    io_dialog.checkbox("Save output stream", has_ostream);

    //display dialog
    if (io_dialog.ask()) {
        vgui_dialog null_inputs("Name Missing Inputs");
        bool found_missing = false;
        for ( unsigned int i=0; i < input_choices.size(); i++){
            if( input_choices[i]+1 < (int)available_storage_classes[i].size() ){
                input_names[i] = available_storage_classes[i][input_choices[i]];
                null_inputs.message((input_type_list[i]+" : "+input_names[i]).c_str());
            }
            else{
                null_inputs.field(input_type_list[i].c_str(), input_names[i]);
                found_missing = true;
            }
        }
        if(found_missing)
            null_inputs.ask();
    }

    //update output_list
    i=0;
    for( vcl_map<vcl_string, vcl_vector<vcl_string> >::iterator mip = outputs_list.begin();
        mip!=outputs_list.end(); mip++,i++)
      {
        vcl_vector<vcl_string> names;
        names.push_back(output_names[i]);
        outputs_list[mip->first]=names;
      }
}


static void 
select_inputs_and_outputs(vcl_vector< vcl_string > const& input_type_list,
                          vcl_vector< vcl_string > const& output_type_list,
                          vcl_vector< vcl_string > const& output_suggested_names,
                          vcl_vector< vcl_string >& input_names,
                          vcl_vector< vcl_string >& output_names)
{
    //Prompt the user to select input/output variable
    vgui_dialog io_dialog("Select Inputs and outputs" );

    //display input options
    if (!input_type_list.empty())
        io_dialog.message("Select Input(s) From Available ones:");

    //store the choices
    vcl_vector<int> input_choices(input_type_list.size());
    //set of existing storage classes
    vcl_vector< vcl_vector <vcl_string> > available_storage_classes(input_type_list.size());
    //resize the resulting set of names
    input_names.resize(input_type_list.size());

    for( unsigned int i = 0 ;
        i < input_type_list.size();
        i++ )
    {
      available_storage_classes[i] = 
        bpro_mview_dbutils::get_all_storage_class_names(input_type_list[i],0);//JLM KLUDGE
        //always have the choice of none of the existing classes
        available_storage_classes[i].push_back("(NONE)");

        //Multiple choice - with the list of options.
        io_dialog.choice(input_type_list[i].c_str(), available_storage_classes[i], input_choices[i]);

    }

    //display output options
    if (!output_type_list.empty())
        io_dialog.message("Select Output(s) From Available ones:");

    //store the choices
    output_names.resize(output_type_list.size());

    //store the choices
    vcl_vector<int> output_choices(output_type_list.size());
    vcl_vector< vcl_vector <vcl_string> > available_output_storage_classes(output_type_list.size());
    //since output storage classes don't exist yet we can only assign names
    //make sure that these names are assigned to the storage classes after the process is executed
    //first build up a collection of all existing names
    vcl_map<vcl_string, vcl_set<vcl_string> > existing_names;
    for( unsigned int i = 0 ;
        i < output_type_list.size();
        i++ )
    {
        if(existing_names.find(output_type_list[i]) == existing_names.end())
        {
            vcl_vector<vcl_string> names = 
                bpro_mview_dbutils::get_all_storage_class_names(output_type_list[i],0);//JLM KLUDGE
            for( unsigned int j=0; j<names.size(); ++j )
                existing_names[output_type_list[i]].insert(names[j]);
        }
    }
    for( unsigned int i = 0 ;
        i < output_type_list.size();
        i++ )
    {
        //generate a default name that can be changed if desired
        vcl_string suggested_name = output_type_list[i];
        if(i < output_suggested_names.size())
            suggested_name = output_suggested_names[i];
        vcl_string name_str;
        int c = 0;
        do{
            vcl_stringstream name_stream;
            name_stream << suggested_name << c++;
            name_str = name_stream.str();
        }while(existing_names[output_type_list[i]].count(name_str) != 0);
        existing_names[output_type_list[i]].insert(name_str);
    }
    for( unsigned int i = 0 ; i < output_type_list.size(); i++ )
        for(vcl_set<vcl_string>::iterator sit = 
            existing_names[output_type_list[i]].begin(); 
            sit !=existing_names[output_type_list[i]].end(); sit++)
            available_output_storage_classes[i].push_back(*sit);

    for( unsigned int i=0; i< output_type_list.size(); i++ )
        io_dialog.choice(output_type_list[i].c_str(),
        available_output_storage_classes[i],
        output_choices[i]);    

    //display dialog
    if (io_dialog.ask()) {
        //fill out outputs
        for( unsigned int i=0; i< output_type_list.size(); i++ )
            output_names[i]=available_output_storage_classes[i][output_choices[i]];

        vgui_dialog null_inputs("Name Missing Inputs");
        bool found_missing = false;
        for ( unsigned int i=0; i < input_choices.size(); i++){
            if( input_choices[i]+1 < (int)available_storage_classes[i].size() ){
                input_names[i] = available_storage_classes[i][input_choices[i]];
                null_inputs.message((input_type_list[i]+" : "+input_names[i]).c_str());
            }
            else{
                null_inputs.field(input_type_list[i].c_str(), input_names[i]);
                found_missing = true;
            }
        }
        if(found_missing)
            null_inputs.ask();
    }

}
static void 
new_inputs_and_outputs(vcl_vector< vcl_string > const& input_type_list,
                       vcl_vector< vcl_string > const& output_type_list,
                       vcl_vector< vcl_string > const& output_suggested_names,
                       vcl_vector< vcl_string >& input_names,
                       vcl_vector< unsigned >& output_view_ids,
                       vcl_vector< vcl_string >& output_names)
{
  bvis_mview_io_dialog::input_dialog(input_type_list,
                                      input_names);
  bvis_mview_io_dialog::output_dialog(output_type_list,
                                       output_suggested_names,
                                       output_view_ids,
                                       output_names);
}

//: execute the tool command to select and reset the tool
template <class T>
void bvis_tool_command<T>::execute()
{
    bvis_tool_manager<T>::instance()->set_active_tool(tool_);
} 

//: execute the tool command to select and reset the tool and prompt the user
// for input and output storage
template <class T>
void bvis_user_io_tool_command<T>::execute()
{
    vcl_vector< vcl_string > input_type_list =  tool_->get_input_type();
    vcl_vector< vcl_string > output_type_list = tool_->get_output_type();
    //user is defining the outputs so suggested names is empty
    vcl_vector< vcl_string > output_suggested_names;
    vcl_vector< vcl_string > input_names, output_names;

    select_inputs_and_outputs(input_type_list,
        output_type_list,
        output_suggested_names,
        input_names, output_names);
    tool_->set_input_names(input_names);
    tool_->set_output_names(output_names);
    bvis_tool_manager<T>::instance()->set_active_tool(tool_);
} 
template <class T>
void bvis_process_command<T>::execute_video_process()
{
    vidpro_repository_sptr repository_sptr = bvis_video_manager::instance()->repository();

    //check if there are enough input frames loaded in memory to execute process
    int current_window = bvis_video_manager::instance()->process_manager()->window_size();
    if ( current_window <  process_->input_frames())
    {
      bvis_video_manager::instance()->process_manager()->set_window_size(process_->input_frames());

      while ( repository_sptr->num_frames() < process_->input_frames())
        bvis_video_manager::instance()->process_manager()->go_to_next_frame();
    }

    //check stream names
    vcl_vector<vcl_string> ostream_names;
    ostream_names.clear();

    vcl_vector< vcl_string > output_suggested_names = process_->suggest_output_names();
    vcl_vector<vcl_string> input_names,output_names;
    vcl_map<vcl_string, vcl_vector<vcl_string> > input_list;
    vcl_map<vcl_string, vcl_vector<vcl_string> > output_list;
    bvis_video_manager::instance()->process_manager()->get_process_input_names(input_list, process_);
    bvis_video_manager::instance()->process_manager()->get_process_output_names(output_list, process_);

    bool has_ostream = false;

    
    generate_choice_menu(repository_sptr, input_list, output_list,
        output_suggested_names, input_names, output_names, has_ostream);

    //load necessary frames into database
    bvis_video_manager::instance()->process_manager()->add_missing_input_names( input_names, process_ );
    //bvis_video_manager::instance()->process_manager()->initialize_process_inputs( process_ );

    bvis_video_manager::instance()->process_manager()->set_process_ostreams( process_, output_list,has_ostream);

    process_->set_input_names(input_names);
    process_->set_output_names(output_names);

    bpro_process_sptr process_to_run = process_;


    //if we are in the middle of recording a macro
    //add this process to the process queue unless the queue is not allowed for this process
    if (bvis_video_manager::instance()->recording_macro() && allow_queue_){
        process_to_run = process_->clone();
        bvis_video_manager::instance()->process_manager()->add_process_to_queue(process_to_run);
    }
    //JLM modifciation
    if(!(bvis_video_manager::instance()->recording_macro()) && allow_queue_)
    {
        bvis_video_manager::instance()->process_manager()->clear_process_queue();
        bvis_video_manager::instance()->process_manager()->
            add_process_to_queue(process_to_run);
    }

    vcl_set<bpro_storage_sptr> modified;
    //now run the process
    bvis_video_manager::instance()->process_manager()->run_process_on_current_frame(process_to_run, &modified);


    //If the process, is "Open Video Stream", load_frame_from_istream is also run to display 
    //first frame of video stream
    if(process_to_run->name() == "Open Video Istream")
    {
        bpro_process_sptr load_image_pro( new vidpro_frame_from_istream_process() );
        vcl_map<vcl_string, vcl_vector<vcl_string> >::iterator vis_it=
            output_list.find("istream");
        vcl_string vis_name;
        vcl_vector<vcl_string> names;
        if (vis_it != output_list.end()){
            if((vis_it->second).size()==1)
            {
                names = vis_it->second;
                vis_name = names[0];
            }

        }
        else
            vcl_cerr<<"error at loading frame from stream"<<vcl_endl;

        bpro_storage_sptr sto = repository_sptr->get_data_by_name(vis_name);
        vcl_vector<bpro_storage_sptr> stos;
        stos.push_back(sto);
        load_image_pro->set_input(stos);
        load_image_pro->set_input_names(names);
        vcl_vector<vcl_string> out; out.push_back("image_from_" +vis_name);
        load_image_pro->set_output_names(out);
        process_to_run=load_image_pro;
        bvis_video_manager::instance()->process_manager()->run_process_on_current_frame(process_to_run, &modified);

    }

   

    // update the display for any modified storage objects
    for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
        itr != modified.end(); ++itr ) {
            bvis_video_manager::instance()->add_to_display(*itr);
    }

    bvis_video_manager::instance()->display_visible_data();
}

template <class T>
void bvis_process_command<T>::execute_mview_process()
{
    vcl_vector<vcl_string> input_names,output_names;
    vcl_vector< vcl_string > input_type_list =  process_->get_input_type();
    vcl_vector< vcl_string > output_type_list = process_->get_output_type();
    //user is defining the outputs so suggested names is empty
    vcl_vector< vcl_string > output_suggested_names;
    vcl_vector< unsigned > output_view_ids;
#if 0
    select_inputs_and_outputs(input_type_list,
        output_type_list,
        output_suggested_names,
        input_names, output_names);
#endif
    new_inputs_and_outputs(input_type_list,
                           output_type_list,
                           output_suggested_names,
                           input_names,
                           output_view_ids,
                           output_names);

    process_->set_input_names(input_names);

    process_->set_output_names(output_names);

    bpro_process_sptr process_to_run = process_;
    //if we are in the middle of recording a macro
    //add this process to the process queue unless the queue is not allowed for this process
    if (T::instance()->recording_macro() && allow_queue_){
        process_to_run = process_->clone();
        bvis_manager_traits<bvis_mview_manager>::process_manager()->
          add_process_to_queue(process_to_run);
    }
    //JLM modifciation
    if(!(bvis_mview_manager::instance()->recording_macro()) && allow_queue_)
    {
        bpro_mview_process_manager::instance()->clear_process_queue();
        bpro_mview_process_manager::instance()->
          add_process_to_queue(process_to_run);
    }

    vcl_set<bpro_storage_sptr> modified;
    //now run the process
    bpro_mview_process_manager::instance()->
      run_process(process_to_run, &modified);

    // update the display for any modified storage objects
    for ( vcl_set<bpro_storage_sptr>::iterator itr = modified.begin();
        itr != modified.end(); ++itr ) {
      bvis_mview_manager::instance()->add_to_display(*itr);
    }

    bvis_mview_manager::instance()->display_visible_data();
}

template <class T>
void bvis_process_command<T>::execute()

{

    if(!process_.ptr()){
        vcl_cerr << "Warning: not able to execute a Null process" << vcl_endl;
        return;
    }
    vgui_dialog param_dialog( process_->name().c_str() );

    // We need to get the parameters for the process first because 
    // I/O types could changewith different parameters

    bpro_parameters_sptr param_sptr = process_->parameters();

    vcl_vector< bpro_param* > param_vector = param_sptr->get_param_list();
    
    //Isa modification: this line should not return
    //If a process has no parameters its execute still needs to be called
    
    T::instance()->parameter_dialog(param_vector, param_dialog);
    if (!param_vector.empty()){
      if (param_dialog.ask()) {

        for( vcl_vector< bpro_param* >::iterator it = param_vector.begin();
          it != param_vector.end();
          ++it )
        {
          (*it)->set_from_temp();
        }
      }
      else return;   //The user has selected "cancel"
    }

    if(bvis_manager_traits<T>::name()=="bvis_video_manager"){
        execute_video_process();
        return;
    }
    if(bvis_manager_traits<T>::name()=="bvis_mview_manager"){
        execute_mview_process();
        return;
    }
    
}

#undef BVIS2_COMMAND_INSTANTIATE
#define BVIS2_COMMAND_INSTANTIATE(T) \
    template class bvis_tool_command< T >; \
    template class bvis_user_io_tool_command< T >; \
    template class bvis_process_command< T >; 




