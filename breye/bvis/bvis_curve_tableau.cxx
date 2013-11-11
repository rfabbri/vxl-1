/**************************************************************/
/*	Name: MingChing (mcchang)
//	File: bvis_curve_storage.h
//	Asgn: bvis
//	Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/
#include "bvis_curve_tableau.h"

bvis_curve_tableau::bvis_curve_tableau()
{
  gl_mode = GL_RENDER;
  curve_ = NULL;
}

bvis_curve_tableau::~bvis_curve_tableau()
{
}

bool bvis_curve_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) {
    if (gl_mode == GL_SELECT)
      draw_select();
    else
      draw_render();
  }

  return false;
}

/*
>>>>>>> 1.5
BaseGUIGeometry* bvis_curve_tableau::select( int ax , int ay )
{
  // select objects within 20 pixels of the mouse
  GLuint *ptr = vgui_utils::enter_pick_mode(ax,ay,20);

  this->gl_mode = GL_SELECT;
  this->handle(vgui_event(vgui_DRAW));
  this->gl_mode = GL_RENDER;

  int num_hits = vgui_utils::leave_pick_mode();

  // get all hits
  vcl_vector<vcl_vector<unsigned> > hits;
  vgui_utils::process_hits(num_hits, ptr, hits);

  // for each hit get the name of the soview if it is
  // being managed by this vcl_list
  //
  // Each hit from a display list has two entries. The first is the id
  // of the display list, and the second is the id of the soview. See
  // vgui_displaybase_tableau::draw_soviews_select(). Thus, an object
  // belongs to this display list iff the first hit number is this
  // list's id.

  vcl_vector<GLuint> my_hits;
  
  for (vcl_vector<vcl_vector<unsigned> >::iterator i=hits.begin();
       i != hits.end(); ++i)
  {
    vcl_vector<unsigned> const& names = *i;
    assert( names.size() == 1 );
    my_hits.push_back( names[0] );
  }

  //vcl_cout << "my_hits.size() = " << my_hits.size() << vcl_endl; 
  
  if( my_hits.size() == 1 ) {

    return (BaseGUIGeometry* ) my_hits[0];
    
  } else if( my_hits.size() > 1 ) {

    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates(ax,ay, ix,iy);

    // vcl_cout << "getting closest" << vcl_endl; 

    float smallest = FLT_MAX;
    int smallest_i = 0;
    
    for( int i = 0 ; i < my_hits.size() ; i++ ) {
    
      BaseGUIGeometry * temp = (BaseGUIGeometry *) my_hits[ i ];
      int expts_size = temp->ExPts.size();

      if( expts_size == 1 ) {
         float tempx_distance = temp->ExPts[0].x() - ix;
         float tempy_distance = temp->ExPts[0].y() - iy;
         float dist_squared = tempx_distance * tempx_distance +
                              tempy_distance * tempy_distance;
         if( dist_squared < smallest ) {
           smallest = dist_squared;
           smallest_i = i;
         }
      } else if( expts_size == 2 ) {
        
        float dist_squared = 
          vgl_distance2_to_linesegment(temp->ExPts[0].x(), 
                                       temp->ExPts[0].y(), 
                                       temp->ExPts[1].x(),
                                       temp->ExPts[1].y(),
                                       ix, 
                                       iy);
        
        if( dist_squared < smallest ) {
          smallest = dist_squared;
          smallest_i = i;
        }
      } else if( expts_size > 2 ) {

        float *x = new float [ expts_size ];
        float *y = new float [ expts_size ];

        for( int a = 0 ; a < expts_size ; a++ ) {
          x[ a ] = temp->ExPts[a].x();
          y[ a ] = temp->ExPts[a].y();
        }

        float dist_squared = 
          vgl_distance_to_non_closed_polygon( x , y , expts_size , ix , iy );

        if( dist_squared < smallest ) {
          smallest = dist_squared;
          smallest_i = i;
        }

		  delete []x;
		  delete []y;

      }
      else {
        vcl_cout << "Error" << vcl_endl;
      }

    }
    //vcl_cout<< "smallest_i: " << smallest_i << vcl_endl; 
    return (BaseGUIGeometry *) my_hits[ smallest_i ];
  }
  return NULL;
}
*/

void bvis_curve_tableau::draw_select()
{
  glPushName( 0 ); // will be replaced by ids
  /*
  if( shock_.ptr() != NULL ) {
    if( shock_->boundary() != NULL ) {
      for( BElmListIterator it =  shock_->boundary()->BElmList.begin();
          it != shock_->boundary()->BElmList.end();
          it++ ) {

        if( it->second->ExPts.size() == 1 ) {

          glLoadName((GLuint)it->second);
          
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f(it->second->ExPts[0].x() , it->second->ExPts[0].y() ); 
          glEnd();

        } else if ( it->second->ExPts.size() > 1 ) {

          glLoadName((GLuint)it->second);
          
          glBegin( GL_LINE_STRIP );
          for( int i = 0 ; i < it->second->ExPts.size() ; i++ ) {
            glVertex2f( it->second->ExPts[i].x() , it->second->ExPts[i].y() );
          }
          glEnd();
        }

      }

      for( SIElmListIterator it = shock_->SIElmList.begin();
          it != shock_->SIElmList.end();
          it++ ) {

        if( it->second->ExPts.size() == 1 ) {

          glLoadName((GLuint)it->second);
          
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f(it->second->ExPts[0].x() , it->second->ExPts[0].y() ); 
          glEnd();

        } else if ( it->second->ExPts.size() > 1 ) {

          glLoadName((GLuint)it->second);
          
          glBegin( GL_LINE_STRIP );
          for( int i = 0 ; i < it->second->ExPts.size() ; i++ ) {
            glVertex2f( it->second->ExPts[i].x() , it->second->ExPts[i].y() );
          }
          glEnd();
        }
      }
    }
  }
*/
  glPopName();
  
}

void bvis_curve_tableau::draw_render()
{
	if (curve_.ptr() != NULL ) {
      glColor3f( 0.0 , 0.0 , 1.0 );
      glBegin( GL_LINE_STRIP );
		for (int i=0; i<curve_->numPoints(); i++) {
         glVertex2f( curve_->x(i) , curve_->y(i) );
		}
		glEnd();
	}
}