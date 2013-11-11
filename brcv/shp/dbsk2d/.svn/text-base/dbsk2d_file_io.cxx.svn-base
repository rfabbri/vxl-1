// This is dbsk2d/dbsk2d_file_io.cxx

//:
// \file




#include "dbsk2d_file_io.h"
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vcl_cstring.h>
#include <dbsk2d/dbsk2d_geometry_utils.h>


//// -----------------------------------------------------------------------
////: read a `.bnd' file and output vsol2D objects
//// return false if loading fails
//bool dbsk2d_file_io::
//load_bnd_v3_0(const vcl_string& filename, 
//         vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list)
//{
//  vsol_list.clear();
//
//  //1)If file open fails, return.
//  vcl_ifstream infp(filename.c_str(), vcl_ios::in);
//
//  if (!infp) {
//    vcl_cerr << "In " __FILE__ << " Error opening file " << filename << vcl_endl;
//    return false;
//  }
//
//  float x, y, sx, sy, ex, ey;
//  float dir;
//  float r;
//  int nud;
//  int nus;
//  char buffer[2000];
//  int version =1;  //default version i.e., old .txt files
//
//  
//
//  //2)Read in each line until EOF.
//  while (infp.getline(buffer,2000)) {
//
//    if (!vcl_strncmp(buffer, "Boundary File v2.0", sizeof("Boundary File v2.0")-1))
//      version = 2;
//    else if (!vcl_strncmp(buffer, "Boundary File v3.0", sizeof("Boundary File v2.0")-1))
//      version = 3;
//
//    //BPOINT with tangent
//    else if (!vcl_strncmp(buffer, "point-tangent-at", sizeof("point-tangent-at")-1)) {
//      sscanf(buffer,"point-tangent-at: (%f %f) (%f)",&(x), &(y), &(dir));
//      
//      vsol_point_2d_sptr new_pt = new vsol_point_2d (x,y);
//      vsol_list.push_back(new_pt->cast_to_spatial_object());
//      continue;
//    }
//
//    //BPOINT
//    else if (!vcl_strncmp(buffer, "point-at", sizeof("point-at")-1)) {
//      sscanf(buffer,"point-at: (%f %f)",&(x), &(y));
//      
//      vsol_point_2d_sptr new_pt = new vsol_point_2d (x,y);
//      vsol_list.push_back(new_pt->cast_to_spatial_object());
//      continue;
//    }
//
//    //BLINE
//    else if (!vcl_strncmp(buffer, "line-from-to", sizeof("line-from-to")-1)) {
//      sscanf(buffer,"line-from-to: (%f %f) (%f %f)", &(sx), &(sy), &(ex), &(ey));
//      
//      vsol_point_2d_sptr p0 = new vsol_point_2d (sx,sy);
//      vsol_point_2d_sptr p1 = new vsol_point_2d (ex,ey);
//      vsol_line_2d_sptr new_line = new vsol_line_2d(p0, p1);
//      vsol_list.push_back(new_line->cast_to_spatial_object());
//      continue;
//    }
//
//    //BARC
//    else if (!vcl_strncmp(buffer, "arc-from-to", sizeof("arc-from-to")-1)) {
//      nus = ARC_NUS_SMALL;
//      nud = ARC_NUD_CCW;
//      if (version ==1)
//        sscanf(buffer,"arc-from-to: (%f %f) (%f %f) (%f)",
//        &(sx), &(sy), &(ex), &(ey), &r);
//      else
//        sscanf(buffer,"arc-from-to: (%f %f) (%f %f) (%f) (%d %d)", 
//        &sx, &sy, &ex, &ey, &r, &nud, &nus);
//
//      vgl_point_2d<double> c = getCenterOfArc (sx, sy, ex, ey, r, nud, nus);
//
//      // make the arc CCW
//      vgl_point_2d<double> s (sx, sy);
//      vgl_point_2d<double> e (ex, ey);
//      if (nud==ARC_NUD_CW) {
//        vgl_point_2d<double> temp = e;
//        e = s;
//        s = temp;
//        nud = ARC_NUD_CCW;
//      }
//
//
//      // parameter of conic (circle)
//      double conic_a = 1;
//      double conic_b = 0;
//      double conic_c = 1;
//      double conic_d = -2*c.x();
//      double conic_e = -2*c.y();
//      double conic_f = c.x()*c.x()+c.y()*c.y()-r*r;
//
//      //Break the big arc into 2
//      if (nus==ARC_NUS_LARGE) { //Large arc...
//
//        VECTOR_TYPE svector = _vPointPoint (c, s);
//        VECTOR_TYPE evector = _vPointPoint (c, e);
//        double angle;
//        if (nud==ARC_NUD_CCW)
//          angle = CCW (svector, evector);
//        else
//          angle = CCW (evector, svector);
//        VECTOR_TYPE mvector;
//        if (nud==ARC_NUD_CCW)
//          mvector = svector + angle/2;
//        else
//          mvector = evector + angle/2;
//        vgl_point_2d<double> m = _translatePoint (c, mvector, r);
//
//        // arc1 : (s, m, c, r, nud, ARC_NUS_SMALL)
//        // arc2 : (m, e, c, r, nud, ARC_NUS_SMALL)
//
//        vsol_conic_2d_sptr new_arc1 = 
//          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
//        new_arc1->set_p0(new vsol_point_2d(s));
//        new_arc1->set_p1(new vsol_point_2d(m));
//        vsol_list.push_back(new_arc1->cast_to_spatial_object());
//
//        vsol_conic_2d_sptr new_arc2 = 
//          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
//        new_arc2->set_p0(new vsol_point_2d(m));
//        new_arc2->set_p1(new vsol_point_2d(e));
//        vsol_list.push_back(new_arc2->cast_to_spatial_object());
//      }
//
//      else { //Small arc...
//        // arc : (s, e, c, r, nud, nus)
//        
//        vsol_conic_2d_sptr new_arc = 
//          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
//        new_arc->set_p0(new vsol_point_2d(s));
//        new_arc->set_p1(new vsol_point_2d(e));
//        vsol_list.push_back(new_arc->cast_to_spatial_object());
//      }
//      continue;
//    }
//  }//end while
//
//  //close file
//  infp.close();
//  return true;
//}

