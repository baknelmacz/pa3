
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"

// Node constructor, given.
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, RGBAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),left(NULL),right(NULL)
	{}

// twoDtree destructor, given.
twoDtree::~twoDtree(){
	clear();
}

// twoDtree copy constructor, given.
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


// twoDtree assignment operator, given.
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

twoDtree::twoDtree(PNG & imIn){ 
  height = imIn.height();
  width = imIn.width();
  stats s = stats(imIn);
  root = buildTree(s,pair<int,int>(0,0),pair<int,int>(width-1,height-1),true);
}

twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {
//base case, if ul and lr are in the same place, return NULL 
  if (ul == lr) return new Node(ul,lr,s.getAvg(ul,lr));
//find two rectangles to split
  int num_tests = (vert)? lr.first-ul.first : lr.second - ul.second; //if vert increment across width, if not increment across height
  if (num_tests < 1) { //case if splitting can no longer occur in current current direction
    vert = !vert;
    num_tests = (vert)? lr.first-ul.first : lr.second - ul.second; 
  }
  int x_incr = (vert)? 1:0;
  int y_incr = (vert)? 0:1;
//left rectangle will always have ul = ul of parent, right rectangle lr will always = lr of parent
  pair<int,int> left_ul = pair<int,int>(ul.first,ul.second);
  pair<int,int> left_lr;
  pair<int,int> right_ul;
  pair<int,int> right_lr = pair<int,int>(lr.first,lr.second);
//we start with a left rectangle of width 1 if the split is vertical
//and a left rectangle of height 1 if the split is horizontal
  if (vert) {
    left_lr = pair<int,int>(ul.first,lr.second);
    right_ul = pair<int,int>(ul.first+1,ul.second);
  }else{
    left_lr = pair<int,int>(lr.first,ul.second);
    right_ul = pair<int,int>(ul.first,ul.second+1);
  }
  long score;
//these values hold the lr and ul values of the rectangles that had the lowest score
  pair<int,int> min_leftlr = pair<int,int>(left_lr.first,left_lr.second);
  pair<int,int> min_rightul = pair<int,int>(right_ul.first,right_ul.second);
//each time through the loop calculate the score, increase the size of the left rectangle, decrease the size of the right rectangle
//then if the score of the new rectangles is less, save the dimensions of them
  for (int i = 1; i < num_tests; i++){
    score = s.getScore(left_ul,left_lr) + s.getScore(right_ul,right_lr);
    left_lr.first += x_incr;
    left_lr.second += y_incr;
    right_ul.first += x_incr;
    right_ul.second += y_incr;
    if (score > s.getScore(left_ul,left_lr) + s.getScore(right_ul,right_lr)) {
      min_leftlr = pair<int,int>(left_lr.first,left_lr.second);
      min_rightul = pair<int,int>(right_ul.first,right_ul.second);
    }
  }

//build subtree of left and right rectangle
  Node* leftRect = buildTree(s,left_ul,min_leftlr,!vert);
  Node* rightRect = buildTree(s,min_rightul,right_lr,!vert);
// create node and point children at two found rectangles
  Node* ret = new Node(ul,lr,s.getAvg(ul,lr));
  ret->left = leftRect;
  ret->right = rightRect;
  return ret;
}

PNG twoDtree::render(){
//initialize PNG and use recursive helper function to render the image
  PNG img = PNG(height,width);
  render(root,img);
  return img;
}

void twoDtree::render(Node* node, PNG &img){
  if (node == NULL) return; //NULL case, dont do anything
  if (node->left == NULL && node->right == NULL){ //if we've reached a leaf, render onto img
    for (int y = node->upLeft.second; y <= node->lowRight.second; y++){
      for (int x = node->upLeft.first; x <= node->lowRight.first; x++){
        RGBAPixel *curr = img.getPixel(x,y);
        *curr = node->avg;
      }
    }
  } else { //if current node is not a leaf, recurse down children looking for leaves
    render(node->left,img);
    render(node->right,img);
  }
  return;
}

int twoDtree::idealPrune(int leaves){

// YOUR CODE HERE!!

}

int twoDtree::pruneSize(int tol){
    
// YOUR CODE HERE!!

}

void twoDtree::prune(int tol){

// YOUR CODE HERE!!

}

void twoDtree::clear() {

// YOUR CODE HERE!!

}

void twoDtree::copy(const twoDtree & orig){

// YOUR CODE HERE!!

}



