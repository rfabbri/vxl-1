#include <dbvis1/tool/dbvis1_homog_view_tools.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_deck_tableau_sptr.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vcl_map.h>
#include <vcl_list.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <sadali/bprt/bprt_homog_interface.h>

//Note add rrel dir to bvis1 cmakelists.txt


/*************************************************************************/

//// Tool to select points to compute stereo image homography
//// and compute and display all corr. points in the two views
//// according to the calculated homography.
//// Uses bprt_homog_interface class
///Constructor

dbvis1_homog_view_tool::dbvis1_homog_view_tool( const vgui_event_condition& lift    ,const vgui_event_condition& end_sel ,
                                          const vgui_event_condition& draw_box,  const vgui_event_condition& img_sel) :
                                           gesture_lift_(lift),  gesture_end_sel_(end_sel), gesture_draw_box_(draw_box) ,
                                           active_(false), object_(NULL), tableau_vsol(NULL),  end_sel_flag(false), imgsel(false) ,
                                           gesture_img_sel_(img_sel) , thresh_(0.6), use_correlation_(true), num_pop(1),
                                           use_epipolar_const_(false)
{

}


//: Destructor
dbvis1_homog_view_tool::~dbvis1_homog_view_tool()
{
}


//: Return the name of this tool
vcl_string
dbvis1_homog_view_tool::name() const
{
        return "Homography View";
}


//: Set the tableau to work with
bool
dbvis1_homog_view_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
        if( tableau.ptr() != NULL && tableau->type_name() == "bgui_vsol2D_tableau"      ){
                tableau_vsol.vertical_cast(tableau);
                //
                return true;
        }
        if( tableau.ptr() != NULL && (tableau->type_name() == "vgui_easy2D_tableau" ||
                tableau->type_name() == "vgui_image_tableau")){
                        tableau_im.vertical_cast(tableau);

                        return true;
                }

                tableau_vsol = NULL;
                tableau_im=NULL;
                // vcl_cout<<"Tableau doesn't exist or isn't right type";
                return false;
}

