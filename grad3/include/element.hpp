#pragma once
#ifndef ELEMENT_H
#define ELEMENT_H

#include <base.hpp>

namespace ionizing {

struct Element {
  string Name;
  int    Num;
  bool   isCart;
  MatX3d atomPos;
  MatX3d atomPosCart;
  MatX3d atomPosDire;
  VecStr comments;

  Element(const char* name, const int num) : Name(name), Num(num) {}
  Element() : Element("", 0) {}
  void init_with_cart(const int     atom_num,
                      const MatX3d  atom_pos_cart,
                      const VecStr  comments);
  void init_with_dire(const int     atom_num, 
                      const Mat33d  atom_pos_dire,
                      const VecStr  comments);
};

};

#endif
