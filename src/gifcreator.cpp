#include "gifcreator.h"
#include <cstring>

GifCreator::GifCreator():duration(0.1)
{
}

GifCreator::~GifCreator()
{
}


bool GifCreator::save(const char* filename, int every)
{
  if (frames.size()==0) return false;
  
  
  GifFileType *GifFile = EGifOpenFileName(filename, FALSE);
  
  if (!GifFile) return false;


  if (EGifPutScreenDesc(
        GifFile,
			  w, h, colorRes, 0,
        outputPalette
      ) == GIF_ERROR) return false;

  

  // ------ loop ------
  int loop_count;
   loop_count=0;
   {
     char nsle[12] = "NETSCAPE2.0";
     char subblock[3];
     if (EGifPutExtensionFirst(GifFile, APPLICATION_EXT_FUNC_CODE, 11, nsle) == GIF_ERROR) {
       return false;
     }
     subblock[0] = 1;
     subblock[2] = loop_count % 256;
     subblock[1] = loop_count / 256;

     if (EGifPutExtensionLast(GifFile, APPLICATION_EXT_FUNC_CODE, 3, subblock) == GIF_ERROR) {
       return false;
     }
 
    }
   //------------------


  for (int ni=0; ni<frames.size(); ni+=every) {      

    unsigned char ExtStr[4] = { 0x04, 0x00, 0x00, 0xff };
 
    
    ExtStr[0] = (false) ? 0x06 : 0x04;
    // ExtStr[1] = delay[ni] % 256;
    // ExtStr[2] = delay[ni] / 256;
    
    int dt = duration*100.0f;
    ExtStr[1] = dt % 256;
    ExtStr[2] = dt / 256;

    /* Dump graphics control block. */
    EGifPutExtension(GifFile, GRAPHICS_EXT_FUNC_CODE, 4, ExtStr);
            
    
    if (EGifPutImageDesc(
       GifFile,
		   0, 0, w, h, FALSE, NULL
       ) == GIF_ERROR)  return false;
       
       
    for (int y = 0, j=(h-1)*w; y < h; y++, j-=w) {
  	  if (EGifPutLine(GifFile, &(frames[ni][j]), w) == GIF_ERROR) return false;
    }
  }
  
  //comment
  if(EGifPutComment(GifFile,"This GIF file was created using QGifer") == GIF_ERROR)
          return false;
   
  if (EGifCloseFile(GifFile) == GIF_ERROR) return false;

  return true;       
}

  
