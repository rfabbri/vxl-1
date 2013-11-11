#pragma once
#include <vil/vil_image_view.h>
#include <vil/algo/vil_threshold.h>
#include <dbbgm/bbgm_wavelet.h>
#include <vil/vil_save.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
template <class T>
bool specificitySensitivityCount(vil_image_view<T> truth,vil_image_view<T> test,float bg_thresh,vcl_string path)
{
	  vil_image_view<bool> truth_thresh=new vil_image_view<bool>(truth.ni(),truth.nj(),1,1);
	  vil_image_view<bool> test_thresh= new vil_image_view<bool>(test.ni(),test.nj(),1,1);
	  vil_threshold_above(truth,truth_thresh,bg_thresh);
	  vil_threshold_above(test,test_thresh,bg_thresh);
      float TP=0,TN=0,FP=0,FN=0;	  
	  for(unsigned int i=0;i<truth.ni();i++)
		  for(unsigned int j=0;j<truth.nj();j++)
			 if (truth_thresh(i,j)&&!test_thresh(i,j))
				 FN++;
			 else if (!truth_thresh(i,j)&& test_thresh(i,j))
				 FP++;
			 else if (!truth_thresh(i,j)&& !test_thresh(i,j))
				 TN++;
			 else
				 TP++;
	   vcl_fstream file_op;
	   vil_save(truth_thresh,(path+"truth.tiff").c_str());
	   vil_save(test_thresh,(path+"test.tiff").c_str());
	   file_op.open(path.c_str(),vcl_fstream::app);
	   file_op<<TP/(TP+FN)<<" "<<TN/(FP+TN)<<vcl_endl;
	   file_op.close();
	   return true;

}