#ifndef dbinfo_feature_format_h
#define dbinfo_feature_format_h
#include <vcl_iostream.h>
enum dbinfo_feature_format
{
  DBINFO_FEATURE_FORMAT_UNKNOWN = 0,
  DBINFO_INTENSITY_FEATURE = 1,
  DBINFO_GRADIENT_FEATURE = 2,
  DBINFO_IHS_FEATURE = 3
};
vcl_ostream & operator << (vcl_ostream &os, dbinfo_feature_format f);

#endif // dbinfo_feature_format_h
