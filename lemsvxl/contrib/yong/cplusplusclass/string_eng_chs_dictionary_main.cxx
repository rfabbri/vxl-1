#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <string.h>
#include <cstring>

#define MAX_BUF_SIZE 10000


struct vcl_string_ltstr
{
  bool operator()(std::string s1, std::string s2) const
  {
      return std::strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};

bool load_dictionary(std::map<std::string, std::string, vcl_string_ltstr> &dictionary, const char* dicstionary_filename)
{
    std::ifstream dictionary_file_istr;
    std::stringstream dictionary_SS;
    long fsize;
    char* buffer;
    std::string CH_word;
    std::string EN_word;
    char* line_buffer;

    // open dictionary file in BINARY format;
    dictionary_file_istr.open("dictionary.txt", std::ifstream::binary | std::ifstream::in);
    if(!dictionary_file_istr.good())
    {
        std::cerr << "Can not load dictionary data. " << std::endl;
        return false;
    }

    // load dictionary into a buffer then to a stringstream;
    dictionary_file_istr.seekg(0, std::ifstream::end);
    fsize = dictionary_file_istr.tellg();
    dictionary_file_istr.seekg(0, std::ifstream::beg);

    buffer = new char[fsize];
    
    dictionary_file_istr.read(buffer, fsize);
    dictionary_SS.write(buffer, fsize);

    dictionary_file_istr.close();
    delete[] buffer;


    // load words from stringstream to a map
    line_buffer = new char[1000];

    unsigned EN_word_len;
    for(; dictionary_SS.tellg() < fsize; )
    {
        dictionary_SS >> CH_word;
        dictionary_SS.getline(line_buffer, 300);
        EN_word_len = std::strlen(line_buffer);
        line_buffer[EN_word_len-1] = '\0';

        EN_word.assign(line_buffer);
        dictionary[CH_word] = EN_word;
    }


    delete[] line_buffer;

    return true;

}
  
#if 0

int main(int argc, char **argv)
{
    std::string dictionary_filename, input_filename, output_filename;
    std::ofstream ostr;
    std::ifstream istr;
    std::stringstream input_SS, output_SS;
    std::map<std::string, std::string, vcl_string_ltstr> CH_EN_dictionary;
    std::map<std::string, std::string, vcl_string_ltstr>::iterator cur;
    long input_fsize, output_fsize;
    char* buffer;


    if(argc != 3)
    {
        std::cerr << "Incorrect number of arguments! " << std::endl;
        return -1;
    }

    if(!load_dictionary(CH_EN_dictionary, "dictionary.txt"))
    {
        std::cerr << "Failed to load disctionary. " << std::endl;
        return -1;
    };

/*
    for( cur = CH_EN_dictionary.begin(); cur != CH_EN_dictionary.end(); cur++)
    {
        std::cout << (*cur).first << std::endl;
        std::cout << CH_EN_dictionary[(*cur).first] << std::endl;
    }
*/
    

    // open input files;
    istr.open(argv[1], std::ifstream::binary | std::ifstream::in);
    if(!istr.good())
    {
        std::cerr << "Can not load dictionary data. " << std::endl;
        return false;
    }
    // load input file into a buffer then to a stringstream;
    istr.seekg(0, std::ifstream::end);
    input_fsize = istr.tellg();
    istr.seekg(0, std::ifstream::beg);

    buffer = new char[input_fsize];
    
    istr.read(buffer, input_fsize);
    input_SS.write(buffer, input_fsize);
    delete[] buffer;


    // open output files;
    ostr.open(argv[2], std::ofstream::binary | std::ofstream::out);
    if(!ostr.good())
    {
        std::cerr << "Can not load dictionary data. " << std::endl;
        istr.close();
        return false;
    }

    
    std::string CH_word, EN_words, input_sentence;
    char* CH_word_buffer = new char[1000];


    // doing the translation
    while(input_SS.tellg() < input_fsize)
    {
        input_SS >> input_sentence;
        std::cout << input_sentence << std::endl;

        output_SS << input_sentence;
        output_SS << "--->";

        unsigned cur_CH_char = 0;
        unsigned input_sentence_len = std::strlen(input_sentence.c_str());

        // searching the CH word through the whole sentence;
        while(cur_CH_char < input_sentence_len)
        {
            std::strcpy(CH_word_buffer, input_sentence.c_str()+cur_CH_char);
            
            unsigned CH_word_len = input_sentence_len;
            while(CH_word_len)
            {
                CH_word_buffer[CH_word_len] = '\0';

                CH_word.assign(CH_word_buffer);

                if(CH_EN_dictionary.find(CH_word) == CH_EN_dictionary.end())
                {
                    CH_word_len--;                
                    if(CH_word_len == 1)
                    {
                        output_SS << CH_word_buffer[0];
                        cur_CH_char++;
                        break;
                    }
                }
                else
                {
                    std::cout << "translation: " << CH_word << "--->" << CH_EN_dictionary[CH_word] << std::endl;
                    cur_CH_char += std::strlen(CH_word.c_str());
                    output_SS << CH_EN_dictionary[CH_word];
                    break;
                }


            }

        }

        output_SS << "\n";
    }
    delete[] CH_word_buffer;


    // save the output_SS into the output file;
    output_fsize = output_SS.tellp();
    buffer = new char[output_fsize];
    output_SS.read(buffer, output_fsize);
    ostr.write(buffer, output_fsize);
    ostr.flush();
    ostr.close();
    delete[] buffer;



    return 1;
}

#endif

