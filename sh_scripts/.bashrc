# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# User specific aliases and functions

export PS1="\[\033[38;5;33m\]\u\[$(tput sgr0)\]\[\033[38;5;226m\]@\[$(tput sgr0)\]\[\033[38;5;46m\]\h\[$(tput sgr0)\]\[\033[38;5;15m\]:\[$(tput sgr0)\]\[\033[38;5;13m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\] \[$(tput sgr0)\]\[\033[38;5;10m\]\d\[$(tput sgr0)\]\[\033[38;5;15m\] \[$(tput sgr0)\]\[\033[38;5;10m\]\t\[$(tput sgr0)\]\[\033[38;5;15m\]\n\[$(tput sgr0)\]\[\033[38;5;9m\]\\$\[$(tput sgr0)\]\[\033[38;5;15m\] \[$(tput sgr0)\]"

export PATH="~/bin:$PATH"
export PATH="~/usr/bin:$PATH"

# Colorful `cat` with syntax highlight
function ccat() {
    local style="monokai"
    if [ $# -eq 0 ]; then
        pygmentize -P style=$style -P tabsize=4 -f terminal256 -g
    else
        for NAME in $@; do
            pygmentize -P style=$style -P tabsize=4 -f terminal256 -g "$NAME"
        done
    fi
}

# Colorize man
export LESS_TERMCAP_mb=$'\E[1m\E[32m'
export LESS_TERMCAP_mh=$'\E[2m'
export LESS_TERMCAP_mr=$'\E[7m'
export LESS_TERMCAP_md=$'\E[1m\E[36m'
export LESS_TERMCAP_ZW=""
export LESS_TERMCAP_us=$'\E[4m\E[1m\E[37m'
export LESS_TERMCAP_me=$'\E(B\E[m'
export LESS_TERMCAP_ue=$'\E[24m\E(B\E[m'
export LESS_TERMCAP_ZO=""
export LESS_TERMCAP_ZN=""
export LESS_TERMCAP_se=$'\E[27m\E(B\E[m'
export LESS_TERMCAP_ZV=""
export LESS_TERMCAP_so=$'\E[1m\E[33m\E[44m'

# Smart Extract
function q-extract() {
    if [ -f $1 ] ; then
        case $1 in
        *.tar.bz2)   tar -xvjf $1    ;;
        *.tar.gz)    tar -xvzf $1    ;;
        *.tar.xz)    tar -xvJf $1    ;;
        *.bz2)       bunzip2 $1     ;;
        *.rar)       rar x $1       ;;
        *.gz)        gunzip $1      ;;
        *.tar)       tar -xvf $1     ;;
        *.tbz2)      tar -xvjf $1    ;;
        *.tgz)       tar -xvzf $1    ;;
        *.zip)       unzip $1       ;;
        *.Z)         uncompress $1  ;;
        *.7z)        7z x $1        ;;
        *)           echo "don't know how to extract '$1'..." ;;
        esac
    else
        echo "'$1' is not a valid file!"
    fi
}

# Smart Compress
function q-compress() {
    if [ -n "$1" ] ; then
        FILE=$1
        case $FILE in
        *.tar) shift && tar -cf $FILE $* ;;
        *.tar.bz2) shift && tar -cjf $FILE $* ;;
        *.tar.xz) shift && tar -cJf $FILE $* ;;
        *.tar.gz) shift && tar -czf $FILE $* ;;
        *.tgz) shift && tar -czf $FILE $* ;;
        *.zip) shift && zip $FILE $* ;;
        *.rar) shift && rar $FILE $* ;;
        esac
    else
        echo "usage: q-compress <foo.tar.gz> ./foo ./bar"
    fi
}

# Display a clock in terminal
# while sleep 1;do tput sc;tput cup 0 $(($(tput cols)-29));date;tput rc;done&

# Key bindings
#bind '"\eh":"\C-b"'                # 绑定 ALT+h 为光标左移，同 CTRL+b / <Left>
#bind '"\el":"\C-f"'                # 绑定 ALT+l 为光标右移，同 CTRL+f / <Right>
#bind '"\ej":"\C-n"'                # 绑定 ALT+j 为下条历史，同 CTRL+n / <Down>
#bind '"\ek":"\C-p"'                # 绑定 ALT+k 为上条历史，同 CTRL+p / <Up>
#bind '"\eH":"\eb"'                 # 绑定 ALT+H 为光标左移一个单词，同 ALT-b 
#bind '"\eL":"\ef"'                 # 绑定 ALT+L 为光标右移一个单词，同 ALT-f 
#bind '"\eJ":"\C-a"'                # 绑定 ALT+J 为移动到行首，同 CTRL+a / <Home>
#bind '"\eK":"\C-e"'                # 绑定 ALT+K 为移动到行末，同 CTRL+e / <End>
#bind '"\e;":"ls -l\n"'             # 绑定 ALT+; 为执行 ls -l 命令

ulimit -c unlimited

function ass() {
  filename=$(echo $1 | rev | cut -d"." -f2- | rev)
  extension=$(echo $1 | rev | cut -d"." -f1 | rev)
  if [[ $extension == "s" ]] ; then
    as $1 -o ${filename}.o
    ld ${filename}.o -o ${filename}.out
  else
    echo "  Invalid file extension : " $extension 
  fi
}

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
