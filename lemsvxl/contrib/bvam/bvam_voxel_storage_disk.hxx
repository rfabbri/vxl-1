#ifndef bvam_voxel_storage_disk_txx_
#define bvam_voxel_storage_disk_txx_

#include <string>
#include <fstream>
#include <cassert>
#include <vul/vul_file.h>
#include <vgl/vgl_vector_3d.h>

#include "bvam_voxel_storage_disk.h"
#include "bvam_voxel_slab.h"

template <class T>
bvam_voxel_storage_disk<T>::bvam_voxel_storage_disk(std::string storage_filename, vgl_vector_3d<unsigned int> grid_size)
: bvam_voxel_storage(grid_size), storage_fname_(storage_filename)
{
  slab_buffer_ = new bvam_memory_chunk(grid_size.x()*grid_size.y()*sizeof(T));

  // check if file exsist already or not
  if (vul_file::exists(storage_fname_))  {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      std::cerr << "error: directory name " << storage_fname_ << " passed to bvam_voxel_storage_disk constructor." << std::endl;
      return;
    }
    // read header and make sure that it matches given dimensions
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::out | std::ios::in);
    if (!fio_.is_open()) {
      std::cerr << "error opening " << storage_fname_ << " for read/write " << std::endl;
      return;
    }
    // seek to beginning of file
    fio_.seekp(0,std::ios::beg);
    bvam_voxel_storage_header<T> header;

    fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

    if ((header.nx_ != grid_size.x()) || (header.ny_ != grid_size.y()) || (header.nz_ != grid_size.z())) {
      std::cerr << "error: file on disk has size " << vgl_vector_3d<unsigned>(header.nx_,header.ny_,header.nz_) << std::endl;
      std::cerr << "       size passed to constructor = " << grid_size << std::endl;
      return;
    }
  }
  else {
    // file does not yet exist. do nothing for now.
  }
}

template <class T>
bool bvam_voxel_storage_disk<T>::initialize_data(T const& value)
{
  // check if file exsist already or not
  if (!(vul_file::exists(storage_fname_)))  {
    // create file
    std::string base_dir = vul_file::dirname(storage_fname_);
    bool base_dir_exists = vul_file::is_directory(base_dir);
    if (!base_dir_exists) {
      std::cerr << "error: base directory " << base_dir << " does not exist. " << std::endl;
      return false;
    }
  }else {
    // make sure filename is not a directory
    bool is_dir = vul_file::is_directory(storage_fname_);
    if (is_dir) {
      std::cerr << "error: directory name " << storage_fname_ << " passed to bvam_voxel_storage_disk constructor." << std::endl;
      return false;
    }
  }
  // everything looks ok. open file for write and fill with data
  bvam_voxel_slab<T> init_slab(grid_size_.x(),grid_size_.y(),1,slab_buffer_,static_cast<T*>(slab_buffer_->data()));
  init_slab.fill(value);

  if (fio_.is_open()) {
    // close the file if it was already open.
    fio_.close();
  }
  fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::out);
  if (!fio_.is_open()) {
    std::cerr << " error opening file " << storage_fname_ << " for write. " << std::endl;
    return false;
  }
  // write the header
  bvam_voxel_storage_header<T> header(grid_size_);
  fio_.write(reinterpret_cast<char*>(&header),sizeof(header));
  // write each slice
  for (unsigned z=0; z <grid_size_.z(); z++) {
    fio_.write(reinterpret_cast<char*>(init_slab.first_voxel()),init_slab.size()*sizeof(T));
  }

  // close output stream
  fio_.close();

  return true;

}





template <class T>
bvam_voxel_slab<T> bvam_voxel_storage_disk<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  if (slice_idx + slab_thickness > grid_size_.z()) {
    std::cerr << "error: tried to get slab " << slice_idx << " with thickness " << slab_thickness << "; grid_size_.z() = " << grid_size_.z() << std::endl;
    bvam_voxel_slab<T> slab;
    return slab;
  }
  // check to see if file is already open
  if (!fio_.is_open()) {
    fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::in | std::ios::out);
  }
  unsigned long slice_pos = grid_size_.x()*grid_size_.y()*slice_idx*sizeof(T) + sizeof(bvam_voxel_storage_header<T>);
  unsigned long file_pos = fio_.tellg();
  if (slice_pos != file_pos) {
    fio_.seekg(slice_pos,std::ios::beg);
  }
  fio_.read(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  bvam_voxel_slab<T> slab(grid_size_.x(),grid_size_.y(),1,slab_buffer_,reinterpret_cast<T*>(slab_buffer_->data()));
  return slab;
}

template <class T>
void bvam_voxel_storage_disk<T>::put_slab()
{
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::out | std::ios::in);
    if (!fio_.is_open()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write " << std::endl;
      return;
    }
  }
  unsigned long in_pos = fio_.tellg();
  if (in_pos < grid_size_.x()*grid_size_.y()*sizeof(T) + sizeof(bvam_voxel_storage_header<T>)) {
    std::cerr << "error: attempted to put_slice() when current file position is not past first slice." << std::endl;
    return;
  }
  unsigned long slice_pos = in_pos - (grid_size_.x()*grid_size_.y()*sizeof(T));
  unsigned long file_pos = fio_.tellp();
  if (slice_pos != file_pos) {
    fio_.seekp(slice_pos,std::ios::beg);
  }
  fio_.write(reinterpret_cast<char*>(slab_buffer_->data()),slab_buffer_->size());
  fio_.flush();
  
  return;

}

template <class T>
unsigned bvam_voxel_storage_disk<T>::num_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::out | std::ios::in);
    if (!fio_.is_open()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write " << std::endl;
      return 0;
    }
  }
  // seek to beginning of file
  fio_.seekp(0,std::ios::beg);
  bvam_voxel_storage_header<T> header;

  fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

  return header.nobservations_;
}

template <class T>
void bvam_voxel_storage_disk<T>::increment_observations()
{
  // read header from disk
  // check to see if file is already open
  if (!fio_.is_open()) {
    // open the file with in and out flags so we dont truncate
    fio_.open(storage_fname_.c_str(),std::ios::binary | std::ios::out | std::ios::in);
    if (!fio_.is_open()) {
      std::cerr << "error opening " << storage_fname_ << "for read/write " << std::endl;
      return;
    }
  }
  // seek to beginning of file
  fio_.seekp(0,std::ios::beg);
  bvam_voxel_storage_header<T> header;
  // read
  fio_.read(reinterpret_cast<char*>(&header),sizeof(header));

  // increment observations
  ++header.nobservations_;

  // write header back to disk
  // seek to beginning of file
  fio_.seekp(0,std::ios::beg);
  // write
  fio_.write(reinterpret_cast<char*>(&header),sizeof(header));

}



#define BVAM_VOXEL_STORAGE_DISK_INSTANTIATE(T)\
template class bvam_voxel_storage_disk<T >; \

#endif
