#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <iostream>
#include <fstream>

using namespace std;

#include "surface_tracer.h"

/******************************************************************************/

void ReadInputsFile(const char inputs_file_name[],
                    char dt_file_dir[],
                    char dt_file_name[],
                    char eno_file_dir[],
                    char eno_file_prefix[],
                    char output_file_dir[],
                    char output_file_name[],
                    bool & dt_file_has_header,
                    bool & dt_file_has_shocks,
                    unsigned int file_dimensions[NUM_DIMENSIONS])
{
  // Open File
  ifstream in_file(inputs_file_name, ios::in);
  if(!in_file.is_open())
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Failed to open inputs file: " << inputs_file_name << endl;
    
    exit(1);
  }
  
  char line[MAX_STRING_LENGTH];
  while(in_file.getline(line, MAX_STRING_LENGTH))
  {
    if((strncmp("//", line, 2) == 0) || (strlen(line) == 0))
    {
      // Skip line
    }
    else if(strncmp("DT_FILE_DIR", line, 11) == 0)
    {
      if(!sscanf(line, "DT_FILE_DIR = %s", dt_file_dir))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DT_FILE_DIR from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("DT_FILE_NAME", line, 12) == 0)
    {
      if(!sscanf(line, "DT_FILE_NAME = %s", dt_file_name))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DT_FILE_NAME from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("ENO_FILE_DIR", line, 12) == 0)
    {
      if(!sscanf(line, "ENO_FILE_DIR = %s", eno_file_dir))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read ENO_FILE_DIR from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("ENO_FILE_PREFIX", line, 15) == 0)
    {
      if(!sscanf(line, "ENO_FILE_PREFIX = %s", eno_file_prefix))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read ENO_FILE_PREFIX from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("OUTPUT_FILE_DIR", line, 15) == 0)
    {
      if(!sscanf(line, "OUTPUT_FILE_DIR = %s", output_file_dir))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read OUTPUT_FILE_DIR from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("OUTPUT_FILE_NAME", line, 16) == 0)
    {
      if(!sscanf(line, "OUTPUT_FILE_NAME = %s", output_file_name))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read OUTPUT_FILE_NAME from inputs file." << endl;
        
        exit(1);
      }
    }
    else if(strncmp("DT_FILE_HAS_HEADER", line, 18) == 0)
    {
      int temp_int;
      if(!sscanf(line, "DT_FILE_HAS_HEADER = %d", &temp_int))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DT_FILE_HAS_HEADER from inputs file." << endl;
        
        exit(1);
      }
      dt_file_has_header = bool(temp_int);
    }
    else if(strncmp("DT_FILE_HAS_SHOCKS", line, 18) == 0)
    {
      int temp_int2;
      if(!sscanf(line, "DT_FILE_HAS_SHOCKS = %d", &temp_int2))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DT_FILE_HAS_SHOCKS from inputs file." << endl;
        
        exit(1);
      }
      dt_file_has_shocks = bool(temp_int2);
    }
    else if(strncmp("DEFAULT_FILE_DIMENSIONS_X", line, 25) == 0)
    {
      if(!sscanf(line, "DEFAULT_FILE_DIMENSIONS_X = %ud", &file_dimensions[DIM_X]))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DEFAULT_FILE_DIMENSIONS_X from inputs file."
             << endl;
        
        exit(1);
      }
    }
    else if(strncmp("DEFAULT_FILE_DIMENSIONS_Y", line, 25) == 0)
    {
      if(!sscanf(line, "DEFAULT_FILE_DIMENSIONS_Y = %ud", &file_dimensions[DIM_Y]))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DEFAULT_FILE_DIMENSIONS_Y from inputs file."
             << endl;
        
        exit(1);
      }
    }
    else if(strncmp("DEFAULT_FILE_DIMENSIONS_Z", line, 25) == 0)
    {
      if(!sscanf(line, "DEFAULT_FILE_DIMENSIONS_Z = %ud", &file_dimensions[DIM_Z]))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to read DEFAULT_FILE_DIMENSIONS_Z from inputs file."
             << endl;
        
        exit(1);
      }
    }
    else
    {
      // Can ignore line if it's all whitespace.
      char * line_ptr = line;
      do
      {
        if(!isspace(*line_ptr))
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "Invalid line in inputs file: " << endl << line << endl;
          
          exit(1);
        }
      } while(*line_ptr++);
    }
  }
  
  in_file.close();
}




