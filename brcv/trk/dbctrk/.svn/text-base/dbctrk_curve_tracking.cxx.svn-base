#include<dbctrk/dbctrk_curve_tracking.h>
#include<dbctrk/dbctrk_curve_matching.h>
#include<dbctrk/dbctrk_curve_clustering.h>
#include<dbctrk/dbctrk_tracker_curve_sptr.h>

#include <vcl_iostream.h>
#include <vcl_ostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vgl/vgl_polygon.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_math.h>


void dbctrk_curve_tracking_params::operator=(dbctrk_curve_tracking_params tp)
{
  clustering_         = tp.clustering_          ;
  transitive_closure  = tp.transitive_closure   ;
  benchmark_          = tp.benchmark_           ;
  window_size         = tp.window_size          ;
  min_length_of_curves= tp.min_length_of_curves ;
  ps_moving_curves    = tp.ps_moving_curves     ;
  ps_match_curves     = tp.ps_match_curves      ;
  isIHS               = tp.isIHS                ;

  thetabins=tp.thetabins;
  r1  = tp.r1  ;
  v1  = tp.v1  ;
  
  //dbctrk_curve_matching_params mp ;
  //dbctrk_curve_clustering_params cp ;
  ps_file_directory=tp.ps_file_directory;

  mp=tp.mp;
  cp=tp.cp;
}
//external functions
vcl_ostream& operator<<(vcl_ostream& s, dbctrk_curve_tracking_params const& params)
{
  s <<"Is Clustering ? "<<params.clustering_
    <<"\n Transitive closure ?"<<params.transitive_closure
    <<"\n Benchmark ?"<<params.benchmark_
    <<"\n Window Size for transitive closure "<<params.window_size
    <<"\n minimum length of curves "<<params.min_length_of_curves
    <<"\n ps_moving curves ? "<<params.ps_moving_curves
    <<"\n ps_match_curves "<<params.ps_match_curves
    <<"\n isIHS "<<params.isIHS
    <<"\n Bins for Hue "<<params.thetabins
    <<"\n R1 for Saturation"<<params.r1
    <<"\n V1 for vaue"<<params.v1
    <<"\n matching params "<<params.mp;
  return s;
}
void dbctrk_curve_tracking_params::print_summary(vcl_ostream &os) const
{
  os << *this;
}

dbctrk_curve_tracking ::dbctrk_curve_tracking(dbctrk_curve_tracking_params &tp)
: tp_(tp)
{
  clustering_=tp.clustering_;
  min_len_of_curves_=tp.min_length_of_curves;
  
  tp_=tp;
  tp_.transitive_closure=tp.transitive_closure;
  tp_.window_size=2;
}

void dbctrk_curve_tracking ::track()
{
  vcl_cout<<"-- testing batch tracking --\n";
  // for all images:
  for (unsigned int t=0; t<input_curves_.size(); t++)
  {
    frame_=t;    
    track_frame(t);
  }
  
  return;
}

//-----------------------------------------------------------------------------
dbctrk_tracker_curve_sptr dbctrk_curve_tracking ::get_output_curve(unsigned int frame_no, int id)
{
  if (frame_no >= output_curves_.size())
  {
    vcl_cerr<<"\n frame "<<frame_no<<" doesn't exist\n";
    return 0;
  }
  else
    return output_curves_[frame_no][id];
}

