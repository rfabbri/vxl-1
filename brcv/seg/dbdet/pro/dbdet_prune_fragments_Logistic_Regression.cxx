#include "dbdet_prune_fragments_Logistic_Regression.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil/vil_load.h>
#include <vil/vil_bilin_interp.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>



#include <bbas/bsol/bsol_algs.h>
#include <vil/vil_convert.h>
#include <bil/algo/bil_color_conversions.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_img_curve_algs_two.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/algo/dbdet_sel.h>

// Prune Curve Fragments using Logistic Regression.

dbdet_prune_fragments_Logistic_Regression::dbdet_prune_fragments_Logistic_Regression() : bpro1_process()
{
  if(!parameters()->add( "color threshold: " , "-color_thres" ,  0.4f ) ||
     !parameters()->add( "color gamma: " , "-color_gamma" ,  14.0f ) ||
     !parameters()->add( "intensity gamma: " , "-intensity_gamma" ,  15.0f ) ||
     !parameters()->add( "region width (pixels): " , "-region_width" ,  5.0f ) ||
     !parameters()->add( "Threshold Probability: " , "-prob" ,  0.3 )
     )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  
  call_in_gui = 1;
}


//: Clone the process
bpro1_process*
dbdet_prune_fragments_Logistic_Regression::clone() const
{
  return new dbdet_prune_fragments_Logistic_Regression(*this);
}


vcl_vector< vcl_string > dbdet_prune_fragments_Logistic_Regression::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel" );
  to_return.push_back( "image" );
  return to_return;
}

vcl_vector< vcl_string > dbdet_prune_fragments_Logistic_Regression::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel" );
  return to_return;
}

void
dbdet_prune_fragments_Logistic_Regression::get_parameters()
{
  	parameters()->get_value("-color_thres", color_threshold);
  	parameters()->get_value("-color_gamma", color_gamma);
  	parameters()->get_value("-intensity_gamma", intensity_gamma);
  	parameters()->get_value("-region_width", region_width);
	parameters()->get_value("-prob", prob);
}

