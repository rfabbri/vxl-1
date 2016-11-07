// This is dbmsh3d/algo/dbmsh3d_xform.cxx
//:
// \file

#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vul/vul_printf.h>
#include <vcl_fstream.h>

#include <dbmsh3d/algo/dbmsh3d_xform.h>


//: Read the transformation file (xx-af.txt) to a vgl_h_matriz_3d<double>
bool dbmsh3d_read_xform_file (const vcl_string& file, vgl_h_matrix_3d<double>& H)
{
  if (vul_file::exists(file) == false)
    return false;

  vcl_ifstream  in;
  in.open (file.c_str());

  ///vcl_ifstream xform_str(file.c_str(), vcl_ios_in);
  vcl_string    linestr;
  vcl_getline (in, linestr);
  vcl_getline (in, linestr);

  vnl_matrix<double > m;
  m.read_ascii (in);
  //verify matrix size
  if (m.rows() != 4 || m.cols() != 4) {
    //vcl_cerr << "Wrong matrix size. The transformation matrix should be 4x4.\n";
    return false;
  }  
  
  vcl_cout << "Loading Polyworks xform matrix file " << file << " ...\n";
  m.print (vcl_cout);
  
  //create homography matrix
  H.set (m);
  in.close();
  return true;
}

//: Write a homography matrix vgl_h_matriz_3d<double> to a transformation file (xx-af.txt)
bool dbmsh3d_write_xform_file (const vcl_string& file, vgl_h_matrix_3d<double>& H)
{
  FILE* fp;
  if ((fp = vcl_fopen (file.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "  can't open output xform file %s\n", file.c_str());
    return false; 
  }
  vcl_cout << "  writing Polyworks xform matrix file: " << file.c_str() << " ...\n";

  vcl_fprintf (fp, "VERSION  =  1\n");
  vcl_fprintf (fp, "MATRIX  =\n");
  

  for (unsigned i=0; i<H.get_matrix().rows(); ++i) {
    for (unsigned j=0; j<H.get_matrix().columns(); ++j)
      vcl_fprintf (fp, "%.16f  ", H.get(i, j));
    vcl_fprintf (fp, "\n");
  }

  fclose (fp);
  return true;
}

//: Read the alignments from list file and compute final xform
bool dbmsh3d_read_xform_listfile (const vcl_string& listfile, vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "dbmsh3d_read_xform_listfile(): %s.\n", listfile.c_str());

  FILE* fp;
  if ((fp = vcl_fopen (listfile.c_str(), "r")) == NULL) {
    vul_printf (vcl_cout, "Can't open listfile %s\n", listfile.c_str());
    return false; 
  }

  //Read in each filename in the list.
  int ret;
  int count = 0;
  do {
    char file[256];
    ret = vcl_fscanf (fp, "%s\n", file);
    if (ret != EOF) {
      vgl_h_matrix_3d<double> h;
      dbmsh3d_read_xform_file (file, h);
      H = h * H;
      count++;
    }
  }
  while (ret != EOF);

  vul_printf (vcl_cout, "\t%d h_matrix xforms done from %s.\n", count, listfile.c_str());
  fclose (fp);
  return true;
}

//: Perform in-place rigid transformation on 3D points
bool dbmsh3d_apply_xform (vcl_vector<vgl_point_3d<double> >& pts, 
                          const vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "  dbmsh3d_apply_xform on pts: %u points", pts.size());

  //Iterate thru all the points in M and compute its image after transformation
  for (unsigned int i=0; i<pts.size(); i++) {
    vgl_homg_point_3d<double > Ph (pts[i]);
    vgl_homg_point_3d<double > xPh = H (Ph);
    double vx, vy, vz;
    if (!xPh.get_nonhomogeneous(vx, vy, vz)) {
      vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
      return false;
    }
    pts[i].set(vx, vy, vz);
  }
  vcl_cout << "  done.\n";
  return true;
}

bool dbmsh3d_apply_xform (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oripts, 
                      const vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "  dbmsh3d_apply_xform on oripts: %u points", oripts.size());

  // iterate thru all the points and compute its image after transformation
  double vx, vy, vz, nx, ny, nz;

  for (unsigned int i=0; i<oripts.size(); i++) {
    vgl_point_3d<double> P = oripts[i].first;
    vgl_vector_3d<double> N = oripts[i].second;

    //Transform the point
    vgl_homg_point_3d<double> Ph (P);
    vgl_homg_point_3d<double> xPh = H (Ph);
    
    if (!xPh.get_nonhomogeneous(vx, vy, vz)) {
      vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
      return false;
    }
    oripts[i].first.set (vx, vy, vz);

    //Transform the normal
    vgl_homg_point_3d<double> Ph2 (P+N);
    vgl_homg_point_3d<double> xPh2 = H (Ph2);
    if (!xPh2.get_nonhomogeneous(nx, ny, nz)) {
      vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
      return false;
    }
    oripts[i].second.set (nx-vx, ny-vy, nz-vz);
  }
  vcl_cout << "  done.\n";
  return true;
}


//: Perform in-place rigid transformation on 3D points
bool dbmsh3d_apply_xform (dbmsh3d_pt_set* PS, const vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "  dbmsh3d_apply_xform on PS: %u points", PS->vertexmap().size());

  //Iterate thru all the points in M and compute its image after transformation
  vcl_map<int, dbmsh3d_vertex*>::iterator it = PS->vertexmap().begin();
  for (; it != PS->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;

    vgl_homg_point_3d<double > Ph (V->pt());
    vgl_homg_point_3d<double > xPh = H (Ph);
    double vx, vy, vz;
    if (!xPh.get_nonhomogeneous(vx, vy, vz)) {
      vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
      return false;
    }
    V->get_pt().set(vx, vy, vz);
  }
  vcl_cout << "  done.\n";
  return true;
}

bool dbmsh3d_apply_xform (dbmsh3d_sg3pi* sg3pi, const vgl_h_matrix_3d<double>& H)
{
  vcl_cout << "dbmsh3d_apply_xform on sg3pi: "; //transform the SG 3PI scan.

  //Iterate thru all the points in SG and compute its image after transformation
  for (unsigned int i=0; i<sg3pi->data().size(); i++) {
    for (unsigned int j=0; j<sg3pi->data(i).size(); j++) {
      dbmsh3d_sg3pi_pt* SP = sg3pi->get_data(i,j);

      vgl_homg_point_3d<double > Ph (SP->pt());
      vgl_homg_point_3d<double > xPh = H (Ph);
      double vx, vy, vz;
      if (!xPh.get_nonhomogeneous(vx, vy, vz)) {
        vcl_cerr << "Error in " << __FILE__ << " : Pt at infinity\n";
        return false;
      }
      SP->pt().set (vx, vy, vz);
    }
  }
  vcl_cout << "  done.\n";
  return true;
}