void dbctrk_curve_tracking ::get_reliable_curves(unsigned int frame_no,unsigned int window)
{
  if (frame_no >= output_curves_.size())
    vcl_cerr<<"\n frame "<<frame_no<<" doesn't exist\n";
  else if (frame_no<window)
    vcl_cerr<<"\n cannot compute reliable curves for frame "<<frame_no<<", window size="<<window<<'\n';
  else
  {
    dbctrk_curve_matching_params mp_tc=tp_.mp;
    mp_tc.motion_in_pixels=(window+1)*tp_.mp.motion_in_pixels;
    dbctrk_curve_matching matcher_alternative(mp_tc);
    matcher_alternative.match(&output_curves_[frame_no],&output_curves_tc_[frame_no-window]);
    unsigned int prev_frame=frame_no-window;
    bool found=false;
    for (unsigned int i=0;i<output_curves_[prev_frame].size();i++)
      {
        // avoid the virtual curves added
        if (output_curves_[prev_frame][i]->isreal_)
        {
         
         dbctrk_tracker_curve_sptr way2;
         for (unsigned int j=0;j<output_curves_tc_[prev_frame].size();j++)
          {
            if (output_curves_tc_[prev_frame][j]->get_id()==output_curves_[prev_frame][i]->get_id())
            {
             way2=output_curves_tc_[prev_frame][j];
            }
        }
        dbctrk_tracker_curve_sptr way1=output_curves_[prev_frame][i];
        vcl_map<int,vcl_vector<dbctrk_tracker_curve_sptr> > way1_map,way2_map;
        way1_map[0].push_back(way1);

        found=true;
        for (unsigned int k=0;k<window;k++)
        {
          for (unsigned int l=0; l<way1_map[k].size(); ++l)
          if (way1_map[k][l]->get_best_match_next())
            for (unsigned int n=0; n<way1_map[k][l]->get_best_match_next()->match_curve_set.size(); ++n)
              way1_map[k+1].push_back(way1_map[k][l]->get_best_match_next()->match_curve_set[n]);
        }

        if (way1_map[window].size()==0)
          found=false;

        if (way2->get_best_match_next())
          for (unsigned int k=0; k<way2->get_best_match_next()->match_curve_set.size(); ++k)
            way2_map[window].push_back(way2->get_best_match_next()->match_curve_set[k]);

        if (way2_map[window].size()==0)
          found=false;

        if (found)
          for (unsigned int k=0; k<way1_map[window].size(); ++k)
            for (unsigned int l=0; l<way2_map[window].size(); ++l)
              if (way2_map[window][l]->get_id()==way1_map[window][k]->get_id())
                way1_map[window][k]->isreliable_=true;
      }
    }
  }
}

void dbctrk_curve_tracking ::write_results(vcl_string filename)
{
  vcl_ofstream ofile(filename.c_str());
  ofile<<"<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n<vxl>\n"
       <<"<results video=\""<<filename<<"\" nframes=\""<<output_curves_.size()<<"\">";
  for (unsigned int i=0; i<output_curves_.size(); ++i)
  {
    double cnt=0;
    for (unsigned int j=0; j<output_curves_[i].size(); ++j)
      if (output_curves_[i][j]->isreliable_ && output_curves_[i][j]->match_id_ >0)
        ++cnt;
    vcl_cout<<"\n frame no : "<<i <<" score : "<<cnt/output_curves_[i].size();
    ofile<<"<frame no=\""<<i<<"\" score=\""<<cnt/output_curves_[i].size()<<"\"/>";
  }
  ofile<<"</results>\n</vxl>\n";
}

