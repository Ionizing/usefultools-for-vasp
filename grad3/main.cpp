#include "OUTCAR.h"
#include "POSCAR.h"
#include "global.h"

//#define DEBUG_MODE
//#define DEBUG_OUTCARONLY


void PrintHelp();
void PrintFileOpenError(const char*);

int main(int argc, char* argv[]) {
	char fname_OUTCAR[100] = "./OUTCAR";
	char fname_POSCAR[100] = "./POSCAR";

#ifdef DEBUG_MODE
	strcpy(fname_OUTCAR, "../test2/OUTCAR");
	strcpy(fname_POSCAR, "../test2/POSCAR");
#else
	if (1 == argc || 3 == argc || 5 == argc) {
		for (size_t i(1); i < argc; i += 2) {
			if (0 == strcmp(argv[i], "-p")) {
				strcpy(fname_POSCAR, argv[i + 1]);
			} else if (0 == strcmp(argv[i], "-o")) {
				strcpy(fname_OUTCAR, argv[i + 1]);
			} else {
				PrintHelp();
			}
		}
	} else {
		PrintHelp();
	}
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

void PrintHelp() {
    printf("%s", g_str_FAIL);
    printf("Invalid input arguments\n");
	printf("%s", g_str_ENDC);

    printf("The default arguments are \'-p POSCAR -o OUTCAR\'");
	printf("Type -p \'POSCAR\' to specify POSCAR file.\n");
	printf("Type -o \'OUTCAR\' to specify OUTCAR file.\n");
	printf("Or you can specify both POSCAR and OUTCAR files.\n");
//	system("pause");
	exit(EXIT_FAILURE);
	return;
}

void PrintFileOpenError(const char* fname) {
    printf("%s", g_str_FAIL);
	printf("Unfortunately, %s open failed. Please check.\n", fname);
    printf("%s", g_str_ENDC);
//	system("pause");
	exit(EXIT_FAILURE);
	return;
}
