#pragma once
#ifndef INCAR_H
#define INCAR_H

struct INCAR {

  double _EDIFF,
         _EDIFFG,
         _ENCUT;
  int _IBRION,
      _ISIF,    
      _ISPIN,   
      _LNONCOLLINEAR, 
      _LORBIT,
      _LSORBIT, 
      _NBANDS,  
      _NELMIN,        
      _NIONS,
      _NSW,     
      _NKPTS;
};
#endif
