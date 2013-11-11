//this is /contrib/bm/dbcl/dbcl_linear_classifier_vrml_view.cxx

#include"dbcl_linear_classifier_vrml_view.h"

void draw_decision_boundaries(dvrml& dvrml_out ,vcl_vector<dbcl_decision_boundary>& decision_boundary_list)
{
            vcl_vector<dbcl_decision_boundary>::iterator dbl_itr;
            vcl_vector<dbcl_decision_boundary>::iterator dbl_end = decision_boundary_list.end();

            vcl_vector<vgl_point_3d<double> > point_list;
            vcl_vector< vcl_vector<unsigned> > connection_list;

            unsigned cnt = 0;
            for( dbl_itr = decision_boundary_list.begin(); dbl_itr != dbl_end; ++dbl_itr )
            {
                point_list.push_back(dbl_itr->line.point1());
                point_list.push_back(dbl_itr->line.point2());
                vcl_vector<unsigned> inner_list;
                vgl_point_3d<double> endpt1(dbl_itr->line.point1());
                inner_list.push_back(cnt);
                ++cnt;
                vgl_point_3d<double> endpt2(dbl_itr->line.point2());
                inner_list.push_back(cnt);
                connection_list.push_back(inner_list);
                ++cnt;
            }//end decision boundary iteration

            dvrml_indexed_lineset_sptr lineset_sptr = new dvrml_indexed_lineset(point_list,connection_list);
            dvrml_out.draw(lineset_sptr.as_pointer());
}//end draw decision boundaries

