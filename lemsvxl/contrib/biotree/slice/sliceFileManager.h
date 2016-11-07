#if !defined(SLICE_FILE_MANAGER_H_)
#define SLICE_FILE_MANAGER_H_ 

#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vcl_string.h>

#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_3d.h>

template <class T> class sliceFileManager{
  public:
    sliceFileManager();
    static vil3d_image_view_base_sptr read(const vcl_string& fname);
    static void read(const vcl_string& fname, vbl_array_3d<T>& img);
    static T* read(const vcl_string& fname, int & ni, int& nj, int & nk);
    static inline vcl_ifstream* openSliceFileForRead(const vcl_string& fname, int& ni, int& nj,int &d);
    static inline bool readOneSlice(vcl_ifstream* stream, T* slice, const int& ni, const int& nj );
    static inline bool readOneSlice(vcl_ifstream* stream, vil_image_view<T>& slice, const int& ni, const int& nj);

    static void write(const vil3d_image_view<T>& img, const vcl_string& fname);
    static void write(const vbl_array_3d<T>& img, const vcl_string& fname);
    static void write(const T* img, const int& ni, const int& nj, const int&d, const vcl_string& fname);
    static inline vcl_ofstream* openSliceFileForWrite(const vcl_string& fname, const int& ni, const int& nj, const int &d);
    static inline void writeOneSlice(vcl_ofstream* stream, T* data, const int& ni, const int& nj);
    static inline void writeOneSlice(vcl_ofstream* stream, const vil_image_view<T>& data);

    static inline void closeSliceFile(vcl_ifstream* stream);
    static inline void closeSliceFile(vcl_ofstream* stream);

  protected:
    static T* readHelper(const vcl_string& fname, int& ni, int& nj, int& nk);
  private:
};



#endif 
