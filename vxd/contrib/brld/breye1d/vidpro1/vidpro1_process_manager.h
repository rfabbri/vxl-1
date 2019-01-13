// This is brl/vidpro1/vidpro1_process_manager.h
#ifndef vidpro1_process_manager_h_
#define vidpro1_process_manager_h_

//:
// \file
// \brief This file defines a class that manages processes run on a video
// \author Based on original code by Amir Tamrakar
// \date 10/27/03
//
// \verbatim
//  Modifications
//    1/26/04 - Matt Leotta - Converted comments to Doxygen style
// \endverbatim


#include <vector>
#include <map>
#include <set>
#include <string>

#include <vbl/vbl_ref_count.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_storage_sptr.h>

class vidpro1_process_manager : public vbl_ref_count 
{
public:

  vidpro1_process_manager();
  virtual ~vidpro1_process_manager();
  
  bpro1_process_sptr get_process_by_name( const std::string& name ) const;

  //: Runs the process on the current frame
  // \param process specific the process and related data to run
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool run_process_on_current_frame( const bpro1_process_sptr& process,
                                     std::set<bpro1_storage_sptr>* modified = NULL );
  bool run_process_queue_on_current_frame ( std::set<bpro1_storage_sptr>* modified = NULL );

  //: Call the finish function on the process 
  // \param modified is an optional parameter.  If specified, all storage objects
  //                 modified by the process are added to the set
  bool finish_process( int first_frame, int last_frame,
                       const bpro1_process_sptr& process_block,
                       std::set<bpro1_storage_sptr>* modified = NULL );
  bool finish_process_queue( int first_frame, int last_frame,
                             std::set<bpro1_storage_sptr>* modified = NULL );

  vidpro1_repository_sptr get_repository_sptr() const;
  //: Set the process manager to use a new repository
  void set_repository(const vidpro1_repository_sptr& rep);
  void add_process_to_queue(const bpro1_process_sptr& process);
  void delete_last_process();
  void clear_process_queue();

  std::vector <std::string> get_process_queue_list() const;

  static void register_process( const bpro1_process_sptr& sptr );
  //: to access frames from future alse
  void set_non_causual(bool non_causal);
private:
  vidpro1_repository_sptr repository_sptr;

  static std::multimap< std::string , bpro1_process_sptr > process_map;
  std::vector< bpro1_process_sptr > process_queue;
    //: flag for access frames from future also. Note access 
    //: is limited to same # of frames in past and future.

  bool non_causal_;
};

#endif // vidpro1_process_manager_h_
