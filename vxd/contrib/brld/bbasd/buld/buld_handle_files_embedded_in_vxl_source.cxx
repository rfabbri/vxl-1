// This is bbasd/buld/buld_handle_files_embedded_in_vxl_source.cxx

//:
// \file
// \brief
// \author Based on original code by  Firat Kalaycilar (@lems.brown.edu)
// \date Feb 09, 2010

#include<string>
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>
#include<vpl/vpl.h>

#include "buld_handle_files_embedded_in_vxl_source.h"

bool buld_is_embedded_files_dir_safe = false;

void buld_create_file_extraction_lock(const char* out_dir)
{
    std::string expanded_out_dir = vul_file::expand_tilde(out_dir);
    std::string lock_file = expanded_out_dir + std::string("/.buldrunlock");
    std::ofstream lock_stream(lock_file.c_str());
    lock_stream << "BULD_LOCK";
    lock_stream.close();
}

void buld_remove_file_extraction_lock(const char* out_dir)
{
    std::string expanded_out_dir = vul_file::expand_tilde(out_dir);
    std::string lock_file = expanded_out_dir + std::string("/.buldrunlock");
    vpl_unlink(lock_file.c_str());
}

bool buld_is_file_extraction_in_progress(const char* out_dir)
{
    std::string expanded_out_dir = vul_file::expand_tilde(out_dir);
    std::string lock_file = expanded_out_dir + std::string("/.buldrunlock");
    return vul_file_exists(lock_file);
}

void buld_default_extract_embedded_file(char* array, int length, const char* out_dir, char* file_name)
{
    std::string expanded_out_dir = vul_file::expand_tilde(out_dir);
    std::string out_file = expanded_out_dir + std::string("/") + std::string(file_name);
    vul_file::make_directory_path(vul_file::dirname(out_file));
    std::FILE* pFile = std::fopen ( out_file.c_str() , "wb" );
    if(pFile == NULL)
    {
        std::cerr << "ERROR: File " <<  out_file << " cannot be created. Cannot continue!" << std::endl;
        buld_remove_file_extraction_lock(out_dir);
        std::exit(1);
    }
    std::fwrite (array, 1 ,length ,pFile);
    std::fclose(pFile);
}

void buld_default_delete_extracted_file(const char* out_dir)
{
    if(buld_is_embedded_files_dir_safe)
    {
        std::string path = vul_file::expand_tilde(out_dir);
        if(vul_file::is_directory(path))
        {
            vul_file_iterator fit(path + std::string("/*"));
            do
            {
                std::string entry_name = fit.filename();
                if (entry_name == "." || entry_name == "..")
                {
                    continue;
                }
                std::string full_entry_path = path + '/' + entry_name;
                buld_default_delete_extracted_file(full_entry_path.c_str());
            }
            while(++fit);
            std::cout << "Deleting " << path.c_str() << std::endl;
            vpl_rmdir(path.c_str());
        }
        else
        {
            std::cout << "Deleting " << path.c_str() << std::endl;
            vpl_unlink(path.c_str());
        }
    }
    else
    {
        std::cerr << "Deleting directory " << out_dir << " may be harmful!" << std::endl;
        return;
    }
}
