//FILEDIR.H
//040223 MingChing Chang

#ifndef _FILE_DIR_H_
#define _FILE_DIR_H_

#include <vcl_string.h>


// "c:\abc\def.esf"
vcl_string getFileBaseName(vcl_string fname);

// "c:\abc\def"
vcl_string getDirBaseName(vcl_string fname);

// "c:\abc"
vcl_string getDirName(vcl_string fname);


#endif

