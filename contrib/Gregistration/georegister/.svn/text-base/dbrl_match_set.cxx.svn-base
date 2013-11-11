#include "dbrl_match_set.h"
#include "dbrl_feature.h"
#include "dbrl_feature_sptr.h"
#include <vsl/vsl_vector_io.h>
#include "dbrl_transformation_sptr.h"
#include "dbrl_affine_transformation.h"
#include "dbrl_thin_plate_spline_transformation.h"
#include <vbl/io/vbl_io_smart_ptr.h>
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include <vnl/vnl_transpose.h>

dbrl_match_set::dbrl_match_set():M_(),tform_(),estimator_()
    {

    }
dbrl_match_set::dbrl_match_set(dbrl_correspondence Minit,dbrl_transformation_sptr tform,
                               dbrl_estimator_sptr estimator):M_(Minit),tform_(tform),estimator_(estimator)
    {
        
    }

dbrl_match_set::~dbrl_match_set()
    {

    }
void dbrl_match_set::set_correspondence_matrix(dbrl_correspondence & M)
    {
        M_=M;
    }
void dbrl_match_set::set_original_features(vcl_vector<dbrl_feature_sptr> f1,vcl_vector<dbrl_feature_sptr> f2)
    {
        feature_set1=f1;
        feature_set2=f2;
    }
void dbrl_match_set::set_mapped_features(vcl_vector<dbrl_feature_sptr> f1,vcl_vector<dbrl_feature_sptr> f2)
    {
        xformed_feature_set1=f1;
        xformed_feature_set2=f2;
    }
void dbrl_match_set::set_transformation_estimator(dbrl_estimator_sptr & estimator)
    {
        estimator_=estimator;
    }
void dbrl_match_set::set_transformation(dbrl_transformation_sptr & tform)
    {
        tform_=tform;
    }
dbrl_feature_sptr dbrl_match_set::correspondence1to2(int index)
    {
    assert(index<static_cast<int>(M_.rows()));
    double maxindex=-1;
    double maxweight=0.0;
    for(int j=0;j<static_cast<int>(M_.cols());j++)
        {
        if(M_(index,j)>maxweight)
            {
                maxweight=M_(index,j);
                maxindex=j;
            }
        }
    if(maxweight>M_.mcols(index))
            return feature_set2[maxindex];
    else return 0;
    }
dbrl_feature_sptr dbrl_match_set::correspondence2to1(int index)
    {
    assert(index<static_cast<int>(M_.cols()));
    double maxindex=-1;
    double maxweight=0.0;
    for(int i=0;i<static_cast<int>(M_.rows());i++)
        {
        if(M_(i,index)>maxweight)
            {
                maxweight=M_(i,index);
                maxindex=i;
            }
        }
    if(maxweight>M_.mrows(index))
        return feature_set1[maxindex];
    else 
        return 0;
    }
int dbrl_match_set::cindex1to2(int index)
    {
    assert(index<static_cast<int>(M_.rows()));
    double maxindex=-1;
    double maxweight=0.0;
    for(int j=0;j<static_cast<int>(M_.cols());j++)
        {
        if(M_(index,j)>maxweight)
            {
                maxweight=M_(index,j);
                maxindex=j;
            }
        }
    if(maxweight>M_.mcols(index))
            return static_cast<int>(maxindex);
    else return -1;
    }
int dbrl_match_set::cindex2to1(int index)
    {
        assert(index<static_cast<int>(M_.cols()));
    double maxindex=-1;
    double maxweight=0.0;
    for(int i=0;i<static_cast<int>(M_.rows());i++)
        {
        if(M_(i,index)>maxweight)
            {
                maxweight=M_(i,index);
                maxindex=i;
            }
        }
    if(maxweight>M_.mrows(index))
            return static_cast<int>(maxindex);
    else return -1;
    }
dbrl_feature_sptr dbrl_match_set::feature_set1_i(int i)
    {
        assert(i<static_cast<int>(feature_set1.size()));
        return feature_set1[i];
    }
dbrl_feature_sptr dbrl_match_set::feature_set2_i(int i)
    {
        assert(i<static_cast<int>(feature_set2.size()));
        return feature_set2[i];
    }
dbrl_feature_sptr dbrl_match_set::xformed_feature_set1_i(int i)
    {
        assert(i<static_cast<int>(xformed_feature_set1.size()));
        return xformed_feature_set1[i];
    }
dbrl_feature_sptr dbrl_match_set::xformed_feature_set2_i(int i)
    {
        assert(i<static_cast<int>(xformed_feature_set2.size()));
        return xformed_feature_set2[i];
    }