void dbctrk_curve_tracking ::track_frame(unsigned int frame)
{
  dbctrk_tracker_curve_sptr                primitive;
  vcl_vector< dbctrk_tracker_curve_sptr >  primitive_list;

  dbctrk_tracker_curve_sptr                primitive1;
  vcl_vector< dbctrk_tracker_curve_sptr >  primitive_list1;
  dbctrk_curve_matching                    matcher(tp_.mp);

  if (input_curves_.size()<=frame) return;

  // init : copy the first curves

  if (frame==0)
  {
    primitive_list.clear();
    primitive_list1.clear();
    for (unsigned int i=0;i<input_curves_[0].size();i++)
    {
      primitive = new dbctrk_tracker_curve();
      primitive->init_set(input_curves_[0][i],i);
      primitive->frame_number=0;
      primitive->ismatchedprev_=true;
      primitive->desc->isIHS=tp_.isIHS;
      //primitive->desc->compute_IHS_histograms(tp_.thetabins,tp_.r1,tp_.v1);
      // primitive->desc->assign_rgb_values(plane0[frame],plane1[frame],plane2[frame],3);
      primitive_list.insert(primitive_list.end(), primitive);

      // for alternate frame tracking
      primitive1= new dbctrk_tracker_curve;
      primitive1->frame_number=0;
      primitive1->ismatchedprev_=true;
      primitive1->init_set(input_curves_[0][i],i);
      
      primitive_list1.insert(primitive_list1.end(), primitive1);
    }
    output_curves_.insert(output_curves_.end(), primitive_list);
    output_curves_tc_.insert(output_curves_tc_.end(), primitive_list1);
    compute_chamfer_image(frame);
    for(unsigned int i=0;i<primitive_list.size();i++)
      {
        primitive_list[i]->desc->assign_rgb_values(plane0[frame],plane1[frame],plane2[frame],3,dtimgs[frame],dtmaps[frame],i);
        primitive_list[i]->desc->compute_IHS_histograms(tp_.thetabins,tp_.r1,tp_.v1);
  vcl_cout<<i<<" ";
      }
  }
  else
  {
    // init : duplicate empty primitive lists
    primitive_list.clear();
    primitive_list1.clear();

    vcl_cout<<"\n frame no i s"<<frame;
    for (unsigned int i=0;i<input_curves_[frame].size();i++)
    {
      primitive = new dbctrk_tracker_curve();
      primitive->frame_number=frame;
      primitive->init_set(input_curves_[frame][i],i);
      primitive->desc->isIHS=tp_.isIHS;
      //primitive->desc->compute_IHS_histograms(tp_.thetabins,tp_.r1,tp_.v1);
      //primitive->desc->assign_rgb_values(plane0[frame],plane1[frame],plane2[frame],3);
      primitive_list.insert(primitive_list.end(), primitive);
      // for alternate frame tracking
      primitive1= new dbctrk_tracker_curve;
      primitive1->frame_number=frame;
      primitive1->init_set(input_curves_[frame][i],i);
      primitive1->desc->compute_IHS_histograms(tp_.thetabins,tp_.r1,tp_.v1);
      primitive_list1.insert(primitive_list1.end(), primitive);
    }
    output_curves_.insert(output_curves_.end(), primitive_list);
    output_curves_tc_.insert(output_curves_tc_.end(), primitive_list1);
    compute_chamfer_image(frame);
    for(unsigned int i=0;i<primitive_list.size();i++)
      {
         primitive_list[i]->desc->assign_rgb_values(plane0[frame],plane1[frame],plane2[frame],3,dtimgs[frame],dtmaps[frame],i);
         primitive_list[i]->desc->compute_IHS_histograms(tp_.thetabins,tp_.r1,tp_.v1);
         vcl_cout<<i<<" ";
      }
    vcl_cout<<tp_<<"\n";
    
    // give the new and old curves to matcher to do the matching
  if(tp_.mp.matching_)
  {
    vcl_cout<<"\n matching params are ";
    vcl_cout<<"\n the no of curves entered "<<output_curves_[frame].size();
    //: to match n and n-1 frame
    matcher.match(&output_curves_[frame],&output_curves_[frame-1]);
    vcl_cout<<"\n matching done";
  //: if transitive closure over 3 frames i srequired then compute matches of n and n-2 frames
    //if(tp_.transitive_closure)
    //{
    // tp_.window_size=2;
    // if( (unsigned int)tp_.window_size<=frame)
    // {
    //  
    //  dbctrk_curve_matching_params mp_tc(tp_.mp.motion_in_pixels*tp_.window_size,tp_.mp.no_of_top_choices);
    //  dbctrk_curve_matching matcher_tc(mp_tc);
    //  matcher_tc.match(&output_curves_tc_[frame],&output_curves_tc_[frame-tp_.window_size]);
    //  matcher.best_matches_tc(&output_curves_[frame-1],&output_curves_[frame-2],&output_curves_tc_[frame]);
    // }
    //}
//    else
    {
      vcl_cout<<"\n greedy assignment";
      matcher.best_matches(&output_curves_[frame],&output_curves_[frame-1]);
      //matcher.greedy_and_closure(5,&output_curves_[frame],&output_curves_[frame-1]);
    }
    if (tp_.clustering_)
    {

     vcl_cout<<"\n clustering params are ";
     vcl_cout<<"\n  no_of_clusters"<<tp_.cp.no_of_clusters;
     vcl_cout<<"\n  min_cost_threshold "<<tp_.cp.min_cost_threshold;
     vcl_cout<<"\n  foreg_backg_threshold "<<tp_.cp.foreg_backg_threshold;
     dbctrk_curve_clustering cl(tp_.cp);
     cl.cluster_curves(&output_curves_[frame]);
     cl.build_network(&output_curves_[frame]);
     cl.get_moving_objects(frame,moving_curves_);
     int t1=moving_curves_.size();
     vcl_cout<<"\n the number of curve segmented is  "<<moving_curves_[t1-1].size();
     frame_moving_curves_.push_back(moving_curves_);

    }

  }
  //  transitive closure : input window size=3,5

  }
  vcl_cout<<"\n leaving track_frame";
}
short
dbctrk_curve_tracking::version()
{
  return 1;
}
//: Binary save self to stream.
void
dbctrk_curve_tracking::b_write( vsl_b_ostream& os )
{
  vsl_b_write(os, version());
  int t=output_curves_.size()-1;
  // write the number of nodes
  vsl_b_write(os, (int)output_curves_[t].size());
  // write all the nodes
  for(unsigned int i=0;i<output_curves_[t].size();i++)
  vsl_b_write(os,output_curves_[t][i]);
}


