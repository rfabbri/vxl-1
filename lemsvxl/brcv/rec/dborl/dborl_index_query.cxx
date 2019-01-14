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

void dborl_index_query::add_object_name(const std::string& name)
{
    object_names_.push_back(name);
}

void dborl_index_query::add_assoc_file_label(const std::string& label)
{
    assoc_file_labels_.push_back(label);
}

void dborl_index_query::add_assoc_file_type(const std::string& type)
{
    assoc_file_types_.push_back(type);
}

std::vector<std::string>& dborl_index_query::get_object_names()
{
    return object_names_;
}

std::vector<std::string>& dborl_index_query::get_assoc_file_labels()
{
    return assoc_file_labels_;
}

std::vector<std::string>& dborl_index_query::get_assoc_file_types()
{
    return assoc_file_types_;
}

void dborl_index_query::print(std::ostream& os)
{
    os << "#############################" << std::endl;
    os << "INDEX QUERY:" << std::endl;
    os << "     OBJECT NAMES:" << std::endl;
    for(int i = 0; i < object_names_.size(); i++)
    {
        os << "\"" << object_names_[i] << "\"";
        if(i != object_names_.size()-1)
        {
            os << " ";
        }
    }
    os << std::endl;
    os << "     ASSOC LABELS:" << std::endl;
    for(int i = 0; i < assoc_file_labels_.size(); i++)
    {
        os << "\"" << assoc_file_labels_[i] << "\"";
        if(i != assoc_file_labels_.size()-1)
        {
            os << " ";
        }
    }
    os << std::endl;
    os << "     ASSOC TYPES:" << std::endl;
    for(int i = 0; i < assoc_file_types_.size(); i++)
    {
        os << "\"" << assoc_file_types_[i] << "\"";
        if(i != assoc_file_types_.size()-1)
        {
            os << " ";
        }
    }
    os << std::endl;
    os << "#############################" << std::endl;
}