//: Handle events
bool 
dbvis1_homog_view_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& selector)
{
        float ix, iy;
        vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
        if( e.type == vgui_MOTION ) 
        {
                last_x = ix;
                last_y = iy;
                if( active_ ) 
                {
                        tableau_vsol->post_overlay_redraw();
                }
        }

        // If selection of points and images have ended

        if (!active_  && tableau_vsol.ptr() && gesture_end_sel_(e) )
        {
                end_sel_flag=true;
   
        }
        //If  an image tableau was active and SHIFT+MMB is pressed
                //add the image_resoruce to images_ list

        if (tableau_im.ptr()  && gesture_img_sel_(e))
        {
                vcl_cout<<tableau_im->type_name()<<": "<<(void *) tableau_im.ptr()<<"\n";
                tableaus_im_.push_back(tableau_im);
                images_.push_back( bvis1_manager::instance() -> storage_from_tableau( tableau_im ) );

        }                       




        //If drawing is active and selection of pts has not ended.
        //Drawing a polygon to select points

        if (active_&&!end_sel_flag)
        {
                //If done drawing box click left button
                if (gesture_end_sel_(e))
                {
                                                //adding last point
                        drawnpt.push_back( new vsol_point_2d (last_x , last_y) );

                        vidpro1_vsol2D_storage_sptr stor;
                                                
                        stor.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableau_vsol ) );
                        vcl_vector <vcl_string > namesake;
                        vsol_polygon_2d_sptr drawn(new vsol_polygon_2d( drawnpt ) );

                                                //Creating a tableeu for the drawn polygon
                        bgui_vsol2D_tableau polyg_tableau ( tableau_vsol.ptr() );
                        polyg_tableau.add_vsol_polygon_2d( drawn );


                        vcl_vector< vsol_spatial_object_2d_sptr > pts_in_stor = stor->all_data();
                        
                        vcl_vector<vsol_point_2d_sptr> select_pts;
                        int count=0;


                        for (int k=0;k<pts_in_stor.size();k++)
                        {
                                vsol_point_2d_sptr p= pts_in_stor[k]->cast_to_point() ;
                                if ( drawn->is_inside(p) )
                                {
                                        select_pts.push_back(p);
                                        count++;
                                }
                        }

                        vcl_cout<<"Total no of pts in polygon:"<<count<<"\n";


                        int found=-1;
                        //add the selected point to the point list that is associated with a particular tableau
                        //Add a new tableau if the point is not displayed among the tableaus already selected

                        for(int i=0;i<tableaus_vsol_.size();i++)
                        {
                                if (tableaus_vsol_[i]==tableau_vsol)
                                {

                                        found=i;
                                }
                        }

                        if (found==-1)
                        {
                                vcl_cout<<"new tableau\n";
                                tableaus_vsol_.push_back(tableau_vsol);
                                pts_.push_back(select_pts);

                        }
                        else
                        {
                                for (int m=0;m<select_pts.size();m++)
                                        pts_[found].push_back(select_pts[m]);
                        }
                        active_=false;
                        drawnpt.clear();
                }

                //If the polygon is completely drawn , click MMB
                if ( gesture_img_sel_(e) )
                { drawnpt.push_back( new vsol_point_2d (last_x , last_y) );
                }

                if( e.type == vgui_OVERLAY_DRAW ) 
                {
                        glLineWidth(1);
                        glColor3f(1,1,1);
                        glBegin(GL_LINE_STRIP);
                        for (unsigned i=0; i<drawnpt.size(); ++i)
                                glVertex2f(drawnpt[i]->x(), drawnpt[i]->y() );
                        glVertex2f(last_x,last_y);



                        glEnd();


                }
        }

        //If there was an active bgui_vsol2D_tableau and SHIFT+LMB is pressed
        //Make drawing active and start drawing polygon

        if (tableau_vsol.ptr() && gesture_draw_box_(e))
        {
                left_cornerx_=last_x;
                left_cornery_=last_y;
                drawnpt.push_back (new vsol_point_2d (last_x , last_y) );
                active_ = true;


        }




        //If a point is selected from a particular vsol2d tableau, add it 
        // to the pointlist of that particular tableau
        // If the tableau doesn't exist among the previous tableaus from 
        // which the points on it were selected, add the tableau pointer
        //to the tableaus_vsol vector.




        if( tableau_vsol.ptr() && gesture_lift_(e))
        {
                //      vcl_cout<<"selected object\n";
                object_ = (vgui_soview2D*)tableau_vsol->get_highlighted_soview();
                //      active_ = true;
                if(( object_ != NULL )&&!end_sel_flag)
                {

                        int found=-1;
                        //add the selected point to the point list that is associated with a particular tableau
                        //Add a new tableau if the point is not displayed among the tableaus already selected

                        for(int i=0;i<tableaus_vsol_.size();i++)
                        {
                                if (tableaus_vsol_[i]==tableau_vsol)
                                {

                                        found=i;
                                }
                        }
                        bgui_vsol_soview2D_point* object_ptr = dynamic_cast<bgui_vsol_soview2D_point *>(object_);
                        


                        if (found==-1)
                        {
                                vcl_cout<<"new tableau\n";
                                tableaus_vsol_.push_back(tableau_vsol);
                                if ((object_ptr))
                                {
                                        //      object_ptr->set_style(vgui_style::new_style(0.4f, 0.2f, 0.8f, 1.0f, 3.0f));
                                        vcl_vector<vsol_point_2d_sptr> newpts;
                                         vcl_vector <vsol_line_2d_sptr> epilines_in_tab;
                                        newpts.push_back(new  vsol_point_2d(object_ptr->sptr()->x(),object_ptr->sptr()->y()));
                                        pts_.push_back(newpts);
                                        epilines.push_back(epilines_in_tab);
                                }
                                else
                                {
                                        vcl_vector<vsol_point_2d_sptr> newpts;
                                        bgui_vsol_soview2D_line_seg* line_ptr=(bgui_vsol_soview2D_line_seg *)object_;
                                        vcl_vector <vsol_line_2d_sptr> epilines_in_tab;
                                        epilines_in_tab.push_back( new vsol_line_2d(*(line_ptr->sptr().ptr() ) ) );
                                        epilines.push_back(epilines_in_tab);
                                        pts_.push_back(newpts);
                                }
                        }
                        else
                        {
                                if ((object_ptr))
                                {
                                        //object_ptr->set_style(vgui_style::new_style(0.4f, 0.2f, 0.8f, 1.0f, 3.0f));
                                        vsol_point_2d_sptr newpt(new vsol_point_2d(object_ptr->sptr()->x(),object_ptr->sptr()->y()));
                                        pts_[found].push_back(newpt);
                                }
                                  else
                                {
                                          
                                        bgui_vsol_soview2D_line_seg* line_ptr=(bgui_vsol_soview2D_line_seg *)object_;
                                        epilines[found].push_back( new vsol_line_2d(*(line_ptr->sptr().ptr() ) ) );
                                 
                                }
                        }

                }


        }




        //Four pairs of points in two images have been selected, find the homography for 
        // these four correspondences


        if ((pts_.size()==2)&&(pts_[0].size()==4)&&(pts_[1].size()==4)&&end_sel_flag)
        {
                if ( this->four_pts() )
                        return true;
                else 
                        return false;

        }



        // More than eight pairs of points in two images have been selected, crosscorrelate regions
        // around these points to find putative correspondences, sample among these
        // these correspondences to find a valid homography.

        else
                if ((pts_.size() == 2)&&(pts_[0].size() >= 8)&&(pts_[1].size() >= 8)&& end_sel_flag)
                {
                        if ( (epilines.size() == 2) && (epilines[0].size() == 2) && (epilines[1].size() == 2) )
                        {
                                   use_epipolar_const_ = true; 
                                epip.push_back( vgl_homg_operators_2d<double>::intersection(epilines[0][0]->vgl_hline_2d(), epilines[0][1]->vgl_hline_2d() ) );
                                epip.push_back( vgl_homg_operators_2d<double>::intersection(epilines[1][0]->vgl_hline_2d(), epilines[1][1]->vgl_hline_2d() ) );
                        }
                        
                        if ( this->Ransac_selected_pts_in_image() )
                                return true;
                        else 
                                return false;                                                                           
                }

                // Less than four pairs of points in two images have been selected, so take all points in both images
                // crosscorrelate regions around these points to find putative correspondences, sample among these
                // these correspondences to find a valid homography.

                else if ((pts_.size() == 2) && (pts_[0].size() < 4)&&(pts_[1].size() < 4) && end_sel_flag)
                {
                        pts_[0].clear();
                        pts_[1].clear();
                  
                        if ( (epilines.size() == 2) && (epilines[0].size() == 2) && (epilines[1].size() == 2) )
                        {
                                use_epipolar_const_ = true; 

                        epip.push_back( vgl_homg_operators_2d<double>::intersection(epilines[0][0]->vgl_hline_2d(), epilines[0][1]->vgl_hline_2d() ) );
                        epip.push_back( vgl_homg_operators_2d<double>::intersection(epilines[1][0]->vgl_hline_2d(), epilines[1][1]->vgl_hline_2d() ) );

                        }
                        if ( this->Ransac_all_pts_in_image() )
                                return true;
                        else 
                                return false;

                }
           // else 
           // {
                 //   for (int q= 0; q<pts_.size();q++)
                 //       pts_[q].clear();
                 //   pts_.clear();
           // }


                return false;
}



