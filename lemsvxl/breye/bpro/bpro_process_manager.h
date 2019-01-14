// This is brl/bpro/bpro_process_manager.h
#ifndef bpro_process_manager_h_
#define bpro_process_manager_h_

//:
// \file
// \brief This file defines a class base process manager
// It is templated to allow its childs to be singleton managers 
// \author Isabel Restrepo
// \date 3/27/07
//
// \verbatim
//  Modifications
//           
// \endverbatim


#include <vector>
#include <map>
#include <set>
#include <string>

#include <vbl/vbl_ref_count.h>
#include <bpro/bpro_process_sptr.h>
#include <bpro/bpro_storage_sptr.h>


template <class T>
class bpro_process_manager : public vbl_ref_count 
{
public:

   //: Destructor
  virtual ~bpro_process_manager();
  
  //: Use this instead of constructor
  static T* instance();
  
  bpro_process_sptr get_process_by_name( const std::string& name ) const;

   //: Call the finish function on the process 
  //virtual bool finish_process()=0;
  //virtual bool finish_process_queue()=0;

  void add_process_to_queue(const bpro_process_sptr& process);
  void delete_last_process();
  void clear_process_queue();

  std::vector <std::string> get_process_queue_list() const;

  static void register_process( const bpro_process_sptr& sptr );
  
protected:
  
  //: Constructor
  bpro_process_manager();
  
  static T* instance_;
  
   //: Initialize the static instance
  void initialize();
  

  static std::multimap< std::string , bpro_process_sptr > process_map;

  std::vector< bpro_process_sptr > process_queue;
};

#endif // bpro_process_manager_h_
