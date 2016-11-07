//this is /contrib/bm/dsm/dsm_utilities.cxx
#include"dsm_utilities.h"

vnl_matrix<double> dsm_utilities::compute_confusion_matrix( vcl_string const& img_gt_filename, vcl_string const& img_prediction_filename)
{
	vnl_matrix<double> confusion_matrix(2,2,double(0));

	vil_image_view<vxl_byte> gt_view = vil_load(img_gt_filename.c_str());
	vil_image_view<vxl_byte> prediction_view = vil_load(img_prediction_filename.c_str());

	if( gt_view.size() != prediction_view.size() )
	{
		vcl_cerr << "----ERROR---- dsm_utilities::compute_confusion_matrix\n"
				 << "\t Ground Truth and Prediction Images are not the same size!" << vcl_flush;
		exit(1);
	}

	confusion_matrix = compute_confusion_matrix(gt_view, prediction_view);

	return confusion_matrix;
}//end compute_confusion_matrix

vnl_matrix<double> dsm_utilities::compute_confusion_matrix( vil_image_view<vxl_byte> const& gt_view, 
												 vil_image_view<vxl_byte> const& prediction_view)
{
	vnl_matrix<double> confusion_matrix(2,2,double(0));

	if( gt_view.size() != prediction_view.size() )
	{
		vcl_cerr << "----ERROR---- dsm_utilities::compute_confusion_matrix\n"
				 << "\t Ground Truth and Prediction Images are not the same size!" << vcl_flush;
		exit(1);
	}

	unsigned ni = gt_view.ni();
	unsigned nj = gt_view.nj();

	for( unsigned i = 0; i < ni; ++i )
		for( unsigned j = 0; j < nj; ++j )
		{
			vxl_byte gt_value = gt_view(i,j,0);
			vxl_byte prediction_value = prediction_view(i,j,0);
			//check if this is a pixel we would like to include in our computation
			if( gt_value != 127 && prediction_value != 127 )
			{
				if( gt_value == 255 )
				{
					//true positive
					if(prediction_value == 255)
						++confusion_matrix[0][0];
					//false negative
					else if(prediction_value == 0)
						++confusion_matrix[0][1];
					else
					{
						vcl_cerr << "----ERROR---- dsm_utilities::compute_confusion_matrix\n"
							     << "\t Unknown value of prediction image at location (" 
								 << i << "," << j  << ") = " << int(prediction_value) << vcl_flush;
						exit(1);
					}
				}//end if gt_value == 255
				else if(gt_value == 0)
				{
					//false positive
					if(prediction_value == 255)
						++confusion_matrix[1][0];
					//true negative
					else if(prediction_value == 0 )
						++confusion_matrix[1][1];
					else
					{
						vcl_cerr << "----ERROR---- dsm_utilities::compute_confusion_matrix\n"
							     << "\t Unknown value of prediction image at location (" 
								 << i << "," << j  << ") = " << int(prediction_value) << vcl_flush;
						exit(1);
					}
				}
				else
				{
					vcl_cerr << "----ERROR---- dsm_utilities::compute_confusion_matrix\n"
							     << "\t Unknown value of ground truth image at location (" 
								 << i << "," << j  << ") = " << prediction_value << vcl_flush;
						exit(1);
				}
			}//end don't care check
		}//end height iteration (j)

	return confusion_matrix;
}
