//:
// \file
// \brief The match patch parameters -- a combination of shock match parameters and extract patch parameters
//                    
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/09/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_match_patch_params_h)
#define _dbskr_match_patch_params_h

#include <bxml/bxml_document.h>
#include "dbskr_match_shock_params.h"
#include "dbskr_extract_patch_params.h"

class dbskr_match_patch_params
{ public:

  //dbskr_match_shock_params shock_match_params_;  // --> will only use match_data part
  dbskr_tree_edit_params shock_match_params_;  // --> will only use match_data part
  dbskr_extract_patch_params query_patch_params_;  // --> will only use patch_data part
  dbskr_extract_patch_params model_patch_params_;  // --> will only use patch_data part

  bool save_matches_;
  vcl_string match_folder_;

  bool orl_format_folders_;
  vcl_string output_file_postfix_;
  vcl_string train_dir_, train_list_, db_dir_, db_list_;

  bool parse_from_data(bxml_data_sptr root);
  bxml_element *create_default_document_data();
};

#endif  //_dbskr_match_patch_params_h