void dbcl_linear_classifier_vrml_view::vrml_view(vcl_ofstream& os, dbcl_classifier_sptr classifier_sptr)
{
    dvrml dvrml_out(os);

    dbcl_linear_classifier_sptr linear_classifier_sptr = dynamic_cast<dbcl_linear_classifier*>(classifier_sptr.as_pointer());

    vnl_matrix<double> w = linear_classifier_sptr->W().transpose();

    assert(w.cols() == 3 || w.cols() == 4);

    if(w.rows() == 4)
    {}// dbcl_linear_classifier_vrml_view::vrml_view_3d(os,classifier_sptr);
    else
    {

        unsigned num_classes = linear_classifier_sptr->T().cols();

        dbcl_classifier::feature_map_type training_data = classifier_sptr->training_data();
        dbcl_classifier::feature_map_type test_data = classifier_sptr->test_data();

        dbcl_classifier::feature_map_type::iterator training_data_itr, test_data_itr;
        dbcl_classifier::feature_map_type::iterator training_data_end = training_data.end();
        dbcl_classifier::feature_map_type::iterator test_data_end = test_data.end();

        vcl_map<unsigned, dvrml_appearance_sptr> class_appearance_map;
        for( unsigned i = 0; i < num_classes; ++i )
        {
            vnl_vector_fixed<double,3> color = dvrml::color_map(double(i)/num_classes);
            class_appearance_map[i] = new dvrml_appearance(color);
        }

        double xmax, xmin, ymax, ymin;
        dvrml_point_sptr point_sptr = new dvrml_point;
        //dvrml_shape_sptr shape_sptr = new dvrml_shape;
        //START HERE!
        xmax = training_data.begin()->second->feature_vector(0);
        xmin = training_data.begin()->second->feature_vector(0);
        ymax = training_data.begin()->second->feature_vector(1);
        ymin = training_data.begin()->second->feature_vector(1);
        for( training_data_itr = training_data.begin(); training_data_itr != training_data_end; ++training_data_itr )
        {    
            double current_x = training_data_itr->second->feature_vector(0);
            double current_y = training_data_itr->second->feature_vector(1);
            if( current_x > xmax ){ xmax = current_x; }
            if( current_x < xmin ){ xmin = current_x; }
            if( current_y > ymax ){ ymax = current_y; }
            if( current_y < ymin ){ ymin = current_y; }
            vgl_point_3d<double> center(training_data_itr->second->feature_vector(0),training_data_itr->second->feature_vector(1),0.0);
            point_sptr->center = center;
            point_sptr->appearance_sptr = class_appearance_map[training_data_itr->second->label()]; 
            point_sptr->radius = 0.75;
            dvrml_out.draw(point_sptr.as_pointer());
        }//end training_data iteration

        dvrml_box_sptr box_sptr = new dvrml_box;
        for( test_data_itr = test_data.begin(); test_data_itr != test_data_end; ++test_data_itr )
        {
            double current_x = test_data_itr->second->feature_vector(0);
            double current_y = test_data_itr->second->feature_vector(1);
            if( current_x > xmax ){ xmax = current_x; }
            if( current_x < xmin ){ xmin = current_x; }
            if( current_y > ymax ){ ymax = current_y; }
            if( current_y < ymin ){ ymin = current_y; }
            vgl_point_3d<double> center(test_data_itr->second->feature_vector(0),test_data_itr->second->feature_vector(1),0.0);
            box_sptr->center = center;
            box_sptr->appearance_sptr = class_appearance_map[test_data_itr->second->label()];
            dvrml_out.draw(box_sptr.as_pointer());
        }//end test_data iteration

        xmax += 10;
        xmin -= 10;
        ymax += 10;
        ymin -= 10;

        vgl_box_2d<double> view(xmin,xmax,ymin,ymax);

        vcl_vector<dbcl_decision_boundary > decision_boundary_list;

        //DRAW DECISION BOUNDARIES there will be a decision boundary between every class ==> num_classes(num_classes-1)/2 boundaries
        for( unsigned i = 0; i < num_classes - 1; ++i )
        {
            for( unsigned k = i + 1; k < num_classes ; ++k )
            {
                double bias_weight_difference = w(i,0) - w(k,0);
                double x_weight_difference = w(i,1) - w(k,1);
                double y_weight_difference = w(i,2) - w(k,2);

                vgl_point_3d<double> endpoint1, endpoint2;

                //double x2 = x_weight_difference * xmax + bias_weight_difference;
                //double y2 = y_weight_difference * ymax + bias_weight_difference;

                //double x1 = x_weight_difference * xmin + bias_weight_difference;
                //double y1 = y_weight_difference * ymin + bias_weight_difference;

                double y1 = ymin;
                double x1 = (-(y_weight_difference*y1) - bias_weight_difference)/x_weight_difference;
               
                double y2 = ymax;
                double x2 = (-(y_weight_difference*y2) - bias_weight_difference)/x_weight_difference;
               
                if( !view.contains(x1,y1) )
                {
                    x1 = xmin;
                    y1 = ((-x_weight_difference*x1) - bias_weight_difference)/y_weight_difference;
                    x2 = xmax;
                    y2 = ((-x_weight_difference*x2) - bias_weight_difference)/y_weight_difference;
                }
                    
             
                endpoint1.set(x1,y1,0.0);
                endpoint2.set(x2,y2,0.0);

                dbcl_decision_boundary boundary(endpoint1,endpoint2,i,k);
    
                decision_boundary_list.push_back(boundary);
            }
        }

        //if there are more than 2 classes, find the decision boundary intersection point (there should be one, this classifier
        //has a singly connected, convex decision boundary)
        vgl_point_3d<double> boundary_intersection;
        bool intersection_in_view = false;
        if( decision_boundary_list.size() > 2 )
        {
            for( unsigned i = 0; i < num_classes - 1; ++i )
            {
                for( unsigned k = i + 1; k < num_classes; ++k )
                {
                    if( vgl_intersection<double>(decision_boundary_list[i].line,decision_boundary_list[k].line,boundary_intersection) )
                        intersection_in_view = true;

                    if(intersection_in_view == true)
                        break;    
                }
                if( intersection_in_view == true )
                    break;
            }
        }

        //if the intersection is in view, then the intersection point becomes the locus from which all decision boundaries eminate
        if( intersection_in_view == true )
        {

            dbcl_classifier::feature_map_type training_data = classifier_sptr->training_data();
            
            vcl_vector<dbcl_decision_boundary> pruned_decision_boundary_list;

            vcl_vector<vgl_point_3d<double> > point_list;
            vcl_vector<vcl_vector<unsigned> > connection_list;

            vcl_vector<dbcl_decision_boundary>::iterator dbl_itr;
            vcl_vector<dbcl_decision_boundary>::iterator dbl_end = decision_boundary_list.end();

            // need to break every line in half and connect to the intersection point
            // intersection point will be the locus from which boundaries eminate.
            for( dbl_itr = decision_boundary_list.begin(); dbl_itr != dbl_end; ++dbl_itr )
            {
                dbcl_decision_boundary half_decision_boundary1, half_decision_boundary2;

                half_decision_boundary1.line.set( boundary_intersection,dbl_itr->line.point1() );
                half_decision_boundary1.top_label = dbl_itr->top_label;
                half_decision_boundary1.bottom_label = dbl_itr->bottom_label;

                half_decision_boundary2.line.set( boundary_intersection,dbl_itr->line.point2() );
                half_decision_boundary2.top_label = dbl_itr->top_label;
                half_decision_boundary2.bottom_label = dbl_itr->bottom_label;

                //accumulate the distance from each half to the training points from the two classes
                //the half which the distance is minimized is the half to draw.
                dbcl_classifier::feature_map_type::iterator training_itr;
                dbcl_classifier::feature_map_type::iterator training_end = training_data.end();

                double distance1 = 0.0;
                double distance2 = 0.0;
                for( training_itr = training_data.begin(); training_itr != training_end; ++training_itr )
                {
                    if( training_itr->second->label() == dbl_itr->top_label || training_itr->second->label() == dbl_itr->bottom_label )
                    {
                        vgl_point_3d<double> training_point(training_itr->second->feature_vector(0),training_itr->second->feature_vector(1),0.0);
                        distance1 += vgl_distance(training_point,half_decision_boundary1.line);
                        distance2 += vgl_distance(training_point,half_decision_boundary2.line);
                    }
                }//end training iteration

                if( distance1 <= distance2 )
                    pruned_decision_boundary_list.push_back(half_decision_boundary1);
                else
                    pruned_decision_boundary_list.push_back(half_decision_boundary2);

            }//end decision_boundary iteration

            draw_decision_boundaries(dvrml_out,pruned_decision_boundary_list);



        }
        else //just draw the decision boundaries
        {
            draw_decision_boundaries(dvrml_out,decision_boundary_list);
        }//end else intersection not in view

    }//end else 2d draw
}//end dbcl_linear_classifier_vrml_view::vrml_view()