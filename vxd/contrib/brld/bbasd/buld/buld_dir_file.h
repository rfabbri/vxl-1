// This is buld/buld_dir_file.h
#ifndef buld_dir_file_h_
#define buld_dir_file_h_
//:
// \file
// \brief Directory and file read/write utilities
// \author Based on original code by  Ming-Ching Chang
// \date   Mar 07, 2007
//
// \verbatim
// Modifications
// \endverbatim

#include <cstdio>
#include <string>

std::string buld_get_dir (const std::string& filename);
std::string buld_get_prefix (const std::string& filename);
std::string buld_get_dir_prefix (const std::string& filename);
std::string buld_get_dir_prefix2 (const std::string& filename);
std::string buld_get_dir_file (const std::string& filename);
std::string buld_get_file (const std::string& filename);
std::string buld_get_suffix (const std::string& filename);

std::string buld_get_str_prior (const std::string& str, const std::string& sub);

std::string remove_commas (std::string& filename);

bool buld_copy_file(const std::string& inf, const std::string& outf);

#endif

