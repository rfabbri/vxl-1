#include<iostream>
#include <fstream>
#include<vil/vil_image_resource_sptr.h>
#include<list>
#include<vidl1/vidl1_codec.h>
#include<vil/vil_image_view.h>
#include<vector>
#include<utility>
#define RAW_FILE_MARKER1     0xaf1ab90d
#define RAW_FILE_MARKER2     0xba1ab91d

#define RAW_FRAME_MARKER_1 0xaf128945
#define RAW_FRAME_MARKER_2 0xab12a946


//: A codec to read the boulder imaging videos
class dbvidl_boulder_video_codec:public vidl1_codec
{

public:
    dbvidl_boulder_video_codec();
    ~dbvidl_boulder_video_codec();

    virtual vil_image_view_base_sptr get_view(int position, int x0, int w, int y0, int h) const;
    virtual bool put_view(int position, const vil_image_view_base &im, int x0, int y0);


    virtual bool probe(std::string const& fname);
    virtual bool save(vidl1_movie* movie, std::string const& fname);
    virtual vidl1_codec_sptr load(std::string const& fname, char mode = 'r' );

    std::string type() const { return "BOULDER RAW"; }

    //: function reads the header
    bool readheader(std::ifstream *infile, char mode = 'r' );
    //: check if the data block iss image block or not
    bool isimageblock(std::ifstream *infile);
   
    bool loadboulder(std::string inputvid);
    


private:
    //: store the frames
    std::list<vil_image_resource_sptr> frames_;
    unsigned int no_of_bits_per_pixel;                   // Bits per pixel

    unsigned int nplanes;

    unsigned int datablocksize;
    unsigned int imgheadersize;

    unsigned int nframes;
    vil_pixel_format format;
    unsigned long m_ulTimeStampBlockSize;
    unsigned long m_ulCaptureInfoBlockSize;
    unsigned long m_ulBlockSize;

    std::vector<std::pair <int,long> > mapoffset;

    std::ifstream *ifile;
    std::string filename;

};
