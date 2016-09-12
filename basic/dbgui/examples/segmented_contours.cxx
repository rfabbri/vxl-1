/* 
\file segmented_countours.cpp
Parses the segmented_contours.xml file and produces a segmented_contour_info structure with the coordinates attributes.
*/
#include <vcl_iostream.h>

#include "segmented_contours.h"
#include <stdio.h>
#include <string.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
//#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>

segmented_contours_info::segmented_contours_info()
{
//printf("segmented_contours_info\n");
  count=0;
  totallen=0;
  pInfo = 0;

}
segmented_contours_info::segmented_contours_info(segmented_contours_info* pinfo)
{
//printf("segmented_contours_info\n");
  pInfo=pinfo;
  count=0;
  totallen=0;

}
segmented_contours_info::~segmented_contours_info()
{
  //printf("~segmented_contours_info\n");

  /*pInfo->coordinates.push_back(new vcl_string(get_full_string()););
    pInfo->increment_count();
    pInfo->add_len(totallen+1);
*/
    if (pInfo)
      pInfo->add_string(new vcl_string(get_full_string()));
//    free up the  space
    coordinates.clear();
    count=0;
    totallen=0;
}


void segmented_contours_info::add_string(vcl_string *p)
{
  increment_count();
  add_len(strlen(p->c_str()));
  coordinates.push_back(*p);
}


void segmented_contours_info::add_data(const char *s, int len)
{
  char v[5000];
  memcpy(v,s,len);
  v[len] = '\0';
  //vcl_cout <<"String: " << v << ".\n";
  add_string(new vcl_string(v));
}

void segmented_contours_info::increment_count()
{
  count = count +1;
}

/*segmented_contours_info::init()

{
   for (vcl_vector<vcl_string>::iterator arg_it = coordinates.begin();
  arg_it != coordinates.end(); ++arg_it){
    string_segments.coordinates.push_back(*p);
  }

  count = 0;
  totallen = 0;
}
*/
vcl_string segmented_contours_info::get_full_string()
{
 vcl_string s=""; 
 for (vcl_vector<vcl_string>::iterator arg_it = coordinates.begin();
  arg_it != coordinates.end(); ++arg_it){
      s = s + *arg_it;
  }
  int i = strlen(s.c_str());
  return(s);
}


void segmented_contours_info::add_len(int len)
{
  totallen = totallen+len;
}

int segmented_contours_info::get_len()
{
  return(totallen);
}

int segmented_contours_info::get_count()
{
  return(count);
}
/*vcl_vector<vcl_string> segmented_contours_info::get_vector()
{
  return(count);
}
*/

//-----
//--- PARSER --
//--
//
// segmented_contours():mDepth(0){};

segmented_contours_parser::segmented_contours_parser (segmented_contours_info* info) 
  {
    bIsPolygon = false;
    pInfo = info;
  }

void 
segmented_contours_parser::WriteIndent()
{
  for (int i = 0; i < mDepth; i++)
    putchar('\t');
}


void 
segmented_contours_parser ::startElement(const char* name, const char** atts)
{
  if(strcmp(name,"coordinates")==0){
//    string_segments.increment_count();
    pPolygon = new segmented_contours_info(pInfo);
    bIsPolygon=true; 
  }
    else {
      bIsPolygon=false;
    }
    /*

    WriteIndent();
    puts(name);
    if (atts) { // write list of attributes indented below element 
      int i;
      for (i=0; atts[i]; i++) {
        WriteIndent();
        putchar('-'); putchar(' ');
        puts(atts[i]);
      }
     */
   
     mDepth++;
     
}

void segmented_contours_parser ::endElement(const char* name)
{
  if(strcmp(name,"coordinates")==0){
    /*
    // Need to free the string_Segments and copy to the coordinates.
     int totallen =   string_segments.get_len() ;
      int count = string_segments.get_count();
 //   vcl_string *p = new vcl_string("test");
     vcl_string *p = new vcl_string(string_segments.get_full_string());
 //   pInfo->coordinates.push_back(*v);
    pInfo->coordinates.push_back(*p);
    */
    bIsPolygon=false;
    delete(pPolygon);
  /*  pInfo->coordinates.push_back(new vcl_string(pPolygon->get_full_string()););

    pInfo->increment_count();
    pInfo->add_len(totallen+1);
    //free up the  space
    string_segments.init();
    */
  }

  mDepth--;
}


void 
segmented_contours_parser ::charData(const XML_Char *s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements
 /* WriteIndent();

// write out the user data bracketed by ()
  putchar('(');
  fwrite(s, len, 1, stdout);
  puts(")");
*/
  if (bIsPolygon == true){
     pPolygon->add_data(s, len);
  }
  
  /*char v[520];
  memcpy(v,s,len);
  memcpy(v+len,"",1);
  //vcl_cout <<"String: " << v << ".\n";
   pPolygon->add_string(new vcl_string(v));
  vcl_string *p = new vcl_string(v);
  string_segments.add_len(len);
  string_segments.coordinates.push_back(*p);
  */
} 



/*


int main(int argc, char** argv)
{
  vcl_FILE *xmlFile;
  segmented_contours_info info;
  segmented_contours_parser parser(&info);

  //int depth = 0;
  // --- Program Arguments ---
  vul_arg<vcl_string> filename("-filename", "input xml file");
  vul_arg<vcl_string> video("-video", "video file");
  vul_arg_parse(argc, argv);
  vidl1_movie_sptr my_movie ;

  if (  filename()== ""){
    fprintf(stderr,"File not specified\n");
     return(1);
  }
   xmlFile = vcl_fopen(filename().c_str(), "r");
   if (!xmlFile){
        fprintf(stderr, " %s error on opening", filename);
        return(1);
       }
    if (!parser.parseFile(xmlFile)) {
       fprintf(stderr,"%s at line %d\n",XML_ErrorString(parser.XML_GetErrorCode()),parser.XML_GetCurrentLineNumber());
         return 1;
       }
       puts("\nfinished!");
      
       int i=info.get_count();
  

 for (vcl_vector<vcl_string>::iterator arg_it = info.coordinates.begin();
  arg_it != info.coordinates.end(); ++arg_it){
        vcl_string s = *arg_it;
        puts("\nPolygon");
        puts(s.c_str());
        i--;
  }
  info.coordinates.clear();

  return(0);
*/
