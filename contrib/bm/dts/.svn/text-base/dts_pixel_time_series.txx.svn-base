//this is /contrib/bm/dts/dts_pixel_time_series.txx

#include<dts/dts_pixel_time_series.h>

#include<vgl/io/vgl_io_point_2d.h>

#include<vul/vul_file.h>


template<class pixelType, class timeType,
            class elementType, unsigned n >
void dts_pixel_time_series<pixelType,timeType,elementType,n>::
    b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the dimension of the pixel time series
    vsl_b_write(os, this->dim);
    
    //write the size of the pixel time series
    vsl_b_write(os, this->pixel_time_series_map.size());

    typename pixel_time_series_map_type::const_iterator
        pts_itr, pts_end = this->pixel_time_series_map.end();

    for( pts_itr = this->pixel_time_series_map.begin();
            pts_itr != pts_end; ++pts_itr )
    {
        //write the pixel
        vsl_b_write(os, pts_itr->first);

		//cast the time series base sptr to
		//a pointer of the right type to write
        dts_time_series<timeType,elementType,n>* ts_ptr =
			static_cast<dts_time_series<timeType,elementType,n>*>
				(pts_itr->second.as_pointer());

		ts_ptr->b_write(os);
    }//end pixel time series iteration

    return;
}//end b_write

template<class pixelType, class timeType,
    class elementType>
void dts_pixel_time_series<pixelType,timeType,elementType,1>::
    b_write( vsl_b_ostream& os ) const
{
    const short version_no = 1;
    vsl_b_write(os, version_no);

    //write the dimension of the pixel time series
    vsl_b_write(os, this->dim);
    
    //write the size of the pixel time series
    vsl_b_write(os, this->pixel_time_series_map.size());

    typename pixel_time_series_map_type::const_iterator
        pts_itr, pts_end = this->pixel_time_series_map.end();

    for( pts_itr = this->pixel_time_series_map.begin();
            pts_itr != pts_end; ++pts_itr )
    {
        //write the pixel
        vsl_b_write(os, pts_itr->first);

		//cast the time series base sptr to a 
		//pointer of the right type to write
		dts_time_series<timeType,elementType,1>* ts_ptr =
			static_cast<dts_time_series<timeType,elementType,1>*>
				(pts_itr->second.as_pointer());

		ts_ptr->b_write(os);

    }//end pixel time series iteration

    return;
    return;
}//end b_write

template<class pixelType, class timeType,
            class elementType, unsigned n >
void dts_pixel_time_series<pixelType,timeType,elementType,n>::
    b_read( vsl_b_istream& is )
{
    if(!is) return;

    this->pixel_time_series_map.clear();

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the dimension
            unsigned d;
            vsl_b_read(is,d);
            if(d != this->dim)
            {
                vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::b_read()\n"
                         << "\tPixel Time Series Instance Dimension" 
                         << " and Written File dimension do not match.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
            }

            //read the size of the pixel time series map
            vcl_size_t npix = 0;
            vsl_b_read(is, npix);

            for(unsigned pix_idx = 0;
                    pix_idx < npix; ++pix_idx )
            {
                //read the pixel location
                vgl_point_2d<pixel_type> loc;
                vsl_b_read(is,loc);

                //make pointer to heap mem
                dts_time_series<time_type,math_type,this->dim>* 
                    ts_ptr = 
						new dts_time_series<time_type,math_type,this->dim>();

				//read binary file to heap
                ts_ptr->b_read(is);

				//make a sptr
				dts_time_series_base_sptr ts_sptr(ts_ptr);

                //insert location time series pair
                this->pixel_time_series_map[loc] = ts_sptr;
            }//end pixel time series iteration

        }
        break;
    default:
        {
            vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::b_read()\n"
                         << "\tUnknown version number.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
        }
    }//end switch(v)

    return;
}

//general class with n dimension
template<class pixelType, class timeType,
            class elementType, unsigned n >
bool dts_pixel_time_series<pixelType,timeType,elementType,n>::
    write_txt( vcl_string const& filename,
					vgl_point_2d<unsigned> const& pt )
{
    typename pixel_time_series_map_type::const_iterator
        pts_itr = this->pixel_time_series_map.find( pt );

    if( pts_itr == this->pixel_time_series_map.end() )
    {
        vcl_cout << "----WARNING----\n"
                 << "\tTime Series at pixel: ("
                 << pts_itr->first.x() << ", " 
                 << pts_itr->first.y() << ") not found.\n";
		return false;
    }
    else
    {
        pts_itr->second->write_txt( filename );
        return true;
    }
}//end write_txt


//specialization for 1 dimensional feature
template<class pixelType, class timeType,
            class elementType >
