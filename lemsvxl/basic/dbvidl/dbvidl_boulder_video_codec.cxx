#include <dbvidl/dbvidl_boulder_video_codec.h>
#include <vcl_ios.h>
#include <vcl_cstdio.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view.h>
#include <vxl_config.h>
#include <vil/vil_save.h>
#include <vul/vul_sprintf.h>
dbvidl_boulder_video_codec::dbvidl_boulder_video_codec()
{
}

dbvidl_boulder_video_codec::~dbvidl_boulder_video_codec()
{

}
//: Get a section of pixels in function of the frame number, position and size.
vil_image_view_base_sptr dbvidl_boulder_video_codec::get_view(int position, int x0, int w, int y0, int h) const
{
  
    vcl_ifstream infile(filename.c_str(),vcl_ios::in|vcl_ios::binary);
    if(infile)
    {
        infile.seekg(0);
        unsigned long raw1;
        infile.read((char*)&raw1,sizeof( long));
        
        for(unsigned i=0;i<mapoffset.size();i++)
        {
            if(position==mapoffset[i].first)
            {
                long offset=mapoffset[i].second;
                infile.seekg(offset);
                vil_memory_chunk_sptr buf = new vil_memory_chunk(width() * height() * get_bytes_pixel() , format );
                char * us = reinterpret_cast<char*>( buf->data() );
                
                for ( int j = 0; j < height(); ++j ) {
                    infile.read( us + j * width() * get_bytes_pixel(), width() * get_bytes_pixel() );
                }
                
                 
                vil_image_view_base_sptr image_sptr( new vil_image_view<unsigned short>( buf, reinterpret_cast<unsigned short*>(buf->data())+y0*width()+x0,
                    w, h, nplanes,nplanes, nplanes*w, 1 ));
                infile.close();
                return image_sptr;
            }
        }
        return 0;
    }
    else 
        return 0;
}



vidl1_codec_sptr dbvidl_boulder_video_codec::load(vcl_string const& fname, char mode  )
{
    dbvidl_boulder_video_codec *cloned_boulder_video_codec = new dbvidl_boulder_video_codec;

    if (!cloned_boulder_video_codec->loadboulder(fname)){
        delete cloned_boulder_video_codec;
        return NULL;
    }

    return vidl1_codec_sptr(cloned_boulder_video_codec);
   
}
//: Put a section of pixels in function of the frame number, position and size.
bool dbvidl_boulder_video_codec::put_view(int /*position*/, const vil_image_view_base & /*im*/, int /*x0*/, int /*y0*/)
{
  vcl_cerr << "dbvidl_boulder_video_codec::put_section not implemented\n";
  return false;
}
bool dbvidl_boulder_video_codec::save(vidl1_movie* movie, vcl_string const& fname)
{
   vcl_cerr << "dbvidl_boulder_video_codec::save not implemented\n";
   return false;
}
bool dbvidl_boulder_video_codec::probe(vcl_string const& fname)
{
    vcl_ifstream ifile(fname.c_str(),vcl_ios::in|vcl_ios::binary);
    if(!ifile)
    {
        vcl_cout<<"\nError Opening File";
        return false;
    }

    unsigned long file_marker_1;
    unsigned long file_marker_2;

    ifile.read((char*)&file_marker_1,sizeof( long));
    ifile.read((char*)&file_marker_2,sizeof( long));

    if(file_marker_1==RAW_FILE_MARKER1 && file_marker_2==RAW_FILE_MARKER2)
    {
        ifile.close();
        return true;
    }
    else
    {
        ifile.close();
        return false;
    }
}