bool dbvis1_homog_view_tool::four_pts()
{
        end_sel_flag=false;
        
        vcl_string alg="4pt";
        bprt_homog_interface homogfind(pts_[0],pts_[1]);
        homogfind.compute_homog();
        
        vcl_cout<<"\n"<<alg<<"\n";
        vidpro1_vsol2D_storage_sptr pt_2b_trans_stor1;
        pt_2b_trans_stor1.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableaus_vsol_[0] ) );
        vidpro1_vsol2D_storage_sptr pt_2b_trans_stor2;
        pt_2b_trans_stor2.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableaus_vsol_[1] ) );

        vcl_vector< vsol_spatial_object_2d_sptr > pt_2b_trans1;
        pt_2b_trans1 = pt_2b_trans_stor1->all_data();
        
        vcl_vector< vsol_spatial_object_2d_sptr > pt_2b_trans2;
        pt_2b_trans2 = pt_2b_trans_stor2->all_data();
        
        homogfind.transfer(pt_2b_trans1,pt_2b_trans2,0);

vcl_vector <vsol_spatial_object_2d_sptr> point,point2;
homogfind.get_output(point      , 1);
homogfind.get_output(point2 , 2);
vcl_vector< vsol_spatial_object_2d_sptr > homog1;
vcl_vector< vsol_spatial_object_2d_sptr > homog2;

#if 0
for (int k=0;k<point.size();k++)
{
        homog1.push_back( new vsol_line_2d(new vsol_point_2d(
                pt_2b_trans1[k]->cast_to_point()->x(),pt_2b_trans1[k]->cast_to_point()->y() ),
                new vsol_point_2d(point[k]->cast_to_point()->x(),point[k]->cast_to_point()->y() ) ) );

        homog2.push_back( new vsol_line_2d(new vsol_point_2d(
                pt_2b_trans2[k]->cast_to_point()->x(),pt_2b_trans2[k]->cast_to_point()->y() ),
                new vsol_point_2d(point2[k]->cast_to_point()->x(),point2[k]->cast_to_point()->y() ) ) );


}


#endif

storage_  = vidpro1_vsol2D_storage_new();
storage2_ = vidpro1_vsol2D_storage_new();
storage_ ->add_objects( point , "Transf pt" );
// storage_ ->add_vsol_sptr( homog1, "residuals");

storage2_->add_objects(point2, "Transf pt2" );
// storage2_->add_vsol_sptr(homog2, "residuals" );
storage_ ->set_name("Homog_1");
storage2_->set_name("Homog_2");


bvis1_manager::instance()->repository()->store_data(storage_);
bvis1_manager::instance()->repository()->store_data(storage2_);
bvis1_manager::instance()->add_to_display(storage_);
bvis1_manager::instance()->add_to_display(storage2_);
bgui_vsol2D_tableau_sptr tableau_a((bgui_vsol2D_tableau *)( bvis1_manager::instance()->make_tableau(storage_).ptr() ) );
bgui_vsol2D_tableau_sptr tableau_b((bgui_vsol2D_tableau *)( bvis1_manager::instance()->make_tableau(storage2_).ptr() ) );

//      vcl_cout<<"\n"<<tableau_a<<"  "<<tableau_b<<"\n";
tableau_a->set_vsol_point_2d_style(vgui_style::new_style(1.0f, 1.0f, 0.0f, 3.0f, 1.0f));
tableau_b->set_vsol_point_2d_style(vgui_style::new_style(.5f, .5f, 0.0f, 3.0f, 1.0f));
for (int m=0;m<tableaus_vsol_.size();m++)
{
        bgui_vsol2D_tableau_sptr seltableau=(bgui_vsol2D_tableau *) (tableaus_vsol_[m].ptr());
        if ( seltableau.ptr() )
        {
                if (m%2)
                        seltableau->set_vsol_point_2d_style(vgui_style::new_style(0.4f, 0.8f, 0.2f, 3.0f, 1.0f));
                else
                        seltableau->set_vsol_point_2d_style(vgui_style::new_style(0.2f, 0.4f, 0.6f, 3.0f, 1.0f));

        }
}


bvis1_manager::instance()->display_current_frame();

tableaus_vsol_.clear();
pts_[0].clear();
pts_[1].clear();

vcl_cout<<"end tool";

pts_.clear();


active_=false;
return true;
}
//
//Takes the point locations from the storage classes(which are extracted from the
//selected tableaus. 
//Calls putative_corr function to Find the putative correspondences
//Uses homography_interface class which computes the homography
//visualizes the transfer error by transforming points from one image to the other
// according to the computed homography.
bool dbvis1_homog_view_tool::Ransac_all_pts_in_image()
{
        end_sel_flag = false;
        vcl_string      alg = "RANSAC-all pts\n";
        vcl_ofstream logfile("d://homography_log.txt", vcl_ios::out);

        vcl_cout<<alg;
 //   vcl_cout<<"\n"<<"\n";
        vidpro1_vsol2D_storage_sptr pt_2b_trans_stor1;
        pt_2b_trans_stor1.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableaus_vsol_[0] ) );
        vidpro1_vsol2D_storage_sptr pt_2b_trans_stor2;
        pt_2b_trans_stor2.vertical_cast ( bvis1_manager::instance()->storage_from_tableau( tableaus_vsol_[1] ) );


        vcl_vector< vsol_spatial_object_2d_sptr > pt_2b_trans1;
        pt_2b_trans1 = pt_2b_trans_stor1->all_data();

        vcl_vector< vsol_spatial_object_2d_sptr > pt_2b_trans2;
        pt_2b_trans2 = pt_2b_trans_stor2->all_data();



        vcl_vector< vsol_point_2d_sptr > featurept1,featurept2;


        for ( int m=0; m<pt_2b_trans1.size(); m++ )
        {
                if (pt_2b_trans1[m]->is_a()=="vsol_point_2d")
                featurept1.push_back( pt_2b_trans1[m] -> cast_to_point() );
        }
        for (int m=0;m<pt_2b_trans2.size();m++)
        {
                if (pt_2b_trans2[m]->is_a()=="vsol_point_2d")
                featurept2.push_back( pt_2b_trans2[m] -> cast_to_point() );
        }



        vcl_vector< vsol_point_2d_sptr > current_unused_points1 = featurept1;
        vcl_vector< vsol_point_2d_sptr > current_unused_points2 = featurept2;


        vcl_vector< vsol_point_2d_sptr > corrpt1;
        vcl_vector< vsol_point_2d_sptr > corrpt2;
        int num_popfound = 0;
        
        storage_=vidpro1_vsol2D_storage_new();
        storage2_=vidpro1_vsol2D_storage_new();
        linestorage_=vidpro1_vsol2D_storage_new();
        linestorage2_=vidpro1_vsol2D_storage_new();
        linestorage3_=vidpro1_vsol2D_storage_new();
        linestorage4_=vidpro1_vsol2D_storage_new();

        vcl_vector< vsol_spatial_object_2d_sptr > residual_lines1;
        vcl_vector< vsol_spatial_object_2d_sptr > residual_lines2;

        vcl_vector< vsol_spatial_object_2d_sptr > corr_lines1;
        vcl_vector< vsol_spatial_object_2d_sptr > corr_lines2;
        num_pop =2 ;
        vidpro1_vsol2D_storage_sptr inliers_11 = vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_12=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_21=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_22=vidpro1_vsol2D_storage_new();
