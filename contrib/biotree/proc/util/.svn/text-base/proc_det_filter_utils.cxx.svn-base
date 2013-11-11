//: 
// \file   proc_det_filter_utils.cxx
// \brief  utility methods to handle the filter responses, such as reading and 
//         reformatting the responses for different needs
//         
// \author    Gamze D. Tunali
// \date      2005-10-01
// 

#include "proc_det_filter_utils.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h>

proc_det_filter_utils::proc_det_filter_utils(void)
{
}

proc_det_filter_utils::~proc_det_filter_utils(void)
{
}

//: reads the responses from files, it also sets the filter dimensions 
// and the number of filters
vcl_vector<xmvg_filter_response<double> >   
proc_det_filter_utils::read_responses(vcl_string response_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size) 
{
  // read the responses from the text file
  FILE *fp = vcl_fopen(response_file.data(),"r");
  vcl_fscanf(fp,"%d %d %d %d\n", &dimx, &dimy, &dimz, &filter_size);
  int num_voxels = dimz*dimy*dimx;
  vcl_vector<xmvg_filter_response<double> > resp_vector;

  for (int dim = 0; dim < num_voxels; dim++) {
    resp_vector.push_back(xmvg_icosafilter_response(filter_size, 0.0));
  }

  double intensity;
  for(int f=0; f<filter_size; f++)
  {
    int index = -1;
    for (int z = 0; z < dimz; z++) {
      for (int y = 0; y < dimy; y++) {
        for (int x = 0; x < dimx; x++) {
          index++;
          if (vcl_fscanf(fp,"%lf ", &intensity) == EOF) {
            vcl_cout << "PREMATURE END OF FILE" << vcl_endl;
            return resp_vector;
          };
          //xmvg_filter_response<double> ptr = resp_vector[index];
          resp_vector[index][f] = intensity;
        }
        //vcl_fscanf(fp,"\n", &intensity);
         vcl_fscanf(fp,"%lf", &intensity);
      }
      //vcl_fscanf(fp,"\n", &intensity);
      vcl_fscanf(fp,"%lf", &intensity);
    }
  //vcl_fscanf(fp,"\n", &intensity);
    vcl_fscanf(fp,"%lf", &intensity);
  }
  
  return resp_vector;

}

void 
proc_det_filter_utils::diff_responses(vcl_string f1, vcl_string f2, vcl_string out) 
{
  int dimx, dimy, dimz, filter_size;

  vcl_vector<xmvg_filter_response<double> > resp1 = read_responses(f1,
                                          dimx, dimy, 
                                          dimz, filter_size);

  vcl_vector<xmvg_filter_response<double> > resp2 = read_responses(f2,
                                          dimx, dimy, 
                                          dimz, filter_size);
  FILE *fp = vcl_fopen(out.data(),"w");
  vcl_fprintf(fp,"%d %d %d\n", dimx, dimy, dimz);
  for(int f=0; f<filter_size; f++)
  {
    
    int index = -1;
    for (int z = 0; z < dimz; z++) {
      for (int y = 0; y < dimy; y++) {
        for (int x = 0; x < dimx; x++) {
          index++;
          /*xmvg_filter_response<double> ptr1 = resp1[index];
          xmvg_filter_response<double> ptr2 = resp2[index];
          double diff = (*ptr1)[f] - (*ptr2)[f];
          double sum = ((*ptr1)[f] + (*ptr2)[f])/2.0;
          double perc = (diff/(*ptr1)[f])*100.0;*/
          double diff = resp1[index][f] - resp2[index][f];
          //double sum = (resp1[index][f] + resp2[index][f])/2.0;
          double perc = (diff/resp1[index][f])*100.0;
          vcl_fprintf(fp, "%lf ", perc);
        }
        vcl_fprintf(fp,"\n");
      }
      vcl_fprintf(fp,"\n");
    }
  vcl_fprintf(fp,"\n");
  }
  vcl_fclose(fp);
}

//: crops the voxels from a given box by the x, y and z margin,
// writes the result of the filter responses into a file
void
proc_det_filter_utils::crop_responses(vcl_string response_file,
                                          vcl_string output_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size,
                                          int xmar, int ymar, int zmar) {

  vcl_vector<xmvg_filter_response<double> > responses;
  responses = read_responses(response_file,dimx, dimy, dimz, filter_size);
  FILE *fp2 = fopen(output_file.data(),"w");
  
  fprintf(fp2,"%d %d %d\n", dimx-2*xmar, dimy-2*ymar, dimz-2*zmar);
  //int filter_num = 0;
     
  for (int f=0; f<filter_size; f++) {
  for(int k=zmar; k<dimz-zmar; k++)
    {
      for(int j=ymar; j<dimy-ymar; j++)
      {
        for(int i=xmar; i<dimx-xmar; i++)
        {    
          
          //}
          int index = dimy * dimx * k + dimx * j + i;
          //xmvg_filter_response<double> *ptr = responses[index];
          double intensity = responses[index][f];
          fprintf(fp2, "%lf ", intensity);
        } 
         fprintf(fp2,"\n");
      }
      fprintf(fp2,"\n");
    }
    fprintf(fp2,"\n");
  }
    
  fclose(fp2);
  
  }

