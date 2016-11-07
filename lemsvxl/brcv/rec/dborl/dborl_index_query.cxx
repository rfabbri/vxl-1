// This is /lemsvxl/brcv/rec/dborl/dborl_index_query.cxx

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

#include "dborl_index_query.h"

void dborl_index_query::add_object_name(const vcl_string& name)
{
    object_names_.push_back(name);
}

void dborl_index_query::add_assoc_file_label(const vcl_string& label)
{
    assoc_file_labels_.push_back(label);
}

void dborl_index_query::add_assoc_file_type(const vcl_string& type)
{
    assoc_file_types_.push_back(type);
}

vcl_vector<vcl_string>& dborl_index_query::get_object_names()
{
    return object_names_;
}

vcl_vector<vcl_string>& dborl_index_query::get_assoc_file_labels()
{
    return assoc_file_labels_;
}

vcl_vector<vcl_string>& dborl_index_query::get_assoc_file_types()
{
    return assoc_file_types_;
}

void dborl_index_query::print(vcl_ostream& os)
{
    os << "#############################" << vcl_endl;
    os << "INDEX QUERY:" << vcl_endl;
    os << "     OBJECT NAMES:" << vcl_endl;
    for(int i = 0; i < object_names_.size(); i++)
    {
        os << "\"" << object_names_[i] << "\"";
        if(i != object_names_.size()-1)
        {
            os << " ";
        }
    }
    os << vcl_endl;
    os << "     ASSOC LABELS:" << vcl_endl;
    for(int i = 0; i < assoc_file_labels_.size(); i++)
    {
        os << "\"" << assoc_file_labels_[i] << "\"";
        if(i != assoc_file_labels_.size()-1)
        {
            os << " ";
        }
    }
    os << vcl_endl;
    os << "     ASSOC TYPES:" << vcl_endl;
    for(int i = 0; i < assoc_file_types_.size(); i++)
    {
        os << "\"" << assoc_file_types_[i] << "\"";
        if(i != assoc_file_types_.size()-1)
        {
            os << " ";
        }
    }
    os << vcl_endl;
    os << "#############################" << vcl_endl;
}
