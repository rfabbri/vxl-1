//this is /contrib/bm/dsm2/dsm2_km_change_observer.h
#ifndef DSM2_CHANGE_KM_OBSERVER_H_
#define DSM2_CHANGE_KM_OBSERVER_H_

#include<dsm2/dsm2_observer_base.h>

#include<cstddef>
#include<vector>

class dsm2_km_change_observer:public dsm2_observer_base
{
public:

	dsm2_km_change_observer():num_nodes_(std::size_t(0)){}

	~dsm2_km_change_observer(){}

	virtual void update( dsm2_subject_base* s, std::string const& interest );

	virtual void b_read( vsl_b_istream& is );

	virtual void b_write( vsl_b_ostream& os ) const;

	void write_txt( std::string const& filename ) const;

	std::vector<bool> change(){ return this->change_; }

	std::size_t num_nodes(){ return this->num_nodes_; }

protected:
	std::vector<bool> change_;

	size_t num_nodes_;
};

#endif //DSM2_KM_CHANGE_OBSERVER_H_