#if 0 
                vidpro1_vsol2D_storage_sptr outliers_11=vidpro1_vsol2D_storage_new();
                vidpro1_vsol2D_storage_sptr outliers_12=vidpro1_vsol2D_storage_new();
                vidpro1_vsol2D_storage_sptr outliers_21=vidpro1_vsol2D_storage_new();
                vidpro1_vsol2D_storage_sptr outliers_22=vidpro1_vsol2D_storage_new();
#endif
        while (num_popfound<num_pop)
        {
                if (use_correlation_)
                {
                        vidpro1_image_storage_sptr image; 
                        vidpro1_image_storage_sptr image2; 
                        image.vertical_cast( images_[0] );
                        image2.vertical_cast( images_[1] );

                        vil_image_resource_sptr image_sptr =  image->get_image();
                        vil_image_resource_sptr image2_sptr = image2->get_image();
                        vil_image_view< float > corr_out;
                        vcl_cout<<"\n Current number of unused points  "<<current_unused_points1.size()<<"\n";
                        corrpt1.clear();
                        corrpt2.clear();

                        putative_correspondences (current_unused_points1, current_unused_points2, image_sptr , image2_sptr, corrpt1, corrpt2);
                        logfile<<"Original POints 1\n";
                        for (int m = 0; m<current_unused_points1.size(); m++)
                        logfile<<current_unused_points1[m]->x()<<"\t"<<current_unused_points1[m]->y()<<"\n";
                        logfile<<"Matches 1\n";
                        for (int m = 0; m<corrpt1.size(); m++)
                        logfile<<corrpt1[m]->x()<<"\t"<<corrpt1[m]->y()<<"\n";
                        logfile<<"Original POints 2\n";
                        for (int m = 0; m<current_unused_points2.size(); m++)
                        logfile<<current_unused_points2[m]->x()<<"\t"<<current_unused_points2[m]->y()<<"\n";
                        logfile<<"Matches 2\n";
                        for (int m = 0; m<corrpt2.size(); m++)
                        logfile<<corrpt2[m]->x()<<"\t"<<corrpt2[m]->y()<<"\n";
                 }
                else
                        {
                                corrpt1 = current_unused_points1;
                                corrpt2 = current_unused_points2;
                        }
                int popul = 1;
                bprt_homog_interface homogfind(corrpt1,corrpt2,8,&popul,1.0,false, true, true, use_epipolar_const_);
                homogfind.set_method((vcl_string)"muset");
                if (use_epipolar_const_)
                        homogfind.set_epipole(epip[0], epip[1]);

                if (!homogfind.compute_homog())
                {
                         vcl_cout<<"Couldn't calculate homography";
                         end_sel_flag=false;
                         break;
                }
                else
            {
            current_unused_points1.clear();
            current_unused_points2.clear();
                        homogfind.get_outlier_points(current_unused_points1, current_unused_points2);
                        vcl_vector<vsol_spatial_object_2d_sptr> outliers_so1;
                        vcl_vector<vsol_spatial_object_2d_sptr> outliers_so2;
                        for (int u = 0; u<current_unused_points1.size(); u++)
                                {
                                outliers_so1.push_back( current_unused_points1[u]->cast_to_spatial_object() );
                                outliers_so2.push_back( current_unused_points2[u]->cast_to_spatial_object() );
                                }
#if 0
                                                if (num_popfound==0)
                                                {
                                                                outliers_11->add_objects(outliers_so1,"Outliers_1");
                                                        outliers_12->add_objects(outliers_so2,"Outliers_2");
                                                }
                                                else
                                                {
                                                                outliers_21->add_objects(outliers_so1,"Outliers_1");
                                outliers_22->add_objects(outliers_so2,"Outliers_2");
                                                
                                                }
#endif  
                                num_popfound++;
                        }
                                


                vcl_vector< vsol_spatial_object_2d_sptr > pop_2b_trans1 = homogfind.get_transf_pop(1,0);
                vcl_vector< vsol_spatial_object_2d_sptr > pop_2b_trans2 = homogfind.get_transf_pop(2,0);

                
                char index[4];
                vcl_string string_index = (vcl_string)itoa(num_popfound,index,10);
                vcl_string point_group_name1(((vcl_string)"Homog_1_popul")+string_index);
                vcl_string point_group_name2(((vcl_string)"Homog_2_popul")+string_index);
                                

                vcl_vector< vsol_spatial_object_2d_sptr > orig_pop1 = homogfind.get_pop(1,0);
                vcl_vector< vsol_spatial_object_2d_sptr > orig_pop2 = homogfind.get_pop(2,0);
                if (num_popfound == 1)
                {
                
                inliers_11->add_objects(orig_pop1, "Inliers_11");
                inliers_12->add_objects(orig_pop2, "Inliers_12");
                }
                else
            if (num_popfound ==2)
                                {
                                inliers_21->add_objects(orig_pop1, "Inliers_21");
                                inliers_22->add_objects(orig_pop2, "Inliers_22");

                                }
                                logfile<<"\n*****************Residuals-----------------\n\n";
                for (int j = 0 ; j<pop_2b_trans1.size(); j++)
                {
                        residual_lines1.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans1[j]->cast_to_point(), orig_pop1[j]->cast_to_point() ) )->cast_to_spatial_object());
                        residual_lines2.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans2[j]->cast_to_point(), orig_pop2[j]->cast_to_point() ) )->cast_to_spatial_object());

                        corr_lines1.push_back(vsol_line_2d_sptr(new vsol_line_2d(orig_pop2[j]->cast_to_point(), orig_pop1[j]->cast_to_point() ) )->cast_to_spatial_object());
                        corr_lines2.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans2[j]->cast_to_point(), pop_2b_trans1[j]->cast_to_point() ) )->cast_to_spatial_object());

                        logfile<<"Residuals 1\n";\
                        logfile<<( (vsol_line_2d *)(residual_lines1[j].ptr() ) )->p0()->x()<<","<<( (vsol_line_2d *)(residual_lines1[j].ptr() ) )->p0()->y()
                                                                <<"\t"<<( (vsol_line_2d *)residual_lines1[j].ptr() )->p1()->x()<<","<<( (vsol_line_2d *)residual_lines1[j].ptr() )->p1()->y()<<"\n";
                        logfile<<"Residuals 2\n";
                        logfile<<( (vsol_line_2d *)(residual_lines2[j].ptr() ) )->p0()->x()<<","<<( (vsol_line_2d *)(residual_lines2[j].ptr() ) )->p0()->y()
                                                                <<"\t"<<( (vsol_line_2d *)residual_lines2[j].ptr() )->p1()->x()<<","<<( (vsol_line_2d *)residual_lines2[j].ptr() )->p1()->y()<<"\n";
                                                
                }
                
                storage_->add_objects(pop_2b_trans1,point_group_name1);
                storage2_->add_objects(pop_2b_trans2,point_group_name2 );
                
                                
                                
  }
  
                linestorage_->add_objects(residual_lines1, "residual1" );
                linestorage2_->add_objects(residual_lines2, "residual2" );
                linestorage3_->add_objects(corr_lines1, "corr_orig_pts" );
                linestorage4_->add_objects(corr_lines2, "corr_transf_pts" );
                
                if (use_epipolar_const_)
                {
                        storage_->add_object(new vsol_point_2d(epip[0]),"epipole" );
                        storage2_->add_object(new vsol_point_2d(epip[1]),"epipole" );
                }
                storage_->set_name("Transfer Points 1");
                storage2_->set_name("Transfer Points 2");
                
                linestorage_->set_name("Residual 1");
                linestorage2_->set_name("Residual 2");
                linestorage3_->set_name("Corr btw orig");
                linestorage4_->set_name("Corr btw transfers");
