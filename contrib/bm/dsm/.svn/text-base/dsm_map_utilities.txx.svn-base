#ifndef DSM_MAP_UTILITIES_TXX_
#define DSM_MAP_UTILITIES_TXX_
#include"dsm_map_utilities.h"
//====================================================================================
//: Write map of maps to binary stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_write(vsl_b_ostream& os, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1> const& v)
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    vsl_b_write(os, v.size());

    for(typename vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1 >::const_iterator itr = v.begin(); itr!=v.end(); ++itr)
    {
        vsl_b_write(os, (*itr).first);
        vsl_b_write(os, (*itr).second.size());
        for( typename vcl_map<T2,T3,Compare2>::const_iterator itr2=((*itr).second).begin(); itr2!=(*itr).second.end();++itr2)
        {
            vsl_b_write(os, (*itr2).first);
            vsl_b_write(os, (*itr2).second);
        }
    }//end outer iteration
}//end vsl_b_write

//====================================================================================
//: Read map from binary stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_read(vsl_b_istream& is, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v)
{
    if(!is) return;

    v.clear();
    unsigned outer_map_size;
    
    short ver;
    vsl_b_read(is,ver);
    
    switch(ver)
    {
    case 1:
        {
            vsl_b_read(is, outer_map_size);
        
            for( unsigned i = 0; i < outer_map_size; ++i)
            {
                T1 key1;
                vsl_b_read(is,key1);
                unsigned inner_map_size;
                vsl_b_read(is, inner_map_size);
                vcl_map<T2, T3, Compare2> inner_map;
                for(unsigned j = 0; j < inner_map_size; ++j)
                {
                    T2 key2;
                    vsl_b_read(is, key2);
                    T3 value;
                    vsl_b_read(is, value);
                    inner_map[key2] = value;
                }//end inner map iteration
                v[key1] = inner_map;
            }//end outer map iteration
        }
        break;
    default:
        {
            vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_map<T1, vcl_map<T2, T3>&)\n"
                << "           Unknown version number "<< ver << '\n';
            is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
            return;
        }
    }
}//end vsl_b_read

//====================================================================================
//: Print Human Readable Summary
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_print_summary( vcl_ostream& os, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v)
{
    os << "vsl_print_summary() -- NOT YET IMPLEMENTED!" << '\n';
}//end vsl_print_summary


#define DSM_IO_MAP_MAP_INSTANTIATE(T1,T2,T3,Compare1,Compare2)\
template void vsl_print_summary( vcl_ostream&, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>&);\
template void vsl_b_write(vsl_b_ostream& os, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v);\
template void vsl_b_read(vsl_b_istream& is, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v)

//====================================================================================
//====================================================================================
// \brief binary IO functions for vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare3>
// \author B.A.Mayer
//====================================================================================
//====================================================================================

//: Read a map of maps of maps from stream
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_read(vsl_b_istream& is,vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& v)
{
    if(!is) return;
    v.clear();

    short ver;
    vsl_b_read(is,ver);

    switch(ver)
    {
    case 1:
        {
            unsigned key1_size;
            vsl_b_read(is,key1_size);

            for(unsigned i = 0; i < key1_size; ++i)
            {
                T1 key1;
                vsl_b_read(is,key1);
                unsigned key2_size;
                vsl_b_read(is,key2_size);
                vcl_map<T2, vcl_map<T3,T4,Compare3>, Compare2> key2_map;
                for(unsigned j = 0; j < key2_size; ++j)
                {
                    T2 key2;
                    vsl_b_read(is,key2);
                    unsigned key3_size;
                    vsl_b_read(is,key3_size);

                    vcl_map<T3,T4,Compare3> key3_map;
                    for(unsigned k = 0; k < key3_size;++k)
                    {
                        T3 key3;
                        T4 value;
                        vsl_b_read(is,key3);
                        vsl_b_read(is,value);
                        key3_map[key3]=value;
                    }//end key3 iteration
                    key2_map[key2]=key3_map;
                }//end key2 iteration
                v[key1] = key2_map;
            }//end key1 iteration
        }//end case 1
        break;
    default:
        {
            vcl_cerr << "I/O ERROR: vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& v\n"
                << "           Unknown version number "<< ver << '\n';
            is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
            return;
        }//end default
    }//end switch(ver)
    return;
}//end vsl_b_read maps of maps of maps



//: Write a map of maps of maps to stream
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_write(vsl_b_ostream& os,vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v)
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    vsl_b_write(os, v.size());

    for( typename vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>::const_iterator itr1 = v.begin(); itr1!=v.end(); ++itr1)
    {
        vsl_b_write(os,(*itr1).first);
        vsl_b_write(os,(*itr1).second.size());

        for( typename  vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>::const_iterator itr2=(*itr1).second.begin(); itr2!=(*itr1).second.end(); ++itr2 )
        {
            vsl_b_write(os,(*itr2).first);
            vsl_b_write(os,(*itr2).second.size());

            for( typename vcl_map<T3, T4, Compare3>::const_iterator itr3=(*itr2).second.begin(); itr3!=(*itr2).second.end(); ++itr3 )
            {
                vsl_b_write(os,(*itr3).first);
                vsl_b_write(os,(*itr3).second);
            }//end key3 iteration
        }//end key2 iteration
    }//end key1 iteration
}//end vsl_b_read map of maps of maps

//: Print Human Readable Summary
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_print_summary( vcl_ostream& os, vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v)
{
    os << "vsl_print_summary() -- NOT YET IMPLEMENTED!" << '\n';
}//end vsl_print_summary

#define DSM_IO_MAP_MAP_MAP_INSTANTIATE(T1,T2,T3,T4,Compare1,Compare2,Compare3)\
template void vsl_print_summary( vcl_ostream& os, vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v);\
template void vsl_b_read(vsl_b_istream& is,vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& v);\
template void vsl_b_write(vsl_b_ostream& os,vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v)

#endif //DSM_MAP_UTILITIES_H_