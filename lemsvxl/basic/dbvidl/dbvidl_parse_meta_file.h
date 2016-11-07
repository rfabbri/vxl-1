#ifndef _dbvidl_parse_meta_file_h_
#define _dbvidl_parse_meta_file_h_

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

class dbvidl_parse_meta_file{

public:
    dbvidl_parse_meta_file();
    ~dbvidl_parse_meta_file();

    bool parse(vcl_string  filename);
    vcl_list<vcl_string>  getlist();


protected:
    vcl_list<vcl_string> filenames;
};

#endif
