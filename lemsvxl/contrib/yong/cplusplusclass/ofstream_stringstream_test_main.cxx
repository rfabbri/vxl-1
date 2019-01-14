#include <iostream>
#include <sstream>
#include <string>

#if 0

int main(int argc, char **argv)
{
    std::string filename;
    std::stringstream SS;
    std::ofstream ostr;
    std::ifstream istr;
    double my_number;
    std::string my_string;
    char option = 'w';

    
    while(option != 'e')
    {
        std::cout << "Please input your option (w: write file; r: read file; e: exit): ";
        std::cin >> option;
        switch(option)
        {
        case 'w':
            std::cout << "Please type filename: ";
            std::cin >> filename;
            std::cout << "Pleast type a number: ";
            std::cin >> my_number;

            // create 3 files for write;
            for(double i=1; i<=3; i++)
            {
                std::string fullname;

                SS << filename << "_" << i << ".txt";
                SS >> fullname;
                std::cout << "Open file: " << fullname << std::endl;
                SS.clear();
                

                ostr.open(fullname.c_str());

                // write the numbe into the file;
                ostr << my_number*i << "|" << my_number*i <<"|" << my_number*i;
                ostr.flush();
                ostr.close();
            }

            break;
        case 'r':

            std::cout << "Pleast type the filename: ";
            std::cin >> filename;
            std::cout << "filename is " << filename << std::endl;
            istr.open(filename.c_str());
            istr.clear();
            if(!istr.is_open())
            {
                std::cout << "can not open " << filename << "!" << std::endl;
                break;
            }

            istr >> my_number;
            istr >> my_string;
            std::cout << "The number read from file: "<< my_number << std::endl;
            std::cout << "The string read from file: "<< my_string << std::endl;
            istr.clear();
            istr.close();

            break;
        case 'e':
            return 1;
        default:
            std::cout << "not a valid option!" << std::endl;
            break;
        }

    }
    
    
    return 1;
}

#endif

