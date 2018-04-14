#include "global.h"

const char  
    g_str_OKGREEN[16] = "\033[92m",
    g_str_WARNING[16] = "\033[93m",
    g_str_FAIL[16]    = "\033[91m",
    g_str_ENDC[16]    = "\033[0m";

char  fname_OUTCAR[256] = "./OUTCAR",
      fname_POSCAR[256] = "./POSCAR";

bool layout_mag = false,   layout_vol = false;