#if 0
                outliers_11->set_name((vcl_string)"Outliers_11");
                                outliers_12->set_name((vcl_string)"Outliers_12");
                                outliers_21->set_name((vcl_string)"Outliers_21");
                                outliers_22->set_name((vcl_string)"Outliers_22");
#endif
        inliers_11->set_name((vcl_string)"inliers_11");
        inliers_12->set_name((vcl_string)"inliers_12");
        inliers_21->set_name((vcl_string)"inliers_21");
        inliers_22->set_name((vcl_string)"inliers_22");
        bvis1_manager::instance()->repository()->store_data(storage_);
        bvis1_manager::instance()->repository()->store_data(storage2_);
        bvis1_manager::instance()->add_to_display(storage_);
        bvis1_manager::instance()->add_to_display(storage2_);
                bvis1_manager::instance()->repository()->store_data(linestorage_);
        bvis1_manager::instance()->repository()->store_data(linestorage2_);
        bvis1_manager::instance()->add_to_display(linestorage_);
        bvis1_manager::instance()->add_to_display(linestorage2_);
                bvis1_manager::instance()->repository()->store_data(linestorage3_);
        bvis1_manager::instance()->repository()->store_data(linestorage3_);
        bvis1_manager::instance()->add_to_display(linestorage4_);
        bvis1_manager::instance()->add_to_display(linestorage4_);
                bvis1_manager::instance()->repository()->store_data(inliers_11);
        bvis1_manager::instance()->repository()->store_data(inliers_12);
        bvis1_manager::instance()->add_to_display(inliers_11);
        bvis1_manager::instance()->add_to_display(inliers_12);
                bvis1_manager::instance()->repository()->store_data(inliers_21);
        bvis1_manager::instance()->repository()->store_data(inliers_22);
        bvis1_manager::instance()->add_to_display(inliers_21);
        bvis1_manager::instance()->add_to_display(inliers_22);
# if 0
                bvis1_manager::instance()->repository()->store_data(outliers_11);
        bvis1_manager::instance()->repository()->store_data(outliers_12);
        bvis1_manager::instance()->add_to_display(outliers_11);
        bvis1_manager::instance()->add_to_display(outliers_12);
                bvis1_manager::instance()->repository()->store_data(outliers_21);
        bvis1_manager::instance()->repository()->store_data(outliers_22);
        bvis1_manager::instance()->add_to_display(outliers_21);
        bvis1_manager::instance()->add_to_display(outliers_22);

