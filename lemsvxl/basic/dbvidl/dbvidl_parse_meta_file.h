#ifndef _dbvidl_parse_meta_file_h_
#define _dbvidl_parse_meta_file_h_

#include <iostream>
#include <string>
#include <list>

class dbvidl_parse_meta_file{

public:
    dbvidl_parse_meta_file();
    ~dbvidl_parse_meta_file();

    bool parse(std::string  filename);
    std::list<std::string>  getlist();


protected:
    std::list<std::string> filenames;
};

#endif
