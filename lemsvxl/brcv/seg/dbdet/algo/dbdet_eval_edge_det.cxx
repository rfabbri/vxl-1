#include "dbdet_eval_edge_det.h"

#include <dbdet/edge/dbdet_edgemap.h>

//: evaluate the accuracy of an edgemap given a ground truth edgemap (as an image)
dbdet_eval_result dbdet_eval_edge_det(dbdet_eval_edge_det_params params, dbdet_edgemap_sptr edge_map, vil_image_view<vxl_byte>& GT_image)
{
  //read the list of thresholds to evaluate the algorithm from the params
  vcl_vector<double>& thresh = params.thresh;

  //create an eval structure for storing the ROC data
  dbdet_eval_result res(params.thresh.size());

  //count the number of TP and FP pixels from the GT image
  int N_P=0, N_F=0;
  for (unsigned i=0; i<GT_image.ni(); i++){
    for (unsigned j=0; j<GT_image.nj(); j++){
      if (GT_image(i,j)<params.no_edge_label) //there are more labels for true edges
        N_P++;
      if (GT_image(i,j)==params.no_edge_label)
        N_F++;
    }
  }

  //for each setting of the parameter, evaluate the result
  for (unsigned i=0; i<thresh.size(); i++)
  {
    //make a copy of the GT_image (needed to keep track of used GT edgels)
    vil_image_view<vxl_byte> GT;
    GT.deep_copy(GT_image);

    double threshold = thresh[i];
    int TP=0, FP=0;

    //go over each edgel
    for (unsigned e=0; e<edge_map->edgels.size(); e++)
    {
      if (edge_map->edgels[e]->strength<threshold)
        continue;

      int x = (int) vcl_floor(edge_map->edgels[e]->pt.x()+0.5);
      int y = (int) vcl_floor(edge_map->edgels[e]->pt.y()+0.5);

      if (x<0 || x>=GT.ni() || y<0 || y>=GT.nj())
        continue; //outside the image

      //if this edgel is in the no edge region , FP++
      if (GT(x,y)==params.no_edge_label){
        FP++;
      }
      else {
        // find the closest GT edge pixel within T_match
        double dist = 20.0;
        int xc=-1, yc=-1;

        //go over the neighborhood around the edgel to search for the nearest pixel
        for (int xx=-params.T_match; xx<=params.T_match; xx++){
          for (int yy=-params.T_match; yy<=params.T_match; yy++){
            if (x+xx<0 || x+xx>=GT.ni() || y+yy<0 || y+yy>=GT.nj())
              continue; //outside the image

            //if the pixel is marked as edge
            if (GT(x+xx,y+yy)<params.no_edge_label){
              //compute distance
              double d = vcl_sqrt(double(xx*xx + yy*yy));

              if (d<=params.T_match && d<dist){
                dist = d;
                xc = x+xx;
                yc = y+yy;
              }
            }
          }
        }

        //if GT edgel is found in the neighborhood, TP++ and mark the GT edgel as used
        if (xc>-1){
          GT(xc, yc) = 200;
          TP++;
        }
        // else it must be in the don't care areas
      }
    }

    //save the TP and FP values at this threshold
    res.params[i] = threshold;
    res.TP[i] = TP;
    res.FP[i] = FP;
    res.TN[i] = N_F - FP;
    res.FN[i] = N_P - TP;

  }

  //return the ROC res
  return res;
}

//: evaluate the performace of contour extraction given a ground truth edgemap (as an image)
dbdet_eval_result dbdet_eval_edge_linking(dbdet_eval_edge_det_params params, dbdet_curve_fragment_graph& CFG, vil_image_view<vxl_byte>& GT_image)
{
  //create an eval structure for storing the ROC data
  dbdet_eval_result res_ROC(params.thresh.size());

  //count the number of TP and FP pixels from the GT image
  int N_P=0, N_F=0;
  for (unsigned i=0; i<GT_image.ni(); i++){
    for (unsigned j=0; j<GT_image.nj(); j++){
      if (GT_image(i,j)<params.no_edge_label) //there are more labels for true edges
        N_P++;
      if (GT_image(i,j)==params.no_edge_label)
        N_F++;
    }
  }

  //for each setting of the parameter, evaluate the result
  for (unsigned i=0; i<params.thresh.size(); i++)
  {
    //make a copy of the GT_image (needed to keep track of used GT edgels)
    vil_image_view<vxl_byte> GT;
    GT.deep_copy(GT_image);

    double len_thresh = params.thresh[i];
    int TP=0, FP=0;

    //for each contour in the CFG
    dbdet_edgel_chain_list_iter cit = CFG.frags.begin();
    for (; cit!=CFG.frags.end(); cit++)
    {
      dbdet_edgel_chain* chain = (*cit);

      //prune the contours by length
      if (chain->edgels.size()<len_thresh)
        continue; 

      //determine the # of TP edgels on the current contour
      int TP_cv=0, FP_cv=0;

      //go over all the edgels of the contour 
      for (unsigned j=0; j<chain->edgels.size(); j++)
      {
        int x = (int) vcl_floor(chain->edgels[j]->pt.x()+0.5);
        int y = (int) vcl_floor(chain->edgels[j]->pt.y()+0.5);

        if (x<0 || x>=GT.ni() || y<0 || y>=GT.nj())
          continue; //outside the image

        //if this edgel is in the no edge region , FP++
        if (GT(x,y)==params.no_edge_label){
          FP_cv++;
        }
        else {
          // find the closest GT edge pixel within T_match
          double dist = 20.0;
          int xc=-1, yc=-1;

          //go over the neighborhood around the edgel to search for the nearest pixel
          for (int xx=-params.T_match; xx<=params.T_match; xx++){
            for (int yy=-params.T_match; yy<=params.T_match; yy++){
              if (x+xx<0 || x+xx>=GT.ni() || y+yy<0 || y+yy>=GT.nj())
                continue; //outside the image

              //if the pixel is marked as edge
              if (GT(x+xx,y+yy)<params.no_edge_label){
                //compute distance
                double d = vcl_sqrt(double(xx*xx + yy*yy));

                if (d<=params.T_match && d<dist){
                  dist = d;
                  xc = x+xx;
                  yc = y+yy;
                }
              }
            }
          }

          //if GT edgel is found in the neighborhood, TP++ and mark the GT edgel as used
          if (xc>-1){
            GT(xc, yc) = 200;
            TP_cv++;
          }
          // else it must be in the don't care areas
        }
      }

      //The main rules of the performance computation for linked contours

      // A) Count all the TP edgels as TP
      // B) If it is significantly overlapped with GT edgels, count all remaining edgels of the contour as FP
      // C) Else only count the FP edgels as FP

      TP += TP_cv;

      if (TP_cv > 0.5*chain->edgels.size())
        FP += (chain->edgels.size()-TP_cv);
      else
        FP += FP_cv;
    }

    //save the TP and FP values at this threshold
    res_ROC.params[i] = len_thresh;
    res_ROC.TP[i] = TP;
    res_ROC.FP[i] = FP;
    res_ROC.TN[i] = N_F - FP;
    res_ROC.FN[i] = N_P - TP;
  }

  return res_ROC;
}


