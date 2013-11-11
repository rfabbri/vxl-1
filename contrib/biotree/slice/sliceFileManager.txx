#if !defined(SLICEFILEMANAGER_TXX_)
#define SLICEFILEMANAGER_TXX_

#include "sliceFileManager.h"

#include <vil3d/vil3d_slice.h>
#include <vil/vil_image_view.h>
template<class T>
T* sliceFileManager<T>::readHelper(const vcl_string& fname, int& ni, int& nj, int& nk)
{
  vcl_ifstream in_stream(fname.c_str());
  if(in_stream.good()){
    in_stream.read((char*)(&ni),sizeof(int));
    in_stream.read((char*)(&nj),sizeof(int));
    in_stream.read((char*)(&nk),sizeof(int));

    T* data = new T[ni*nj*nk];
    T* cur = data;
    for(int i =0; i < nk; i++){
      in_stream.read((char*)(cur),ni*nj*sizeof(T));
      cur += ni*nj;
      if(in_stream.eof()){ vcl_cerr << fname << " reached eof, slice " << i << "\n"; return 0;}
      if(!in_stream.good()){ vcl_cerr << fname << " in_stream no longer good, slice " << i << "\n"; return 0;}
    }

    in_stream.close();
    return data;
  }
  else{
    vcl_cerr << "Bad instream " << fname << "\n";
    return 0;
  }
}

template<class T>
vil3d_image_view_base_sptr sliceFileManager<T>::read(const vcl_string& fname)
{
  int ni, nj, nk;
  vcl_ifstream* in_stream = sliceFileManager<T>::openSliceFileForRead(fname,ni,nj,nk);
  if(in_stream){
    vil3d_image_view<T> * img = new vil3d_image_view<T>(ni,nj,nk,1);
    T* cur = img->origin_ptr();
    for(int i =0; i < nk; i++){
      in_stream->read((char*)(cur),ni*nj*sizeof(T));
      cur += ni*nj;
      if(in_stream->eof()){ vcl_cerr << fname << " reached eof, slice " << i << "\n"; return 0;}
      if(!in_stream->good()){ vcl_cerr << fname << " in_stream no longer good, slice " << i << "\n"; return 0;}
    }

    sliceFileManager<T>::closeSliceFile(in_stream);

    return img;
  }
  else{
    return 0;
  }
} 

template<class T>
void sliceFileManager<T>::read(const vcl_string& fname, vbl_array_3d<T>& img)
{
        int ni, nj, nk;
        T* in_data = sliceFileManager<T>::readHelper(fname,ni,nj,nk);
        if(in_data){
          img.resize(ni,nj,nk);
          img.set(in_data);
        }
}

template<class T>
 T* sliceFileManager<T>::read(const vcl_string& fname, int & ni, int& nj, int & nk)
{
        return sliceFileManager<T>::readHelper(fname,ni,nj,nk);
}

template<class T>
 void sliceFileManager<T>::write(const vil3d_image_view<T>& img, const vcl_string& fname)
{
        int ni = img.ni();
        int nj = img.nj();
        int nk = img.nk();

        vcl_ofstream out(fname.c_str(), vcl_ios_binary);

  if(out.good()){
        out.write((char*)(&ni), sizeof(int));
        out.write((char*)(&nj), sizeof(int));
        out.write((char*)(&nk), sizeof(int));
        T const*  cur = img.origin_ptr();
        for(int z = 0; z < nk; z++){
          out.write((char*)(cur), ni*nj*sizeof(T));
          cur += ni*nj;
        }
        out.close();
  }
  else{
    vcl_cerr << "Bad out stream " << fname << "\n";
  }
}

