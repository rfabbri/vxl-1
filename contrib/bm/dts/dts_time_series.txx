//this is /contrib/bm/dts/dts_time_series.txx
#ifndef DTS_TIME_SERIES_TXX_
#define DTS_TIME_SERIES_TXX_

#include<dts/dts_time_series.h>

#include<vcl_iostream.h>

#include<vnl/io/vnl_io_vector_fixed.h>

template<class timeT, class elementType, unsigned n>
void dts_time_series<timeT, elementType, n>::
    b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the dimension of the time series
    vsl_b_write(os, this->dim);

    //write the size of the time series
    vsl_b_write(os, this->time_map.size());

    typename map_type::const_iterator
        ts_itr, ts_end = this->time_map.end();

    for( ts_itr = this->time_map.begin(); ts_itr != ts_end; ++ts_itr )
    {
        //write the time
        vsl_b_write(os, ts_itr->first);

        //write the feature
        vsl_b_write(os, ts_itr->second);

    }//end time series iteration

    return;
}//end b_write

template<class timeT, class elementType, unsigned n>
void dts_time_series<timeT, elementType, n>::
    b_read( vsl_b_istream& is )
{
    if(!is) return;

    this->time_map.clear();

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the dimension of the time series
            unsigned d;
            vsl_b_read(is, d);

            //check if dimensions match
            if( this->dim != d )
            {
                vcl_cerr << "----ERROR---- "
                         << "dts_time_series_fixed::b_read()\n"
                         << "\tDimension Mismatch.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
                exit(-1);
            }

            //read the size of the time series
            vcl_size_t tm_size;
            vsl_b_read(is, tm_size);

            for( vcl_size_t t_idx = vcl_size_t(0);
                    t_idx < tm_size; ++t_idx )
            {
                //read the time
                time_type time;
                vsl_b_read(is,time);

                //read the feature
                feature_type f;
                vsl_b_read(is,f);

                //insert into map
                this->time_map[time] = f;
            }//end time series iteration

            break;
        }//end case 1
    default:
        {
            vcl_cerr << "----ERROR---- "
                         << "dts_time_series_fixed::b_read()\n"
                         << "\tUnknown version number.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
        }//end default
    }//end swtich(v)

    return;
}//end b_read

template<class timeT, class elementType, unsigned n>
void dts_time_series<timeT, elementType, n>::
write_txt( vcl_string const& filename) const
{
    // each row will be a data point
    // first column will be time
    // next columns will be feature vectors
    vcl_ofstream of( filename.c_str(), vcl_ios::out);

    typename map_type::const_iterator 
        ts_itr, ts_end = this->time_map.end();

    for( ts_itr = this->time_map.begin(); 
            ts_itr != ts_end; ++ts_itr )
    {
        of << ts_itr->first;

        typename feature_type::const_iterator 
            f_itr, f_end = ts_itr->second.end();

        for( f_itr = ts_itr->second.begin(); 
            f_itr != f_end; ++f_itr )
        {
            of << ' ' << *f_itr;
        }//end feature iteration

        of << '\n';
    }//end time series iteration

    return;

}//end write txt

template<class timeT, class elementType>
void dts_time_series<timeT, elementType, 1>::
write_txt( vcl_string const& filename) const
{
    // each row will be a data point
    // first column will be time
    // the next column will be the feature
    vcl_ofstream of( filename.c_str(), vcl_ios::out);

    typename map_type::const_iterator 
        ts_itr, ts_end = this->time_map.end();

    for( ts_itr = this->time_map.begin(); 
            ts_itr != ts_end; ++ts_itr )
    {
        of << ts_itr->first;

        of << ' ' << ts_itr->second << '\n';

        
    }
}//end write txt

template<class timeT, class elementType>
void dts_time_series<timeT, elementType, 1>::
    b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the dimension of the time series
    vsl_b_write(os, unsigned(1));

    //write the size of the time series
    vsl_b_write(os, this->time_map.size());

    typename map_type::const_iterator
        ts_itr, ts_end = this->time_map.end();

    for( ts_itr = this->time_map.begin(); ts_itr != ts_end; ++ts_itr )
    {
        //write the time
        vsl_b_write(os, ts_itr->first);

        //write the feature
        vsl_b_write(os, ts_itr->second);

    }//end time series iteration

    return;
}//end b_write


template<class timeT, class elementType>
void dts_time_series<timeT, elementType, 1>::
    b_read( vsl_b_istream& is )
{
    if(!is) return;

    this->time_map.clear();

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the dimension of the time series
            unsigned d;
            vsl_b_read(is, d);

            //check if dimensions match
            if( this->dim != d )
            {
                vcl_cerr << "----ERROR---- "
                         << "dts_time_series_fixed::b_read()\n"
                         << "\tDimension Mismatch.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
                exit(-1);
            }

            //read the size of the time series
            vcl_size_t tm_size;
            vsl_b_read(is, tm_size);

            for( vcl_size_t t_idx = vcl_size_t(0);
                    t_idx < tm_size; ++t_idx )
            {
                //read the time
                time_type time;
                vsl_b_read(is,time);

                //read the feature
                feature_type f;
                vsl_b_read(is,f);

                //insert into map
                this->time_map[time] = f;
            }//end time series iteration

            break;
        }//end case 1
    default:
        {
            vcl_cerr << "----ERROR---- "
                         << "dts_time_series_fixed::b_read()\n"
                         << "\tUnknown version number.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
        }//end default
    }//end swtich(v)

    return;
}//end b_read


#define DTS_TIME_SERIES_INSTANTIATE( timeT, elementType, dim)\
template class dts_time_series<timeT, elementType, dim>

#endif //DTS_TIME_SERIES_TXX_