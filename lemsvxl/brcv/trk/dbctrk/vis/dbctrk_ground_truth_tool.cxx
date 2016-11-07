#include <dbctrk/vis/dbctrk_ground_truth_tool.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_tracker_curve.h>
#include <dbctrk/dbctrk_curveMatch.h>
#include <dbctrk/dbctrk_utils.h>
dbctrk_ground_truth_tool::dbctrk_ground_truth_tool()
{

}
dbctrk_ground_truth_tool::~dbctrk_ground_truth_tool()
{

}
bool
dbctrk_ground_truth_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbctrk_ground_truth_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;

  if (storage->type() == "dbctrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbctrk_ground_truth_tool::tableau()
{
  return tableau_;
}

dbctrk_storage_sptr
dbctrk_ground_truth_tool::storage()
{
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage_);
  return dbctrk_storage;
}
bool
dbctrk_ground_truth_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view) 
{
    
       int frame = view->frame();


       if( e.type == vgui_KEY_PRESS && e.key == 's' && vgui_SHIFT ){
               static vcl_string filename="";
               static vcl_string ext="*.*";

               vgui_dialog inputfile("Load Ground Truth");
               inputfile.file("Ground Truth File Name",ext,filename);
               if(!inputfile.ask())
                    return true;

         vcl_cout<<"\n reading ground truth file .... ";
         vcl_cout<<"\n reading ground truth file .... ";
               vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > >gmatches;
               readcorrespondence(filename,gmatches);
         vcl_cout<<"\n reading done ";
         vcl_cout<<"\n reading done ";

               dbctrk_storage_sptr p,q;
               vcl_vector<dbctrk_tracker_curve_sptr> tc1,tc2;
               p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
               p->get_tracked_curves(tc1);
         vcl_cout<<"\n the number of curves in frame 1 is "<<tc1.size();
         vcl_cout<<"\n the number of curves in frame 1 is "<<tc1.size();
               q.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame+1));
               q->get_tracked_curves(tc2);

         vcl_cout<<"\n the number of curves in frame 2 is "<<tc2.size();
         vcl_cout<<"\n the number of curves in frame 2 is "<<tc2.size();
         
               vcl_vector< vcl_pair<vcl_vector<int>,vcl_vector<int> > >::iterator iter;
               vcl_vector< vcl_map<vcl_string, vgui_tableau_sptr> > seq=bvis1_manager::instance()->tableau_sequence();
               vcl_map<vcl_string, vgui_tableau_sptr>::iterator iters;
               int frameno=0;
               for(unsigned int j=0;j<seq.size();j++,frameno++)
                {  
                for(iters=seq[j].begin();iters!=seq[j].end();iters++)
                {
                    if(set_tableau((*iters).second))
                    {    
                     int cnt=0;
                     for(iter=gmatches.begin();iter!=gmatches.end();iter++,cnt++)
                     {
                      for(unsigned int k=0;k<(*iter).first.size();k++)
                        {
                          vcl_cout<<k<<"\t"; 
        float r,g,b;
                             if(frameno==0)
                             {
                                dbctrk_soview2D* obj=new dbctrk_soview2D(tc1[(*iter).first[k]]);
                                utils::set_changing_colors( cnt ,r, g, b );                             
        vgui_style_sptr sty= vgui_style::new_style(r,g,b,2.0,2.0);
                                tableau_->add( obj );
                                obj->set_style(sty);
                             }
                        }                          
                      for(unsigned int k=0;k<(*iter).second.size();k++)
                        {
                             float r,g,b;
                             if(frameno==1)
                             {
                                dbctrk_soview2D* obj=new dbctrk_soview2D(tc2[(*iter).second[k]]);
                                utils::set_changing_colors( cnt ,r, g, b );
                                vgui_style_sptr sty= vgui_style::new_style(r,g,b,2.0,2.0);
                                tableau_->add( obj );
                                obj->set_style(sty);
                             }
                        }       
                    }
                   }
                 }
                }
            bvis1_manager::instance()->post_redraw();
           }
 

     if( e.type == vgui_KEY_PRESS && e.key == 'e' && vgui_SHIFT ){

     vcl_string filename="";
     vcl_string filenameI="";
     vcl_string filenameS="";

     vcl_string ext="*.*";

     vgui_dialog inputfile("Load Ground Truth");
     inputfile.file("Ground Truth File Name",ext,filename);
     inputfile.file("Cost for Intensity File Name",ext,filenameI);
     inputfile.file("Cost for Shape File Name",ext,filenameS);

     if(!inputfile.ask())
          return true;
        dbctrk_storage_sptr p;
        vcl_vector<dbctrk_tracker_curve_sptr> tc1,tc2;
        
        p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
        p->get_tracked_curves(tc1);
        p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame+1));
        p->get_tracked_curves(tc2);

        vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > >gmatches;
        readcorrespondence(filename,gmatches);
        vcl_vector< vcl_pair<vcl_vector<int>,vcl_vector<int> > >::iterator iter;


        vcl_map<int,int> mapping;
        vnl_matrix <double> R;
        vnl_matrix <double> Tbar;              
        vcl_map<int,vcl_vector<double> > imap;
        vcl_map<int,vcl_vector<double> > smap;

        vcl_ofstream ofileI(filenameI.c_str());
        vcl_ofstream ofileS(filenameS.c_str());

        for(iter=gmatches.begin();iter!=gmatches.end();iter++)
        {
            if((*iter).second.size()>0)
            {
                for(unsigned int j=0;j<(*iter).first.size();j++)
                {
                    vcl_vector<double> icost;
                    vcl_vector<double> scost;
                    ofileI<<tc1[(*iter).first[j]]->get_id()<<" ";
                    ofileS<<tc1[(*iter).first[j]]->get_id()<<" ";
                     for(unsigned int i=0;i<tc2.size();i++)
                     {
    /*                     double cost11=IcurveMatch(tc1[(*iter).first[j]],tc2[i],mapping,R,Tbar,scale,0,0,1,true);
                         double cost12=IcurveMatch(tc1[(*iter).first[j]],tc2[i],mapping,R,Tbar,scale,0,0,1,false);
                         double cost1=cost11>cost12 ? cost12:cost11;
                         double cost21=IcurveMatch(tc1[(*iter).first[j]],tc2[i],mapping,R,Tbar,scale,1,1,0,true);
                         double cost22=IcurveMatch(tc1[(*iter).first[j]],tc2[i],mapping,R,Tbar,scale,1,1,0,false);
                         double cost2=cost21>cost22 ? cost22:cost21;
                         ofileI<<cost1<<" ";
                         ofileS<<cost2<<" ";

                         icost.push_back(cost1);
                         scost.push_back(cost2);*/
                     }
                     ofileI<<"\n";
                     ofileS<<"\n";
                     imap[tc1[(*iter).first[j]]->get_id()]=icost;
                     smap[tc1[(*iter).first[j]]->get_id()]=scost;
                }
                

            }

        }
     } 
      return true;

 if( e.type == vgui_KEY_PRESS && e.key == 'k' && vgui_SHIFT ){

     vcl_string filename="";

     vcl_string ext="*.*";

     vgui_dialog inputfile("Load Curves for cues");
     inputfile.file("Load curves",ext,filename);

     if(!inputfile.ask())
          return true;

     vcl_vector<int>  ids;
     load_curve_ids(filename,ids);

     dbctrk_storage_sptr p;
     vcl_vector<dbctrk_tracker_curve_sptr> tc1,tc2;
        
        p.vertical_cast(bvis1_manager::instance()->repository()->get_data_at("dbctrk",frame));
        p->get_tracked_curves(tc1);

 
        for(unsigned int i=0;i<ids.size();i++)
        {
           if(ids[i]>-1)
           {  
           dbctrk_soview2D* obj=new dbctrk_soview2D(tc1[ids[i]]);
           //float r,g,b;
           //utils::set_changing_colors( cnt ,r, g, b );
           vgui_style_sptr sty= vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
           tableau_->add( obj );
           obj->set_style(sty);
           }
        }


     } 
      return true;


}
bool
dbctrk_ground_truth_tool::load_curve_ids(vcl_string filename,vcl_vector<int> & ids)
{
    vcl_ifstream ifile(filename.c_str());
    if(!ifile)
        return false;

    while(!ifile.eof())
    {
        double id;
        ifile>>id;
        ids.push_back((int)id);
    }
    return true;
}
bool readcorrespondence(vcl_string filename,vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > >&corr)
{
    vcl_ifstream ifile(filename.c_str());
    if(!ifile)
    {
        vcl_cout<<"\n error opening the ground truth file ";
        return false;
    }
    corr.clear();

    vcl_string buffer;
    char tempbuffer[100];
    while(!ifile.eof())
    {
        ifile.getline(tempbuffer,100,'\n');

        buffer=vcl_string(tempbuffer);
        int pos11=buffer.find_first_of("[",0);
        int pos12=buffer.find_first_of("]",0);

        vcl_string s1;
        if(pos11>0 || pos12>0)
            s1=buffer.substr(pos11,pos12-pos11);
        int pos21=buffer.find_first_of("[",pos12+1);
        int pos22=buffer.find_first_of("]",pos12+1);
        vcl_string s2;
        if(pos21>0 || pos22>0)
            s2=buffer.substr(pos21,pos22-pos21);
        
        vcl_vector<vcl_string> vs2 = parseLineForNumbers(s2);
        vcl_vector<vcl_string> vs1 = parseLineForNumbers(s1);

        vcl_vector<int> ids1,ids2;
        for (unsigned int i=0;i<vs1.size();i++){
         ids1.push_back(atoi(vs1[i].c_str()));
        }
        for (unsigned int i=0;i<vs2.size();i++){
    ids2.push_back(atoi(vs2[i].c_str()));
        }

        vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > >::iterator iter;
    int flag=0;
        for(iter=corr.begin();iter!=corr.end();iter++)
        {

   for(unsigned int i=0;i<(*iter).second.size();i++)
            {
                for(unsigned int j=0;j<ids2.size();j++)
                {
                    if(ids2[j]==(*iter).second[i])
                    {
                        flag=1;
                    }
                }
            }
            if(flag>0)
            {
                bool isinsert=true;
                for(unsigned int j=0;j<ids1.size();j++)
                {
                  for(unsigned int i=0;i<(*iter).first.size();i++)
                    {
                        if(ids1[j]==(*iter).first[i])
                        {
                            isinsert=false;
                        }
                    }
                    if(isinsert)
                        (*iter).first.push_back(ids1[j]);
                } 
            }
        
        }

        if(flag==0)
                corr.push_back(vcl_make_pair(ids1,ids2));

        
    }

    ifile.close();
    return true;
}

vcl_vector<vcl_string> parseLineForNumbers(vcl_string s)
{
  vcl_vector <vcl_string> vec;

  if (s.size() > 0){

    vcl_string numerics("0123456789.-+e");
    vcl_string::size_type pos1=0,pos2=0;

    while((pos1=s.find_first_of(numerics,pos1)) != vcl_string::npos) {
      pos2=s.find_first_not_of(numerics,pos1);
      vec.push_back(s.substr(pos1,pos2-pos1));
      pos1=pos2;
    }
  }
  return vec;
}

//: Return the name of this tool
vcl_string 
dbctrk_ground_truth_tool::name() const

{

  return "Load ground truth";

}
