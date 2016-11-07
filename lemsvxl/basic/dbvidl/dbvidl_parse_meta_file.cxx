#include <dbvidl/dbvidl_parse_meta_file.h>
#include <vcl_fstream.h>
dbvidl_parse_meta_file::dbvidl_parse_meta_file()
{
}
dbvidl_parse_meta_file::~dbvidl_parse_meta_file()
{
}

bool dbvidl_parse_meta_file::parse(vcl_string filename)
{
    vcl_ifstream ifile(filename.c_str());
    if(!ifile)
        return false;

    char buf[2500];
    while(ifile.getline(buf,2500))
    {
        vcl_string str(buf);
        if(str.find("<file")!= vcl_string::npos)
        {
            int bpos=str.find_first_of("\"",0)+1;
            int epos=str.find_first_of("\"",bpos);

            vcl_string fname=str.substr(bpos,epos-bpos);
            filenames.push_back(fname);
            vcl_cout<<fname<<"\n";
        }

    }

    ifile.close();
    return true;
}

vcl_list<vcl_string>  dbvidl_parse_meta_file::getlist()
{
    return filenames;
}
