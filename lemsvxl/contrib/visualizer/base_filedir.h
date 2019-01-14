//FILEDIR.H
//040223 MingChing Chang

#ifndef _FILE_DIR_H_
#define _FILE_DIR_H_

#include <string>


// "c:\abc\def.esf"
std::string getFileBaseName(std::string fname);

// "c:\abc\def"
std::string getDirBaseName(std::string fname);

// "c:\abc"
std::string getDirName(std::string fname);


#endif

