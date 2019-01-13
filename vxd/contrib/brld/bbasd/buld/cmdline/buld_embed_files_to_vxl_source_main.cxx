// This is bbasd/buld/cmdline/buld_embed_files_to_vxl_source_main.cxx

//:
// \file
// \brief
// \author Based on original code by  Firat Kalaycilar (@lems.brown.edu)
// \date Feb 09, 2010

//  -i ~/Desktop/input.txt -h /home/firat/lemsvxl/src/contrib/firat/any2cpp/test/test.h -c /home/firat/lemsvxl/src/contrib/firat/any2cpp/test/test.cxx -e extractTestFiles -d deleteTestFiles

#include<cstdlib>
#include<ctime>
#include<iostream>
#include<string>
#include<vector>

#include<vul/vul_file.h>
#include<vul/vul_arg.h>

#include "buld_embed_files_to_vxl_source_utils.h"

int main(int argn, char* args[])
{
    // Arguments
    vul_arg_info_list arg_list;
    vul_arg<std::string> arg_list_of_files(arg_list, "-i", "Input: path of the text file containing the list of files to be embedded.", "/path/input.txt");
    vul_arg<std::string> arg_output_h_file_name(arg_list, "-h", "Output: h-file", "out.h");
    vul_arg<std::string> arg_output_cpp_file_name(arg_list, "-c", "Output: cpp-file", "out.cpp");
    vul_arg<std::string> arg_extract_command(arg_list, "-e", "Name of the file extraction command", "extractFiles");
    vul_arg<std::string> arg_delete_command(arg_list, "-d", "Name of the file deletion command", "deleteFiles");
    vul_arg<bool > arg_print_usage_only(arg_list,"-usage", "print usage info and exit",false);
    vul_arg<bool > arg_print_help(arg_list,"-help", "print usage info and exit",false);

    vul_arg_include(arg_list);
    vul_arg_parse(argn, args);

    if (arg_print_help() || arg_print_usage_only())
    {
        vul_arg_display_usage_and_exit();
        return 0;
    }

    std::string input_file_name = arg_list_of_files();
    std::string output_h_file_name = arg_output_h_file_name();
    std::string output_cpp_file_name = arg_output_cpp_file_name();
    std::string extract_command = arg_extract_command();
    std::string delete_command = arg_delete_command();

    if(!vul_file_exists(input_file_name))
    {
        std::cerr << "Input file <" << input_file_name << "> cannot be found!" << std::endl;
        return 1;
    }

    std::string h_file_tag = getRandomHFileTag(32);

    std::vector<std::string> original_files;
    std::vector<std::string> variable_names;
    std::vector<int> data_sizes;
    std::string root_path;

    parseInputFile(input_file_name, original_files, variable_names, root_path);

    create_h_file(output_h_file_name, h_file_tag, extract_command, delete_command);
    create_cpp_file(output_cpp_file_name, output_h_file_name, root_path, original_files, variable_names, data_sizes, extract_command, delete_command);
    return 0;

}

