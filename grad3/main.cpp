#include "OUTCAR.h"
#include "POSCAR.h"
#include "global.h"
#include "arg_parse.h"

//#define DEBUG_MODE
//#define DEBUG_OUTCARONLY

int main(int argc, char* argv[]) {

#ifdef DEBUG_MODE
	strcpy(fname_OUTCAR, "./test2/OUTCAR");
	strcpy(fname_POSCAR, "./test2/POSCAR");
#else
  Parse(argc, argv);
  // printf("layout_vol = %d,  layout_mag = %d\n", layout_vol, layout_mag);
#endif // DEBUG_MODE

	std::ifstream ifs_OUTCAR(fname_OUTCAR);
	if (false == ifs_OUTCAR.good()) {
		PrintFileOpenError(fname_OUTCAR);
	}
	OUTCAR outcar(ifs_OUTCAR);
	ifs_OUTCAR.close();

#ifndef DEBUG_OUTCARONLY
	std::ifstream ifs_POSCAR(fname_POSCAR);
	if (false == ifs_POSCAR.good()) {
		PrintFileOpenError(fname_POSCAR);
	}
    size_t nAtoms = outcar.shownNumOfAtoms();
//    printf("@main 47 nAtoms = %zu\n", nAtoms);
	POSCAR poscar(ifs_POSCAR, /*outcar.shownNumOfAtoms()*/ nAtoms);
	ifs_POSCAR.close();
	outcar.inputnIons(poscar.shownIons());
	outcar.inputdarrActive(poscar.showdarrActive());
	outcar.inputSelectiveDynamics(poscar.showbSelective_Dynamics());
#else
	outcar.GetPosInfo();
#endif // !DEBUG_OUTCARONLY

	outcar.GetOtherVals();
//	system("pause");
	return 0;
}
