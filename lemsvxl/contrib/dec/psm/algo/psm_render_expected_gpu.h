#ifndef psm_render_expected_gpu_h_
#define psm_render_expected_gpu_h_


#include <GL/glew.h>
#include <GL/glut.h>

#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_save.h>


#include "psm_parallel_raytrace_function.h"

const GLchar* vertex_shader_src[] = {
  "uniform vec3 cam_center;"
  "attribute float cell_alpha_attrib;"
  "varying float cell_alpha;"
  "varying float cam_dist;"
  "void main()"
  "{"
  "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  "  gl_FrontColor = gl_Color;"
  "  cell_alpha = cell_alpha_attrib;"
  "  cam_dist = distance(gl_Vertex.xyz, cam_center);"
  "}"
};

const GLchar* fragment_shader_src[] = {
  "uniform sampler2D expected_in_sampler, data_in_sampler;"
  "const int color_out_idx = 0;"
  "const int expected_out_idx = 1;"
  "const int data_out_idx = 2;"
  "varying float cell_alpha;"
  "varying float cam_dist;"
  ""
  "void main()"
  "{"
  "  vec2 tex_coord = vec2(gl_FragCoord.x/1280, gl_FragCoord.y/720);"
  "  vec4 expected_in = texture2D(expected_in_sampler, tex_coord);"
  "  vec4 data_in = texture2D(data_in_sampler, tex_coord);"
  "  float prev_dist = data_in.r;"
  "  float alpha_int = data_in.g;"
  "  float vis_start = data_in.b;"
  "  float seg_len = 0.0;"
  "  if (prev_dist > 0.0) { "
  "    seg_len = cam_dist - prev_dist;"
  "    alpha_int += seg_len * cell_alpha;"
  "  }"
  "  float vis_end = exp(-alpha_int);"
  "  float weight = vis_start - vis_end;"
  ""
  "  gl_FragData[data_out_idx] = vec4(cam_dist, alpha_int, vis_end, 1.0);"
  "  gl_FragData[color_out_idx] = vec4(gl_Color.r, gl_Color.g, gl_Color.b, cell_alpha);"
  "  gl_FragData[expected_out_idx].rgb = expected_in.rgb + weight*gl_Color.rgb;"
  "  gl_FragData[expected_out_idx].a = vis_end;"
  "}"
};


template <psm_apm_type APM>
void psm_render_expected(psm_scene<APM> &scene, vpgl_perspective_camera<double> const& cam, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> &expected, vil_image_view<float> &mask)
{
  // initialize OpenGL context
  // create empty input arguments
  int glut_argc = 1;
  char *glut_argv[] = {"psm_render_expected"};
  glutInit(&glut_argc, glut_argv);
  int main_window = glutCreateWindow("Render Expected");
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    vcl_cerr << "Error: " << glewGetErrorString(err) << vcl_endl;
    return;
  }
  if (!GLEW_VERSION_2_1) {
    vcl_cerr << "error: need OpenGL 2.1 compatibility for GPU optimizations." << vcl_endl;
    return;
  }
  if (!GLEW_EXT_framebuffer_object) {
    vcl_cerr << "error: need support for EXT_framebuffer_object extension for GPU optimizations." << vcl_endl;
    return;
  }
  vcl_cout << "* using GPU optimization *" << vcl_endl;

  psm_parallel_raytrace_function<psm_render_expected_functor<APM>, APM> raytrace_fn(scene, cam, expected.ni(), expected.nj());

  psm_render_expected_functor<APM> functor(cam, expected.ni(), expected.nj());

  raytrace_fn.run(functor);

  // retrieve expected image from functor
  functor.read_expected(expected, mask);

  //: Functor class to normalize expected image
  class normalize_expected_functor
  {
  public:
    normalize_expected_functor(float tol = 1e-4) : tol_(tol) {}

    void operator()(float vis_inf, typename psm_apm_traits<APM>::obs_mathtype &pix) const 
    {
      float nvis = 1-vis_inf;
      if (nvis >= tol_)
        pix /= nvis;
      else
        pix = 0;
    }

    float tol_;
  };

  // normalize image
  normalize_expected_functor norm_fn;
  vil_transform2(mask,expected,norm_fn);

  return;
}


template <psm_apm_type APM>
class psm_render_expected_functor
{
public:

  //: default constructor
  psm_render_expected_functor(vpgl_perspective_camera<double> const& cam, unsigned int ni, unsigned int nj) 
    : cam_(cam), ni_(ni), nj_(nj), step_count_(0)
  {
    // initialize cube face indices array
    static const GLubyte cube_face_indices[] = {2,0,4,6, 1,3,7,5, 0,1,5,4, 7,3,2,6, 0,2,3,1, 4,5,7,6};
    for (unsigned int i=0; i<24; ++i) {
      cube_face_indices_[i] = cube_face_indices[i];
    }

 // enable neccesary operations
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
   // disable uncessary operations
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LINE_STIPPLE);
    // we want to render the "backs" of the cells only
    glCullFace(GL_FRONT);

