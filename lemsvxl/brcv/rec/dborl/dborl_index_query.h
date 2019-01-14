// This is /lemsvxl/brcv/rec/dborl/dborl_index_query.h

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

#ifndef DBORL_INDEX_QUERY_H_
#define DBORL_INDEX_QUERY_H_

#include <vector>
#include <string>
#include <iostream>

class dborl_index_query
{
private:
    std::vector<std::string> object_names_;
    std::vector<std::string> assoc_file_labels_;
    std::vector<std::string> assoc_file_types_;
public:
    void add_object_name(const std::string& name);
    void add_assoc_file_label(const std::string& label);
    void add_assoc_file_type(const std::string& type);

    std::vector<std::string>& get_object_names();
    std::vector<std::string>& get_assoc_file_labels();
    std::vector<std::string>& get_assoc_file_types();

    void print(std::ostream& os);
};

#endif /* DBORL_INDEX_QUERY_H_ */
