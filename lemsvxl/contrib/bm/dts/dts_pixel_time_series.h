//this is /contrib/bm/dts/dts_pixel_time_series.h
#ifndef DTS_PIXEL_TIME_SERIES_H_
#define DTS_PIXEL_TIME_SERIES_H_

#include<dts/dts_pixel_time_series_base.h>

#include<dts/dts_time_series_base_sptr.h>
#include<dts/dts_time_series.h>

#include<vgl/vgl_point_2d.h>

template<class pixelType>
class dts_vgl_point_2d_coord_compare
{
public:
    dts_vgl_point_2d_coord_compare
        ( vgl_point_2d<pixelType> const& p = 
            vgl_point_2d<pixelType>
                (pixelType(0),pixelType(0)) ):focus_(p){}

    bool operator()( vgl_point_2d<pixelType> const& pa,
                        vgl_point_2d<pixelType> const& pb ) const
    {
        if( pa.x() != pb.x() )
            return pa.x() < pb.x();
        else
            return pa.y() < pb.y();
    }

    ~dts_vgl_point_2d_coord_compare(){}

protected:
    vgl_point_2d<pixelType> focus_;
};

template<class pixelType, class timeType,
            class elementType, unsigned n >
class dts_pixel_time_series: public dts_pixel_time_series_base
{
public:

    dts_pixel_time_series(){}

    ~dts_pixel_time_series(){}

    typedef pixelType pixel_type;

    typedef timeType time_type;

    typedef elementType math_type;

    typedef vnl_vector_fixed<math_type,n> feature_type;

    enum{dim = n};

    typedef 
       dts_vgl_point_2d_coord_compare<pixelType>
            compare_type;

    typedef 
        dts_time_series<time_type,math_type,dim>
            time_series_type;

    typedef vcl_map<vgl_point_2d<pixelType>,
                dts_time_series_base_sptr,
                    compare_type > 
                        pixel_time_series_map_type;

    virtual vcl_size_t size() const
    { return this->pixel_time_series_map.size(); }

    virtual unsigned dimension()
    { return this->dim; }

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );

    
    virtual bool write_txt( vcl_string const& filename,
								vgl_point_2d<unsigned> const& pt );

    void insert( vgl_point_2d<pixelType> const& pt, 
                    time_type const& time, 
                        feature_type const& feature );

    pixel_time_series_map_type pixel_time_series_map;
};

template<class pixelType, class timeType,
            class elementType>
class dts_pixel_time_series<pixelType,timeType,elementType,1>: 
            public dts_pixel_time_series_base
{
public:

    dts_pixel_time_series(){}

    ~dts_pixel_time_series(){}

    typedef pixelType pixel_type;

    typedef timeType time_type;

    typedef elementType math_type;

    typedef elementType feature_type;

    enum{dim = 1};

    typedef 
       dts_vgl_point_2d_coord_compare<pixelType>
            compare_type;

    typedef 
        dts_time_series<time_type,math_type,1>
            time_series_type;

    typedef vcl_map<vgl_point_2d<pixelType>,
                dts_time_series_base_sptr,
                    compare_type > 
                        pixel_time_series_map_type;

    virtual vcl_size_t size() const
    { return this->pixel_time_series_map.size(); }

    virtual unsigned dimension()
    { return 1; }

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );

	virtual bool write_txt( vcl_string const& filename,
								vgl_point_2d<unsigned> const& pt );

    void insert( vgl_point_2d<pixelType> const& pt, 
                    time_type const& time, 
                        feature_type const& feature );

    pixel_time_series_map_type pixel_time_series_map;
};

#endif //DTS_PIXEL_TIME_SERIES_H_