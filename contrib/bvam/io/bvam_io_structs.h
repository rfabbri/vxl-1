#ifndef bvam_io_structs_h_
#define bvam_io_structs_h_

#include <vcl_string.h>

// Parameter TAGS
#define BVAM_MODEL_DIR_TAG "model_dir"
#define BVAM_PARAM_CORNER_TAG "corner"
#define BVAM_PARAM_VOXEL_LENGTH_TAG "voxel_length"
#define BVAM_PARAM_VOXEL_DIM_TAG "num_voxels"
#define BVAM_APPEAR_MODEL_TAG "appearence_model"
#define BVAM_APPEAR_NUM_MODES_TAG "appearence_num_modes"
#define BVAM_NORM_INTENS_TAG "normalize_intensities"

// Process Parameter TAGS
#define BWAM_IMAGES_PATH_TAG "images"
#define BWAM_CAMERAS_PATH_TAG "cameras"
#define BWAM_VIEW_CAMERA_PATH_TAG "view_camera"
#define BWAM_LIGHTS_PATH_TAG "lights"
#define BWAM_OUTPUT_PATH_TAG "output_path"
#define BWAM_SCHEDULE_TAG "schedule"
#define BWAM_SCHEDULE_SUBSET_TAG "schedule_subset"
//#define BWAM_SCHEDULE_TYPE_DATE "date"
//#define BWAM_SCHEDULE_TYPE_SHUFFLE "shuffle"

// Process TAGS
#define BVAM_PROCESS_TRAIN_TAG "train"
#define BVAM_PROCESS_DETECT_CHANGE_TAG "detect_change"
#define BVAM_PROCESS_RENDER_EXP_TAG "render_expected"
#define BVAM_PROCESS_RENDER_VIEW_TAG "render_from_view"
#define BVAM_PROCESS_WRITE_RAW_TAG "write_raw_world"

class bvam_io_process {
public:
  bvam_io_process(vcl_string type, vcl_string path) 
    : process_type(type), output_path(path) {}
  virtual ~bvam_io_process() {}
  vcl_string process_type;
  vcl_string output_path;
};

class bvam_io_schedule {
public:
  bvam_io_schedule() {}
  bvam_io_schedule(bool is_date, bool is_shuffle, bool is_subset) 
    : order_by_date(is_date), shuffle(is_shuffle), subset(is_subset) {}
  void set_subset(unsigned s, unsigned e, unsigned i) {
    subset = true;
    start = s; 
    end = e;
    inc = i;
  }
  virtual ~bvam_io_schedule() {}

  bool order_by_date, shuffle, subset;
  unsigned start, end, inc;
};

struct bvam_io_process_train : public bvam_io_process {
  bvam_io_process_train(vcl_string images, vcl_string cameras, 
    vcl_string light, bvam_io_schedule seq, vcl_string output_path) 
    : bvam_io_process(BVAM_PROCESS_TRAIN_TAG, output_path), 
    image_path(images), camera_path(cameras), light_path(light), schedule(seq) {}

  vcl_string image_path;
  vcl_string camera_path;
  vcl_string light_path;
  bvam_io_schedule schedule;
};

struct bvam_io_process_detect : public bvam_io_process {
  bvam_io_process_detect(vcl_string images, vcl_string cameras, 
    vcl_string light, bvam_io_schedule seq, vcl_string output_path) 
    : bvam_io_process(BVAM_PROCESS_DETECT_CHANGE_TAG, output_path), 
    image_path(images), camera_path(cameras), light_path(light), schedule(seq) {}

  vcl_string image_path;
  vcl_string camera_path;
  vcl_string light_path;
  bvam_io_schedule schedule;
};

struct bvam_io_process_render_expected : public bvam_io_process {
  bvam_io_process_render_expected(int dimx, int dimy, vcl_string camera, 
    vcl_string light, bvam_io_schedule seq, vcl_string output_path) 
    : bvam_io_process(BVAM_PROCESS_RENDER_EXP_TAG, output_path), 
    x(dimx), y(dimy), camera_path(camera), light_path(light), schedule(seq) {}

  int x, y;
  vcl_string camera_path;
  vcl_string light_path;
  bvam_io_schedule schedule;
};

struct bvam_io_process_render_from_view : public bvam_io_process {
  bvam_io_process_render_from_view(int dimx, int dimy, 
    vcl_string images, vcl_string cameras, 
    vcl_string view_camera, int view_camera_idx, 
    bvam_io_schedule sch, bvam_io_schedule view_sch, vcl_string output_path) 
    : bvam_io_process(BVAM_PROCESS_RENDER_VIEW_TAG, output_path), 
    x(dimx), y(dimy), image_path(images), 
    camera_path(cameras), view_camera_path(view_camera), 
    view_camera_index(view_camera_idx), schedule(sch), view_schedule(sch) {}

  int x, y;
  vcl_string image_path;
  vcl_string camera_path;
  vcl_string view_camera_path;
  int view_camera_index;
  bvam_io_schedule schedule;
  bvam_io_schedule view_schedule;
};

struct bvam_io_process_write_raw : public bvam_io_process {
  bvam_io_process_write_raw(vcl_string output_path) 
    : bvam_io_process(BVAM_PROCESS_WRITE_RAW_TAG, output_path) {}
};

#endif