template<class T>
 void sliceFileManager<T>::write(const vbl_array_3d<T>& img, const vcl_string& fname)
{
  int ni = img.get_row1_count();
  int nj = img.get_row2_count();
  int nk = img.get_row3_count();
  vcl_ofstream out(fname.c_str(), vcl_ios_binary);
  if(out.good()){
    out.write((char*)(&ni), sizeof(int));
    out.write((char*)(&nj), sizeof(int));
    out.write((char*)(&nk), sizeof(int));

    vcl_cerr << "vbl_array_3d is column_major; writing " << sizeof(T) << "bytes at a time\n";
    for(int k = 0; k < nk; k++){
      for(int j = 0; j < nj; j++){
        for(int i = 0; i < ni; i++){
          out.write((char*)(&img(i,j,k)), sizeof(T));
        }
      }
    }

    /*
     * vbl_array_3d stores as column-major so this will not work
     T const* cur = img.data_block();
     for(int z = 0; z < nk; z++){
     out.write((char*)(cur), ni*nj*sizeof(T));
     cur += ni*nj;
     }
     */
    out.close();
  }
  else{
    vcl_cerr << "Bad out stream " << fname << "\n";
  }
}

  template<class T>
void sliceFileManager<T>::write(const T* img, const int& ni, const int& nj, const int& nk, const vcl_string& fname)
{
  vcl_ofstream out(fname.c_str(), vcl_ios_binary);

  if(out.good()){
    out.write((char*)(&ni), sizeof(int));
    out.write((char*)(&nj), sizeof(int));
    out.write((char*)(&nk), sizeof(int));

    T const* cur = img;
    for(int z = 0; z < nk; z++){
      out.write((char*)(cur), ni*nj*sizeof(T));
      cur += ni*nj;
    }
    out.close();
  }
  else{
    vcl_cerr << "Bad out stream " << fname << "\n";
  }
}

  template<class T>
vcl_ofstream* sliceFileManager<T>::openSliceFileForWrite(const vcl_string& fname, const int& ni, const int& nj, const int &nk)
{
  vcl_ofstream * out = new vcl_ofstream(fname.c_str(),vcl_ios_binary);
  if(out->good()){
    out->write((char*)(&ni), sizeof(int));
    out->write((char*)(&nj), sizeof(int));
    out->write((char*)(&nk), sizeof(int));

    return out;
  }
  else{
    vcl_cerr << "Bad out stream " << fname << "\n";
    return 0;
  }
}

template<class T>
 void sliceFileManager<T>::closeSliceFile(vcl_ifstream* stream)
{
  stream->close();
  delete stream;
}
template<class T>
 void sliceFileManager<T>::closeSliceFile(vcl_ofstream* stream)
{
  stream->close();
  delete stream;
}


template<class T>
 void sliceFileManager<T>::writeOneSlice(vcl_ofstream* out, T* data, const int& ni, const int& nj)
{
  out->write((char*)(data), ni*nj*sizeof(T));
}
template<class T>
 void sliceFileManager<T>::writeOneSlice(vcl_ofstream* out, const vil_image_view<T>& data)
{
  out->write((char*)(data.top_left_ptr()), data.ni()*data.nj()*sizeof(T));
}

template<class T>
 inline vcl_ifstream* sliceFileManager<T>::openSliceFileForRead(const vcl_string& fname, int& ni, int& nj,int& nk)
{
  vcl_ifstream * in = new vcl_ifstream(fname.c_str());
  if(in->good()){
    in->read((char*)(&ni), sizeof(int));
    in->read((char*)(&nj), sizeof(int));
    in->read((char*)(&nk), sizeof(int));
    return in;
  }
  else{
    vcl_cerr << "Bad instream " << fname << "\n";
    return 0;
  }
}

template<class T>
 inline bool sliceFileManager<T>::readOneSlice(vcl_ifstream* in, T* slice, const int& ni, const int& nj )
{
  if(in->eof()){ vcl_cerr << "reached eof\n"; return 0;}
  if(!in->good()){ vcl_cerr << "in_stream no longer good!\n"; return 0;}
  in->read((char*)(slice), ni*nj*sizeof(T));
  return 1;
}
template<class T>
inline bool sliceFileManager<T>::readOneSlice(vcl_ifstream* in, vil_image_view<T>& slice, const int& ni, const int& nj)
{
  if(in->eof()){ vcl_cerr << "reached eof\n"; return 0;}
  if(!in->good()){ vcl_cerr << "in_stream no longer good!\n"; return 0;}
  slice.set_size(ni,nj);
  in->read((char*)(slice.top_left_ptr()), ni*nj*sizeof(T));
  return 1;
}

#define SLICEFILEMANAGER_INSTANTIATE(T)\
            template class sliceFileManager<T>;

#endif
