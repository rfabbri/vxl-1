// This is brl/vidpro/bpro_spatemp_process_manager.h
#ifndef bpro_spatemp_process_manager_h_
#define bpro_spatemp_process_manager_h_

//:


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>

#include <vbl/vbl_ref_count.h>
#include <vidpro/vidpro_repository_sptr.h>
#include <bpro/bpro_process_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <bpro/bpro_process_manager.h>

#include <vidl/vidl_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_ostream.h>
#include <vidl/vidl_ostream_sptr.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidpro/storage/vidpro_ostream_storage.h>
#include <vidpro/storage/vidpro_ostream_storage_sptr.h>
#include <vidpro/storage/vidpro_obj_stream_storage_sptr.h>

class bpro_spatemp_process_manager : public bpro_process_manager<bpro_spatemp_process_manager> 
{
public:

    //:Destructor 
    virtual ~bpro_spatemp_process_manager(); 

    //: repository-related functions

    vidpro_repository_sptr get_repository_sptr() const;

    //: Set the process manager to use a new repository
    void set_repository(const vidpro_repository_sptr& rep);

    bool go_to_next_frame();
    bool go_to_prev_frame();



    bool get_process_input_names(vcl_map<vcl_string,vcl_vector<vcl_string> > &input_names,
        const bpro_process_sptr& process );

    bool get_process_output_names(vcl_map<vcl_string,vcl_vector<vcl_string> > &output_names,
        const bpro_process_sptr& process );

    bool set_process_ostreams (const bpro_process_sptr& process, 
        vcl_map<vcl_string, vcl_vector<vcl_string> > & output_list,
        bool has_ostream);
    bool check_process_ostreams(bpro_process_sptr const &process,
        vcl_string const & ostream_name,
        vcl_string const & ostream_type); 

    

    bool add_missing_input_names(vcl_vector<vcl_string> &input_names, const bpro_process_sptr& process);

    bool load_frame_from_istream(int frame, vidpro_istream_storage_sptr& vistream_sto);
    bool load_object_from_stream(int global_frame, int local_frame, 
                                                vidpro_obj_stream_storage_sptr obj_sto);

  
    bool new_ostream(const vcl_string& directory,
        const vcl_string& storage_name = "default",
        const vcl_string& name_format = "%05d",
        const vcl_string& file_format = "tiff",
        const unsigned int init_index = 0);

        bool new_obj_stream(const vcl_string& directory,
        const vcl_string& storage_name = "default");

    
    bool write_to_ostream(const bpro_process_sptr& process, const bpro_storage_sptr& sto);
    bool write_to_object_ostream(const bpro_storage_sptr& sto, vcl_string const & os_name);
    bool close_ostreams();


    //: Runs the process on the current frame
    // \param process specific the process and related data to run
    // \param modified is an optional parameter.  If specified, all storage objects
    //                 modified by the process are added to the set
    bool run_process_on_current_frame( const bpro_process_sptr& process,
        vcl_set<bpro_storage_sptr>* modified = NULL );
    bool run_process_queue_on_current_frame ( vcl_set<bpro_storage_sptr>* modified = NULL );

    //: Call the finish function on the process 
    // \param modified is an optional parameter.  If specified, all storage objects
    //                 modified by the process are added to the set
    bool finish_process( int first_frame, int last_frame,
        const bpro_process_sptr& process_block,
        vcl_set<bpro_storage_sptr>* modified = NULL );
    bool finish_process_queue( int first_frame, int last_frame,
        vcl_set<bpro_storage_sptr>* modified = NULL );

    bool recording_macro();
    void start_recording_macro();
    void stop_recording_macro();
    void set_costum_window(int size);
    void unset_costum_window();
    void set_window_size(int);
    int window_size();


    friend class bpro_process_manager<bpro_spatemp_process_manager>;

protected:
    vidpro_repository_sptr repository_sptr;
private:

    bpro_spatemp_process_manager();

    vidpro_istream_storage_sptr vistream_sto_;
    vidl_istream_sptr vistream_;

    // vidl_ostream_sptr vostream_;

    vcl_string ostream_dir_;

    bool allow_costum_window_;

    //variable to keep the maximum number of frames allowed in the database
    int window_size_;

    //variable to keep the maximum number of frames allowed in the database
    int costum_window_;

    //varuiable to keep the total number of frames in database
    int current_frames_;

    //variable to keep the absolute frame number in the video sequence
    int absolute_frame_;

    bool recording_macro_;

};

#endif // bpro_spatemp_process_manager_h_