#endif
        bgui_vsol2D_tableau_sptr tableau_a((bgui_vsol2D_tableau *)(bvis1_manager::instance()->make_tableau(storage_).ptr() ) );
        bgui_vsol2D_tableau_sptr tableau_b((bgui_vsol2D_tableau *)(bvis1_manager::instance()->make_tableau(storage2_).ptr() ) );
        //      vcl_cout<<"\n"<<tableau_a<<"  "<<tableau_b<<"\n";
   // tableau_a->set_vsol_point_2d_style(vgui_style::new_style(1.0f, 1.0f, 0.0f, 3.0f, 1.0f));
   // tableau_b->set_vsol_point_2d_style(vgui_style::new_style(.5f, .5f, 0.0f, 3.0f, 1.0f));

           for (int m=0;m<tableaus_vsol_.size();m++)
        {
                bgui_vsol2D_tableau_sptr seltableau=(bgui_vsol2D_tableau *) (tableaus_vsol_[m].ptr());
                if ( seltableau.ptr() )
                {
                        if (m%2)
                                seltableau->set_vsol_point_2d_style(vgui_style::new_style(1.0f, 1.0f, 0.0f, 3.0f, 1.0f));
                        else
                                seltableau->set_vsol_point_2d_style(vgui_style::new_style(0.6f, 0.2f, 0.8f, 3.0f, 1.0f));
                }
        }



//              bvis1_manager::instance()->regenerate_all_tableaux();
        bvis1_manager::instance()->display_current_frame();
                
        
        //                        tableau()->post_redraw();

        tableaus_vsol_.clear();

        vcl_cout<<"end tool\n";
        end_sel_flag=false;

        active_=false;
        pts_.clear();
        tableaus_vsol_.clear();

        return true;



}

bool dbvis1_homog_view_tool::Ransac_selected_pts_in_image()
{
        vcl_ofstream logfile("d://homography_log_.txt", vcl_ios::out);
        storage_=vidpro1_vsol2D_storage_new();
        storage2_=vidpro1_vsol2D_storage_new();
        linestorage_=vidpro1_vsol2D_storage_new();
        linestorage2_=vidpro1_vsol2D_storage_new();
        linestorage3_=vidpro1_vsol2D_storage_new();
        linestorage4_=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_11 = vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_12=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_21=vidpro1_vsol2D_storage_new();
        vidpro1_vsol2D_storage_sptr inliers_22=vidpro1_vsol2D_storage_new();
        vcl_vector< vsol_spatial_object_2d_sptr > residual_lines1;
        vcl_vector< vsol_spatial_object_2d_sptr > residual_lines2;

        vcl_vector< vsol_spatial_object_2d_sptr > corr_lines1;
        vcl_vector< vsol_spatial_object_2d_sptr > corr_lines2;
        end_sel_flag = false;
        vcl_string alg = "RANSAC-selectpts\n";
        vcl_vector< vsol_point_2d_sptr > featurept1,featurept2;



        for ( int m=0; m<pts_[0].size(); m++ )
           
                        featurept1.push_back( pts_[0][m]  );
        for ( int m=0; m<pts_[1].size(); m++ )
           
                         featurept2.push_back( pts_[1][m]  );
        int num_popfound = 0;
        vidpro1_image_storage_sptr image; 
        vidpro1_image_storage_sptr image2; 
        if (use_correlation_)
        {
        
        image.vertical_cast( images_[0] );
        image2.vertical_cast( images_[1] );
        }
    
    while (num_popfound<num_pop)
    {
        vcl_vector< vsol_point_2d_sptr > corrpt1;
        vcl_vector< vsol_point_2d_sptr > corrpt2;
        if (use_correlation_)
        {
            assert(images_.size()>=2);
            
            vil_image_resource_sptr image_sptr =  image->get_image();
            vil_image_resource_sptr image2_sptr = image2->get_image();
            
            putative_correspondences (featurept1,featurept2, image_sptr , image2_sptr, corrpt1, corrpt2);
        }
        
        else
        {
            corrpt1 = featurept1;
            corrpt2 = featurept2;
        }
        
        
        int popul =1;
        bprt_homog_interface homogfind(corrpt1,corrpt2,8,&popul,1.0,false,true,true,use_epipolar_const_);
        homogfind.set_method((vcl_string)"muset");
        if (use_epipolar_const_)
            homogfind.set_epipole(epip[0], epip[1]);
        
        if (!homogfind.compute_homog())
        {
            vcl_cout<<"Couldn't calculate homography";
            end_sel_flag=false;
            return false;
        }
        
        else
        {
            num_popfound++; 
            
            vcl_vector< vsol_spatial_object_2d_sptr > pop_2b_trans1 = homogfind.get_transf_pop(1,0);
            vcl_vector< vsol_spatial_object_2d_sptr > pop_2b_trans2 = homogfind.get_transf_pop(2,0);
            char index[4];
            vcl_string string_index = (vcl_string)itoa(num_popfound,index,10);
            vcl_string point_group_name1(((vcl_string)"Homog_1_popul")+string_index);
            vcl_string point_group_name2(((vcl_string)"Homog_2_popul")+string_index);
            
            
            vcl_vector< vsol_spatial_object_2d_sptr > orig_pop1 = homogfind.get_pop(1,0);
            vcl_vector< vsol_spatial_object_2d_sptr > orig_pop2 = homogfind.get_pop(2,0);
            
            if (num_popfound == 1)
            {
                inliers_11->add_objects(orig_pop1, "Inliers_11");
                inliers_12->add_objects(orig_pop2, "Inliers_12");
            }
            else
                if (num_popfound ==2)
                {
                    inliers_21->add_objects(orig_pop1, "Inliers_21");
                    inliers_22->add_objects(orig_pop2, "Inliers_22");
                    
                }
                
                for (int j = 0 ; j<pop_2b_trans1.size(); j++)
                {
                    residual_lines1.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans1[j]->cast_to_point(), orig_pop1[j]->cast_to_point() ) )->cast_to_spatial_object());
                    residual_lines2.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans2[j]->cast_to_point(), orig_pop2[j]->cast_to_point() ) )->cast_to_spatial_object());
                    
                    corr_lines1.push_back(vsol_line_2d_sptr(new vsol_line_2d(orig_pop2[j]->cast_to_point(), orig_pop1[j]->cast_to_point() ) )->cast_to_spatial_object());
                    corr_lines2.push_back(vsol_line_2d_sptr(new vsol_line_2d(pop_2b_trans2[j]->cast_to_point(), pop_2b_trans1[j]->cast_to_point() ) )->cast_to_spatial_object());
                    
                }
                
                storage_->add_objects(pop_2b_trans1,point_group_name1);
                storage2_->add_objects(pop_2b_trans2,point_group_name2 );
                
                
                if (use_epipolar_const_)
                {
                    storage_->add_object(new vsol_point_2d(epip[0]),"epipole" );
                    storage2_->add_object(new vsol_point_2d(epip[1]),"epipole" );
                }
                
        }
    }
    
    
           
        logfile<<"\n*****************Residuals-----------------\n\n";
                                
        linestorage_->add_objects(residual_lines1, "residual1" );
        linestorage2_->add_objects(residual_lines2, "residual2" );
    linestorage3_->add_objects(corr_lines1, "Corr1");
    linestorage4_->add_objects(corr_lines2, "Corr2");
                                
                                
                                
        storage_->set_name("Transfer Points 1");
        storage2_->set_name("Transfer Points 2");
                                
        linestorage_->set_name("Residual 1");
        linestorage2_->set_name("Residual 2");
        linestorage3_->set_name("Corr btw orig");
        linestorage4_->set_name("Corr btw transfers");
    storage_->set_name("Homog_1");
        storage2_->set_name("Homog_2");
        
        linestorage_->set_name("Residual_1");
        linestorage2_->set_name("Residual_2");
    linestorage3_->set_name("Corr btw orig_pts");
    linestorage4_->set_name("Corr btw trans_pts");


