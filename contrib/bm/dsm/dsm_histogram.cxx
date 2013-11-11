////this is /contrib/bm/dsm/dsm_histogram.cxx
//#include"dsm_histogram.h"
//
////dsm_histogram::dsm_histogram( double const& range, double const& refine_threshold ):min_(0.0),max_(range),refine_threshold_(refine_threshold),nobs_(0.0), area_(0.0)
////{
////	dsm_bin lower(this->min_,range/2), upper(range/2,this->max_);
////
////	this->bins_.insert(vcl_make_pair<double,dsm_bin>(lower.max_,lower));
////	this->bins_.insert(vcl_make_pair<double,dsm_bin>(upper.max_,upper));
////}
//
//bool dsm_histogram::increment( double const& time, double const& v)
//{
//	vcl_map<double,dsm_bin>::iterator bin_itr = this->find_bin(time);
//
//	if( bin_itr == this->bins_.end() )
//	{
//		vcl_cerr << time << " exceeds the range of this histogram.\n" << vcl_flush;
//		return false;
//	}
//
//	bin_itr->second.counts_+=v;
//	this->nobs_+=1;
//	//recompute area
//	this->area_ = this->area();
//
//	if( bin_itr->second.counts_/this->area_ ) //we need to refine
//	{
//		double min = bin_itr->second.min_;
//		double max = bin_itr->second.max_;
//		double bin_width = max-min;
//		double counts = bin_itr->second.counts_;
//		this->bins_.erase(bin_itr);
//		dsm_bin lower(min,min+bin_width), upper(min+bin_width,max);
//		lower.counts_=counts;
//		upper.counts_=counts;
//	}
//
//	return true;
//}
//
//vcl_map<double,dsm_bin>::iterator dsm_histogram::find_bin( double const& time )
//{
//	vcl_map<double, dsm_bin>::iterator bin_itr, bin_end = this->bins_.end();
//
//	while( time < bin_itr->first && bin_itr != bin_end )
//	{
//		++bin_itr;
//	}
//
//	return bin_itr;
//}
//
//double dsm_histogram::p( double const& time )
//{
//	if(this->nobs_ == 0)
//	{
//		vcl_cerr << "WARNING: dsm_histogram::p There have been no observations thus far.\n" << vcl_flush;
//		return 0.0;
//	}
//	vcl_map<double,dsm_bin>::iterator bin_itr = find_bin(time);
//
//	if( bin_itr == this->bins_.end() )
//	{
//		vcl_cerr << "WARNING: dsm_histogram::p " <<  time << " exceeds the range of this histogram.\n" << vcl_flush;
//		return 0.0;
//	}
//	
//	return bin_itr->second.counts_/this->area_;
//}
//
//double dsm_histogram::area()
//{
//	vcl_map<double,dsm_bin>::const_iterator b_itr, b_end = this->bins_.end();
//
//	double area = 0.0;
//
//	for( b_itr = this->bins_.begin(); b_itr != b_end; ++b_itr )
//	{
//		double bin_width = b_itr->second.max_ - b_itr->second.min_;
//
//		if( bin_width < 0 )
//		{
//			vcl_cerr << "WARNING: dsm_histogram::area bin_width must be positive.\n " << vcl_flush;
//			return 0.0;
//		}
//
//		area += bin_width*b_itr->second.counts_;
//	}
//
//	return area;
//}