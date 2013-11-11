//: this is /contrib/bm/dsm/io/dsm_io_map_map_map.txx
#ifndef DSM_IO_MAP_MAP_MAP_TXX_
#define DSM_IO_MAP_MAP_MAP_TXX_
#include<dsm/io/dsm_io_map_map_map.h>
#include<dsm/io/dsm_io_map_map.h>
#include<vcl_cstdlib.h>
//=============================================================================
//: Write a map of maps of maps to stream
template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_b_write( vsl_b_ostream& os,
                  vcl_map<T1, vcl_map<T2, 
                  vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& m)
{
    const short version_no = 1;
    vsl_b_write(os, version_no);
    vsl_b_write(os, m.size());

    typename vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>,
                    Compare2>, Compare1>::const_iterator 
        m_itr, m_end = m.end();

    for( m_itr = m.begin(); m_itr != m_end; ++m_itr )
    {
        //write the key
        vsl_b_write(os, m_itr->first);
        //write the vcl_map<T2,vcl_map<T3,T4>>
        //using signature already defined
        vsl_b_write(os, m_itr->second);
    }

}//end vsl_b_write

//=============================================================================
//: Read a map of maps from stream
template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_b_read( vsl_b_istream& is,
                  vcl_map<T1, vcl_map<T2, 
                  vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& m)
{
    if(!is) return;
    
    m.clear();
    unsigned outer_map_size;

    short ver;
    vsl_b_read(is,ver);
    
    switch(ver)
    {
    case1:
        {
            //read size of outter map
            unsigned outer_map_size;
            vsl_b_read(is, outer_map_size);
            for( unsigned i = 0; i < outer_map_size; ++i )
            {
                T1 key1;
                vsl_b_read(is, key1);
                vcl_map<T2, vcl_map<T3,T4,Compare3>,Compare2> inner_map;
                vsl_b_read(is,inner_map);
                m[key1]=inner_map;
            }//end outer map iteration
            
            break;
        }//end case1
    default:
        {
            vcl_cerr << "I/O ERROR: "
                     << "vsl_b_read( vsl_b_istream&, "
                     << "vcl_map<T1, vcl_map<T2, vcl_map<T3,T4, "
                     << "Compare3>, Compare2>, Compare1>&)&)\n"
                     << "\t\tUnknown Version Number " << ver << '\n';
            //set an unrecoverable IO error on the stream
            is.is().clear(vcl_ios::badbit);
            return;
        }//end default
    }//end swtich
    return;
}//end vsl_b_read

template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_print_summary( vcl_ostream& os, 
                        vcl_map<T1, vcl_map<T2, 
                        vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& m)
{
        vcl_cout << "vsl_print_summary( vcl_ostream& os,\n"
                 << "                   vcl_map<T1, vcl_map<T2,\n"
                 << "                   vcl_map<T3, T4, Compare3>, Compare2>,"
                 << " Compare1 >& m)\n"
                 << "\tNot Yet Implemented." << vcl_endl;
}//end vsl_print_summary

#define DSM_IO_MAP_MAP_MAP_INSTANTIATE(T1,T2,T3,T4,Compare1,Compare2,Compare3) \
template void vsl_b_write( vsl_b_ostream&, vcl_map<T1, vcl_map<T2,vcl_map<T3,T4,Compare3>,Compare2>, Compare1> const& m); \
template void vsl_b_read( vsl_b_istream&,  vcl_map<T1, vcl_map<T2,vcl_map<T3,T4,Compare3>,Compare2>, Compare1>& m); \
template void vsl_print_summary( vcl_ostream&, vcl_map<T1, vcl_map<T2, vcl_map<T3,T4,Compare3>,Compare2>,Compare1>& m)

#endif //DSM_IO_MAP_MAP_MAP_TXX_
