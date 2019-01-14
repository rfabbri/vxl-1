#include <dbvidl/dbvidl_parse_meta_file.h>
#include <fstream>
dbvidl_parse_meta_file::dbvidl_parse_meta_file()
{
}
dbvidl_parse_meta_file::~dbvidl_parse_meta_file()
{
}

bool dbvidl_parse_meta_file::parse(std::string filename)
{
    std::ifstream ifile(filename.c_str());
    if(!ifile)
        return false;

    char buf[2500];
    while(ifile.getline(buf,2500))
    {
        std::string str(buf);
        if(str.find("<file")!= std::string::npos)
        {
            int bpos=str.find_first_of("\"",0)+1;
            int epos=str.find_first_of("\"",bpos);

            std::string fname=str.substr(bpos,epos-bpos);
            filenames.push_back(fname);
            std::cout<<fname<<"\n";
        }

    }

    ifile.close();
    return true;
}

std::list<std::string>  dbvidl_parse_meta_file::getlist()
{
    return filenames;
}
