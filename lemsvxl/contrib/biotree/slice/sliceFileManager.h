#if !defined(SLICE_FILE_MANAGER_H_)
#define SLICE_FILE_MANAGER_H_ 

#include <vector>
#include <fstream>
#include <string>

#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_3d.h>

template <class T> class sliceFileManager{
  public:
    sliceFileManager();
    static vil3d_image_view_base_sptr read(const std::string& fname);
    static void read(const std::string& fname, vbl_array_3d<T>& img);
    static T* read(const std::string& fname, int & ni, int& nj, int & nk);
    static inline std::ifstream* openSliceFileForRead(const std::string& fname, int& ni, int& nj,int &d);
    static inline bool readOneSlice(std::ifstream* stream, T* slice, const int& ni, const int& nj );
    static inline bool readOneSlice(std::ifstream* stream, vil_image_view<T>& slice, const int& ni, const int& nj);

    static void write(const vil3d_image_view<T>& img, const std::string& fname);
    static void write(const vbl_array_3d<T>& img, const std::string& fname);
    static void write(const T* img, const int& ni, const int& nj, const int&d, const std::string& fname);
    static inline std::ofstream* openSliceFileForWrite(const std::string& fname, const int& ni, const int& nj, const int &d);
    static inline void writeOneSlice(std::ofstream* stream, T* data, const int& ni, const int& nj);
    static inline void writeOneSlice(std::ofstream* stream, const vil_image_view<T>& data);

    static inline void closeSliceFile(std::ifstream* stream);
    static inline void closeSliceFile(std::ofstream* stream);

  protected:
    static T* readHelper(const std::string& fname, int& ni, int& nj, int& nk);
  private:
};



#endif 
