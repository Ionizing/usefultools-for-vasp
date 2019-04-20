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


export PP_PBE=/public/apps/vasp/pp/potpaw_PBE
export PP_LDA=/public/apps/vasp/pp/potpaw_LDA
export VASP_SCRIPT_DIR="$HOME/scripts"

function genincar() {
  arg=$(echo $1 | tr 'A-Z' 'a-z')
  valid_args=("rlx" "nve" "nvt" "scf" "dos" "band" "namd")

  # https://www.codelast.com/tag/判断字符串是否在shell数组中/
  if echo "${valid_args[@]}" | grep -w $arg &>/dev/null; then
    echo "Copying ${VASP_SCIRPT_DIR}/INCAR.$arg to current folder ..."
    if [ "$arg" = "namd" ] ; then
      cp ${VASP_SCRIPT_DIR}/INCAR.$arg ./inp ;
    else
      cp ${VASP_SCRIPT_DIR}/INCAR.$arg ./INCAR ;
    fi
    cp ${VASP_SCRIPT_DIR}/INCAR.$arg ./ ;
    echo "done"
  else
    echo "  Invalid arg input: " $1
    echo "  Expected args: " ${valid_args[@]}
  fi
}

# function genpotcar() {
#   ;
# }

function checkpot() {
# A VASP input files checker
# Contributor: Tang Gang, ionizing, and so on
# 23:54 Dec 13, 2018

  nAtoms=$(sed 7'q;d' POSCAR | awk '{sum=0; for (i=1; i<=NF; i++) { sum+= $i } print sum}' 2>/dev/null)
  varkpo=$(tail -3 KPOINTS 2>/dev/null)
  varinc=$(grep "KSPACING" INCAR 2>/dev/null)
  varpos=$(sed -n '6,8p' POSCAR 2>/dev/null)
  varatom=$(sed -n '6p' POSCAR | tr -d "\r\n" 2>/dev/null)
  FromPOT=$(grep -o "VRHFIN =[a-zA-Z]\+" POTCAR | cut -d '=' -f2 | tr 'a-z' 'A-Z' 2>/dev/null);
  FromPOS=$(head -6 POSCAR | tail -1 | grep -o "[a-zA-Z]\+" | tr 'a-z' 'A-Z' 2>/dev/null);
  kspacing=$(grep -o "KSPACING" INCAR | cut -d '=' -f1 | tr -d "\r\n" 2>/dev/null)

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
}

function cleandir() {
  to_be_deleted=(
    "DOSCAR"   "EIGENVAL"  "OSZICAR"   "PCDAT"  "vasprun.xml"
    "WAVECAR"  "XDATCAR")
  deeper_clean=(
    "CHGCAR" "CONTCAR"  "IBZKPT"  "*.molden"  "*.err" "*.log"
    "OUTCAR*")

  arg=$(echo $1 | tr 'A-Z' 'a-z')

  echo "Performing normal cleanning ..."
  rm ${to_be_deleted[@]} >/dev/null 2>&1
  if [ "$arg" = "deep" ] ; then
    echo "Performing deeper cleaning ..."
    rm ${deeper_clean[@]} >/dev/null 2>&1
  fi
  print_ok "done"
}

function genpot() {
# $1 could be (lda LDA pbe PBE)
  arg=$(echo $1 | tr 'a-z' 'A-Z')
  PP=""
  if [ "$arg" = "LDA" ] ; then
    PP="${PP_LDA}"
  elif [[ "$arg" = "PBE" || -z "$arg" ]] ; then
    PP="${PP_PBE}"
  else
    echo "genpot usage: 'genpot lda' or 'genpot pbe'"
    return 1
  fi
  FromPos=($(sed -n '6p' POSCAR | grep -o "[a-zA-Z_\s]\+" 2>/dev/null))
#  FromPos=($(echo "${FromPos[@]^}")) # uppercase first char of each str in list
  AtomTypes=($(sed -n '7p' POSCAR | grep -o "[0-9\s]\+" 2>/dev/null))
  # For debug
  echo "Found element in POSCAR: ${FromPos[@]}"
  echo "Found atoms for each element: ${AtomTypes[@]}"
  if [ ${#FromPos[@]} != ${#AtomTypes[@]} ]; then
    print_warning "  Number of element tags inconsistent with atom types!"
    return 1
  fi

  if [[ -f POTCAR && ! -f POTCAR.bak ]]; then  # backup current POTCAR if it exists
    echo "Backing up your current POTCAR to POTCAR.bak ..."
    mv POTCAR POTCAR.bak
  fi
  
  touch POTCAR.tmp
  for elem in ${FromPos[@]} ; do
    potcar="${PP}/${elem}/POTCAR"
    if [ -f ${potcar} ] ; then
      cat ${potcar} >> POTCAR.tmp
    elif [ -f ${potcar}.Z ] ; then
      zcat ${potcar}.Z >> POTCAR.tmp
    elif [ -f ${potcar}.gz ] ; then
      gunzip -c ${potcar}.gz >> POTCAR.tmp
    else
      print_warning "  No suitable POTCAR for element $elem in $PP !"
      return 1
    fi
  done
  mv POTCAR.tmp POTCAR
  print_ok "POTCAR generated from POSCAR successfully."
}