bool dbdet_prune_fragments_Logistic_Regression::execute()
{
//	double prob;
//	float color_threshold, color_gamma, region_width, intensity_gamma;
  //get the parameters
	if(call_in_gui)
  		get_parameters();

	vidpro1_image_storage_sptr frame_image;
  	frame_image.vertical_cast(input_data_[0][1]);
  	vil_image_resource_sptr image_sptr = frame_image->get_image();
  	vil_image_view<vxl_byte> img = image_sptr->get_view();
	vil_image_view<float> L_, A_, B_;

  	bool LAB = false;
  	if( img.nplanes() != 3 ) {
    	vcl_cout << "image is not colored, using intensity differences!!!\n"; 
  	} else {
    	LAB = true;
    	convert_RGB_to_Lab(img, L_, A_, B_);
  	}
		
	dbdet_sel_storage_sptr input_sel;
	input_sel.vertical_cast(input_data_[0][0]);
	dbdet_edgemap_sptr EM = input_sel->EM();
  	dbdet_curve_fragment_graph& CFG_input = input_sel->CFG();
	
	dbdet_sel_storage_sptr output_sel1 = dbdet_sel_storage_new();
	dbdet_curve_fragment_graph &CFG_output1 = output_sel1->CFG();
	output_sel1->set_EM(EM);

	int Final_contours1=0;
	int Nbins1=50;int Nbins2=85;
	vcl_vector<dbdet_edgel*> new_chain3;
	dbdet_edgel_chain* new_chain4=new dbdet_edgel_chain();
	for(vcl_list<dbdet_edgel_chain*>::const_iterator f = CFG_input.frags.begin();f!=CFG_input.frags.end();f++)
	{
        	dbdet_edgel_chain *test1=*f;
        	for(int d=0;d<test1->edgels.size();d++) new_chain3.push_back(test1->edgels[d]); 
	}
	int n1=0;
	for(int i=0;i<EM->edgels.size();i++)
	{
        	for(int j=0;j<new_chain3.size();j++)
        	{
			if(EM->edgels[i]==new_chain3[j]) {n1=5; break;}
                	else continue;       	
		}
     		if(n1==0) {new_chain4->edgels.push_back(EM->edgels[i]);}
     		else {n1=0; continue;}
	}
	for(vcl_list<dbdet_edgel_chain*>::const_iterator fit = CFG_input.frags.begin();fit!=CFG_input.frags.end();fit++)
   	{
		double contrast=0;
		dbdet_edgel_chain* c1 = new dbdet_edgel_chain (**fit);
		if(c1->edgels.size() < 2) continue;
		vbl_array_2d<double> first,second;
  		vbl_array_2d<double> firsthist,secondhist;
		vcl_vector<vsol_point_2d_sptr> point_samples;
		for(int i=0;i<c1->edgels.size();i++) point_samples.push_back(new vsol_point_2d(c1->edgels[i]->pt.x(),c1->edgels[i]->pt.y()));
		dbsol_interp_curve_2d_sptr curve = new dbsol_interp_curve_2d();
		dbsol_curve_algs::interpolate_linear(curve.ptr(),point_samples,false);
		vcl_vector<vsol_point_2d_sptr > region_pts;                                                     
 		dbsol_curve_algs::sample_region_along_curve(*curve, region_pts, 0.5f, curve->length(), 3, false);
		//Total Number of Neighborhood Edges
		double Total_Edges=0;
		for(int q=0;q<new_chain4->edgels.size();q++)
		{	
			for(unsigned j = 0; j < region_pts.size(); ++j)	
  			{
				if((dbdet_round(new_chain4->edgels[q]->pt.x()) == dbdet_round(region_pts[j]->x())) 
				&& (dbdet_round(new_chain4->edgels[q]->pt.y()) == dbdet_round(region_pts[j]->y())))
				{
					Total_Edges=Total_Edges + 1;
					break;
				}
			}
		}
		for (vcl_list<dbdet_edgel_chain*>::const_iterator f_it2 = CFG_input.frags.begin();f_it2!=CFG_input.frags.end();f_it2++)
  		{
			dbdet_edgel_chain* chain2=(*f_it2);
			double len=0;
			for(int i=0;i<chain2->edgels.size()-1;i++)
			{
				dbdet_edgel* ed2=chain2->edgels[i];
				dbdet_edgel* ed3=chain2->edgels[i+1];
				double d=vgl_distance(ed2->pt,ed3->pt);
				len+=d;		
			}
			if(len>5 || len<2) continue;
			if(chain2==c1) continue;			
			for(int i=0;i<chain2->edgels.size();i++)
			{
				for(unsigned j = 0; j < region_pts.size(); ++j)	
  				{
					if((dbdet_round(chain2->edgels[i]->pt.x()) == dbdet_round(region_pts[j]->x())) 
						&& (dbdet_round(chain2->edgels[i]->pt.y()) == dbdet_round(region_pts[j]->y())))
					{
						Total_Edges=Total_Edges + 1;
						break;
					}
				}
			}
		}
		//Histograms and then the Histogram Distances using chi square distance
		vbl_array_2d<double> Histogram;
		Histogram=get_changed_color_distance_of_curve_regions(curve, region_width,L_,A_,B_,color_gamma);
		int rows= Histogram.rows();
		first.resize(rows/2,5);second.resize(rows/2,5);
		firsthist.resize(Nbins2,5);secondhist.resize(Nbins2,5); 
		for(int x=0;x<Nbins2;x++)
		{
			firsthist[x][0]=0;firsthist[x][1]=0;firsthist[x][2]=0;firsthist[x][3]=0;firsthist[x][4]=0;
			secondhist[x][0]=0;secondhist[x][1]=0;secondhist[x][2]=0;secondhist[x][3]=0;secondhist[x][4]=0;
		}
		for(int i=0;i<rows/2;i++) 
		{
			first[i][0]=Histogram[i][0];
			double m1=0,m2=0;
			m1=0;m2= m1 + 100/Nbins1;
			for(int j=0;j<Nbins1;j++)
			{
				if(first[i][0] >=m1 && first[i][0]< m2) {firsthist[j][0]+=1; break;}
				m1=m2;
				m2=m2 + 100/Nbins1;
			}
			
			first[i][1]=Histogram[i][1];
			m1=-128; m2= m1 + 255/Nbins2;
			for(int j=0;j<Nbins2;j++)
			{
				if(first[i][1] >=m1 && first[i][1]< m2) {firsthist[j][1]+=1; break;}
				m1=m2;
				m2=m2 + 255/Nbins2;
			}
	
			first[i][2]=Histogram[i][2];
			m1=-128; m2= m1 + 255/Nbins2;
			for(int j=0;j<Nbins2;j++)
			{
				if(first[i][2] >=m1 && first[i][2]< m2) {firsthist[j][2]+=1; break;}
				m1=m2;
				m2=m2 + 255/Nbins2;
			}
		 first[i][3]=vcl_sqrt(first[i][1]*first[i][1]+first[i][2]*first[i][2])/vcl_sqrt(first[i][1]*first[i][1]+first[i][2]*first[i][2]+first[i][0]*first[i][0]);
		 first[i][3]=first[i][3]*100;
			m1=0;m2= m1 + 100/Nbins1;
			for(int j=0;j<Nbins1;j++)
			{
				if(first[i][3] >=m1 && first[i][3]< m2) {firsthist[j][3]+=1; break;}
				m1=m2;
				m2=m2 + 100/Nbins1;
			}
			first[i][4]=atan2(first[i][3],first[i][2]);
			first[i][4]=first[i][4]*(180/3.14142);
			m1=-180; m2= m1 + 360/Nbins2;
			for(int j=0;j<Nbins1;j++)
			{
				if(first[i][4] >=m1 && first[i][4]< m2) {firsthist[j][4]+=1; break;}
				m1=m2;
				m2=m2 + 360/Nbins1;
			}		
		}
		for(int i=rows/2,k=0;i<rows;k++,i++) 
		{	
			second[k][0]=Histogram[i][0];
			double m1=0,m2=0,check=0;
			m1=0;m2= m1 + 100/Nbins1;
			for(int j=0;j<Nbins1;j++)
			{
				if(second[k][0] >=m1 && second[k][0]< m2) {secondhist[j][0]+=1; break;}
				m1=m2;
				m2=m2 + 100/Nbins1;
			}	
			second[k][1]=Histogram[i][1];
			m1=-128; m2= m1 + 255/Nbins2;
			for(int j=0;j<Nbins2;j++)
			{
				if(second[k][1] >=m1 && second[k][1]< m2) {secondhist[j][1]+=1; break;}
				m1=m2;	
				m2=m2 + 255/Nbins2;
			}
			second[k][2]=Histogram[i][2];
			m1=-128; m2= m1 + 255/Nbins2;
			for(int j=0;j<Nbins2;j++)
			{
				if(second[k][2] >=m1 && second[k][2]< m2) {secondhist[j][2]+=1; break;}
				m1=m2;
				m2=m2 + 255/Nbins2;
			}	
      second[k][3]=vcl_sqrt(second[k][1]*second[k][1]+second[k][2]*second[k][2])/vcl_sqrt(second[k][1]*second[k][1]+second[k][2]*second[k][2]+second[k][0]*second[k][0]);
      second[k][3]=second[k][3]*100;
			m1=0;m2= m1 + 100/Nbins1;
			for(int j=0;j<Nbins1;j++)
			{
				if(second[k][3] >=m1 && second[k][3]< m2) {secondhist[j][3]+=1; break;}
				m1=m2;
				m2=m2 + 100/Nbins1;
			}
	second[k][4]=atan2(second[k][3],second[k][2]);
	second[k][4]=second[k][4]*(180/3.14142);
			m1=-180; m2= m1 + 360/Nbins2;
			for(int j=0;j<Nbins1;j++)
			{
				if(second[k][4] >=m1 && second[k][4]< m2) {secondhist[j][4]+=1; break;}
				m1=m2;
				m2=m2 + 360/Nbins1;
			}	
		}
	
		double BG_Lvalue=0,CG_avalue=0,CG_bvalue=0,Saturation=0,Hue=0;
		// Use chi square distance to find the histogram distance
		for(int i=0;i<Nbins1;i++)
		{
			BG_Lvalue += ((firsthist[i][0]/(rows/2) - secondhist[i][0]/(rows/2))*(firsthist[i][0]/(rows/2) - secondhist[i][0]/(rows/2)))/(firsthist[i][0]/(rows/2) + secondhist[i][0]/(rows/2) + 2e-16);
			Saturation += ((firsthist[i][3]/(rows/2) - secondhist[i][3]/(rows/2))*(firsthist[i][3]/(rows/2) - secondhist[i][3]/(rows/2)))/(firsthist[i][3]/(rows/2) + secondhist[i][3]/(rows/2) + 2e-16);
		}
		for(int i=0;i<Nbins2;i++)
		{
			Hue+=((firsthist[i][4]/(rows/2) - secondhist[i][4]/(rows/2))*(firsthist[4][2]/(rows/2) - secondhist[4][2]/(rows/2)))/(firsthist[4][2]/(rows/2) + secondhist[4][2]/(rows/2) + 2e-16);
		}
		BG_Lvalue=BG_Lvalue/2.0;Saturation=Saturation/2.0;Hue=Hue/2.0;
		//Average  Normalized LAB Contrast
		if(LAB) contrast=get_color_distance_of_curve_regions(curve, region_width,L_,A_,B_,color_gamma);
		else contrast = get_intensity_distance_of_curve_regions(curve, region_width, img, intensity_gamma);
		contrast=1 - contrast;
		// Length of the Contour
		double length=0;
		for(int i=0;i<c1->edgels.size()-1;i++)
		{
			dbdet_edgel* ed2=c1->edgels[i];
			dbdet_edgel* ed3=c1->edgels[i+1];
			double d=vgl_distance(ed2->pt,ed3->pt);
			length+=d;
		}
		Total_Edges=Total_Edges/length;	
		//All the beta values
		double b0=-3.9111,b1=-1.2212,b2=0.2794,b3=-0.2985,b4=4.7712,b5=-0.3965,b6=0.0416;
		//Logistic Regression Probability
		double Logist_prob= 1 -(1/(1 + vcl_exp(b0 +b1*BG_Lvalue + b2*Hue + b3*Saturation + b4*contrast + b5*Total_Edges + b6*length)));
		if(Logist_prob > prob) {Final_contours1=Final_contours1 + 1;CFG_output1.frags.push_back(c1);}
	}
		output_data_[0].push_back(output_sel1);
		vcl_cout << "Done logistic regression prune" << vcl_endl;
		vcl_cout << "INITIAL CONTOURS: " << CFG_input.frags.size() << vcl_endl;
		vcl_cout << "FINAL CONTOURS: " << Final_contours1 << vcl_endl;
		return true;
  	
}