#if 0
        inliers_11->set_name((vcl_string)"inliers_11");
        inliers_12->set_name((vcl_string)"inliers_12");
        inliers_21->set_name((vcl_string)"inliers_21");
        inliers_22->set_name((vcl_string)"inliers_22");
#endif
        bvis1_manager::instance()->repository()->store_data(storage_);
        bvis1_manager::instance()->repository()->store_data(storage2_);
        bvis1_manager::instance()->add_to_display(storage_);
        bvis1_manager::instance()->add_to_display(storage2_);
        bvis1_manager::instance()->repository()->store_data(linestorage_);
        bvis1_manager::instance()->repository()->store_data(linestorage2_);
        bvis1_manager::instance()->add_to_display(linestorage_);
        bvis1_manager::instance()->add_to_display(linestorage2_);
        bvis1_manager::instance()->repository()->store_data(linestorage3_);
        bvis1_manager::instance()->repository()->store_data(linestorage4_);
        bvis1_manager::instance()->add_to_display(linestorage3_);
        bvis1_manager::instance()->add_to_display(linestorage4_);
#if 0
        bvis1_manager::instance()->repository()->store_data(inliers_11);
        bvis1_manager::instance()->repository()->store_data(inliers_12);
        bvis1_manager::instance()->add_to_display(inliers_11);
        bvis1_manager::instance()->add_to_display(inliers_12);
        bvis1_manager::instance()->repository()->store_data(inliers_21);
        bvis1_manager::instance()->repository()->store_data(inliers_22);
        bvis1_manager::instance()->add_to_display(inliers_21);
        bvis1_manager::instance()->add_to_display(inliers_22);
#endif


        vcl_cout<<alg<<"\n";


  if (use_epipolar_const_)
        {
        storage_->add_object(new vsol_point_2d(epip[0]) ,"epipole");
        storage2_->add_object(new vsol_point_2d(epip[1]) ,"epipole" );
        }




        bgui_vsol2D_tableau_sptr tableau_a((bgui_vsol2D_tableau *)(bvis1_manager::instance()->make_tableau(storage_).ptr() ) );
        bgui_vsol2D_tableau_sptr tableau_b((bgui_vsol2D_tableau *)(bvis1_manager::instance()->make_tableau(storage2_).ptr() ) );
        //      vcl_cout<<"\n"<<tableau_a<<"  "<<tableau_b<<"\n";
        tableau_a->set_vsol_point_2d_style(vgui_style::new_style(1.0f, 1.0f, 0.0f, 3.0f, 1.0f));
    tableau_b->set_vsol_point_2d_style(vgui_style::new_style(.5f, .5f, 0.6f, 3.0f, 1.0f));
        
        

        for (int m=0;m<tableaus_vsol_.size();m++)
        {
                bgui_vsol2D_tableau_sptr seltableau=(bgui_vsol2D_tableau *) (tableaus_vsol_[m].ptr());
                if ( seltableau.ptr() )
                {
                          if (m%2)
                                seltableau->set_vsol_point_2d_style(vgui_style::new_style(1.0f, 1.0f, 0.0f, 3.0f, 1.0f));
                        else
                                seltableau->set_vsol_point_2d_style(vgui_style::new_style(0.6f, 0.2f, 0.8f, 3.0f, 1.0f));
                }
                
        }
        //vgui_deck_tableau_sptr deck = vgui_deck_tableau_new();
        //deck->add(tableau_a);
        //deck->add(tableau_b);


         bvis1_manager::instance()->regenerate_all_tableaux();
        bvis1_manager::instance()->display_current_frame();




        tableaus_vsol_.clear();
        tableaus_im_.clear();
        pts_[0].clear();
        pts_[1].clear();


        // end_sel_flag=false;

        active_=false;

        pts_.clear();
        
        return true;




}



