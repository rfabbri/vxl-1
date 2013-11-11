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

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

class dborl_index_query
{
private:
    vcl_vector<vcl_string> object_names_;
    vcl_vector<vcl_string> assoc_file_labels_;
    vcl_vector<vcl_string> assoc_file_types_;
public:
    void add_object_name(const vcl_string& name);
    void add_assoc_file_label(const vcl_string& label);
    void add_assoc_file_type(const vcl_string& type);

    vcl_vector<vcl_string>& get_object_names();
    vcl_vector<vcl_string>& get_assoc_file_labels();
    vcl_vector<vcl_string>& get_assoc_file_types();

    void print(vcl_ostream& os);
};

#endif /* DBORL_INDEX_QUERY_H_ */