    // create 2 framebuffer objects
    glGenFramebuffersEXT(1,&fbo_);
    glGenRenderbuffersEXT(1,&depth_rb_);
    glGenTextures(1, &color_tex_);
    glGenTextures(1, &data_tex_in_);
    glGenTextures(1, &data_tex_out_);
    glGenTextures(1, &expected_tex_in_);
    glGenTextures(1, &expected_tex_out_);


    // Initialize FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    // initialize depth renderbuffer
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb_);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, ni, nj);
    // attach renderbuffer to framebuffer depth buffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb_);

    // initialize main color buffer
    glBindTexture(GL_TEXTURE_2D, color_tex_);
    // initialize texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, ni, nj, 0, GL_RGBA, GL_FLOAT, 0);
    // set texture parameters
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // attach texture to framebuffer color buffer 0
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex_, 0);

    // initialize expected image buffers
    glBindTexture(GL_TEXTURE_2D, expected_tex_out_);
    // initialize texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, ni, nj, 0, GL_RGBA, GL_FLOAT, 0);
    // set texture parameters
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // attach texture to framebuffer color buffer 1
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, expected_tex_out_, 0);

    glBindTexture(GL_TEXTURE_2D, expected_tex_in_);
    // initialize input texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, ni, nj, 0, GL_RGBA, GL_FLOAT, 0);
    // set texture parameters
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


    // initialize data buffers
    glBindTexture(GL_TEXTURE_2D, data_tex_out_);
    // initialize texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, ni_, nj_, 0, GL_RGBA, GL_FLOAT, 0);
    // set texture parameters
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // attach texture to framebuffer color buffer 2
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, data_tex_out_, 0);


    glBindTexture(GL_TEXTURE_2D, data_tex_in_);
    // initialize texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, ni_, nj_, 0, GL_RGBA, GL_FLOAT, 0);
    // set texture parameters
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    // check framebuffer status to make sure we are all set
    GLuint fbo_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (fbo_status != GL_FRAMEBUFFER_COMPLETE_EXT) {
      vcl_cerr << "error creating framebuffer: error code = 0x" << vcl_hex << fbo_status;
      if (fbo_status == GL_FRAMEBUFFER_UNSUPPORTED_EXT) {
        vcl_cerr << " GL_FRAMEBUFFER_UNSUPPORTED_EXT";
      }
      vcl_cerr << vcl_endl;
    }
    vcl_cout << "frame buffer created successfully." << vcl_endl;
    // bind framebuffer object
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);

    // initialize expected color accumulator
    glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // initialize data image
    glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, expected_tex_in_);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, data_tex_in_);

    vcl_cout << "camera_center = " << cam.get_camera_center() << vcl_endl;
    vcl_cout << "camera_R = " << cam.get_rotation().as_h_matrix_3d().get_matrix() << vcl_endl;
    vcl_cout << "camera_K = " << cam.get_calibration().get_matrix() << vcl_endl;

    vgl_point_3d<double> cam_center(cam_.camera_center());
#if 1
    // set up OpenGL camera matrices
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // orient camera to point along negative z-axis
    glRotated(180.0, 0.0, 1.0, 0.0);
    // multiply by cameras RT matrix
    vnl_matrix_fixed<double,4,4> RT(0.0);
    vnl_matrix_fixed<double,3,3> R = cam.get_rotation().as_matrix();
    vnl_vector_fixed<double,3> T = -R*vnl_vector_fixed<double,3>(cam_center.x(), cam_center.y(), cam_center.z());
    //R[2][0] *= -1.0; R[2][1] *= -1.0; R[2][2] *= -1.0;
    RT.update(R,0,0);
    RT.set_column(3,vnl_vector_fixed<double,4>(T(0),T(1),T(2),1.0));
    glMultTransposeMatrixd(RT.data_block());


    // set camera calibration matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(double)ni, 0.0, -(double)nj, 0.0, 1.0, -1.0);
    vnl_matrix_fixed<double,4,4> GL_K(0.0);
    vnl_matrix_fixed<double,3,4> K(0.0); // add extra column to K
    K.update(cam.get_calibration().get_matrix(),0,0);
    GL_K.set_row(0,K.get_row(0));
    GL_K.set_row(1,K.get_row(1));
    double near = 400.0;
    double far =  800.0;
    double C = (near + far)/(near - far);
    double D = (2*far*near)/(near - far);
    GL_K.put(2,2, C); 
    GL_K.put(2,3, D);
    GL_K.set_row(3,K.get_row(2)*-1.0);

    glMultTransposeMatrixd(GL_K.data_block());

    vcl_cout << "RT = " << RT << vcl_endl;
    vcl_cout << "GL_K = " << GL_K << vcl_endl;
