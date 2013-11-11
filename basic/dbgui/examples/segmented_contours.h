#include <stdio.h>
#include <expatpp/expatpplib.h>
#include <string.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
//#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>

class segmented_contours_info  {
public:

//  Functions
  segmented_contours_info(segmented_contours_info* info);
  segmented_contours_info();
  ~segmented_contours_info();
  void add_data(const char *s, int len);
  void add_string(vcl_string *p);
  int get_len();
  int get_count();

//  Data
  vcl_vector<vcl_string> coordinates; 

private:
    
  //Functions
    void increment_count();
    void add_len(int len);
    vcl_string get_full_string();
// Temporary Data 
    segmented_contours_info *pInfo;  //ptr to the coordinates
//Data
    int count;
    int totallen;
};
class segmented_contours_parser : public expatpp {
public:
  segmented_contours_parser(segmented_contours_info *info);
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char *s, int len);

private:
   void handleAtts(const XML_Char** atts);
   void WriteIndent();
 //Temporary Data
  int mDepth;
  bool bIsPolygon;
  segmented_contours_info *pInfo; // pointer to User Area to put results of parse
  segmented_contours_info *pPolygon; //pointer to the current polygon we are parsing
};