// -----------------------------------------------------------------------
//: read a `.bnd' file and output vsol2D objects
// return false if loading fails
bool dbsk2d_file_io::
load_bnd_v3_0(const vcl_string& filename, 
         vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list)
{
  vsol_list.clear();

  //1)If file open fails, return.
  vcl_ifstream infp(filename.c_str(), vcl_ios::in);

  if (!infp) {
    vcl_cerr << "In " __FILE__ << " Error opening file " << filename << vcl_endl;
    return false;
  }

  double x, y, sx, sy, ex, ey;
  double dir;
  double r;
  int nud;
  int nus;
  char buffer[2000];
  int version =1;  //default version i.e., old .txt files

  //2)Read in each line until EOF.
  while (infp.getline(buffer,2000)) {

    if (!vcl_strncmp(buffer, "Boundary File v2.0", sizeof("Boundary File v2.0")-1))
      version = 2;
    else if (!vcl_strncmp(buffer, "Boundary File v3.0", sizeof("Boundary File v2.0")-1))
      version = 3;

    //BPOINT with tangent
    else if (!vcl_strncmp(buffer, "point-tangent-at", sizeof("point-tangent-at")-1)) {
      sscanf(buffer,"point-tangent-at: (%lf %lf) (%lf)",&(x), &(y), &(dir));
      
      vsol_point_2d_sptr new_pt = new vsol_point_2d (x,y);
      vsol_list.push_back(new_pt->cast_to_spatial_object());
      continue;
    }

    //BPOINT
    else if (!vcl_strncmp(buffer, "point-at", sizeof("point-at")-1)) {
      sscanf(buffer,"point-at: (%lf %lf)",&(x), &(y));
      
      vsol_point_2d_sptr new_pt = new vsol_point_2d (x,y);
      vsol_list.push_back(new_pt->cast_to_spatial_object());
      continue;
    }

    //BLINE
    else if (!vcl_strncmp(buffer, "line-from-to", sizeof("line-from-to")-1)) {
      sscanf(buffer,"line-from-to: (%lf %lf) (%lf %lf)", &(sx), &(sy), &(ex), &(ey));
      
      vsol_point_2d_sptr p0 = new vsol_point_2d (sx,sy);
      vsol_point_2d_sptr p1 = new vsol_point_2d (ex,ey);
      vsol_line_2d_sptr new_line = new vsol_line_2d(p0, p1);
      vsol_list.push_back(new_line->cast_to_spatial_object());
      continue;
    }

    //BARC
    else if (!vcl_strncmp(buffer, "arc-from-to", sizeof("arc-from-to")-1)) {
      nus = ARC_NUS_SMALL;
      nud = ARC_NUD_CCW;
      if (version ==1)
        sscanf(buffer,"arc-from-to: (%lf %lf) (%lf %lf) (%lf)",
        &(sx), &(sy), &(ex), &(ey), &r);
      else
        sscanf(buffer,"arc-from-to: (%lf %lf) (%lf %lf) (%lf) (%d %d)", 
        &sx, &sy, &ex, &ey, &r, &nud, &nus);

      vgl_point_2d<double> c = getCenterOfArc (sx, sy, ex, ey, r, (ARC_NUD)nud, (ARC_NUS)nus);

      // make the arc CCW
      vgl_point_2d<double> s (sx, sy);
      vgl_point_2d<double> e (ex, ey);
      if (nud==ARC_NUD_CW) {
        vgl_point_2d<double> temp = e;
        e = s;
        s = temp;
        nud = ARC_NUD_CCW;
      }


      // parameter of conic (circle)
      double conic_a = 1;
      double conic_b = 0;
      double conic_c = 1;
      double conic_d = -2*c.x();
      double conic_e = -2*c.y();
      double conic_f = c.x()*c.x()+c.y()*c.y()-r*r;

      //Break the big arc into 2
      if (nus==ARC_NUS_LARGE) { //Large arc...

        VECTOR_TYPE svector = _vPointPoint (c, s);
        VECTOR_TYPE evector = _vPointPoint (c, e);
        double angle;
        if (nud==ARC_NUD_CCW)
          angle = CCW (svector, evector);
        else
          angle = CCW (evector, svector);
        VECTOR_TYPE mvector;
        if (nud==ARC_NUD_CCW)
          mvector = svector + angle/2;
        else
          mvector = evector + angle/2;
        vgl_point_2d<double> m = _translatePoint (c, mvector, r);

        // arc1 : (s, m, c, r, nud, ARC_NUS_SMALL)
        // arc2 : (m, e, c, r, nud, ARC_NUS_SMALL)

        vsol_conic_2d_sptr new_arc1 = 
          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
        new_arc1->set_p0(new vsol_point_2d(s));
        new_arc1->set_p1(new vsol_point_2d(m));
        vsol_list.push_back(new_arc1->cast_to_spatial_object());

        vsol_conic_2d_sptr new_arc2 = 
          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
        new_arc2->set_p0(new vsol_point_2d(m));
        new_arc2->set_p1(new vsol_point_2d(e));
        vsol_list.push_back(new_arc2->cast_to_spatial_object());
      }

      else { //Small arc...
        // arc : (s, e, c, r, nud, nus)
        
        vsol_conic_2d_sptr new_arc = 
          new vsol_conic_2d(conic_a, conic_b, conic_c, conic_d, conic_e, conic_f);
        new_arc->set_p0(new vsol_point_2d(s));
        new_arc->set_p1(new vsol_point_2d(e));
        vsol_list.push_back(new_arc->cast_to_spatial_object());
      }
      continue;
    }
  }//end while

  //close file
  infp.close();
  return true;
}

