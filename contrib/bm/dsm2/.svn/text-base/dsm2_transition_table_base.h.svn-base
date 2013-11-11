//this is /contrib/bm/dsm2/dsm2_transition_table_base.h
#ifndef DSM2_TRANSITION_TABLE_BASE_H_
#define DSM2_TRANSITION_TABLE_BASE_H_
#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_string.h>

#include<vsl/vsl_binary_io.h>
class dsm2_transition_table_base: public vbl_ref_count
{
public:
	dsm2_transition_table_base(){}

	virtual ~dsm2_transition_table_base(){}

	virtual void b_write( vsl_b_ostream& os ) const = 0;

	virtual void b_read( vsl_b_istream& is ) = 0;

	virtual void write_txt( vcl_string const& filename ) const = 0;
};

#endif //DSM2_TRANSITION_TABLE_BASE_H_