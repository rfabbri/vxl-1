#include <vul/vul_arg.h>
#include <vcl_cstring.h> // needed for strcmp()
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>


#if 0

#define MAX_FCOPY_BUF_SIZE 100000 // uint: byte


int my_fcopy(vcl_string const input_filename, vcl_string const output_filename)
{

    vcl_ofstream ostr;
    vcl_ifstream istr;
    unsigned long input_fsize;
    char* fcopy_buffer;
    unsigned long copy_byte_cnt = 0;

    vcl_string my_string;

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


int main(int argc, char* argv[])
{
  vcl_stringstream SS;
  vcl_string source_filename;
  vcl_string des_filename;

  vul_arg_info_list arglist;
  vul_arg<char const*>     source_base_filename(arglist, 0, "Source filename");
  vul_arg<char const*>     des_base_filename(arglist, 0, "Destination filename");
  vul_arg<char const*>     arg_ext(arglist, 0, "image file extension");
  vul_arg<unsigned>           arg_dig_number(arglist, 0, "Number of Digits in filename");
  vul_arg<vcl_list<int> >  arg_ind(arglist, "-ind", "indices of image frames...");

  arglist.parse(argc, argv, true);

  //check the input image indices
  vcl_list<int> indices = arg_ind();

  unsigned start_ind = 0;
  unsigned cur_ind = start_ind;


  if(!indices.size())
  {    
      for(cur_ind = 0; ; cur_ind++)
      {
          SS.clear();
          source_filename.clear();
          des_filename.clear();

          SS << source_base_filename();
          SS << vcl_setw (arg_dig_number());
          SS << vcl_setfill('0');
          SS << cur_ind;
          SS << ".";
          SS << arg_ext();
          SS >> source_filename;

          if(!vul_file::exists(source_filename))
          {
              vcl_cout << "File: " << source_filename << " does not exist. Copy is finished. " << vcl_endl;
              break;
          }
          else
          {
              vcl_cout << "Copy File: " << source_filename;
          }

          SS.clear();
          SS << des_base_filename();
          SS << vcl_setw (arg_dig_number());
          SS << vcl_setfill('0');
          SS << cur_ind;
          SS << ".";
          SS << arg_ext();
          SS >> des_filename;

          if(my_fcopy(source_filename, des_filename) != 1)
          {
            return -1;
          }
          else
          {
            vcl_cout << " is done!" << vcl_endl;
          }
      }
  }
  else
  {
      unsigned indices_number = indices.size();
      vcl_list<int>::iterator iter;
      for(iter = indices.begin(); iter != indices.end(); iter++)
      {
          cur_ind = (*iter);

          SS.clear();
          source_filename.clear();
          des_filename.clear();

          SS << source_base_filename();
          SS << vcl_setw (arg_dig_number());
          SS << vcl_setfill('0');
          SS << cur_ind;
          SS << ".";
          SS << arg_ext();
          SS >> source_filename;

          if(!vul_file::exists(source_filename))
          {
              vcl_cout << "File: " << source_filename << " does not exist. Copy is finished. " << vcl_endl;
              break;
          }
          else
          {
              vcl_cout << "Copy File: " << source_filename;
          }

          SS.clear();
          SS << des_base_filename();
          SS << vcl_setw (arg_dig_number());
          SS << vcl_setfill('0');
          SS << cur_ind;
          SS << ".";
          SS << arg_ext();
          SS >> des_filename;


          if(my_fcopy(source_filename, des_filename) != 1)
          {
            return -1;
          }
          else
          {
            vcl_cout << " is done!" << vcl_endl;
          }

      }
  }

  return 1;
}

#endif


#if 0
void main(int argc, char* argv[])
{  
  vcl_cout << "Current directory: " << vul_file::get_cwd() << vcl_endl;    
    vcl_cout << "Change directory to huston" << vcl_endl;
    vul_file::change_directory("huston");
    vcl_cout << "Current directory: " << vul_file::get_cwd() << vcl_endl;
    vul_file::make_directory("huston_2");

    vul_file::make_directory_path ("huston_3");
    
    if(vul_file::is_directory("C:\\VXL\\brown_eyes_build\\contrib\\yong\\cplusplusclass\\debug\\huston"))
    {
        vcl_cout << "this is a directory" << vcl_endl;
    }
    else
    {
        vcl_cout << "this is NOT a directory" << vcl_endl;
    }

    vcl_cout << vul_file::expand_tilde (vcl_string("World_Map_Blocked_0.tiff")) << vcl_endl;

    if(vul_file::exists("World_Map_Blocked_0.tiff"))
    {
        vcl_cout << "World_Map_Blocked_0.tiff exists!" << vcl_endl;
    }
    else
    {
        vcl_cout << "World_Map_Blocked_0.tiff does NOT exist!" << vcl_endl;
    }


    vcl_cout << "Size of file World_Map_Blocked_0.tiff is " << vul_file::size("World_Map_Blocked_0.tiff") << vcl_endl;

    vcl_cout << "The dir name of file World_Map_Blocked_0.tiff is " << vul_file::dirname("World_Map_Blocked_0.tiff") << vcl_endl;

    vcl_cout << "The extension of file World_Map_Blocked_0.tiff is " << vul_file::extension("World_Map_Blocked_0.tiff") << vcl_endl;

    vcl_cout << "The base name of file World_Map_Blocked_0.tiff is " << vul_file::basename("World_Map_Blocked_0.tiff", "0.tiff") << vcl_endl;

    vcl_cout << "The file name after strip dir of World_Map_Blocked_0.tiff is " << vul_file::strip_directory("C:\\VXL\\brown_eyes_build\\contrib\\yong\\cplusplusclass\\debug\\huston\\World_Map_Blocked_0.tiff") << vcl_endl;

    vcl_cout << "The file name after strip ext of World_Map_Blocked_0.tiff is " << vul_file::strip_extension("C:\\VXL\\brown_eyes_build\\contrib\\yong\\cplusplusclass\\debug\\huston\\World_Map_Blocked_0.tiff") << vcl_endl;




  if (argc > 1) {
    // Initialize a vul_sequence_filename_map with the pattern
    vul_sequence_filename_map map(argv[1]);

    // Print out all the files that are specified by that pattern.
    for (int i = 0; i < map.get_nviews(); ++i) {
      vcl_cout << map.name(i) << vcl_endl;
    }
  } else {
    // Capes' examples
    {
      vul_sequence_filename_map map("images/img.%03d.pgm,0:2:100");
    }
    {
      vul_sequence_filename_map map("plop/plip/img_####.tiff",5,10,200);
    }
    {
      vul_sequence_filename_map map("pop.pop/poo_poo/img.###.ppm;1:2:10");
    }
    {
      vul_sequence_filename_map map("im.###,:5:");
      vcl_cerr << map.name(10) << vcl_endl
               << map.pair_name(10,11) << vcl_endl
               << map.triplet_name(10,11,12) << vcl_endl
               << map.image_name(10) << vcl_endl;
    }
  }


    return;


}

#endif

#if 0

vul_arg<double> a_naughty_global_arg("-hack", "Fudge", 1.2);

void main(int argc, char* argv[])
{
  vul_arg<char const*>     a_filename(0, "Input filename");
  vul_arg<char const*>     b_filename(0, "Output filename");
  vul_arg<char const*>     c_filename(0, "Other filename");
  vul_arg<bool>            a_fast("-fast", "Go fast", false);
  vul_arg_parse(argc, argv);
  a_filename.print_value(vcl_cout<<vcl_endl);
  a_naughty_global_arg.print_value(vcl_cout<<vcl_endl);
  a_fast.print_value(vcl_cout<<vcl_endl);

  vcl_cout << vcl_endl << a_naughty_global_arg.option() << vcl_endl;
  vcl_cout << a_naughty_global_arg.help() << vcl_endl;


  vcl_cerr << vcl_endl << "Filename [" << a_filename() << "]\n";
}


#endif

#if 0


char const * my_argv_1[] = {
  "progname",
  "f",
  "-int", "3",
  "g",
  "-bool1",
  "-bool1",
  "-bool2",
  "-list1",
  "1:2,10,21:3:25,-1:-2:-7",
  "-list2",
  "1.5,-1.5,8.0",
  "h",
  "i",
  "j",
  0
};

int list1_contents[] = {
  1,2,10,21,24,-1,-3,-5,-7
};

double list2_contents[] = {
  1.5,-1.5,8.0
};


int count_my_args(char const * const * my_argv)
{
  int c = 0;
  for (; *my_argv; ++my_argv)
    ++c;
  return c;
}

void test_do_vul_arg()
{
  vul_arg_info_list arglist;

  vul_arg<int> int1(arglist, "-int", "A help string", 1);
  vul_arg<int> int2(arglist, "-int2", "Another help string", 2);
  vul_arg<bool> bool1(arglist, "-bool1", "And another", false);
  vul_arg<bool> bool2(arglist, "-bool2", "And another", true);
  vul_arg<bool> bool3(arglist, "-bool3", "And a final help test just to finish off...", true);
  vul_arg<vcl_list<int> > list1(arglist, "-list1", "List...");
  vul_arg<char*> filename1(arglist);
  vul_arg<vcl_vector<double> > list2(arglist, "-list2", "double List...");

  int my_argc = count_my_args(my_argv_1);
  vcl_cout << "vul_argc = " << my_argc
           << ", int1 = " << int1()
           << ", int2 = " << int2()
           << ", bool1 = " << bool1()
           << ", bool2 = " << bool2()
           << ", bool3 = " << bool3()
           << ", list1 size = " << list1().size()
           << ", list2 size = " << list2().size()
           << vcl_endl;
  char **my_argv = (char**) my_argv_1;

  arglist.parse(my_argc, my_argv, true);

//  TEST("int1", int1(), 3);
//  TEST("int2", int2(), 2);
//  TEST("filename == f", vcl_strcmp(filename1(), "f"), 0);

  {
    unsigned true_list_length = sizeof list1_contents / sizeof list1_contents[0];
    vcl_list<int> l = list1();
    //TEST("list1 length", l.size(), true_list_length);
    bool ok = true;
    for (unsigned int i = 0; i < true_list_length; ++i) {
      if (vcl_find(l.begin(), l.end(), list1_contents[i]) == l.end()) {
        vcl_cout << "Integer [" << list1_contents[i] << "] not found in list\n";
        ok = false;
      }
    }
//    TEST("list1 contents", ok, true);
  }
  {
    unsigned true_list_length = sizeof list2_contents / sizeof list2_contents[0];
    vcl_vector<double> l = list2();
//    TEST("list2 length", l.size(), true_list_length);
    bool ok = true;
    for (unsigned int i = 0; i < true_list_length; ++i) {
      if (vcl_find(l.begin(), l.end(), list2_contents[i]) == l.end()) {
        vcl_cout << "Value [" << list2_contents[i] << "] not found in list\n";
        ok = false;
      }
    }
//    TEST("list2 contents", ok, true);
  }

  
}


void test_arg()
{
  test_do_vul_arg();
}

//TESTMAIN(test_arg);
int main(int, char*[])
{

  test_arg();

  return 1;
}


#endif