void dbvis1_homog_view_tool::putative_correspondences(vcl_vector<vsol_point_2d_sptr >  &featurepts1,vcl_vector<vsol_point_2d_sptr >      &featurepts2,
                                                                                                        vil_image_resource_sptr &image_sptr, vil_image_resource_sptr &image2_sptr, 
                                                                                                        vcl_vector<vsol_point_2d_sptr > &corrpt1, vcl_vector<vsol_point_2d_sptr > &corrpt2)
{

        vil_image_view< float > corr_out;
        vcl_vector<vsol_point_2d_sptr > smallgroup;
        vcl_vector<vsol_point_2d_sptr > largegroup;
        int smallnumpoint;
        int largenumpoint;
        bool change_in_order;
                int radius =3;
        if ((!image_sptr)|| (!image2_sptr))
                {
                                vcl_cout<<"Images not selected";
                                corrpt1 = featurepts1;
                                corrpt2 = featurepts2;
                                return;
                }
        //Check to see whether mistakenly the same image is selected twice

        vil_image_view< vil_rgb < vxl_byte > > img1 = image_sptr->get_view(0 , image_sptr->ni(), 0 , image_sptr->nj() );
        vil_image_view< vil_rgb < vxl_byte > > img2 = image2_sptr->get_view(0, image2_sptr->ni() , 0 , image2_sptr->nj() );
        if (vil_image_view_deep_equality (img1, img2))
        {
                vcl_cout<<"Warning two  image deep-equal";

        }



        int startnumpoint = featurepts1.size();
        int numpoint = featurepts2.size();
        if (startnumpoint>numpoint)
        {
                largegroup = featurepts1;
                smallgroup = featurepts2;
                smallnumpoint = numpoint;
                largenumpoint = startnumpoint;
                change_in_order = true;


        }
        else
        {
                 largegroup = featurepts2;
                smallgroup = featurepts1;
                smallnumpoint = startnumpoint;
                largenumpoint = numpoint;
                change_in_order = false;
        }
        int posscorr;
        float maxcorrval= 0.0f;


        for (int y=0 ; y<smallnumpoint ; ++y)
        {
                double ptlocx= smallgroup[y]->x();
                double ptlocy= smallgroup[y]->y();
                if ( ( ( ptlocx-radius)>0 ) && ( ( ptlocy-radius)>0 ) && ( (ptlocx+radius)<image_sptr->ni() )  &&  ( (ptlocy+radius)<image_sptr->nj() ) )
                {
                        vil_image_view < vxl_byte >  greyscale_view;
                        if (change_in_order)
                                greyscale_view = vil_convert_to_grey_using_rgb_weighting( image2_sptr->get_view(ptlocx-radius, 2*radius+1, ptlocy-radius,2*radius+1 ));
                        else
                                greyscale_view = vil_convert_to_grey_using_rgb_weighting( image_sptr->get_view(ptlocx-radius, 2*radius+1, ptlocy-radius, 2*radius+1));
                        vil_image_view< float > neigh;
                        vil_convert_cast( greyscale_view,neigh);

                        int countcorr=0;
                        maxcorrval =0.0;
                        posscorr = -1;


                        for ( int z=0; (z<largenumpoint); ++z )
                        {

                                double pt2locx= largegroup[z]->x();
                                double pt2locy= largegroup[z]->y();
                                if ( ( ( pt2locx-radius)>0 ) && ( ( pt2locy-radius)>0 ) && ( (pt2locx+radius)<image2_sptr->ni() )  &&  ( (pt2locy+radius)<image2_sptr->nj() ) )
                                {
                                                                                vil_image_view < vxl_byte > greyscale2_view;
                                                                                 if (change_in_order)
                                          greyscale2_view = vil_convert_to_grey_using_rgb_weighting( image_sptr->get_view(pt2locx-radius, 2*radius+1, pt2locy-radius, 2*radius+1) );
                                                                                 else
                                                                                                 greyscale2_view = vil_convert_to_grey_using_rgb_weighting( image2_sptr->get_view(pt2locx-radius, 2*radius+1, pt2locy-radius, 2*radius+1) );


                                        vil_image_view< float > neigh2;
                                        vil_convert_cast( greyscale2_view , neigh2);
                                        brip_vil_float_ops::cross_correlate(neigh , neigh2, corr_out , radius);

                                        float maxv, minv;
                                        vil_math_value_range (corr_out , minv , maxv);
                                        if (( maxv > thresh_ )&&( maxv>maxcorrval))
                                        {
                                                posscorr = z;
                                                maxcorrval = maxv;

                                  
                                        }
                                  

                                }
                        }
                        if (posscorr!=-1)
                        {
                                if (change_in_order)
                                {
                                        corrpt2.push_back( new vsol_point_2d( ptlocx , ptlocy   )  );
                                        corrpt1.push_back( new vsol_point_2d( largegroup[posscorr]->x() , largegroup[posscorr]->y() )  );
                                }
                                else
                                        {
                                        corrpt1.push_back( new vsol_point_2d( ptlocx , ptlocy   )  );
                                        corrpt2.push_back( new vsol_point_2d(  largegroup[posscorr]->x() , largegroup[posscorr]->y() )  );
                                        }
                        }

           }
   }
                        


}

void 
dbvis1_homog_view_tool::get_popup( const vgui_popup_params& params, 
                                                                         vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((use_correlation_)?on:off)+"Use correlation", 
                        bvis1_tool_toggle, (void*)(&use_correlation_) );
 // menu.add( ((end_sel_flag)?on:off)+"Selection of Pts have ended", 
 //               bvis1_tool_toggle, (void*)(&end_sel_flag) );



   
//      menu.add( "Prune by Gamma", new bvis1_prune_command(this, bvis1_prune_command::GAMMA));
//      menu.add( "Prune by Directed", new bvis1_prune_command(this, bvis1_prune_command::DIRECT));
}

void
dbvis1_homog_view_tool::deactivate()
{
#if 0
tableaus_vsol_.clear();
 pts_[0].clear();
 pts_[1].clear();
// pts_.clear();
 tableaus_im_.clear();

 active_ = false;
 images_.clear();
 delete object_;
   
#endif
}


