#include <element.hpp>

namespace ionizing {
  Element::Element(const char*  name,
                   const int    num,
                   const Mat33d acell) : 
    Name(name), Num(num), Acell(acell), isInited(false) {
    Bcell = Acell.inverse();
  }

  Element::Element() :
    Element("", 0, Mat33d{}) {};

  bool Element::init_with_cart(const MatX3d atom_pos_cart,
                               const VecStr comments) {
    isCart         = true;
    atomPos        = atom_pos_cart;
    atomPosCart    = std::move(atom_pos_cart);
    this->comments = std::ref(comments);
    atomPosCart    = atomPosDire * Acell;
    isInited       = true;
    return true;
  }

  bool Element::init_with_dire(const MatX3d atom_pos_dire,
                               const VecStr comments) {
    isCart         = false;
    atomPos        = atom_pos_dire;
    atomPosCart    = std::move(atom_pos_dire);
    this->comments = std::ref(comments);
    atomPosDire    = atomPosCart * Bcell;
    isInited       = true;
    return true;
  }



};
