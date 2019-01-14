
#include <sstream>
#include "base_sstream2.h"

SStream2::SStream2(const std::string& str_,  const std::string& specialChars_) 
  :  _origStr(str_)
{
  ///std::istringstream(str_),
  StdDelim = ' ';
  StdQuote = '\"';
  StdEsc = '\\';
  setSpecialChars(specialChars_);
}

SStream2::SStream2(const std::string& str_,  
         const std::vector<std::string>& specialStrings_) 
  :  _origStr(str_)
{
  ///std::istringstream(str_),
  StdDelim = ' ';
  StdQuote = '\"';
  StdEsc = '\\';
  setSpecialStrings(specialStrings_);
}

SStream2::~SStream2(){};

void SStream2::setSpecialChars(const std::string& specialChars_)
{
  std::vector<std::string> specialStrings;
  for(int cnt=0; cnt<specialChars_.size() ; cnt++){
    std::string str; str.append(1,specialChars_[cnt]);
    specialStrings.push_back(str);
  }
  setSpecialStrings(specialStrings);
}

void SStream2::setSpecialStrings(const std::vector<std::string>& specialStrings_) {
  _specialStrings = specialStrings_;
  _convStr();
}

void SStream2::_convStr() {
  std::istringstream istrm(_origStr);
  std::ostringstream ostrm;
  while(!istrm.eof()){
    std::string str1,str2;
    istrm >> str1;
    for(int curPos=0; curPos<str1.size() ; ){
      int pos;
      if(str1[curPos] == StdQuote){
  // If this were the beginning of the quotation.
  // Extract up to the next quotation mark.
  int curPos2=curPos;
  while(curPos2<str1.size()){
    if((pos=str1.find(StdQuote,curPos2+1))!=std::string::npos){
      // Check for the backslash before the quotation
      if(str1[pos-1] == StdEsc){
        curPos2 = pos+1;
        continue;
      }else{
        if(pos-(curPos+1)>0)
    ostrm << str1.substr(curPos+1,pos-(curPos+1)) << StdDelim;
        curPos = pos+1; // Make sure to skip the second quotation.
        break;
      }
    }
  }
  if(curPos2 == str1.size() && curPos+1<str1.size()){
    if(str1.size()-curPos>0)
      ostrm << str1.substr(curPos+1,str1.size()-curPos) << StdDelim;
    curPos = str1.size(); // I know this isn't necessary, but whatever.
    break;
  }
      }

      int endSearchPos;
      if((pos=str1.find(StdQuote,curPos))!=std::string::npos){
  // If there's a quotation up ahead,
  // limit the search to the before that point.
  endSearchPos = pos;
      }else{
  endSearchPos = str1.size();
      }

      // Check for special strings.
      std::vector<std::string>::iterator itr=_specialStrings.begin();
      for(;itr!=_specialStrings.end() ; itr++) {
  if((pos=str1.find(*itr,curPos))!=std::string::npos &&
     pos<endSearchPos){
    if(pos-curPos>0)
      ostrm << str1.substr(curPos,(pos-curPos)) << StdDelim;
    ostrm << *itr << StdDelim;
    curPos = pos+itr->size();
    break;
  }
      }
      if(itr==_specialStrings.end()){
  // No special strings found.
  ostrm << str1.substr(curPos) << StdDelim;
  break;
      }
    }
  }
  // Reset the underlining std::string to the new _resStr;
  _resStr = ostrm.str();
  this->str(_resStr);
}

Check::Check(const std::string& skipStr_)
  : _skipStr(skipStr_)
{}

Check::~Check(){}

std::istream &operator >> (std::istream &strm, const Check &obj) {
  std::string str;
  strm >> str;
  if(str != obj._skipStr){
#if 0
    strm.setf(ios::failbit | ios::eofbit);
#else
    // The above flag-setting doesn't work for some reason.
    // So, empty out the stream to signify the error instead.
    while(strm>>str)
      ;
#endif
  }
  return strm;
}

