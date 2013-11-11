#ifndef _dbbgm_bgmodel_h_
#define _dbbgm_bgmodel_h_

#include <vbl/vbl_array_2d.h>
#include <dbbgm/dbbgm_model_data.h>
#include <vil/vil_image_view.h>

template<class T>
class dbbgm_bgmodel:public vbl_ref_count
{
public:   
    dbbgm_bgmodel();
    dbbgm_bgmodel(vil_image_view<T> img,int nummodes=3, int numframes=10);
    dbbgm_bgmodel(dbbgm_model_data modelin);

    ~dbbgm_bgmodel();

    bool updateModel(vil_image_view<T>  img);

    void freeze();
    void unfreeze();

    bool isbackground(int, int);

    void setintialstddev(float stddev);

    void setlframes(int L);

    vil_image_view<T> writeforeground();
    vil_image_view<T> writebackground(int k);
 
    vil_image_view<T> writeweightimage(int k);

    vil_image_view<T> detectforeground();

    
    void setminweight(float minweight);
    dbbgm_model_data model;
    //: Print an ascii summary to the stream
protected:

    bool docalculations();
    float getprobabilty(float mu, float sigma,float x);
    int no_of_modes; //: number of modes for gaussian mixture
    int L;           //: no of frames used to update the model
    vbl_array_2d<int> currentwinner;
    //vbl_array_2d<dbbgm_model_data> model;
    //: stores the model
    
    vil_image_view<T> curr_img_;            //: current image in consideration
    int ni,nj;
    vbl_array_2d<dbbgm_model_data>::iterator iter;
    bool updatefrozen;
    float initialstd_;
    float MINWEIGHT;
};

#endif //_dbbgm_bgmodel_h_
