
#include "stats.h"

stats::stats(PNG & im){
  sumRed.resize(im.height());
  sumGreen.resize(im.height());
  sumBlue.resize(im.height());
  sumsqRed.resize(im.height());
  sumsqGreen.resize(im.height());
  sumsqBlue.resize(im.height());

  for (unsigned int y = 0; y < im.height(); y++){
    sumRed[y].resize(im.width());
    sumGreen[y].resize(im.width());
    sumBlue[y].resize(im.width());
    sumsqRed[y].resize(im.width());
    sumsqGreen[y].resize(im.width());
    sumsqBlue[y].resize(im.width());

    for (unsigned int x = 0; x < im.width(); x++){
      long redAbove = 0,greenAbove = 0,blueAbove = 0,redLeft = 0,greenLeft = 0,blueLeft = 0;
      long redSqAbove = 0,greenSqAbove = 0,blueSqAbove = 0,redSqLeft = 0,greenSqLeft = 0,blueSqLeft = 0;
      if (x != 0){
        redLeft = sumRed[y][x-1];
        greenLeft = sumGreen[y][x-1];
        blueLeft = sumBlue[y][x-1];
        redSqLeft = sumsqRed[y][x-1];
        greenSqLeft = sumsqGreen[y][x-1];
        blueSqLeft = sumsqBlue[y][x-1];
      }
      if (y != 0){
        redAbove = sumRed[y-1][x];
        greenAbove = sumGreen[y-1][x];
        blueAbove = sumBlue[y-1][x];
        redSqAbove = sumsqRed[y-1][x];
        greenSqAbove = sumsqGreen[y-1][x];
        blueSqAbove = sumsqBlue[y-1][x];
      }
      sumRed[y][x] = redAbove + redLeft + im.getPixel(x,y)->r;
      sumGreen[y][x] = greenAbove + greenLeft + im.getPixel(x,y)->g;
      sumBlue[y][x] =  blueAbove + blueLeft + im.getPixel(x,y)->b;
      sumsqRed[y][x] = redSqAbove + redSqLeft + im.getPixel(x,y)->r * im.getPixel(x,y)->r;
      sumsqGreen[y][x] = greenSqAbove + greenSqLeft + im.getPixel(x,y)->g * im.getPixel(x,y)->g;
      sumsqBlue[y][x] = blueSqAbove + blueSqLeft + im.getPixel(x,y)->b * im.getPixel(x,y)->b;
    }
  }
}

long stats::getSum(char channel, pair<int,int> ul, pair<int,int> lr){
  if (channel == 'r') return sumRed[lr.second][lr.first] - sumRed[ul.second][ul.first];
  if (channel == 'g') return sumGreen[lr.second][lr.first] - sumGreen[ul.second][ul.first];
  if (channel == 'b') return sumBlue[lr.second][lr.first] - sumBlue[ul.second][ul.first];
  return -1;
}

long stats::getSumSq(char channel, pair<int,int> ul, pair<int,int> lr){
  if (channel == 'r') return sumsqRed[lr.second][lr.first] - sumsqRed[ul.second][ul.first];
  if (channel == 'g') return sumsqGreen[lr.second][lr.first] - sumsqGreen[ul.second][ul.first];
  if (channel == 'b') return sumsqBlue[lr.second][lr.first] - sumsqBlue[ul.second][ul.first];
  return -1;
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
  return (lr.first-ul.first+1)*(lr.second-ul.second+1);
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
long stats::getScore(pair<int,int> ul, pair<int,int> lr){
  long redScore = getSumSq('r',ul,lr) - getSum('r',ul,lr)*getSum('r',ul,lr) / rectArea(ul,lr);
  long greenScore = getSumSq('g',ul,lr) - getSum('g',ul,lr)*getSum('g',ul,lr) / rectArea(ul,lr);
  long blueScore = getSumSq('b',ul,lr) - getSum('b',ul,lr)*getSum('b',ul,lr) / rectArea(ul,lr);
  return redScore + greenScore + blueScore;
}
		
RGBAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
  long red = getSum('r',ul,lr)/rectArea(ul,lr);
  long green = getSum('g',ul,lr)/rectArea(ul,lr);
  long blue = getSum('b',ul,lr)/rectArea(ul,lr);
  return RGBAPixel((int)red,(int)green,(int)blue);
}
