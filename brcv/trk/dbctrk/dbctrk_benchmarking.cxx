#include<dbctrk/dbctrk_benchmarking.h>
#include<vcl_iostream.h>
#include<vcl_sstream.h>
#include<dbctrk/dbctrk_tracker_curve.h>
#include<vul/vul_psfile.h>
#include<dbctrk/dbctrk_utils.h>
#include<vcl_cstdlib.h>
#include<vcl_algorithm.h>

void dbctrk_benchmarking::in_or_out(vcl_vector<vgl_point_2d<double> > p, 
                                  vcl_vector<dbctrk_tracker_curve_sptr> curves,
                                  double &truePositive, double &trueNegative)
{  
  
  vgl_polygon<double> plygn(p);

  double in=0,out=0;
  double innonsegmented=0;
  double outnonsegmented=0;
  for(unsigned i=0;i<curves.size();i++)
  {
   bool curve_inside_=false;
   if(curves[i]->ismovingobject_)
   {
  for(int j=0;j<curves[i]->desc->curve_->numPoints();j++)
  {
    if(plygn.contains(curves[i]->desc->curve_->point(j)))
      curve_inside_=true;
    }
    if(curve_inside_)
        in++;
    else
        out++;

   }
   else
   {
    
    for(int j=0;j<curves[i]->desc->curve_->numPoints();j++)
  {
    if(plygn.contains(curves[i]->desc->curve_->point(j)))
      curve_inside_=true;
    }
    if(curve_inside_)
        innonsegmented++;
    else
        outnonsegmented++;
   }
  }
  if (in + out ==0)
  truePositive = 0;
  else
    truePositive=in/(in+out);
  
  
  if (innonsegmented+in ==0)
  trueNegative=0;
  else
  trueNegative=innonsegmented/(innonsegmented+in);

  vcl_cout<<"True Pos ="<<truePositive<<"\t True Neg="<<trueNegative;
}


bool dbctrk_benchmarking::write_moving_curves_to_ps(vcl_vector<dbctrk_tracker_curve_sptr> curves,
                          vcl_string outputdir,int frameno)
{
  vcl_ostringstream s;
  s<<frameno;
  vcl_string outputfile=outputdir+"//frame"+s.str()+".ps";
  vul_psfile psfile(outputfile.c_str(), false);
  psfile.set_scale_x(50);
  psfile.set_scale_y(50);
  
  psfile.line(0,0,1024,0);
  psfile.line(1024,0,1024,768);
  psfile.line(1024,768,0,768);
  psfile.line(0,768,0,0);

  if(curves.size()>0)
  {
    for (unsigned int i=0;i<curves.size();i++)
    {
      if(curves[i]->ismovingobject_)
      {
        dbctrk_tracker_curve_sptr test_curve1=curves[i];
        psfile.set_line_width(1.0);
        psfile.set_fg_color(0,0,0);
        for(int k=0;k<test_curve1->desc->curve_->numPoints()-1;k++)
          psfile.line((float)test_curve1->desc->curve_->point(k).x(),
                      (float)test_curve1->desc->curve_->point(k).y(),
                      (float)test_curve1->desc->curve_->point(k+1).x(),
                      (float)test_curve1->desc->curve_->point(k+1).y());
      }
    }
    return true;
  }
  return false;
}

bool dbctrk_benchmarking::read_outlines(vcl_string fileName,vcl_vector<vgl_point_2d<double> > & v1)
{

  vcl_ifstream infp(fileName.c_str(),vcl_ios::in);
  v1.clear();
  if (!infp){
    vcl_cout << " Error opening file  " << fileName << "\n";
    return false;
  }
  
  
  char lineBuffer[200];
  infp.getline(lineBuffer,200);
  if (strncmp(lineBuffer,"CONTOUR",7)){
    vcl_cerr << "Invalid File " << fileName.c_str() << "\n";
    vcl_cerr << "Should be CONTOUR " << lineBuffer << "\n";
    exit(1);
  }
  
  char openFlag[200];
  infp.getline(openFlag,200);
  if (!strncmp(openFlag,"OPEN",4))
  {}
  else{
    vcl_cerr << "Invalid File " << fileName.c_str() << "\n";
    vcl_cerr << "Should be OPEN " <<  "\n";
    exit(1);
  }
  

  infp.getline(lineBuffer,200);
  int i,numOfPoints;
  infp >> numOfPoints;
  
  double  x,y;
  for (i=0;i<numOfPoints;i++) {
    infp >> x >> y;
    vgl_point_2d<double>  p(x,y);
    v1.push_back(p);
  }

  infp.close();
  return true;
}





bool dbctrk_benchmarking::matching_stats(vcl_string filename,vcl_vector<dbctrk_tracker_curve_sptr> curr_curves)
{
  vcl_ifstream infp(filename.c_str());
  if (!infp){
  vcl_cout << " Error opening file  "  << "\n";
  exit(1);
  return false;
  }
  char buf[100];
  vcl_map<vcl_vector<int> , vcl_vector<int>  > matches;
  vcl_map<vcl_vector<int> , vcl_vector<int>  >::iterator iter;
  vcl_vector<int>::iterator fiter;
  vcl_vector<int>::const_iterator giter;
  while(infp.getline(buf,100)){
   
  vcl_string s(buf);
  int pos11=s.find_first_of("[",0);
  int pos12=s.find_first_of("]",0);
  vcl_string s1=s.substr(pos11,pos12-pos11);
   
  int pos21=s.find_first_of("[",pos12);
  int pos22=s.find_first_of("]",pos12);
  vcl_string s2=s.substr(pos21,pos22-pos21);
   
  vcl_vector<vcl_string> vs1 = utils::parseLineForNumbers(s1);
  vcl_vector<vcl_string> vs2 = utils::parseLineForNumbers(s2);

   vcl_vector<int> temp1;
   vcl_vector<int> temp2;
   for(unsigned int i=0;i<vs1.size();i++)
   {
     temp1.push_back(vcl_atoi(vs1[i].c_str()));
   }
   //vcl_cout<<"\n size1 is "<<temp1.size();
   for(unsigned int i=0;i<vs2.size();i++)
   {
   temp2.push_back(vcl_atoi(vs2[i].c_str()));
   }
   //vcl_cout<<"\t size2 is "<<temp2.size();
   matches[temp1]=temp2;
  }
  vcl_cout<<"\n the no of matches are "<<matches.size();
  
  int cnt=0; 
  for(unsigned int i=0;i<curr_curves.size();i++)
  {
  dbctrk_tracker_curve_sptr test_curve=curr_curves[i];
  int id=test_curve->get_id();

  for(iter=matches.begin();iter!=matches.end();iter++)
  {
    fiter=vcl_find((*iter).second.begin(),(*iter).second.end(),id);
    if(fiter!=(*iter).second.end())
    {
      if(test_curve->get_best_match_prev().ptr())
      {
        int prev_id=test_curve->get_best_match_prev()->match_curve_set[0]->get_id();
        vcl_vector<int>::const_iterator siter=((*iter).first).begin();
        vcl_vector<int>::const_iterator eiter=((*iter).first).end();
        giter=vcl_find(siter,eiter,prev_id);
        if(giter!=(*iter).first.end())
        {
          cnt++;        
        }
      }
      else if(test_curve->match_id_==-1)
      {
        if((*iter).first.size()<1)
        {
          cnt++;
        }
      }
    }
  }


  }

  vcl_cout<<"\n the total no of curves are "<<curr_curves.size();
  vcl_cout<<"\n the total correct count is "<<cnt;

  return true;
}
