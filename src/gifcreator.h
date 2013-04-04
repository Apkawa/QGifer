
/*
  This class is based on AnimatedGifSaver class by Marco Tarini
*/

#include <gif_lib.h>
#include <cstdio>
#include <vector>

using namespace std;

typedef vector<GifByteType> Frame;
typedef unsigned char Byte;

class GifCreator
{

public:
     GifCreator();
  ~GifCreator();
  
  void resize(int width, int height) {w = width; h = height;}

  // Adds a frame that is to last [dt] seconds
  // "data" is the image (r,g,b,r,g,b,r,g,b...), bottom-to-top
  // The first frame defines the palette
  bool addFrame(Byte* data,  float dt);
  
  // Saves the results in a gif file
  bool save(const char* filename, int every = 1);
  void setPalette(ColorMapObject* palette) {outputPalette = palette;}
  void setDuration(float d){duration = d;}
private:
     static bool addLoop(GifFileType *gf);

protected:
  vector<Frame> frames;
  //vector<int> delay; //na razie rezygnujemy z roznych opoznien
  ColorMapObject* outputPalette;
  int w;
  int h;
  float duration;
  //int paletteSize;
};