#else
    // TEMP

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(17.5, -16.5, -700);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-200,200,-200,200, -10, -1000);
    glFrustum(-116,116, -76, 76, 400, 800);

    /*
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 0, 20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10,10, -10,10,  -15, -25);
    */
#endif
    vnl_matrix_fixed<double,4,4> modelview(0.0);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview.data_block());
    vnl_matrix_fixed<double,4,4> projection(0.0);
    glGetDoublev(GL_PROJECTION_MATRIX, projection.data_block());
    vcl_cout << "MODELVIEW = " << modelview.transpose() << vcl_endl;
    vcl_cout << "PROJECTION = " << projection.transpose() << vcl_endl;

    // set viewport
    glViewport(0,0,ni,nj);



    // set up vertex shader
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, vertex_shader_src, 0);
    glCompileShader(vert_shader);
    GLint vert_compiled;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &vert_compiled);

    if(!vert_compiled) {
      GLint len;
      GLchar* err_log;
      glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &len);
      err_log = new GLchar[len];
      glGetShaderInfoLog(vert_shader, len, &len, err_log);
      vcl_cerr << "ERROR: vertex shader failed to compile: " << vcl_endl;
      vcl_cerr << err_log << vcl_endl;
      delete[] err_log;
      return;
    }

    // set up fragment shader
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, fragment_shader_src, 0);
    glCompileShader(frag_shader);
    GLint frag_compiled;
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_compiled);

    if(!frag_compiled) {
      GLint len;
      GLchar* err_log;
      glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &len);
      err_log = new GLchar[len];
      glGetShaderInfoLog(frag_shader, len, &len, err_log);
      vcl_cerr << "ERROR: fragment shader failed to compile: " << vcl_endl;
      vcl_cerr << err_log << vcl_endl;
      delete[] err_log;
      return;
    }

    // set up shader program
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vert_shader);
    glAttachShader(shader_program_, frag_shader);
    glBindAttribLocation(shader_program_, 1, "cell_alpha_attrib");
    GLint prog_linked;
    glLinkProgram(shader_program_);
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &prog_linked);
  

    if(prog_linked) {
      vcl_cout << "shader program sucessfully linked" << vcl_endl;
      glUseProgram(shader_program_);
    } else {
      GLint len;
      GLchar* err_log;
      glGetProgramiv(shader_program_, GL_INFO_LOG_LENGTH, &len);
      err_log = new GLchar[len];
      glGetProgramInfoLog(shader_program_, len, &len, err_log);
      vcl_cerr << "ERROR: fragment program failed to link: " << vcl_endl;
      vcl_cerr << err_log << vcl_endl;
      delete[] err_log;
      return;
    }

    // set up samplers for fragment shader
    GLint sampler_loc = glGetUniformLocation(shader_program_, "expected_in_sampler");
    glUniform1i(sampler_loc, 0);
    sampler_loc = glGetUniformLocation(shader_program_, "data_in_sampler");
    glUniform1i(sampler_loc, 1);

    // set camera center
    GLint cam_center_uniform_loc = glGetUniformLocation(shader_program_,"cam_center");
    glUniform3f(cam_center_uniform_loc, cam_center.x(), cam_center.y(), cam_center.z());

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices_);

    GLenum err_code = glGetError();
    if (err_code != GL_NO_ERROR) {
      vcl_cerr << "OpenGL ERROR: 0x" << vcl_hex << err_code << vcl_endl;
      const GLubyte *errString = gluErrorString(err_code);
      vcl_cerr << errString << vcl_endl;
    }

  }

    //: destructor
  ~psm_render_expected_functor()
  {
    glDeleteFramebuffersEXT(1,&fbo_);
  }

  //: accumulate 
  inline bool step_cells(hsds_fd_tree<psm_sample<APM>,3> &block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    ++step_count_;

    // copy previous expected image to input texture
    glActiveTexture(GL_TEXTURE0);
    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 0, 0, ni_, nj_, 0);

    // copy previous data to input texture
    glActiveTexture(GL_TEXTURE1);
    glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, 0, 0, ni_, nj_, 0);

    // initialize color buffer
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set up this FBO's draw buffers as output
    GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
    glDrawBuffers(3, draw_buffers);


    // project each cell into the image
    vcl_vector<hsds_fd_tree_node_index<3> >::iterator cell_it = cells.begin();
    double xverts_3d[2], yverts_3d[2], zverts_3d[2];
    for (; cell_it != cells.end(); ++cell_it) {
      // get cell bounding box
      vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(*cell_it);
      // get cell value
      psm_sample<APM> const& cell_value = block[*cell_it];
      // get expected color of cell
      typename psm_apm_traits<APM>::obs_datatype cell_expected  = 
        //psm_apm_traits<APM>::apm_processor::most_probable_color(cell_value.appearance);
        psm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance);

      // give vertices of cube to opengl for rendering
      xverts_3d[0] = cell_bb.xmin(); xverts_3d[1] = cell_bb.xmax();
      yverts_3d[0] = cell_bb.ymin(); yverts_3d[1] = cell_bb.ymax();
      zverts_3d[0] = cell_bb.zmin(); zverts_3d[1] = cell_bb.zmax();

      unsigned int n=0;
      for (unsigned int k=0; k<2; ++k) {
        for (unsigned int j=0; j<2; ++j) {
          for (unsigned int i=0; i<2; ++i) {
            cube_vertices_[n++] = xverts_3d[i];
            cube_vertices_[n++] = yverts_3d[j];
            cube_vertices_[n++] = zverts_3d[k];
          }
        }
      }
      glVertexAttrib1f(1,cell_value.alpha);
      glColor3f(cell_expected, cell_expected, cell_expected);
      glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, cube_face_indices_);
    }
    glFinish();

    
    if (step_count_ == 200) {
    vcl_cout << vcl_endl << "ncubes = " << cells.size() << vcl_endl;
    glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
    vil_image_view<float> rgba_test(ni_,nj_,4);
    rgba_test.fill(0.78f);
    extract_image_rgba(rgba_test);
    vil_save(rgba_test,"c:/research/psm/output/rgba_test.tiff");
    }
    

    GLenum err_code = glGetError();
    if (err_code != GL_NO_ERROR) {
      vcl_cerr << "OpenGL ERROR: 0x" << vcl_hex << err_code << vcl_endl;
      const GLubyte *errString = gluErrorString(err_code);
      if (errString) {
        vcl_cerr << (char*)errString << vcl_endl;
      }
      return false;
    }

    return true;

  }

  void read_expected(vil_image_view<float> &expected, vil_image_view<float> &vis_inf) {
    //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
    vil_image_view<float> expected_rgba(ni_,nj_,4);
    extract_image_rgba(expected_rgba);
    // convert expected_rgba to expected type
    expected.deep_copy(vil_plane(expected_rgba,0));
    //expected = vil_plane(expected_rgba,0);
    // read vis_inf from alpha channel
    vis_inf.deep_copy(vil_plane(expected_rgba,3));
    // temp debug
    vil_save(expected_rgba, "c:/research/psm/output/expected_debug.tiff");
    return;
  }

