//FILEDIR.CPP
//040223 MingChing Chang

#include "base_filedir.h"

// "c:\abc\def.esf"
vcl_string getFileBaseName(vcl_string fname) {
  int l1,l2;
  l1=fname.find_last_of ('\\');
  l2=fname.find_last_of ('.');
  return fname.substr (l1+1, l2-l1-1);
}

// "c:\abc\def"
vcl_string getDirBaseName(vcl_string fname) {
  int l2;
  l2=fname.find_last_of('.');
  return fname.substr (0, l2);
}

// "c:\abc"
vcl_string getDirName(vcl_string fname) {
  int l2;
  l2=fname.find_last_of('\\');
  return fname.substr (0, l2);
}
