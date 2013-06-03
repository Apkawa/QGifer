/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/


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
  void setColorRes(int res) {colorRes = res;}
  void addPalette(ColorMapObject* pal) {cmaps.push_back(pal);}
  void setDuration(float d){duration = d;}
  void appendReversedCopy();
  void removeReversedCopy();
protected:
  virtual void savingProgress(int){}; //procentowy postep w zapisie
  virtual void endProgress(){}; //zakonczenie pracy
  vector<Frame> frames;
  //vector<int> delay; //na razie rezygnujemy z roznych opoznien
  vector<ColorMapObject*> cmaps;
  int w;
  int h;
  int colorRes;
  float duration;
  int revStart; //indeks klatki od ktorej zaczyna sie odwrocone powtorzenie
  //int paletteSize;
};
