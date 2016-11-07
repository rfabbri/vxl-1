//this is /contrib/bm/dsm2/exe/dsm2_node_size_main.cxx
// a simple program to test the advantage of storing a gaussian model 
// on the node class or separating the sufficient statistics.
#include<dsm2/dsm2_node_base.h>
#include<dsm2/dsm2_node_gaussian.h>

#include<bsta/bsta_gaussian_full.h>
#include<bsta/io/bsta_io_gaussian_full.h>

template<class T, unsigned n>
class dsm2_node_model: public dsm2_node_base
{
public:
	typedef T math_type;

	typedef vnl_vector_fixed<T,n> vector_type;

	typedef vnl_matrix_fixed<T,n,n> matrix_type;

	enum{dimension = n};

	dsm2_node_model() {}

	virtual ~dsm2_node_model() {}

	inline static vcl_string gauss_type() {return vcl_string("full"); }

	virtual void b_write( vsl_b_ostream& os ) const;

	virtual void b_read( vsl_b_istream& is );

	bsta_gaussian_full<math_type,dimension> model;
};

template<class T, unsigned n>
void dsm2_node_model<T,n>::b_write( vsl_b_ostream& os ) const
{
	const short version_no = 1;

	vsl_b_write(os, version_no);

	vsl_b_write(os, model);
}//end b_write

template<class T, unsigned n>
void dsm2_node_model<T,n>::b_read( vsl_b_istream& is )
{
	if(!is) return;
	
	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			vsl_b_read(is,this->model);
			break;
		}//end case 1
	default:
		{
			vcl_cerr << "ERROR: dsm2_node_model::b_read() -- unknown version number." << vcl_flush;
			return;
		}//end default
	}//end switch(v)
}//end b_read

template class dsm2_node_model<double,2>;

int main()
{
	
	vcl_cout << "Running dsm2_node_size_main.exe..." << vcl_endl;

	dsm2_node_gaussian<bsta_gaussian_full<double,2> >gaussian_full;
		

	dsm2_node_model<double,2> model;
		 

	vcl_cout << "Size of dsm2_node_gaussian_full: " << sizeof(gaussian_full)
		     << " bytes." << vcl_endl;

	vcl_cout << "Size of dsm2_node_model: " << sizeof(model)
		     << " bytes." << vcl_endl;
	return 0;
}