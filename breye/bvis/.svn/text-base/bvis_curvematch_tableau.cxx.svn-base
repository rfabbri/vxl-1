/**************************************************************/
/*	Name: MingChing (mcchang)
//	File: bvis_curvematch_tableau.h
//	Asgn: bvis
//	Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/
#include "bvis_curvematch_tableau.h"

bvis_curvematch_tableau::bvis_curvematch_tableau()
{
  gl_mode = GL_RENDER;
  active_tool_sptr = NULL;

  curvematch_ = NULL;
}

bool bvis_curvematch_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) {
    if (gl_mode == GL_SELECT)
      draw_select();
    else
      draw_render();
  }

  if( active_tool_sptr.ptr() != NULL ) {
    active_tool_sptr->handle( e );
  }

  return false;
}

void bvis_curvematch_tableau::draw_select()
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

#define OX +0.5
#define OY +0.5

void DrawCurveMatching (curve_2d_sptr curve1, curve_2d_sptr curve2, FinalMapType* fmap, 
								double OffsetX1, double OffsetY1, double OffsetX2, double OffsetY2,
								bool bShowMatchingLines)
{
	int i;
	int indexP1, indexP2, index;

	#define COLORARRAYSIZE 12
	double ColorArray[][3] = {
		{1,0,0}, {1,0.5,0}, {1,1,0}, {0.5,1,0}, {0,1,0}, {0,1,0.5}, {0,1,1}, {0,0.5,1}, {0,0,1},
		{0.5,0,1}, {1,0,1}, {1,0,0.5}
	};

	///curve1group->eraseAllChildren(); //pushStyle (Hidden(true));
	///curve2group->eraseAllChildren(); //pushStyle (Hidden(true));
	///matchgroup->eraseAllChildren(); //pushStyle (Hidden(true));

	//1)Draw points on the curve 1
	for (i=0; i<curve1->numPoints(); i++) {
          glColor3f( 0.0 , 1.0 , 0.0 );
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f (curve1->x(i)+OffsetX1+OX, curve1->y(i)+OffsetY1+OY);
          glEnd();
	}

	//2)Draw points on the curve 2
	for (i=0; i<curve2->numPoints(); i++) {
          glColor3f( 0.0 , 0.0 , 1.0 );
          glPointSize( 3.0 );
          glBegin( GL_POINTS );
          glVertex2f (curve2->x(i)+OffsetX2+OX, curve2->y(i)+OffsetY2+OY);
          glEnd();
	}

	//3)Draw the matching on curve 1
	int size = (*fmap).size();
	int prev_index1 = (*fmap)[size-1].first;

	for (i=size-2; i>=0; i--) {
		int index1 = (*fmap)[i].first;

		int step = index1 - prev_index1;
		if (step==1) {
			//Matching line on curve 1
			double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
			double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
			double pt2x = curve1->x(index1)+OffsetX1+OX;
			double pt2y = curve1->y(index1)+OffsetY1+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
			glEnd();
		}
		else if (step==2) {
			int indexm = prev_index1+1;
			double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
			double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
			double pt2x = curve1->x(indexm)+OffsetX1+OX;
			double pt2y = curve1->y(indexm)+OffsetY1+OY;
			double pt3x = curve1->x(index1)+OffsetX1+OX;
			double pt3y = curve1->y(index1)+OffsetY1+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
	      glVertex2f( pt3x , pt3y );
			glEnd();
		}
		else if (step==3) {
			int index2 = prev_index1+1;
			int index3 = prev_index1+2;
			double pt1x = curve1->x(prev_index1)+OffsetX1+OX;
			double pt1y = curve1->y(prev_index1)+OffsetY1+OY;
			double pt2x = curve1->x(index2)+OffsetX1+OX;
			double pt2y = curve1->y(index2)+OffsetY1+OY;
			double pt3x = curve1->x(index3)+OffsetX1+OX;
			double pt3y = curve1->y(index3)+OffsetY1+OY;
			double pt4x = curve1->x(index1)+OffsetX1+OX;
			double pt4y = curve1->y(index1)+OffsetY1+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
	      glVertex2f( pt3x , pt3y );
			glEnd();
		}

		prev_index1 = index1;
	}

	//4)Draw the matching on curve 2
	int prev_index2 = (*fmap)[size-1].second;

	for (i=size-2; i>=0; i--) {
		int index1 = (*fmap)[i].first;
		int index2 = (*fmap)[i].second;

		int step = index2 - prev_index2;
		if (step==1) {
			double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
			double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
			double pt2x = curve2->x(index2)+OffsetX2+OX;
			double pt2y = curve2->y(index2)+OffsetY2+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
			glEnd();
		}
		else if (step==2) {
			int indexm = prev_index2+1;
			double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
			double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
			double pt2x = curve2->x(indexm)+OffsetX2+OX;
			double pt2y = curve2->y(indexm)+OffsetY2+OY;
			double pt3x = curve2->x(index2)+OffsetX2+OX;
			double pt3y = curve2->y(index2)+OffsetY2+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
	      glVertex2f( pt3x , pt3y );
			glEnd();
		}
		else if (step==3) {
			int indexm = prev_index2+1;
			int indexn = prev_index2+2;
			double pt1x = curve2->x(prev_index2)+OffsetX2+OX;
			double pt1y = curve2->y(prev_index2)+OffsetY2+OY;
			double pt2x = curve2->x(indexm)+OffsetX2+OX;
			double pt2y = curve2->y(indexm)+OffsetY2+OY;
			double pt3x = curve2->x(indexn)+OffsetX2+OX;
			double pt3y = curve2->y(indexn)+OffsetY2+OY;
			double pt4x = curve2->x(index2)+OffsetX2+OX;
			double pt4y = curve2->y(index2)+OffsetY2+OY;

			glColor3f (ColorArray[index1%COLORARRAYSIZE][0],
							ColorArray[index1%COLORARRAYSIZE][1],
							ColorArray[index1%COLORARRAYSIZE][2]);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
	      glVertex2f( pt3x , pt3y );
	      glVertex2f( pt4x , pt4y );
			glEnd();
		}

		prev_index2 = index2;
	}

	//5)Draw the matching lines
	if (bShowMatchingLines) {
		for (i=0; i<(*fmap).size(); i++) {
			indexP1 = (*fmap)[i].first;
			indexP2 = (*fmap)[i].second;
			double pt1x = curve1->point(indexP1).x()+OffsetX1+OX;
			double pt1y = curve1->point(indexP1).y()+OffsetY1+OY;
			double pt2x = curve2->point(indexP2).x()+OffsetX2+OX;
			double pt2y = curve2->point(indexP2).y()+OffsetY2+OY;

			glColor3f (0.5, 0.5, 0.5);
	      glBegin( GL_LINE_STRIP );
	      glVertex2f( pt1x , pt1y );
	      glVertex2f( pt2x , pt2y );
			glEnd();
		}
	}
}

void bvis_curvematch_tableau::draw_render()
{
	if (curvematch_.ptr() != NULL) {
		DrawCurveMatching (curvematch_->curve1(), curvematch_->curve2(), curvematch_->finalMap(),
			0, 0, 100, 0, true);
	}
}

