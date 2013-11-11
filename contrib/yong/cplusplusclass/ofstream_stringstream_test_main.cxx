#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>

#if 0

int main(int argc, char **argv)
{
    vcl_string filename;
    vcl_stringstream SS;
    vcl_ofstream ostr;
    vcl_ifstream istr;
    double my_number;
    vcl_string my_string;
    char option = 'w';

    
    while(option != 'e')
    {
        vcl_cout << "Please input your option (w: write file; r: read file; e: exit): ";
        vcl_cin >> option;
        switch(option)
        {
        case 'w':
            vcl_cout << "Please type filename: ";
            vcl_cin >> filename;
            vcl_cout << "Pleast type a number: ";
            vcl_cin >> my_number;

            // create 3 files for write;
            for(double i=1; i<=3; i++)
            {
                vcl_string fullname;

                SS << filename << "_" << i << ".txt";
                SS >> fullname;
                vcl_cout << "Open file: " << fullname << vcl_endl;
                SS.clear();
                

                ostr.open(fullname.c_str());

                // write the numbe into the file;
                ostr << my_number*i << "|" << my_number*i <<"|" << my_number*i;
                ostr.flush();
                ostr.close();
            }

            break;
        case 'r':

            vcl_cout << "Pleast type the filename: ";
            vcl_cin >> filename;
            vcl_cout << "filename is " << filename << vcl_endl;
            istr.open(filename.c_str());
            istr.clear();
            if(!istr.is_open())
            {
                vcl_cout << "can not open " << filename << "!" << vcl_endl;
                break;
            }

            istr >> my_number;
            istr >> my_string;
            vcl_cout << "The number read from file: "<< my_number << vcl_endl;
            vcl_cout << "The string read from file: "<< my_string << vcl_endl;
            istr.clear();
            istr.close();

            break;
        case 'e':
            return 1;
        default:
            vcl_cout << "not a valid option!" << vcl_endl;
            break;
        }

    }
    
    
    return 1;
}

#endif

