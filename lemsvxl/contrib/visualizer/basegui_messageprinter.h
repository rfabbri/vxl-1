#ifndef MESSAGE_PRINTER_H
#define MESSAGE_PRINTER_H

#include <iostream>

#include <vector>
#include <string>
#include <fstream>

#define    NO_PRINT                       0
#define    TTY_PRINT                      1
#define    FILE_PRINT                     2

class MessagePrinter
  {
    private:
        static int       _state;
        static int       _priorityThreshold;
        static std::string    _outputFileName;
        static std::ofstream   _fp;
        static int       _count;
        

    public:
        MessagePrinter()
          {
            _state = TTY_PRINT;
            _count++;
            _priorityThreshold = 1000;
           }
   
        ~MessagePrinter()
          {
            _count--;
            if (_count==0)
             {
               if (_fp.is_open())
                   _fp.close();
              }
           }


        static int PrintMessage(std::string mesg_, int priority_, std::string functionName_, std::string fileName_, int lineNumber_)
          {
            if (priority_>_priorityThreshold)
              {
                return 0;
               }
          
           switch(_state)
             {
               case(NO_PRINT):
                     {
                       if (priority_>0)
                         {
                           return 0;
                          }
                       std::cout<<"{"<<fileName_<<" : "<<lineNumber_<<"} <"<<functionName_<<"> "<<std::endl;
                       std::cout<<mesg_<<std::endl;
                       return 0;
                      };

               case(TTY_PRINT):
                     {
                       std::cout<<"{"<<fileName_<<" : "<<lineNumber_<<"} <"<<functionName_<<"> "<<std::endl;
                       std::cout<<mesg_<<std::endl;
                       return 0;
                      }

               case(FILE_PRINT):
                     {
                       if (_outputFileName.length()==0)
                         {
                           std::cout<<"{"<<__FILE__<<" : "<<__LINE__<<"} Output file name not set:"<<std::endl;
                           return 0;
                          }
                      
                       if (!_fp.is_open())
                         {
                           _fp.open(_outputFileName.c_str(), std::ios::out);
                           if (!_fp.is_open())
                             {
                               std::cout<<"{"<<__FILE__<<" : "<<__LINE__<<"} Unable to Open output file :"<<_outputFileName<<std::endl;
                               return -1;
                              }
                          }
           
                       _fp<<"{"<<fileName_<<" : "<<lineNumber_<<"} <"<<functionName_<<"> "<<std::endl;
                       _fp<<mesg_<<std::endl;

                      }
              
              }
           }
       
        static int getState()
          {
            return _state;
           }

        static int setState(int state_)
          {
            _state=state_;
            return 0;
           }
       
        static void setOutputFileName(std::string fileName)
          {
            _outputFileName=fileName; 
            if (_fp.is_open())
                _fp.close();
           }

        static void setPriorityThreshold(int thresh)
          {
            _priorityThreshold=thresh;
           }
   
   };

#define     MESSAGE1(m)           MessagePrinter::PrintMessage(m, 10, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define     MESSAGE2(m,p)         MessagePrinter::PrintMessage(m, p,  __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define     MESSAGE3(m,p,f)       MessagePrinter::PrintMessage(m, p,  f  , __FILE__, __LINE__)

#endif   /* MESSAGE_PRINTER_H */
