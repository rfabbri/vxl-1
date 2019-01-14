// This is /lemsvxl/brcv/rec/dborl/algo/dborl_search_index_file.cxx

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

#include <dborl/algo/dborl_search_index_file.h>
#include <dbxml/dbxml_algos.h>
#include <dbxml/dbxml_xio.h>
#include <bxml/bxml_read.h>

void
dborl_filter_objects_by_attribute(const std::string& attribute, std::vector<bxml_data_sptr>& objs, std::vector<std::string>& values, std::vector<bxml_data_sptr>& ret)
{
    int num_values = values.size();
    if(num_values > 0)
    {
        for(int i = 0; i < objs.size(); i++)
        {
            bxml_data_sptr obj_data = objs[i];
            bxml_element* obj = static_cast<bxml_element*>(obj_data.ptr());
            std::string obj_val = obj->attribute(attribute);
            for(int j = 0; j < num_values; j++)
            {
                if(obj_val == values[j])
                {
                    ret.push_back(obj_data);
                    break;
                }
            }
        }
    }
    else
    {
        ret = objs;
    }
}

void
dborl_get_objects_from_index_file(const std::string& index_file_path, std::vector<bxml_data_sptr>& all_objects)
{
    bxml_document doc_in = bxml_read(index_file_path);
    bxml_data_sptr root_xml = doc_in.root_element();

    bxml_element* node = dbxml_algos::find_by_name(root_xml, "node");
    bxml_element* objects = dbxml_algos::find_by_name(node, "objects");

    dbxml_algos::find_all_elems_by_name(objects, "object", all_objects);
}

std::vector<std::string>
dborl_get_assoc_file_paths(const std::string& index_file_path, dborl_index_query& query)
{
    std::vector<bxml_data_sptr> all_objects;
    dborl_get_objects_from_index_file(index_file_path, all_objects);

    std::vector<bxml_data_sptr> filtered_objects;
    dborl_filter_objects_by_attribute("name", all_objects, query.get_object_names(), filtered_objects);

    std::cout << "Total number of objects in the index file = " << all_objects.size() << std::endl;
    std::cout << "Number of query objects in the index file = " << filtered_objects.size() << std::endl;

    std::vector<std::string> ret_vector;
    for(int i = 0; i < filtered_objects.size(); i++)
    {
        bxml_data_sptr data = filtered_objects[i];

        std::vector<bxml_data_sptr> assoc_files;
        dbxml_algos::find_all_elems_by_name(data, "assocfile", assoc_files);
        std::vector<bxml_data_sptr> filtered_assoc_files;
        dborl_filter_objects_by_attribute("label", assoc_files, query.get_assoc_file_labels(), filtered_assoc_files);
        std::vector<bxml_data_sptr> filtered_assoc_files2;
        dborl_filter_objects_by_attribute("type", filtered_assoc_files, query.get_assoc_file_types(), filtered_assoc_files2);

        std::cout << "   Total number of assoc files = " << assoc_files.size() << std::endl;
        std::cout << "   Number of filtered assoc files = " << filtered_assoc_files2.size() << std::endl;

        if(filtered_assoc_files2.size() != 0)
        {
            bxml_data_sptr data2 = filtered_assoc_files2[0];
            bxml_element* filt_assoc = static_cast<bxml_element*>(data2.ptr());
            ret_vector.push_back(filt_assoc->attribute("path"));
        }

        if(filtered_assoc_files2.size() > 1)
        {
            std::cerr << "WARNING: Query returns more than one assoc file per object! First one is chosen by default." << std::endl;
        }
    }
    return ret_vector;
}

std::vector<std::string>
dborl_get_object_folders(const std::string& index_file_path, dborl_index_query& query)
{
    std::vector<std::string> ret;
    std::vector<bxml_data_sptr> all_objects;
    dborl_get_objects_from_index_file(index_file_path, all_objects);

    std::vector<bxml_data_sptr> filtered_objects;
    dborl_filter_objects_by_attribute("name", all_objects, query.get_object_names(), filtered_objects);

    for(int i = 0; i < filtered_objects.size(); i++)
    {
        bxml_data_sptr data = filtered_objects[i];
        bxml_element* filt_obj = static_cast<bxml_element*>(data.ptr());
        ret.push_back(filt_obj->attribute("path"));
    }

    return ret;
}

std::vector<std::string>
dborl_get_object_full_paths(const std::string& index_file_path, dborl_index_query& query, std::string ext)
{
    std::vector<std::string> folders = dborl_get_object_folders(index_file_path, query);
    for(int i = 0; i < folders.size(); i++)
    {
        folders[i] = folders[i] + "/" + query.get_object_names()[i] + ext;
    }
    return folders;
}
