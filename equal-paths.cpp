#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool checkPaths(Node* node, int depth, int& expectedDepth){

  if(node == nullptr){
    return true;
  }

  if(node->left == nullptr && node->right == nullptr){
    if(expectedDepth == -1){
      expectedDepth = depth;
    }
    return depth == expectedDepth;
  }

  return checkPaths(node->left, depth +1, expectedDepth) && checkPaths(node->right, depth + 1, expectedDepth);


}




bool equalPaths(Node * root)
{
    // Add your code below

    int expectedDepth = -1;
    return checkPaths(root, 0, expectedDepth);

}

