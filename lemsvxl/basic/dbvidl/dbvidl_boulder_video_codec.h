#include<vcl_iostream.h>
#include <vcl_fstream.h>
#include<vil/vil_image_resource_sptr.h>
#include<vcl_list.h>
#include<vidl1/vidl1_codec.h>
#include<vil/vil_image_view.h>
#include<vcl_vector.h>
#include<vcl_utility.h>
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


    virtual bool probe(vcl_string const& fname);
    virtual bool save(vidl1_movie* movie, vcl_string const& fname);
    virtual vidl1_codec_sptr load(vcl_string const& fname, char mode = 'r' );

    vcl_string type() const { return "BOULDER RAW"; }

    //: function reads the header
    bool readheader(vcl_ifstream *infile, char mode = 'r' );
    //: check if the data block iss image block or not
    bool isimageblock(vcl_ifstream *infile);
   
    bool loadboulder(vcl_string inputvid);
    


private:
    //: store the frames
    vcl_list<vil_image_resource_sptr> frames_;
    unsigned int no_of_bits_per_pixel;                   // Bits per pixel

    unsigned int nplanes;

    unsigned int datablocksize;
    unsigned int imgheadersize;

    unsigned int nframes;
    vil_pixel_format format;
    unsigned long m_ulTimeStampBlockSize;
    unsigned long m_ulCaptureInfoBlockSize;
    unsigned long m_ulBlockSize;

    vcl_vector<vcl_pair <int,long> > mapoffset;

    vcl_ifstream *ifile;
    vcl_string filename;

};
