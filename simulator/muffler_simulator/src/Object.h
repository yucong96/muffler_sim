#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Constant.h"

#include <assert.h>

/////////////////////////////////////

class Complex {
public:
  double real, imag;
  Complex() {}
  Complex operator=(const Complex& a) {
    real = a.real;
    imag = a.imag;
    return *this;
  }
  Complex operator+(const Complex& a) {
    real += a.real;
    imag += a.imag;
    return *this;
  }
  Complex operator-(const Complex& a) {
    real -= a.real;
    imag -= a.imag;
    return *this; 
  }
  Complex operator*(const Complex& a) {
    real = real * a.real - imag * a.imag;
    imag = real * a.imag + imag * a.real;
    return *this; 
  }
  Complex operator/(const Complex& a) {
    double denominator = a.real * a.real + a.imag * a.imag;
    real = 1.0 * (real * a.real + imag * a.imag) / denominator;
    imag = 1.0 * (imag * a.real - real * a.imag) / denominator;
    return *this; 
  }
};

/////////////////////////////////////

typedef int NodeIndex;
typedef int EdgeIndex;
typedef int FaceIndex;
typedef int VolumeIndex;

template<class T, int n>
class SeveralNode {
private:
  T node[n];
public:
  SeveralNode() {}
  SeveralNode(const SeveralNode& a) {
    for (int i=0; i<n; i++) {
      node[i] = a.node[i];
    }
  }
  const T &operator[](int i) const {
    assert(i < n && i >= 0);
    return node[i];
  }
  T &operator[](int i){
    assert(i < n && i >= 0);
    return node[i];
  }
  void operator=(const SeveralNode &a) {
    for (int i=0; i<n; i++) {
      node[i] = a[i];
    }
  }
  bool operator==(const SeveralNode &a) const {
    for (int i=0; i<n; i++) {
      if (node[i] != a[i]) return false;
    }
    return true;
  } 
  bool operator<(const SeveralNode &a) const {
    for (int i=0; i<n; i++) {
      if (node[i] > a[i]) return false;
      if (node[i] < a[i]) return true;
    }
    return false;
  }
  bool contains(const T &a) const {
    for (int i=0; i<n; i++) {
      if (node[i] == a) return true;
    }
    return false;
  }
  void sort() {
    for (int i=0; i<n; i++) {
      for (int j=i+1; j<n; j++) {
	if (node[i] > node[j]) {
	  int temp = node[i];
	  node[i] = node[j];
	  node[j] = temp;
	}
      }
    }
  }
};

typedef SeveralNode<NodeIndex, 2> Edge;
typedef SeveralNode<NodeIndex, FACE_NODE_NUM> Face;
typedef SeveralNode<NodeIndex, VOLUME_NODE_NUM> Volume;

typedef SeveralNode<double, 4> Plain; // for ax+by+cz+d=0

enum ElementType {
  INNER, HARD, INLET, OUTLET
};

const int REAL = 1;
const int IMAG = 2;

#endif