private:
  unsigned int ni_;
  unsigned int nj_;
  int step_count_;

  GLuint shader_program_;

  // indices into buffer
  GLuint fbo_;
  GLuint depth_rb_;

  // color (rgb) + alpha for each set of cells
  GLuint color_tex_;
  // data stored in rgba as (enter depth, alpha_int, vis_old, (alpha unused))
  GLuint data_tex_out_;
  GLuint data_tex_in_;
  // expected color accumulation rgb (alpha unused)
  GLuint expected_tex_in_;
  GLuint expected_tex_out_;

  // array to store cube vertices in
  GLfloat cube_vertices_[24]; // 8 vertices * 3 dimensions
  GLubyte cube_face_indices_[24]; // 6 faces * 4 indices

  vpgl_perspective_camera<double> const& cam_;

  //: extract an image from the currently active read buffer into a vil_image_view
  void extract_image_rgba(vil_image_view<float>& img)
  {
    unsigned int ni = img.ni();
    unsigned int nj = img.nj();
    if (img.nplanes() != 4) {
      vcl_cerr << "exptract_image_rgba: image must have 4 planes" << vcl_endl;
      return;
    }
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    float *pixels = new float[ni*nj*4];
    glReadPixels(0,0,ni,nj, GL_RGBA, GL_FLOAT, pixels);

    for(unsigned int jj = 0; jj < nj; ++jj) {
      for (unsigned int ii = 0; ii < ni; ++ii) {
        img(ni - ii,jj,0) = pixels[4*ii + 4*ni_*jj + 0];
        img(ni - ii,jj,1) = pixels[4*ii + 4*ni_*jj + 1];
        img(ni - ii,jj,2) = pixels[4*ii + 4*ni_*jj + 2];
        img(ni - ii,jj,3) = pixels[4*ii + 4*ni_*jj + 3];
      }
    }
    delete[] pixels;
  }


};






#endif

