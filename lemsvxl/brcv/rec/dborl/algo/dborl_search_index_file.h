// This is /lemsvxl/brcv/rec/dborl/algo/dborl_search_index_file.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 9, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef DBORL_SEARCH_INDEX_FILE_H_
#define DBORL_SEARCH_INDEX_FILE_H_

#include <string>
#include <vector>
#include <dborl/dborl_index_query.h>

std::vector<std::string> dborl_get_assoc_file_paths(const std::string& index_file_path, dborl_index_query& query);
std::vector<std::string> dborl_get_object_folders(const std::string& index_file_path, dborl_index_query& query);
std::vector<std::string> dborl_get_object_full_paths(const std::string& index_file_path, dborl_index_query& query, std::string ext);

#endif /* DBORL_SEARCH_INDEX_FILE_H_ */
