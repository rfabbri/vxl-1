#ifndef _dbrl_correspondence_h_
#define _dbrl_correspondence_h_
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_io.h>

class dbrl_correspondence
    {
    public:
        dbrl_correspondence();
        //: intialize matrix by user
        dbrl_correspondence(vnl_matrix<double> M);
        //: intialize uniform
        dbrl_correspondence(int m,int n);
        virtual ~dbrl_correspondence();

        void setinitialvalue(double init);
        void setinitialoutlier(double outlier);

        bool normalize();

        double operator() (unsigned i,unsigned j) const;

        unsigned rows() const {return M_.rows();}
        unsigned cols() const {return M_.cols();}
        double mcols(int index){if(index<static_cast<int>(mcols_.size()))return (mcols_)[index];else return 0.0;}
        double mrows(int index){if(index<static_cast<int>(mrows_.size()))return (mrows_)[index];else return 0.0;}

        vnl_matrix<double> & M(){return M_;};
        //: give a new matrix and return the normalized matrix
        bool updateM(vnl_matrix<double> & M);

        bool updateM(vnl_matrix<double> & M, double annealrate);

        double errdist();
        bool binarize(double threshold);
        // ==== Binary IO methods ======

        //: Binary save self to stream.
        void b_write(vsl_b_ostream &os) const;

        //: Binary load self from stream.
        void b_read(vsl_b_istream &is);

        //: Return IO version number;
        short version() const;

        //: Print an ascii summary to the stream
        void print_summary(vcl_ostream &os) const;

        //: Return a platform independent string identifying the class
        virtual vcl_string is_a() const {return "dbrl_correspondence";}
    protected:
        vnl_matrix<double> M_;
        vnl_matrix<double> Mold_;
        vnl_vector<double> mcols_;
        vnl_vector<double> mrows_;


        double initm;
        double moutlier;

    };

//: Binary save dbrl_correspondence* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_correspondence p);

//: Binary load dbrl_correspondence* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_correspondence &p);

inline void operator<<(vcl_ostream & os, dbrl_correspondence& M)
{
        M.print_summary(os);
    }
#endif
