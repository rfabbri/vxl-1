// This is dbsksp/vis/dbsksp_soview_transform.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/vis/dbskfg_soview_transform.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/dbskfg_composite_node.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
// vgl headers
#include <vgl/vgl_area.h>
#include <vgl/vgl_distance.h>

//--------------------------------------------------------------------------
// dbskfg_soview_transform
//-------------------------------------------------------------------------


//: Constructor
dbskfg_soview_transform::
dbskfg_soview_transform(const dbskfg_transform_descriptor_sptr& transform)
    :vgui_soview2D(),
     transform_(transform)
{

    
}

//: Destructor
dbskfg_soview_transform::~dbskfg_soview_transform()
{

}

//: Render this transform node the display.
void dbskfg_soview_transform::
draw() const
{
    if ( transform_->hidden_ )
    {
        return;
    }

    if ( this->transform_->transform_type_== dbskfg_transform_descriptor::
         LOOP )
    {
        for (unsigned int s = 0; s < transform_->poly_.num_sheets(); ++s)
        { 
            glBegin(GL_LINE_LOOP);
            for (unsigned int p = 0; p < transform_->poly_[s].size(); ++p)
            {
          
                glVertex2f(transform_->poly_[s][p].x(), 
                           transform_->poly_[s][p].y());
  
            }
            glEnd();
        } 
    }
    else    
    {

        // Draw new contours spatial objects
        glBegin(GL_LINE_STRIP);

        // Loop over all parts of new contours spatial objects and record 
        // distances
        for ( unsigned int t=0; 
              t < transform_->new_contours_spatial_objects_.size() ; ++t)
        {
            if ( transform_->new_contours_spatial_objects_[t]->cast_to_curve()
                 ->cast_to_line())
            {
                // Grab line
                vsol_line_2d* line = 
                    transform_->new_contours_spatial_objects_[t]
                    ->cast_to_curve()->cast_to_line();

                vgl_point_2d<double> p0=vgl_point_2d<double>(line->p0()->x(),
                                                             line->p0()->y());
                vgl_point_2d<double> p1=vgl_point_2d<double>(line->p1()->x(),
                                                             line->p1()->y());

                glVertex2f(p0.x(),
                           p0.y());

                glVertex2f(p1.x(),
                           p1.y());

            }
            else
            {
                // Grab poly line
                vsol_polyline_2d* poly_line = 
                    transform_->new_contours_spatial_objects_[t]
                    ->cast_to_curve()->cast_to_polyline();
    

                for ( unsigned int v=0; v < poly_line->size()-1 ; ++v )
                {
                    vgl_point_2d<double> p0=
                        vgl_point_2d<double>(poly_line->vertex(v)->x(),
                                             poly_line->vertex(v)->y());
                    vgl_point_2d<double> p1=
                        vgl_point_2d<double>(poly_line->vertex(v+1)->x(),
                                             poly_line->vertex(v+1)->y());

                    glVertex2f(p0.x(),
                               p0.y());
                    
                    glVertex2f(p1.x(),
                               p1.y());
                }
            }

        }
        glEnd();
    }
}
    

//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_transform::
distance_squared(float x, float y) const
{

    vgl_point_2d<double> point(x,y);
    return vgl_distance(transform_->poly_,point);
}

//: Returns the centroid of this soview2D.
void dbskfg_soview_transform::
get_centroid(float* x, float* y) const
{

    vgl_point_2d<double> center = vgl_centroid(transform_->poly_);
    *x = center.x();
    *y = center.y();
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_transform::
print(vcl_ostream& os) const
{
    transform_->print(os);
    return os;
}
