//this is /contrib/bm/dsm/dsm_feature.cxx
#include"dsm_feature.h"

void dsm_feature::b_write(vsl_b_ostream& os) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    vsl_b_write(os, v);
    vsl_b_write(os, t);
}//end dsm_feature::b_write

void dsm_feature::b_read(vsl_b_istream& is)
{
    if(!is) return;

    short v;
    vsl_b_read(is,v);
    switch(v)
    {
    case 1:
        {
            vsl_b_read(is,this->v);
            vsl_b_read(is,this->t);
        }//end case 1
        break;
    default:
        {
            vcl_cerr << "----I/O ERROR: dsm_feature::b_read ----\n"
				     << "	 UNKNOWN VERSION NUMBER " << v << "\n";
			is.is().clear(vcl_ios::badbit); //set an unrecoverable IO error on stream
			return;
        }//end default
    }//end switch
}//end dsm_feature::b_read