//: Binary load self from stream.
void
dbctrk_curve_tracking::b_read( vsl_b_istream& is )
{
  if (!is) return;

  int t=output_curves_.size();
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      output_curves_[t].clear();

      unsigned int num_curves;
      vsl_b_read(is, num_curves);
      for (unsigned int n=0; n<num_curves; ++n) {
       dbctrk_tracker_curve_sptr c;
       vsl_b_read(is,c);
       output_curves_[t].push_back(c);
      }
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: dbctrk_curve_tracking::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
void dbctrk_curve_tracking :: write_clusters(vcl_string filename,int i)
{


 vcl_ofstream f;
 f.open(filename.c_str());
 if(!f)
  {
   vcl_cerr<<"\n could not open the file to write ";
   return;
 }
 //header of the file
 f<<"#.cem files\n";
 f<<"#Each contour block will consist of the following\n";
 f<<"#[Begin Contour ]\n";


 if(moving_curves_.size()>0)
 {
  //for(int i=0;i<moving_curves_.size();i++)
  {
  for(unsigned int j=0;j<moving_curves_[i].size();j++)
  {
    f<<"[BEGIN CONTOUR]\n";
    f<<"EDGE_COUNT="<<moving_curves_[i][j]->desc->curve_->numPoints()<<"\n";
    for(int k=0;k<moving_curves_[i][j]->desc->curve_->numPoints();k++)
    {
      f<<"[0,0]\t0\t0\t"<<"["<<moving_curves_[i][j]->desc->curve_->point(k).x()<<","
       <<moving_curves_[i][j]->desc->curve_->point(k).y()<<"]"
       <<"\t"<<moving_curves_[i][j]->desc->angles_[k]<<"\t"
       <<moving_curves_[i][j]->desc->grad_[k]<<"\n";
    }
    f<<"[BEGIN PREVIOUS CURVE]\n";
    dbctrk_tracker_curve_sptr prev_curve=moving_curves_[i][j]->get_best_match_prev()->match_curve_set[0];

   for(int k=0;k<prev_curve->desc->curve_->numPoints();k++)
    {
      f<<"[0,0]\t0\t0\t"<<"["<<prev_curve->desc->curve_->point(k).x()<<","
       <<prev_curve->desc->curve_->point(k).y()<<"]"
       <<"\t"<<prev_curve->desc->angles_[k]<<"\t"
       <<prev_curve->desc->grad_[k]<<"\n";
    }
    f<<"[END PREVIOUS CURVE]\n";

    vcl_map<int,int>:: iterator iter;
    f<<"[BEGIN ALIGNMENT]\n";
    for(iter=moving_curves_[i][j]->get_best_match_prev()->mapping_.begin();
      iter!=moving_curves_[i][j]->get_best_match_prev()->mapping_.end();iter++)
      {
        f<<(*iter).first<<"\t"<<(*iter).second<<"\n";
      }
    f<<"[END ALIGNMENT]\n";


    f<<"[BEGIN TRANSFORMATION]\n";
    f<<"["<<moving_curves_[i][j]->get_best_match_prev()->Tbar(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->Tbar(1,0)<<"\t"<<
      moving_curves_[i][j]->get_best_match_prev()->R_(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->R_(0,1)<<"\t"
      <<moving_curves_[i][j]->get_best_match_prev()->R_(1,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->R_(1,1)<<"\t"
      <<moving_curves_[i][j]->get_best_match_prev()->scale_<<"]\n";
     f<<"[END TRANSFORMATION]\n";
    f<<"[END CONTOUR]\n";
   }

  }

 }
 f.close();

}

vcl_vector< dbctrk_tracker_curve_sptr> *dbctrk_curve_tracking ::get_output_curves(unsigned int frame_no)

{
  if (frame_no >= output_curves_.size())
    return NULL;
  else
    return &output_curves_[frame_no];
}
void dbctrk_curve_tracking ::obtain_tracks()
{
 if(output_curves_.size()<1)
   return ;

 int trackid=0;
 for(unsigned int i=0;i<output_curves_.size();i++)
 {
  for(int j=0;j<get_output_size_at(i);j++)
  {
   dbctrk_tracker_curve_sptr curve=get_output_curve(i,j);
   if(curve->track_id_==-1)
   {
     vcl_list<dbctrk_tracker_curve_sptr> tr;
     vcl_list<dbctrk_tracker_curve_sptr>::iterator iter;

    level_order_traversal(curve,tr);
    if(tr.size()>0)
    {
      for (iter=tr.begin();iter!=tr.end();iter++)
        (*iter)->track_id_=trackid;
      len_of_tracks_[trackid]=tr.size();
      trackid++;
    }
   }
  }
 }
}
void dbctrk_curve_tracking ::set_tracking_params(dbctrk_curve_tracking_params &tp)
{
    tp_=tp;
}

void dbctrk_curve_tracking ::write_tracks(dbctrk_tracker_curve_sptr curve,
                                        vcl_string fname,
                                        int min_length_of_track)
{
  vcl_map<int, vcl_list<dbctrk_tracker_curve_sptr> > track_;
  vcl_map<int, vcl_list<dbctrk_tracker_curve_sptr> >::iterator  itrack;
  vcl_list<dbctrk_tracker_curve_sptr> tr;
  vcl_list<dbctrk_tracker_curve_sptr>::iterator iter;

  level_order_traversal(curve,tr);
  for (iter=tr.begin();iter!=tr.end();iter++)
    track_[(*iter)->frame_number].push_back((*iter));

  if (int(track_.size())>min_length_of_track)
  {
    vcl_ofstream f(fname.c_str());
    f<<"# CONTOUR_EDGE_MAP : canny+van-ducks\n"
     <<"# .cem files\n"
     <<"# Format :\n"
     <<"# Each contour block will consist of the following\n"
     <<"# [BEGIN CONTOUR]\n"
     <<"# EDGE_COUNT=num_of_edges\n"
     <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf\n"
     <<"CURVE\n";
    for (itrack=track_.begin();itrack!=track_.end();itrack++)
    {
      f<<"[BEGIN FRAME NO = "<<(*itrack).first<<"]\n";
      for (iter=(*itrack).second.begin();iter!=(*itrack).second.end();iter++)
      {
        dbctrk_tracker_curve_sptr obj=(*iter);
        f<<"[BEGIN CONTOUR]\n"
         <<"EDGE_COUNT="<<obj->desc->curve_->numPoints()<<"\n";

        for (int i=0; i<obj->desc->curve_->numPoints(); ++i)
        {
          f<<" ["<<obj->desc->curve_->point(i).x()<<", "<<obj->desc->curve_->point(i).y()
           <<"]   0 0\n";
        }
        f<<"[END CONTOUR]\n";
      }
    }
    f<<"END CURVE\n";
    f.close();
  }
}

void dbctrk_curve_tracking ::level_order_traversal(dbctrk_tracker_curve_sptr curve,vcl_list<dbctrk_tracker_curve_sptr> & tr)
{
  tr.clear();
  vcl_list<dbctrk_tracker_curve_sptr> q;
  if (!curve)
    return;

  q.push_back(curve);
  while (q.size()>0)
  {
    dbctrk_tracker_curve_sptr c=q.front();
    q.pop_front();
    if (c)
    {
      tr.push_back(c);
      if (c->get_best_match_next())
        for (unsigned int i=0; i<c->get_best_match_next()->match_curve_set.size(); ++i)
          q.push_back(c->get_best_match_next()->match_curve_set[i]);
    }
    else
      break;
  }
}


double dbctrk_curve_tracking::test_output(vcl_vector<vgl_point_2d<double> > points,int t)
{
  vcl_cout<<"\n"<<points.size()<<"\t"<<t;
  vgl_polygon<double> plygn(points);

  double in=0,out=0;
  vcl_cout<<"\n the size of the segmented curves "<<moving_curves_[t-1].size();
  for(unsigned int i=0;i<moving_curves_[t-1].size();i++)
  {
   vcl_cout<<moving_curves_[t-1].size();
   for(int j=0;j<moving_curves_[t-1][i]->desc->curve_->numPoints();j++)
   {
    if(plygn.contains(moving_curves_[t-1][i]->desc->curve_->point(j)))
      in++;
    else
      out++;
   }
  }
  if(in+out>0)
  return in/(in+out);
  else
    return -1;
}





bool dbctrk_curve_tracking::compute_chamfer_image(int frame)
{
  if(frame>=(int)output_curves_.size())
     return false;
  vcl_map<vsol_digital_curve_2d_sptr,int> curvewids;
  vcl_vector<vsol_digital_curve_2d_sptr> curves;
  for(unsigned int i=0;i<output_curves_[frame].size();i++)
    {
      curvewids[output_curves_[frame][i]->dc_]=i;
      curves.push_back(output_curves_[frame][i]->dc_);
    }

  int width=plane0[frame].ni();
  int height=plane0[frame].nj();
  vcl_cout<<"\n width is "<<width;
  vcl_cout<<"\n height is "<<height;
  dbspi_curve_map map(curves, width, height);

  int ni = map.width();
  int nj = map.height();
  vil_image_view<float> dtimg(ni, nj);
  vil_image_view<int> dtmap(ni, nj);

  for(int i=0; i<ni; ++i)
    {
    for(int j=0; j<nj; ++j)
      {
  dtimg(i,j) = map.distance(i+map.x_offset(),j+map.y_offset());
  vsol_digital_curve_2d_sptr c=map.closest(i+map.x_offset(),j+map.y_offset());
  dtmap(i,j)=curvewids[c];
      }
    }
  dtimgs.push_back(dtimg);
  dtmaps.push_back(dtmap);

  return true;
}