//: function will store all the offsets of the images
bool dbvidl_boulder_video_codec::loadboulder(vcl_string inputvid)
{
    mapoffset.clear();
    filename=inputvid;
    ifile= new vcl_ifstream(inputvid.c_str(),vcl_ios::in|vcl_ios::binary);
    if(!ifile->is_open())
    {
        vcl_cout<<"\nError Opening File";
        return false;
    }
    if(readheader(ifile,'r'))
    {
       int countframes=0;
       ifile->seekg(0);
       ifile->seekg(m_ulBlockSize+8+m_ulCaptureInfoBlockSize);
       while(countframes<length())
       {
        ifile->seekg((long)ifile->tellg()+m_ulTimeStampBlockSize);
        if(isimageblock(ifile))
        {
            mapoffset.push_back(vcl_make_pair(countframes,(long)ifile->tellg()));
            ifile->seekg((long)ifile->tellg()+datablocksize);
        }
        countframes++;
       }
        return true;
    }
    else 
    {
        return false;
    }
}
bool dbvidl_boulder_video_codec::readheader(vcl_ifstream *ifile, char mode )
{
    typedef unsigned long ULONG;
    typedef enum
    {
        RTIA_MONOCHROME,
        RTIA_PACKED_RGB,
        RTIA_MONOCHROME_ROI,
        RTIA_PACKED_RGB_ROI,
        RTIA_ROI,
        RTIA_PLANAR_RGB,
        RTIA_PLANAR_RGB_ROI,
        RTIA_BAYER_RGB
    }RTIAImageType;
    typedef enum
    {
        RTIA_UNSIGNED_BYTE,
        RTIA_BYTE,
        RTIA_UNSIGNED_SHORT,
        RTIA_SHORT,
        RTIA_LONG,
        RTIA_FLOAT
    }RTIAImageFormat;

    //vcl_ifstream ifile(fname.c_str(),vcl_ios::in|vcl_ios::binary);
    if(!ifile)
    {
        vcl_cout<<"\nError Opening File";
        return false;
    }

    
    ULONG m_ulFile_Marker_1;
    ULONG m_ulFile_Marker_2;
    
    ULONG m_ulBlockVersion= 0x01; // Current version number for this block
    ULONG m_ulFramesInThisFile;       // Number of image frames in the file
    ULONG m_ulWidth;                          // Image width in pixels
    ULONG m_ulHeight;                         // Image height in pixels
    ULONG m_ulBitsPerPixel; // Bits per pixel   
    unsigned char m_ucReservedFields[168];
    
    ifile->read((char*)&m_ulFile_Marker_1,sizeof( long));
    ifile->read((char*)&m_ulFile_Marker_2,sizeof( long));
    ifile->read((char*)&m_ulBlockSize,sizeof( long));
    ifile->read((char*)&m_ulBlockVersion,sizeof( long));
    ifile->read((char*)&m_ulFramesInThisFile,sizeof( long));
    ifile->read((char*)&m_ulWidth,sizeof( long));
    ifile->read((char*)&m_ulHeight,sizeof( long));
    ifile->read((char*)&m_ulBitsPerPixel,sizeof( long));
    nframes=m_ulFramesInThisFile;

    vcl_cout<<"Header Size = "<<m_ulBlockSize<<"\n "
            <<"version No. = "<<m_ulBlockVersion<<"\n "
            <<"No of Frames = "<<m_ulFramesInThisFile<<"\n "
            <<"Width = "<<m_ulWidth<<"\n"
            <<"Height = "<<m_ulHeight<<"\n"
            <<"Bits per pixel= "<<m_ulBitsPerPixel<<"\n";
   
    set_width(m_ulWidth);
    set_height(m_ulHeight);
    set_number_frames(m_ulFramesInThisFile);
    set_bits_pixel(m_ulBitsPerPixel);

    //width=m_ulWidth;
    //height=m_ulHeight;
    no_of_bits_per_pixel=m_ulBitsPerPixel;
 
    RTIAImageType m_eType;
    RTIAImageFormat m_eFormat;
    ULONG m_ulFrameHeaderBlockSize;
    ULONG m_ulFrameBlockSize;

    if(m_ulBlockVersion>=2)
    {
        ifile->read((char*)&m_eType,sizeof(m_eType));
        ifile->read((char*)&m_eFormat,sizeof(m_eFormat));

        if(m_eFormat==RTIA_UNSIGNED_SHORT && m_eType==RTIA_MONOCHROME)
        {
            format=VIL_PIXEL_FORMAT_UINT_16;
            no_of_bits_per_pixel=16;
            nplanes=1;
        }
        else
            vcl_cerr<<"Format unexpected ";

        ifile->read((char*)&m_ulCaptureInfoBlockSize,sizeof(long));
        ifile->read((char*)&m_ulTimeStampBlockSize,sizeof(long));
        ifile->read((char*)&m_ulFrameHeaderBlockSize,sizeof(long));
        ifile->read((char*)&m_ulFrameBlockSize,sizeof(long));

        vcl_cout<<"Image Type "<<m_eType<<"\n";
        vcl_cout<<"Image Format "<<m_eFormat<<"\n"
            <<"CaptureInfoBlockSize = "<<m_ulCaptureInfoBlockSize<<"\n"
            <<"TimeStampBlockSize = "<<m_ulTimeStampBlockSize<<"\n"
            <<"FrameHeaderBlockSize = "<<m_ulFrameHeaderBlockSize<<"\n"
            <<"FrameBlockSize = "<<m_ulFrameBlockSize<<"\n";

        imgheadersize=m_ulFrameHeaderBlockSize;
        datablocksize=m_ulFrameBlockSize;
    }
    if(m_ulBlockVersion>=3)
    {
        long m_llDirOffset;
        ULONG m_ulDirSaveSize;
        ifile->read((char*)&m_llDirOffset,sizeof(long));
        ifile->read((char*)&m_ulDirSaveSize,sizeof(long));

    }
    if(m_ulBlockVersion>=4)
    {
        ULONG m_ulROIMaxBufferSize;
        ifile->read((char*)&m_ulROIMaxBufferSize,sizeof(long));
    }

    ifile->read((char*)&m_ucReservedFields,sizeof( char)*140);
    ifile->seekg(m_ulBlockSize+8);
    //skipping capture info and timestampinfo
    ifile->seekg((long)ifile->tellg()+m_ulCaptureInfoBlockSize);

    

    return true;
}
bool dbvidl_boulder_video_codec::isimageblock(vcl_ifstream *ifile)
{
    if(!ifile)
        return false;
    else
    {
        
        unsigned long m_ulBlock_Marker_1;
        unsigned long m_ulBlock_Marker_2;
        unsigned long m_ulFrameBlockSize;

        ifile->read((char*)&m_ulBlock_Marker_1,sizeof( long));
        ifile->read((char*)&m_ulBlock_Marker_2,sizeof( long));
        ifile->read((char*)&m_ulFrameBlockSize,sizeof( long));

        //vcl_cout<<"\n the block size is "<<m_ulBlockSize;

        if(m_ulBlock_Marker_1==RAW_FRAME_MARKER_1 &&
           m_ulBlock_Marker_2==RAW_FRAME_MARKER_2)
        {
            ifile->seekg((long)ifile->tellg()+imgheadersize-12);
            return true;
        }
        else
        {
            long nextpos =(long)ifile->tellg()+ (m_ulFrameBlockSize-4);
            ifile->seekg(nextpos);
            return false;
        }
    }

}
