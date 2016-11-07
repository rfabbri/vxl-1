#include <dbinfo/dbinfo_feature_format.h>

vcl_ostream & operator << (vcl_ostream &os, dbinfo_feature_format f)
{
  switch (f)
    {
    case DBINFO_FEATURE_FORMAT_UNKNOWN: os<< "DBINFO_FEATURE_FORMAT_UNKNOWN";
      break;
    case DBINFO_INTENSITY_FEATURE : os<< "DBINFO_INTENSITY_FEATURE";
      break;
    case DBINFO_GRADIENT_FEATURE : os<< "DBINFO_GRADIENT_FEATURE";
      break;
    case DBINFO_IHS_FEATURE : os<< "DBINFO_IHS_FEATURE";
      break;
    default:
      os<< "DBINFO_FEATURE_FORMAT_UNKNOWN";
    }
  return os;
}
