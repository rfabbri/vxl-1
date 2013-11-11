#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <string.h>
#include <vcl_cstring.h>
#include <vcl_bitset.h>

#define CRC32_MPEG2_NORMAL_DWORD 0x04C11DB7
#define ULONG_B31_MASK 0x80000000
#define ULONG_B0_MASK 0x1


int load_input_files(const char* filename, vcl_stringstream& input_stringstream)
{
    vcl_ifstream istr;
    char * buffer;
    unsigned input_fsize;

    // open input files;
    istr.open(filename, vcl_ifstream::binary | vcl_ifstream::in);
    if(!istr.good())
    {
        vcl_cerr << "Can not open input file. " << vcl_endl;
        return -1;
    }
    // load input file into a buffer then to a stringstream;
    istr.seekg(0, vcl_ifstream::end);
    input_fsize = istr.tellg();
    vcl_cout << "Input file size: " << input_fsize << vcl_endl;
    istr.seekg(0, vcl_ifstream::beg);

    buffer = new char[input_fsize];
    
    istr.read(buffer, input_fsize);
    istr.close();
    input_stringstream.write(buffer, input_fsize);
    delete[] buffer;

    return 1;
}

int generate_1D_parity_checksum(vcl_stringstream& input_stringstream, const char * original_filename)
{
    vcl_ofstream ostr;
    vcl_stringstream output_filename_SS;
    vcl_string output_filename;
    char parity_checksum = 0;
    long data_size;
    char cur_data;
    char* buffer;


    data_size = input_stringstream.tellp();
    vcl_cout << "data_size: " << data_size << vcl_endl;

    for(input_stringstream.seekg(vcl_stringstream::beg); input_stringstream.tellg() < data_size; )
    {
        cur_data = input_stringstream.get();
        parity_checksum ^= cur_data;

    }

    output_filename_SS << "p_";
    output_filename_SS << original_filename;
    output_filename_SS >> output_filename;

    // open output files;
    ostr.open(output_filename.c_str(), vcl_ofstream::binary | vcl_ofstream::out);
    if(!ostr.good())
    {
        vcl_cerr << "Can not open output file! " << vcl_endl;
        return -1;
    }

    buffer = new char[data_size+1];
    input_stringstream.seekg(vcl_stringstream::beg);
    input_stringstream.read(buffer, data_size);
    buffer[data_size] = parity_checksum;

    ostr.write(buffer, data_size+1);
    ostr.flush();
    ostr.close();
    delete[] buffer;

    return 1;
}

int check_data_integrity_using_1D_parity(vcl_stringstream& input_stringstream)
{
    char parity_checksum = 0;
    long data_size;
    char cur_data;

    data_size = input_stringstream.tellp();
    vcl_cout << "data_size: " << data_size << vcl_endl;
    for(input_stringstream.seekg(vcl_stringstream::beg); input_stringstream.tellg() < data_size; )
    {
        cur_data = input_stringstream.get();
        parity_checksum ^= cur_data;
    }

    if(!parity_checksum)
    {
        vcl_cout << "The file ingerity is good by checking 1D parity checksum!" << vcl_endl;
        return 1;
    }
    else
    {
        vcl_cout << "The file ingerity is bad by checking 1D parity checksum!" << vcl_endl;
        return -1;
    }
}


int generate_CRC32_MPEG2_checksum(vcl_stringstream& input_stringstream, const char * original_filename)
{
    vcl_ofstream ostr;
    vcl_stringstream output_filename_SS;
    vcl_string output_filename;
    long data_size;
    char cur_data;
    char* buffer;
    unsigned long CRC32_MPEG2_normal = CRC32_MPEG2_NORMAL_DWORD;
    bool cur_bit;
    unsigned long REGISTER_32 = 0;
    bool REGISTER_b31;
    

    // stuff 32 empty bits into the input data;
    for(int i=0; i<4; i++)    input_stringstream << static_cast<char>(0);

    data_size = input_stringstream.tellp();
    vcl_cout << "data_size: " << data_size << vcl_endl;

    for(input_stringstream.seekg(vcl_stringstream::beg); input_stringstream.tellg() < data_size;)
    {
        cur_data = input_stringstream.get();

        for(unsigned bit_mask = static_cast<unsigned>(0x80); bit_mask != 0; bit_mask = bit_mask>>1)
        {
            if(cur_data & static_cast<char>(bit_mask))
                cur_bit = 1;
            else
                cur_bit = 0;

            REGISTER_b31 = !((REGISTER_32 & ULONG_B31_MASK) == 0);

            REGISTER_32 = REGISTER_32<<1;
            REGISTER_32 = (REGISTER_32 & (~ULONG_B0_MASK)) + static_cast<unsigned long>(cur_bit);

            if(REGISTER_b31)
            {
                REGISTER_32 ^= CRC32_MPEG2_normal;
            }
        }
    }

    output_filename_SS << "r_";
    output_filename_SS << original_filename;
    output_filename_SS >> output_filename;

    // open output files;
    ostr.open(output_filename.c_str(), vcl_ofstream::binary | vcl_ofstream::out);
    if(!ostr.good())
    {
        vcl_cerr << "Can not open output file! " << vcl_endl;
        return -1;
    }

    buffer = new char[data_size];
    input_stringstream.seekg(vcl_stringstream::beg);
    input_stringstream.read(buffer, data_size);

    ostr.write(buffer, data_size-4);
    for(int i=24; i>=0; i-=8) ostr.put(static_cast<char>(REGISTER_32 >> i));
//    for(int i=24; i>=0; i-=8) ostr.put((char)(REGISTER_32 >> i));
//    for(int i=24; i>=0; i-=8) ostr.put((REGISTER_32 >> i));

    ostr.flush();
    ostr.close();
    delete[] buffer;

    return 1;
}

