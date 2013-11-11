#ifndef DSM_IO_MAP_MAP_TXX
#define DSM_IO_MAP_MAP_TXX
#include<dsm/io/dsm_io_map_map.h>
#include<vsl/vsl_indent.h>
//: this is /contrib/bm/dsm/io/dsm_io_map_map.txx

//==============================================================================
//: Write map of maps to binary stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_write(vsl_b_ostream& os, 
                 vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1> const& v)
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    vsl_b_write(os, v.size());

    for(typename vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1 >::
                 const_iterator itr = v.begin(); itr!=v.end(); ++itr)
    {
        vsl_b_write(os, (*itr).first);
        vsl_b_write(os, (*itr).second.size());
        for( typename vcl_map<T2,T3,Compare2>::const_iterator 
                      itr2=((*itr).second).begin(); 
                      itr2!=(*itr).second.end();++itr2)
        {
            vsl_b_write(os, (*itr2).first);
            vsl_b_write(os, (*itr2).second);
        }
    }//end outer iteration
}//end vsl_b_write

//=============================================================================
//: Read map from binary stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_read(vsl_b_istream& is, 
                vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v)
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
                vcl_cerr << "I/O ERROR: "
                         << "vsl_b_read(vsl_b_istream&, "
                         << "vcl_map<T1, vcl_map<T2, T3>&)\n"
                         << "\t\tUnknown version number "<< ver << '\n';
            // Set an unrecoverable IO error on stream
            is.is().clear(vcl_ios::badbit); 
            return;
        }
    }
}//end vsl_b_read

//=============================================================================
//: Print Human Readable Summary
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_print_summary( vcl_ostream& os, 
                        const vcl_map<T1, 
                        vcl_map<T2, T3, Compare2>, Compare1>& v)
{
    os << "vsl_print_summary() -- NOT YET IMPLEMENTED!" << '\n';
}//end vsl_print_summary


#define DSM_IO_MAP_MAP_INSTANTIATE(T1,T2,T3,Compare1,Compare2)\
template void vsl_print_summary( vcl_ostream&, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>&);\
template void vsl_b_write(vsl_b_ostream& os, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v);\
template void vsl_b_read(vsl_b_istream& is, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v)

#endif //DSM_IO_MAP_MAP_TXX
