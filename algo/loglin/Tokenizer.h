#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vcl_functional.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

//
//For the case the default is a space.
//This is the default predicate for the Tokenize() function.
//
class SepSpace : public unary_function<char, bool>
  {
    public:
        inline bool operator()(char c) const
          {
            //isspace<char> returns true if c is a white-space character (0x09-0x0D or 0x20)
            //return isspace<char>(c);
            return isspace(c);
           }
   };


//
//For the case the separator is a comma
//
class SepComma : public unary_function<char, bool>
  {
    public:
        inline bool operator()(char c) const
          {
            return (',' == c);
           }
   };


//
//For the case the separator is a character from a set of characters given in a string
//
class SepFromString : public unary_function<char, bool>
  {
    public:
        //Constructor specifying the separators
        SepFromString::SepFromString(string const& rostr) : m_ostr(rostr) {}
        inline bool operator()(char c) const
          {
            int iFind = m_ostr.find(c);
            if((unsigned int)iFind != string::npos)
                return true;
            else
                return false;
           }
        
    private:
        string m_ostr;
   };


//String Tokenizer
template <class Pred=SepSpace>
class StringTokenizer
  {
    public:
        //The predicate should evaluate to true when applied to a separator.
        static void Tokenize(vcl_vector<string>& roResult, string const& rostr, Pred const& roPred=Pred())
          {
            string::const_iterator it = rostr.begin();
            string::const_iterator itTokenEnd = rostr.begin();
            while(it != rostr.end())
              {
                //Eat seperators
                while((roPred(*it))&&(it!=rostr.end()))
                    it++;

                if (it==rostr.end())
                    break;


                //Find next token
                itTokenEnd = find_if(it, rostr.end(), roPred);
                //Append token to result
                if(it < itTokenEnd)
                    roResult.push_back(string(it, itTokenEnd));
                it = itTokenEnd;
               }
           }
   };

#endif //TOKENIZER_H