int check_data_integrity_using_CRC32_MPEG2(vcl_stringstream& input_stringstream)
{
    vcl_ofstream ostr;
    vcl_stringstream output_filename_SS;
    vcl_string output_filename;
    long data_size;
    char cur_data;
    vcl_bitset<32> CRC32_MPEG2_normal(CRC32_MPEG2_NORMAL_DWORD);
    bool cur_bit;
    vcl_bitset<32> REGISTER_32(0);
    bool REGISTER_b31;
    

    data_size = input_stringstream.tellp();
    vcl_cout << "data_size: " << data_size << vcl_endl;

    for(input_stringstream.seekg(vcl_stringstream::beg); input_stringstream.tellg() < data_size; )
    {
        cur_data = input_stringstream.get();

        for(unsigned bit_mask = static_cast<unsigned>(0x80); bit_mask != 0; bit_mask = bit_mask>>1)
        {
            if(cur_data & static_cast<char>(bit_mask))
                cur_bit = 1;
            else
                cur_bit = 0;

            REGISTER_b31 = REGISTER_32[31];

            REGISTER_32 = REGISTER_32 << 1;
            REGISTER_32 = (REGISTER_32.to_ulong() & (~ULONG_B0_MASK)) + static_cast<unsigned long>(cur_bit);

            if(REGISTER_b31)
            {
                REGISTER_32 ^= CRC32_MPEG2_normal;
            }
        }
    }

    if(!REGISTER_32.to_ulong())
    {
        vcl_cout << "The file ingerity is good by checking CRC32_MPEG2 checksum!" << vcl_endl;
        return 1;
    }
    else
    {
        vcl_cout << "The file ingerity is bad by checking CRC32_MPEG2 checksum!" << vcl_endl;
        return -1;
    }


    return 1;
}


#if 0
int check_data_integrity_using_CRC32_MPEG2(vcl_stringstream& input_stringstream)
{
    vcl_ofstream ostr;
    vcl_stringstream output_filename_SS;
    vcl_string output_filename;
    long data_size;
    char cur_data;
    unsigned long CRC32_MPEG2_normal = CRC32_MPEG2_NORMAL_DWORD;
    bool cur_bit;
    unsigned long REGISTER_32 = 0;
    bool REGISTER_b31;
    

    data_size = input_stringstream.tellp();
    vcl_cout << "data_size: " << data_size << vcl_endl;

    int count = 0;
    for(input_stringstream.seekg(vcl_stringstream::beg); input_stringstream.tellg() < data_size; count++)
    {
        cur_data = input_stringstream.get();

        if(data_size - count < 7)
            vcl_cout << "cur_data: " << (int)cur_data << ". REGISTER_32: " << REGISTER_32 << vcl_endl;

        for(unsigned bit_mask = (unsigned)0x80; bit_mask != 0; bit_mask = bit_mask>>1)
        {
            if(cur_data & (char)bit_mask)
                cur_bit = 1;
            else
                cur_bit = 0;

            REGISTER_b31 = !((REGISTER_32 & ULONG_B31_MASK) == 0);

            REGISTER_32 = REGISTER_32<<1;
            REGISTER_32 = (REGISTER_32 & (~ULONG_B0_MASK)) + (unsigned long)cur_bit;

            if(REGISTER_b31)
            {
                REGISTER_32 ^= CRC32_MPEG2_normal;
            }
        }
    }

    if(!REGISTER_32)
    {
        vcl_cout << "The file ingerity is good by checking CRC32_MPEG2 checksum!" << vcl_endl;
        return 1;
    }
    else
    {
        vcl_cout << "The file ingerity is bad by checking CRC32_MPEG2 checksum!" << vcl_endl;
        return -1;
    }

}
#endif


#if 0

int main(int argc, char **argv)
{
    vcl_string dictionary_filename, input_filename, output_filename;
    vcl_ofstream ostr;
    vcl_stringstream input_SS, output_SS;
    vcl_string option;

    vcl_bitset<10> bs;

    if(argc != 2)
    {
        vcl_cerr << "Incorrect number of arguments! " << vcl_endl;
        return -1;
    }
    
    if(load_input_files(argv[1], input_SS) == -1)
    {
        vcl_cout << "Failed to load input file. " << vcl_endl;
        return -1;
    }


    vcl_cout << "please input your option: " << vcl_endl 
        << "  generate 1D parity checksum: (1)" << vcl_endl
        << "  check the integrity using 1D parity: (2)" << vcl_endl 
        << "  generate CRC-32-MPEG2 checksum: (3)" << vcl_endl
        << "  check the integrity using CRC-32-MPEG2: (4)" << vcl_endl
        << "  exit: (e)" << vcl_endl;
    
    vcl_cin >> option;

    while(option != vcl_string("e"))
    {
        switch(*(option.c_str()))
        {
            case '1':
                vcl_cout << "Generate 1D parity checksum" << vcl_endl;
                return generate_1D_parity_checksum(input_SS, argv[1]);

            case '2':
                vcl_cout << "Check the data integrity using 1D parity checksum" << vcl_endl;
                return check_data_integrity_using_1D_parity(input_SS);
                break;
            
            case '3':
                vcl_cout << "Generate CRC-32-MPEG2 checksum" << vcl_endl;
                return generate_CRC32_MPEG2_checksum(input_SS, argv[1]);

            case '4':
                vcl_cout << "Check the data integrity using CRC-32-MPEG2 checksum" << vcl_endl;
                return check_data_integrity_using_CRC32_MPEG2(input_SS);
                break;

            case 'e':
                break;

            default:
                vcl_cout << "Not a valid option." << vcl_endl;
                break;

        }
    }

    return 1;
}

#endif

