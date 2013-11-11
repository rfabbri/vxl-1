#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <string.h>
#include <vcl_cstring.h>

#define MAX_BUF_SIZE 10000


struct vcl_string_ltstr
{
  bool operator()(vcl_string s1, vcl_string s2) const
  {
      return vcl_strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};

bool load_dictionary(vcl_map<vcl_string, vcl_string, vcl_string_ltstr> &dictionary, const char* dicstionary_filename)
{
    vcl_ifstream dictionary_file_istr;
    vcl_stringstream dictionary_SS;
    long fsize;
    char* buffer;
    vcl_string CH_word;
    vcl_string EN_word;
    char* line_buffer;

    // open dictionary file in BINARY format;
    dictionary_file_istr.open("dictionary.txt", vcl_ifstream::binary | vcl_ifstream::in);
    if(!dictionary_file_istr.good())
    {
        vcl_cerr << "Can not load dictionary data. " << vcl_endl;
        return false;
    }

    // load dictionary into a buffer then to a stringstream;
    dictionary_file_istr.seekg(0, vcl_ifstream::end);
    fsize = dictionary_file_istr.tellg();
    dictionary_file_istr.seekg(0, vcl_ifstream::beg);

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
        EN_word_len = vcl_strlen(line_buffer);
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
    vcl_string dictionary_filename, input_filename, output_filename;
    vcl_ofstream ostr;
    vcl_ifstream istr;
    vcl_stringstream input_SS, output_SS;
    vcl_map<vcl_string, vcl_string, vcl_string_ltstr> CH_EN_dictionary;
    vcl_map<vcl_string, vcl_string, vcl_string_ltstr>::iterator cur;
    long input_fsize, output_fsize;
    char* buffer;


    if(argc != 3)
    {
        vcl_cerr << "Incorrect number of arguments! " << vcl_endl;
        return -1;
    }

    if(!load_dictionary(CH_EN_dictionary, "dictionary.txt"))
    {
        vcl_cerr << "Failed to load disctionary. " << vcl_endl;
        return -1;
    };

/*
    for( cur = CH_EN_dictionary.begin(); cur != CH_EN_dictionary.end(); cur++)
    {
        vcl_cout << (*cur).first << vcl_endl;
        vcl_cout << CH_EN_dictionary[(*cur).first] << vcl_endl;
    }
*/
    

    // open input files;
    istr.open(argv[1], vcl_ifstream::binary | vcl_ifstream::in);
    if(!istr.good())
    {
        vcl_cerr << "Can not load dictionary data. " << vcl_endl;
        return false;
    }
    // load input file into a buffer then to a stringstream;
    istr.seekg(0, vcl_ifstream::end);
    input_fsize = istr.tellg();
    istr.seekg(0, vcl_ifstream::beg);

    buffer = new char[input_fsize];
    
    istr.read(buffer, input_fsize);
    input_SS.write(buffer, input_fsize);
    delete[] buffer;


    // open output files;
    ostr.open(argv[2], vcl_ofstream::binary | vcl_ofstream::out);
    if(!ostr.good())
    {
        vcl_cerr << "Can not load dictionary data. " << vcl_endl;
        istr.close();
        return false;
    }

    
    vcl_string CH_word, EN_words, input_sentence;
    char* CH_word_buffer = new char[1000];


    // doing the translation
    while(input_SS.tellg() < input_fsize)
    {
        input_SS >> input_sentence;
        vcl_cout << input_sentence << vcl_endl;

        output_SS << input_sentence;
        output_SS << "--->";

        unsigned cur_CH_char = 0;
        unsigned input_sentence_len = vcl_strlen(input_sentence.c_str());

        // searching the CH word through the whole sentence;
        while(cur_CH_char < input_sentence_len)
        {
            vcl_strcpy(CH_word_buffer, input_sentence.c_str()+cur_CH_char);
            
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
                    vcl_cout << "translation: " << CH_word << "--->" << CH_EN_dictionary[CH_word] << vcl_endl;
                    cur_CH_char += vcl_strlen(CH_word.c_str());
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

