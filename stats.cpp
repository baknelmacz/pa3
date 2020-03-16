
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
    long redLeft = 0, greenLeft = 0, blueLeft = 0, redSqLeft = 0, greenSqLeft = 0, blueSqLeft = 0;

    for (unsigned int x = 0; x < im.width(); x++){
      long redAbove = 0,greenAbove = 0,blueAbove = 0;
      long redSqAbove = 0,greenSqAbove = 0,blueSqAbove = 0;
      
      if (y != 0){
        redAbove = sumRed[y-1][x];
        greenAbove = sumGreen[y-1][x];
        blueAbove = sumBlue[y-1][x];
        redSqAbove = sumsqRed[y-1][x];
        greenSqAbove = sumsqGreen[y-1][x];
        blueSqAbove = sumsqBlue[y-1][x];
      }
	
	redLeft += im.getPixel(x,y)->r;
        greenLeft += im.getPixel(x,y)->g;
        blueLeft += im.getPixel(x,y)->b;
        redSqLeft += im.getPixel(x,y)->r * im.getPixel(x,y)->r;
        greenSqLeft += im.getPixel(x,y)->g * im.getPixel(x,y)->g;
        blueSqLeft += im.getPixel(x,y)->b * im.getPixel(x,y)->b;


      sumRed[y][x] = redAbove + redLeft;
      sumGreen[y][x] = greenAbove + greenLeft;
      sumBlue[y][x] =  blueAbove + blueLeft;
      sumsqRed[y][x] = redSqAbove + redSqLeft;
      sumsqGreen[y][x] = greenSqAbove + greenSqLeft;
      sumsqBlue[y][x] = blueSqAbove + blueSqLeft;
    }
  }
}

long stats::getSum(char channel, pair<int,int> ul, pair<int,int> lr){
  if (ul.first < 0 || ul.second < 0 || lr.first < 0 || lr.second < 0) return 0;
  if (channel == 'r') {
    return (sumRed[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));
  }  
  if (channel == 'g') {
    return (sumGreen[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));
 }
  if (channel == 'b') {
    return (sumBlue[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));
 }
  return -1;
}

long stats::getSumSq(char channel, pair<int,int> ul, pair<int,int> lr){
  if (ul.first < 0 || ul.second < 0 || lr.first < 0 || lr.second < 0) return 0;
  if (channel == 'r') {
    return (sumsqRed[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));

  }
  if (channel == 'g') {
    return (sumsqGreen[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));

  }
  if (channel == 'b') {
    return (sumsqBlue[lr.second][lr.first] - 
	   getSum(channel,pair<int,int>(0,0),pair<int,int>(ul.first-1,lr.second)) - 
	   getSum(channel,pair<int,int>(ul.first,0),pair<int,int>(lr.first,ul.second-1)));
  }
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