// -----------------------------------------------------------------------
//: saves a list of vsol2D objects into a .bnd file
// Only handle points, lines, and arcs and ignore the rest
bool dbsk2d_file_io::
save_bnd_v3_0(const vcl_string& filename, 
              const vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list)
{
  //1)If file open fails, return.
  vcl_ofstream outfp(filename.c_str(), vcl_ios::out);

  if (!outfp){
    vcl_cerr << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  //: \todo we don't have height or width values
  // these are arbitrary
  int height= -1;
  int width= -1;

  // output header information
  outfp <<"Boundary File v3.0"<<vcl_endl;
  outfp <<"width: "<< width <<vcl_endl;
  outfp <<"height: "<< height <<vcl_endl;
  outfp <<"number-of-elements: "<< -1 <<vcl_endl; //: \todo this is not correct

  outfp.precision (15);

  // parse through all the vsol classes and save curve objects only
  //traverse the list again and output the data
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    // POINTS
    if( vsol_list[b]->cast_to_point() )
    {
      vsol_point_2d_sptr pt = vsol_list[b]->cast_to_point();
      outfp << "point-at: (" << pt->x() << " " << pt->y() << ")" << vcl_endl;
    }
    else if( vsol_list[b]->cast_to_curve())
    {
      vsol_curve_2d_sptr curve2d = vsol_list[b]->cast_to_curve();
      // SINGLE LINE SEGMENT
      if( curve2d->cast_to_line() )
      {
        vsol_point_2d_sptr p0 = curve2d->cast_to_line()->p0();
        vsol_point_2d_sptr p1 = curve2d->cast_to_line()->p1();

        outfp <<"line-from-to: (" << p0->x() << " " << p0->y() << ") (" << 
          p1->x() << " " << p1->y() << ")" << vcl_endl;
      }
      // POLYLINE
      else if( curve2d->cast_to_polyline() )
      {
        vsol_polyline_2d_sptr polyline = curve2d->cast_to_polyline();
        //first pt
        vsol_point_2d_sptr lpt = polyline->vertex(0);
        //the rest of them
        for (unsigned int i=1; i<polyline->size(); ++i)
        {
          vsol_point_2d_sptr cpt = polyline->vertex(i);
          outfp <<"line-from-to: (" << lpt->x() << " " << lpt->y() << ") (" << cpt->x() << " " << cpt->y() << ")" << vcl_endl;
          lpt = cpt;
        }
      }
      // CIRCULAR ARC
      else if (curve2d->cast_to_conic())
      {
        if (curve2d->cast_to_conic()->is_real_circle())
        {
          vsol_conic_2d_sptr conic = curve2d->cast_to_conic();
          vsol_point_2d_sptr p1 = conic->p0();
          vsol_point_2d_sptr p2 = conic->p1();
          double xc, yc, major_axis, minor_axis, phi;
          conic->ellipse_parameters(xc, yc, phi, major_axis, minor_axis);
          
          // we always assume small (<half circle) arc
          vgl_point_2d<double > cir_center(xc, yc);
          int nud = -1;
          if (cross_product<double >(p1->get_p()-cir_center, 
            p2->get_p()-cir_center) < 0) 
          {
            nud = 1;
          }
          outfp <<"arc-from-to: (" << p1->x() << " " << p1->y() << ") (" << 
            p2->x() << " " << p2->y() << ") (" << major_axis << ") (" <<
            nud << "  " << 1 << ")" << vcl_endl;
        }
      }
    }
    // POLYGON
    else if( vsol_list[b]->cast_to_region() )
    {
      if (vsol_list[b]->cast_to_region()->cast_to_polygon())
      {
        vsol_polygon_2d_sptr polygon = 
          vsol_list[b]->cast_to_region()->cast_to_polygon();
      
        unsigned int len = polygon->size();

        //first pt
        vsol_point_2d_sptr fpt = polygon->vertex(0);
        vsol_point_2d_sptr lpt = fpt;
        
        //the rest of them
        for (unsigned int i=1; i<len;i++)
        {
          vsol_point_2d_sptr cpt = polygon->vertex(i);
          outfp <<"line-from-to: (" << lpt->x() << " " << lpt->y() << ") (" <<
            cpt->x() << " " << cpt->y() << ")" << vcl_endl;
          lpt = cpt;
        }
        //last line
        vsol_point_2d_sptr cpt = polygon->vertex(len-1);
        outfp <<"line-from-to: (" << cpt->x() << " " << cpt->y() << ") (" << 
          fpt->x() << " " << fpt->y() << ")" << vcl_endl;
      }
    }
  }

  //close file
  outfp.close();
  return true;

}









