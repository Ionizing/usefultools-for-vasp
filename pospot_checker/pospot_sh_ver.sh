#!/bin/bash
FromPOT=$(grep -o "VRHFIN =[a-zA-Z]\+" POTCAR | cut -d '=' -f2 | tr 'a-z' 'A-Z');
FromPOS=$(head -6 POSCAR | tail -1 | grep -o "[a-zA-Z]\+" | tr 'a-z' 'A-Z');

if [[ ${FromPOT} == ${FromPOS} ]]; then
  echo "OK"
else
  echo "Check failed " "Elements in POTCAR are:"
  echo ${FromPOT}
  echo "while elements in POSCAR are:"
  echo ${FromPOS}
fi
