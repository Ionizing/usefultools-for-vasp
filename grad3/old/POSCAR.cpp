#include "POSCAR.h"
#include "global.h"

POSCAR::~POSCAR() {
	return;
}

POSCAR::POSCAR(std::ifstream& ifs, const size_t numAtoms) {
	inputAtoms(numAtoms);
	darrActive.resize(nAtoms, std::vector<double>(3, .0));

	char str_tmp[200] = {0};
	for (size_t i(0); i!=6; ++i) {
		ifs.getline(str_tmp, 200);
		//puts("@14");
	}
	//puts("@16");
	//puts(str_tmp);
	char str[200] = {0};
	sscanf(str_tmp, "%s", str);
	size_t len = strlen(str);
	bool isAllAlpha = true;
	for(size_t i=0; i!=len; ++i) {
		if( 0 == isalpha(str[i]) ) {
			isAllAlpha = false;
			break;
		}
		//puts("@24");
	}

	if ( true == isAllAlpha ) {
		ifs.getline(str_tmp, 200);
	}

	nIons = 0;
	size_t nIons_tmp = 0;

#ifdef DEBUG_MODE
  puts(str_tmp);
#endif
	char* str_ptr = strtok(str_tmp, " ");
	while( nullptr != str_ptr ) {
    nIons_tmp = 0;
		sscanf(str_ptr, "%zu", &nIons_tmp);
#ifdef DEBUG_MODE
    printf("DEBUG: %zu\n", nIons_tmp);
#endif
		nIons += nIons_tmp;
		str_ptr = strtok(nullptr, " ");
	}
	//while( 0 != (sscanf(str_tmp, "%zu", &nIons_tmp)) ){
	//	nIons += nIons_tmp;
	//}
#ifdef DEBUG_MODE
  printf("DEBUG:NIONS = %zu, nAtoms = %zu\n", nIons, nAtoms);
#endif
	if (nIons != nAtoms) {
        printf("%s", g_str_FAIL);
		printf("Incompactable values in POSCAR and OUTCAR\n");
//		system("pause");
		exit(EXIT_FAILURE);
	}

	bSelective_Dynamics = false;
	ifs.getline(str_tmp, 200);
	sscanf(str_tmp, "%s", str);
    *str = toupper(*str);
	if ('S' == str[0]) {
		bSelective_Dynamics = true;
		ifs.getline(str_tmp, 200);
		sscanf(str_tmp, "%s", str);
	}

	if ( true == bSelective_Dynamics ) {
		char SelDynStrArr[3][10];
		for(size_t i(0); i!=nIons; ++i) {
			ifs.getline(str_tmp, 200);
			sscanf(str_tmp, "%*f%*f%*f%s%s%s", 
				SelDynStrArr[0], SelDynStrArr[1], SelDynStrArr[2]);
			for(size_t j(0); j!=3; ++j) {
                *SelDynStrArr[j] = toupper(*SelDynStrArr[j]);
				darrActive[i][j] = ( 'F' == SelDynStrArr[j][0] ) ? 0 : 1;
			}
		}
	}
//	puts("POSCAR::POSCAR complete");
}

void POSCAR::inputAtoms(const size_t num) {
	this->nAtoms = num;
}

size_t POSCAR::shownIons() {
	return nIons;
}

std::vector<std::vector<double>> POSCAR::showdarrActive() {
	return darrActive;
}

bool POSCAR::showbSelective_Dynamics() {
	return bSelective_Dynamics;
}
