#pragma once
#ifndef ELEMENT_H
#define ELEMENT_H

#include <base.hpp>

namespace ionizing {

struct Element {
  string Name;
  int    Num;
  Mat33d Acell;
  Mat33d Bcell;
  bool   isCart;
  bool   isInited;


  MatX3d atomPos;
  MatX3d atomPosCart;
  MatX3d atomPosDire;
  VecStr comments;

  Element(const char* name, const int num, const Mat33d acell);
  Element();
  bool init_with_cart(const MatX3d  atom_pos_cart,
                      const VecStr  comments);
  bool init_with_dire(const MatX3d  atom_pos_dire,
                      const VecStr  comments);
};

};

#endif
