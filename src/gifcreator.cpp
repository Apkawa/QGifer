#include "gifcreator.h"

GifCreator::GifCreator()
{
}

GifCreator::~GifCreator()
{
}

// bool GifCreator::addFrame(Byte* data, float dt)
// {
    
//   unsigned int npix=w*h;
//   Frame output(npix);
  
//     // maunal assignment of color indices
//     for (int i = 0, j=0; i < npix; i++) {
//         int minIndex = 0,
//             minDist = 3 * 256 * 256;
//         GifColorType *c = outputPalette->Colors;
 
//         /* Find closest color in first color map to this color. */
//         for (int k = 0; k < outputPalette->ColorCount; k++) {
//           int dr = (int(c[k].Red) - data[j] ) ;
//           int dg = (int(c[k].Green) - data[j+1] ) ;
//           int db = (int(c[k].Blue) - data[j+2] ) ;
          
//           int dist=dr*dr+dg*dg+db*db;
          
//           if (minDist > dist) {
//             minDist  = dist;
//             minIndex = k;
//           }
//         }
//         j+=3;
//         output[i] = minIndex;
//     }

  
//   frames.push_back(output);

//   delay.push_back(int(dt*100.0));
//   return true;       
// }



bool GifCreator::addLoop(GifFileType *gf)
{
   int loop_count;
   loop_count=0;
   {
     char nsle[12] = "NETSCAPE2.0";
     char subblock[3];
     if (EGifPutExtensionFirst(gf, APPLICATION_EXT_FUNC_CODE, 11, nsle) == GIF_ERROR) {
       return false;
     }
     subblock[0] = 1;
     subblock[2] = loop_count % 256;
     subblock[1] = loop_count / 256;
     if (EGifPutExtensionLast(gf, APPLICATION_EXT_FUNC_CODE, 3, subblock) == GIF_ERROR) {
       return false;
     }
 
    }
    return true;
}


bool GifCreator::save(const char* filename){
  if (frames.size()==0) return false;
  
  
  GifFileType *GifFile = EGifOpenFileName(filename, FALSE);
  
  if (!GifFile) return false;

  if (EGifPutScreenDesc(
        GifFile,
			  w, h, 8, 0,
        outputPalette
      ) == GIF_ERROR) return false;

  if (!addLoop(GifFile)) return false;

  for (int ni=0; ni<frames.size(); ni++) {      

    static unsigned char
    ExtStr[4] = { 0x04, 0x00, 0x00, 0xff };
 
    
    ExtStr[0] = (false) ? 0x06 : 0x04;
    ExtStr[1] = delay[ni] % 256;
    ExtStr[2] = delay[ni] / 256;
 
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

  if (EGifCloseFile(GifFile) == GIF_ERROR) return false;

  return true;       
}

  