vcl_map<double,dbrl_feature_sptr> dbrl_match_set::correspondences1to2(int index)
    {
        assert(index<static_cast<int>(M_.rows()));
        //double maxindex=-1;
        //double maxweight=0.0;
        vcl_vector<dbrl_feature_sptr> to_return;
        vcl_map<double,dbrl_feature_sptr> feature_map;
        for(int j=0;j<static_cast<int>(M_.cols());j++)
        {
            if(M_(index,j)>M_.mcols(index))
            {
                feature_map[1-M_(index,j)]=feature_set2[j];
                //to_return.push_back( feature_set2[j]);              
            }
        }

        //vcl_map<double,dbrl_feature_sptr>::iterator iter;
        //for(iter=feature_map.begin();iter!=feature_map.end();iter++)
        //    to_return.push_back((*iter).second);
        //return to_return;
        return feature_map;
}

vcl_map<double,dbrl_feature_sptr> dbrl_match_set::correspondences2to1(int index)
    {
    assert(index<static_cast<int>(M_.cols()));
    //double maxindex=-1;
    //double maxweight=0.0;
    vcl_vector<dbrl_feature_sptr> to_return;
            vcl_map<double,dbrl_feature_sptr> feature_map;

    for(int i=0;i<static_cast<int>(M_.rows());i++)
        {
        if(M_(i,index)>M_.mrows(index))
            {
                feature_map[1-M_(i,index)]=feature_set1[i];
            }
        }
        //vcl_map<double,dbrl_feature_sptr>::iterator iter;
        //for(iter=feature_map.begin();iter!=feature_map.end();iter++)
        //    to_return.push_back((*iter).second);
        //return to_return;
        return feature_map;
}

double dbrl_match_set::operator()(unsigned i,unsigned j)
    {
        assert(i<M_.rows());
        assert(j<M_.cols());

        return M_(i,j);
    
    }

void
dbrl_match_set::b_write(vsl_b_ostream &os) const
    {
        vsl_b_write(os, version());
        vsl_b_write(os, M_);
        vsl_b_write(os, tform_);
        vsl_b_write(os, estimator_);
        vsl_b_write(os, feature_set1);
        vsl_b_write(os, feature_set2);
        vsl_b_write(os, xformed_feature_set1);
        vsl_b_write(os, xformed_feature_set2);

      }

void dbrl_match_set::b_read(vsl_b_istream &is) 
    {

    if (!is)
        return;
    short ver;
    vsl_b_read(is, ver);
    switch (ver)
        {
        case 1:
            vsl_b_read(is, M_);
            vsl_b_read(is, tform_);
            vsl_b_read(is, estimator_);
            vsl_b_read(is, feature_set1);
            vsl_b_read(is, feature_set2);
            vsl_b_read(is, xformed_feature_set1);
            vsl_b_read(is, xformed_feature_set2);

            break;
        default:
            vcl_cerr << "dbrl_estimator_point_thin_plate_spline: unknown I/O version " << ver << '\n';
        }
    }

short dbrl_match_set::version() const
    {
    return 1;
    }
void dbrl_match_set::print_summary(vcl_ostream &os) const
{
  
}
dbrl_match_set * dbrl_match_set::clone()const
    {
    return new dbrl_match_set(*this);
    }

//: Binary save dbrl_match_set* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_match_set* p)
    {
    if (p==0) {
        vsl_b_write(os, false); // Indicate null pointer stored
        }
    else{
        vsl_b_write(os,true); // Indicate non-null pointer stored
        p->b_write(os);
        }

    }

//: Binary load dbrl_match_set* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_match_set* &p)
    {
    if(p)
    delete p;
    bool not_null_ptr;
    vsl_b_read(is, not_null_ptr);
    if (not_null_ptr) {
        p = new dbrl_match_set();
        p->b_read(is);
        }
    else
        p = 0;

    }

void dbrl_match_set::normalize_point_set(vnl_matrix<double> & M, vcl_vector<dbrl_feature_sptr> & f2)
    {

    vcl_string featuretype="";
    vnl_matrix<double> pts(f2.size(),2,0.0);
    for(unsigned i=0;i<f2.size();i++)
        {
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
            {
                pts(i,0)=pt->location()[0];
                pts(i,1)=pt->location()[1];
                featuretype="POINT";
            }
            else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
            {
                pts(i,0)=pt->location()[0];
                pts(i,1)=pt->location()[1];
                featuretype="POINT-TANGENT";
            }
        }
    vnl_transpose Mtrans(M);
    vnl_matrix<double> Mt=Mtrans.asMatrix();
    vnl_matrix<double> vpts=Mtrans*pts;
    f2.clear();
    for(int i=0;i<Mt.rows();i++)
        {
        if(Mt.get_row(i).sum()<0.7)
            vpts.set_row(i,-100);
        else
            {
            vpts.scale_row(i,1/Mt.get_row(i).sum());
            }
        if(featuretype=="POINT")
            f2.push_back(new dbrl_feature_point(vpts.get_row(i)));
        else if(featuretype=="POINT-TANGENT")
            f2.push_back(new dbrl_feature_point_tangent(vpts.get_row(i),0));
        }
    }
