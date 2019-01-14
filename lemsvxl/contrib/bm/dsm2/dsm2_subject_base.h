//this is /contrib/bm/dsm2/dsm2_observable_base.h
#ifndef DSM2_OBSERVABLE_BASE_H_
#define DSM2_OBSERVABLE_BASE_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A base class for observable classes.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_observer_base_sptr.h>

#include<list>
#include<map>
#include<string>
#include<vector>

class dsm2_subject_base
{
public:
	

	virtual ~dsm2_subject_base(){}

	void notify(std::string const& interest);

	void attatch( dsm2_observer_base_sptr const& o, std::string const& interest );

	//: function to attach a single observer to multiple interests.
	void attatch( dsm2_observer_base_sptr const& o, std::vector<std::string> const& interests );

	void remove( dsm2_observer_base_sptr& o );

protected:
	dsm2_subject_base(){}

	std::map<std::string, std::list<dsm2_observer_base_sptr> > observers_;
};

#endif //DSM2_OBSERVABLE_BASE_H_