#!/bin/bash

# A VASP input files checker
# Contributor: Tang Gang, ionizing, and so on
# 23:54 Dec 13, 2018

RED="\033[31m"
GREEN="\033[32m"
NORMAL="\033[97m"
END="\033[0m"

function print_warning() {
  echo -e "${RED} $1 ${END}"
}

function print_ok() {
  echo -e "${GREEN} $1 ${END}"
}

function print_normal() {
  echo -e "${NORMAL} $1 ${END}"
}

nAtoms=$(sed 7'q;d' POSCAR | awk '{sum=0; for (i=1; i<=NF; i++) { sum+= $i } print sum}')
varkpo=$(tail -3 KPOINTS)
varinc=$(grep "KSPACING" INCAR)
varpos=$(sed -n '6,8p' POSCAR)
varatom=$(sed -n '6p' POSCAR | tr -d "\r\n")
FromPOT=$(grep -o "VRHFIN =[a-zA-Z]\+" POTCAR | cut -d '=' -f2 | tr 'a-z' 'A-Z');
FromPOS=$(head -6 POSCAR | tail -1 | grep -o "[a-zA-Z]\+" | tr 'a-z' 'A-Z');
kspacing=$(grep -o "KSPACING" INCAR | cut -d '=' -f1 | tr -d "\r\n")

if [ -e INCAR ]&&[ -e POSCAR ]&&[ -e POTCAR ]; then
  if [ -e KPOINTS ]; then
    print_ok " Input Files are ok."
    print_normal " KPOINTS: "
    print_normal ${varkpo}
    echo ""
  else
    if [ "$kspacing"x == "KSPACING"x ]; then
      print_ok " Input Files are ok. "
      print_normal " $varinc "
    else
      print_warning $(grep "KSPACING" INCAR)
      print_warning " KPOINTS or KSPACING do not exist ! "
    fi
  fi
  
  if [[ ${FromPOT} == ${FromPOS} ]]; then
    print_ok " Elements are the same ($varatom) in POSCAR and POTCAR. "
  else
    print_warning " ** Check failed ** Note that: Elements in POTCAR are: "
    print_normal ${FromPOT}
    print_warning " while elements in POSCAR are: "
    print_normal ${FromPOS}
  fi
  
  print_normal "\n Number of atoms is : ${nAtoms} \n"

else
  for i in INCAR POTCAR KPOINTS POSCAR ; do
    if [ -e $i ] && [ $i = "POSCAR" ]; then
      print_normal " POSCAR: \n ${varpos}"
      print_normal "\n Number of atoms is : ${nAtoms} \n"
    elif [ ! -e $i  ]; then
      print_warning " ${i} do not exist! "
    fi
  done
fi