bool dts_pixel_time_series<pixelType,timeType,elementType,1>::
    write_txt( vcl_string const& filename,
				vgl_point_2d<unsigned> const& pt)
{
    typename pixel_time_series_map_type::const_iterator
        pts_itr = this->pixel_time_series_map.find( pt );

    if( pts_itr == this->pixel_time_series_map.end() )
    {
        vcl_cout << "----WARNING----\n"
                 << "\tTime Series at pixel: ("
                 << pts_itr->first.x() << ", " 
                 << pts_itr->first.y() << ") not found.\n";
		return false;
    }
    else
    {
        pts_itr->second->write_txt( filename );
        return true;
    }
}//end write_txt


template<class pixelType, class timeType,
            class elementType >
void dts_pixel_time_series<pixelType,timeType,elementType,1>::
    b_read( vsl_b_istream& is )
{
    if(!is) return;

    this->pixel_time_series_map.clear();

    short v;
    vsl_b_read(is,v);

    switch(v)
    {
    case 1:
        {
            //read the dimension
            unsigned d;
            vsl_b_read(is,d);
            if(d != this->dim)
            {
                vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::b_read()\n"
                         << "\tPixel Time Series Instance Dimension" 
                         << " and Written File dimension do not match.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
            }

            //read the size of the pixel time series map
            vcl_size_t npix = 0;
            vsl_b_read(is, npix);

            for(unsigned pix_idx = 0;
                    pix_idx < npix; ++pix_idx )
            {
                //read the pixel location
                vgl_point_2d<pixel_type> loc;
                vsl_b_read(is,loc);

                //make pointer on the heap
				dts_time_series<time_type,math_type,this->dim>* 
					ts_ptr = new dts_time_series<time_type,math_type,this->dim>();

				//read the time series to heap
                ts_ptr->b_read(is);

				//make a sptr
				dts_time_series_base_sptr ts_sptr(ts_ptr);

                //insert location time series base sptr pair
                this->pixel_time_series_map[loc] = ts_sptr;
            }//end pixel time series iteration

        }
        break;
    default:
        {
            vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::b_read()\n"
                         << "\tUnknown version number.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
        }
    }//end switch(v)

    return;
}

template<class pixelType, class timeType,
    class elementType, unsigned n>
void dts_pixel_time_series<pixelType,timeType,elementType,n>::
    insert( vgl_point_2d<pixelType> const& pt, 
                    time_type const& time, 
                        feature_type const& feature )
{
    if( !this->pixel_time_series_map.count(pt) )
    {
        //we need a new time series base sptr 
		dts_time_series_base_sptr ts_base_sptr =
			new dts_time_series<timeType,elementType,n>(time,feature);
        this->pixel_time_series_map[pt] = ts_base_sptr;
    }
    else
	{
		//1. get the time series base sptr
        dts_time_series_base_sptr ts_base_sptr =
			this->pixel_time_series_map[pt];

		//2. cast to a pointer of child class
		dts_time_series<timeType,elementType,n>* ts_ptr =
			static_cast<dts_time_series<timeType,elementType,n>*>(ts_base_sptr.as_pointer());
		if(!ts_ptr)
		{
			vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::insert(...)\n"
                         << "\tFailed to cast time series base pointer to"
						 << " time series pointer.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
		}

		//3.insert the time/feature pair
		ts_ptr->insert(time,feature);
	}
}

template<class pixelType, class timeType,
    class elementType>
void dts_pixel_time_series<pixelType,timeType,elementType,1>::
    insert( vgl_point_2d<pixelType> const& pt, 
                    time_type const& time, 
                        feature_type const& feature )
{
    if( !this->pixel_time_series_map.count(pt) )
    {
        //we need a new time series base sptr 
		dts_time_series_base_sptr ts_base_sptr =
			new dts_time_series<timeType,elementType,1>(time,feature);
        this->pixel_time_series_map[pt] = ts_base_sptr;
    }
    else
	{
		//1. get the time series base sptr
        dts_time_series_base_sptr ts_base_sptr =
			this->pixel_time_series_map[pt];

		//2. cast to a pointer of child class
		dts_time_series<timeType,elementType,1>* ts_ptr =
			static_cast<dts_time_series<timeType,elementType,1>*>(ts_base_sptr.as_pointer());
		if(!ts_ptr)
		{
			vcl_cerr << "----ERROR---- "
                         << "dts_pixel_time_series::insert(...)\n"
                         << "\tFailed to cast time series base pointer to"
						 << " time series pointer.\n"
                         << "\tFILE: " << __FILE__ << '\n'
                         << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
            return;
		}

		//3.insert the time/feature pair
		ts_ptr->insert(time,feature);
	}
     
}

#define DTS_PIXEL_TIME_SERIES_INSTANTIATE( pixelType, timeType, elementType, n)\
template class dts_vgl_point_2d_coord_compare<pixelType>;\
template class dts_pixel_time_series<pixelType,timeType,elementType,n>