
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
//base case, if ul and lr are in the same place, return new node, this is one pixel 
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
  score = s.getScore(left_ul,left_lr) + s.getScore(right_ul,right_lr);
  for (int i = 1; i < num_tests; i++){
    left_lr.first += x_incr;
    left_lr.second += y_incr;
    right_ul.first += x_incr;
    right_ul.second += y_incr;
    int newScore = s.getScore(left_ul,left_lr) + s.getScore(right_ul,right_lr);
    if (newScore <= score) {
      score = newScore;
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
  PNG img = PNG(width,height);
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
  int guess = 0;
  int increment = 1000;
  return idealPrune(leaves,guess,increment);
}
int twoDtree::idealPrune(int leaves, int guess, int increment){
  int temp = pruneSize(guess);
  guess += increment;
  if (temp < leaves){
    return idealPrune(leaves,guess,increment);
  }
  else if (temp > leaves) {
    return idealPrune(leaves,guess-increment,increment/10);
  }
  else if (temp == leaves){
    if (increment == 1) return guess-increment;
    else return idealPrune(leaves,guess-increment,increment/10);
  }
  return -1;
}

int twoDtree::pruneSize(int tol){
  return pruneSize(tol,root);    
}
int twoDtree::pruneSize(int tol,Node* node){
  if (node == NULL) return 0;
  if (vibeCheck(tol,node->avg,node->left) && vibeCheck(tol,node->avg,node->right)){
    return 1;
  } else {
    return pruneSize(tol,node->left) + pruneSize(tol,node->right);
  }
}

void twoDtree::prune(int tol){
  prune(tol,root);
}

void twoDtree::prune(int tol, Node* node){
  if (node == NULL) return;
  if (vibeCheck(tol,node->avg,node->left) && vibeCheck(tol,node->avg,node->right)){
    clear(node->left);
    node->left = NULL;
    clear(node->right);
    node->right = NULL;
  } else {
    prune(tol,node->left);
    prune(tol,node->right);
  }
}

bool twoDtree::vibeCheck(int tol,RGBAPixel avg,Node* node){
  if (node == NULL) return true;
  if (node->left == NULL && node->right == NULL){ 
    long distance =((avg.r-node->avg.r)*(avg.r-node->avg.r))+((avg.g-node->avg.g)*(avg.g-node->avg.g))+((avg.b-node->avg.b)*(avg.b-node->avg.b));
    if (distance < tol) return true;
    else return false;
  } else {
    return vibeCheck(tol,avg,node->left) && vibeCheck(tol,avg,node->right);
  }
}


void twoDtree::clear() {
  clear(root);
}

void twoDtree::clear(Node* node){
  if (node == NULL) return;
  if (node->left) clear(node->left);
  if (node->right) clear(node->right);
  delete node;
  return;
}

void twoDtree::copy(const twoDtree & orig){
  height = orig.height;
  width = orig.width;
  copy(root,orig.root);
}
void twoDtree::copy(Node* &subroot, const Node * orig){
  if (orig == NULL) return;
  subroot = new Node(orig->upLeft,orig->lowRight,orig->avg);
  copy(subroot->left,orig->left);
  copy(subroot->right,orig->right);
  return;
}
