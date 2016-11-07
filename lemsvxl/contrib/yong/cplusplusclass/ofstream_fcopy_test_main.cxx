#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>


#define MAX_FCOPY_BUF_SIZE 10000 // uint: byte

#if 0

int main(int argc, char **argv)
{
    vcl_string input_filename;
    vcl_string output_filename;
    vcl_stringstream SS;
    vcl_ofstream ostr;
    vcl_ifstream istr;
    unsigned long input_fsize;
    char* fcopy_buffer;
    unsigned long copy_byte_cnt = 0;

    vcl_string my_string;
    char option = 'w';

    if(argc != 3)
    {
        vcl_cerr << "Incorrect number of arguments! " << vcl_endl;
        return -1;
    }

    // assign filename from programe arguments to stings
    input_filename.assign(argv[1]);
    output_filename.assign(argv[2]);

    vcl_cout << "Starting copying " << input_filename << " to " << output_filename << "." << vcl_endl;

    // open input and output files in binary format.
    istr.open(input_filename.c_str(), vcl_ifstream::binary | vcl_ifstream::in);
    if(!istr.good())
    {
        vcl_cerr << "Input file " << input_filename << " can not be openned successfully. " << vcl_endl;
        return -1;
    }

    ostr.open(output_filename.c_str(), vcl_ofstream::binary | vcl_ofstream::out);
    if(!ostr.good())
    {
        vcl_cerr << "Output file " << output_filename << " can not be openned successfully. " << vcl_endl;
        return -1;
    }
    

    // read the length of the inputfile.
    istr.seekg(0, vcl_ifstream::end);
    input_fsize = istr.tellg();
    istr.seekg(0, vcl_ifstream::beg);

    // create the file copy buffer
    if(input_fsize <= MAX_FCOPY_BUF_SIZE)
    {
        fcopy_buffer = new char[input_fsize];
    }
    else
    {
        fcopy_buffer = new char[MAX_FCOPY_BUF_SIZE];

    }

    // copy the file;
    while(copy_byte_cnt < input_fsize)
    {
        if(input_fsize - copy_byte_cnt > MAX_FCOPY_BUF_SIZE)
        {
            istr.read(fcopy_buffer, MAX_FCOPY_BUF_SIZE);
            ostr.write(fcopy_buffer, MAX_FCOPY_BUF_SIZE);
            ostr.flush();
            vcl_cout << ".";
            copy_byte_cnt += MAX_FCOPY_BUF_SIZE;
        }
        else
        {
            istr.read(fcopy_buffer, input_fsize - copy_byte_cnt);
            ostr.write(fcopy_buffer, input_fsize - copy_byte_cnt);
            ostr.flush();
            vcl_cout << ".";
            copy_byte_cnt += input_fsize - copy_byte_cnt;
        }
    }
    
    delete[] fcopy_buffer;

    istr.close();
    ostr.close();
    return 1;
}

#endif

