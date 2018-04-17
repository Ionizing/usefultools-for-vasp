#include "OUTCAR.h"

OUTCAR::~OUTCAR() {
	for(auto str : strData){
		delete[] str;
	}
	//puts("OUTCAR Destructed");
	return;
}

OUTCAR::OUTCAR(std::ifstream& ifs) {
	char* str;
	while (!ifs.eof()) {
		str = new char[200];
		ifs.getline(str, 200);
		this->strData.push_back(str);
		//printf("%s\n", strData.back());
	}
//	puts("OUTCAR Reading Complete.");
	this->GetNumOfAtoms();
	this->GetNumOfElems();
	this->GetEDiff();			// Can be optimized later.
								// Get those three values in one loop.
	return;
}

void OUTCAR::GetNumOfAtoms() {
	char* str_ptr = nullptr;
	for( auto str : strData ){
		str_ptr = strstr(str, "number of ions");
		if( nullptr != str_ptr ) {
			break;
		}
//		printf("str = %s\n", str);
	}
//	printf("@OUTCAR 36 str_ptr = \"%s\"\n", str_ptr);
	sscanf(str_ptr, "%*s%*s%*s%*s%*s%zu", &nNumOfAtoms);
//	printf("%zu\n", nNumOfAtoms);

//	puts("GetNumOfAtoms Complete");
	return;
}

void OUTCAR::GetNumOfElems() {
	char* str_ptr = nullptr;
	for( auto str : strData ) {
		str_ptr = strstr(str, "NELM");
		if( nullptr != str_ptr ) {
			break;
		}
	}

	sscanf(str_ptr, "%*s%*s%zu", &nNumOfElems);
	//printf("%zu\n", nNumOfElems);
//	puts("GetNumOfElems Complete");
	return;
}

void OUTCAR::GetEDiff() {
	char* str_ptr = nullptr;
	for( auto str : strData ) {
		str_ptr = strstr(str, "EDIFF");
		if( nullptr != str_ptr ) {
			break;
		}
	}

	sscanf(str_ptr, "%*s%*s%lf", &dEDiff);
	dEDiff = log10(dEDiff);
	//printf("%lf\n", dEDiff);
//	puts("GetNumOfEDiff Complete");
	return;
}

void OUTCAR::GetOtherVals() {
	// char* str_ptr;
	// Must call inputnIons(const size_t nIons) first, cause 'nIons' variable 
	// will be used below.
	nIterations = 0;
	dCPUTime = .0;
	dEnergy = .0;
	dLastEnergy = .0;
	nSteps = 1;
	dTotalTime = .0;
	dVolume = .0;
  bSpinPolarized = false;

	std::vector<double> darrMagnitudes(nIons, .0);
  std::vector<double> totForce(3, .0);
  size_t atom_ind = 0;

	for(size_t _i(0); _i!=strData.size(); ++_i) {
		char* str = strData[_i];
		//str_ptr = strstr(str, "Iteration");
		if( nullptr != strstr(str, "Iteration") ) {
			++nIterations;
		} 
		
		if ( nullptr != strstr(str, "TOTAL-FORCE") ) {
			darrForces.resize(nIons, std::vector<double> (3, .0));
			for(size_t j(0); j!=nIons; ++j){
				char str_tmp[200];
				strcpy(str_tmp, strData[_i + 2 + j]);
				
				double force[3];
				sscanf(str_tmp, "%*s%*s%*s%lf%lf%lf",
					&force[0], &force[1], &force[2]);
				darrForces[j][0] = force[0];
				darrForces[j][1] = force[1];
				darrForces[j][2] = force[2];
			}
			if ( true == bSelectiveDynamics ) {
				size_t len = darrForces.size();
				for(size_t i(0); i!=len; ++i) {
					for(size_t j(0); j!=3; ++j) {
						darrForces[i][j] *= darrActive[i][j];
					}
				}
			}

			size_t len = darrMagnitudes.size();
			double sum_of_magnitudes = .0;
			double max_of_magnitudes = -1e10;
			for(size_t i(0); i!=len; ++i) {
				double sum_of_squared_force = .0;
				for(size_t j(0); j!=3; ++j){
					sum_of_squared_force += darrForces[i][j] * darrForces[i][j];
				}
				darrMagnitudes[i] = sqrt(sum_of_squared_force);
				sum_of_magnitudes += darrMagnitudes[i];
				// max_of_magnitudes = ( darrMagnitudes[i] > max_of_magnitudes )
					// ? darrMagnitudes[i] : max_of_magnitudes;
        
        if(darrMagnitudes[i] > max_of_magnitudes){
          max_of_magnitudes = darrMagnitudes[i];
          atom_ind = i + 1;
        }

			}
			dAverage = sum_of_magnitudes / nIons;
			dMaxForce = max_of_magnitudes;
		}

		if ( false == bSpinPolarized && nullptr != strstr(str, " number of electron ") ){
			char* str_ptr = strtok(str, " ");
			size_t i(0);
			while( str_ptr != nullptr) {
				str_ptr = strtok(nullptr, " ");
				++i;
			}
      // printf("i = %zu @152 in OUTCAR.cpp\n", i); // For debug

			if ( i > 5 ) {
				bSpinPolarized = true;
				sscanf(str, "%*s%*s%*s%*s%*s%lf", &dMagmom);
			}
		}

		if ( nullptr != strstr(str, "LOOP:") ) {
			double time;
			sscanf(str, "%*s%*s%*s%*s%*s%*s%lf", &time);
			dCPUTime += time / 60;
		}

		if ( nullptr != strstr(str, "volume of cell")) {
				sscanf(str, "%*s%*s%*s%*s%lf", &dVolume);
		}

		if ( nullptr != strstr(str, "  free  energy") ) {
			dLastEnergy = dEnergy;
			sscanf(str, "%*s%*s%*s%*s%lf", &dEnergy);
			dDE = log10(std::abs(dEnergy - dLastEnergy + 1.0e-12));

      static const char directions[3] = {'x', 'y', 'z'};
      double maxf = 1e-10;
      size_t maxfdirect = 0;
      std::vector<double> maxfatom(darrForces[atom_ind - 1]);
      for(size_t i=0; i!=3; ++i) {
        // printf("%lf ", maxfatom[i]);
        if(std::abs(maxfatom[i]) > maxf) {
          maxf = std::abs(maxfatom[i]);
          maxfdirect = i;
        } 
      } 
      // puts("");

			char stepstr[200],  energystr[200],   logdestr[200],
			    iterstr[200],   avgstr[200],      maxfstr[200],
          timestr[200],   volstr[200],      magstr[200],
          maxfatomstr[200];

			sprintf(stepstr, "%4zu ", nSteps);
			sprintf(energystr, "%sEnergy%s:%9.3f  ", g_str_OKGREEN, g_str_ENDC, dEnergy);
			sprintf(logdestr, "%sLog|dE|%s:%4.1f  ", g_str_OKGREEN, g_str_ENDC, dDE);
			sprintf(iterstr, "%sSCF%s:%3zu  ", g_str_OKGREEN, g_str_ENDC, nIterations);
			sprintf(avgstr, "%sAvf|F|%s:%6.3f  ", g_str_OKGREEN, g_str_ENDC, dAverage);
			sprintf(maxfstr, "%sMax|F|%s:%6.3f  ", g_str_OKGREEN, g_str_ENDC, dMaxForce);
			sprintf(timestr, "%sTime%s:%6.2f  ", g_str_OKGREEN, g_str_ENDC, dCPUTime);
			sprintf(volstr, "%sVol.%s:%5.1f  ", g_str_OKGREEN, g_str_ENDC, dVolume);
      sprintf(maxfatomstr, "%sAtomInd%s:%3zu%c  ", g_str_OKGREEN, g_str_ENDC, atom_ind, directions[maxfdirect]);
      

            if (nNumOfElems == nIterations) {
                printf("%s", g_str_FAIL);
            }

            if (dDE < dEDiff) {
                printf("%s", g_str_OKGREEN);
            }

			if ( nIterations == nNumOfElems ) {
				printf("FAILED\n");
			}

      if ( dDE < dEDiff ) {
        printf("OK\n");
      }

			// if (true == bSpinPolarized) {
				sprintf(magstr, "%sMag%s: %6.2f  ", g_str_OKGREEN, g_str_ENDC, dMagmom);
				printf("%s%s%s%s%s%s%s%s%s%s\n", 
					stepstr, energystr, logdestr, iterstr, avgstr, maxfstr, maxfatomstr, 
          layout_vol ? volstr : "",
					layout_mag ? magstr : "", 
          timestr);
/*			
  			} else {
				printf("%s%s%s%s%s%s%s%s\n", 
					stepstr, energystr, logdestr, iterstr, avgstr, maxfstr, 
          layout_vol ? volstr : "",
					timestr);
    }
*/
            printf("%s", g_str_ENDC);

            ++nSteps;
			nIterations = 0;
			dTotalTime += dCPUTime;
			dCPUTime = 0;
		} //else if ()
	}

//	puts("OUTCAR::GetOtherVals() complete");
	return;
}

void OUTCAR::inputnIons(const size_t nIons) {
	this->nIons = nIons;
//	puts("OUTCAR::inputnIons complete");
	return;
}

void OUTCAR::inputSelectiveDynamics(const bool bSelectiveDynamics) {
	this->bSelectiveDynamics = bSelectiveDynamics;
//	puts("OUTCAR::inputSelectiveDynamics complete");
	return;
}

void OUTCAR::inputdarrActive(const std::vector<std::vector<double>> arr) {
	this->darrActive = arr;
//	puts("OUTCAR::inputdarrActive complete");
	return;
}

size_t OUTCAR::shownNumOfAtoms() {
//	printf("OUTCAR::shownNumOfAtoms complete. nNumOfAtoms = %zu\n", nNumOfAtoms);
	return nNumOfAtoms;
}

void OUTCAR::GetPosInfo() {
	bool bCatchednIons = false;
	bool bCatchedbSelectiveDynamics = false;
	this->bSelectiveDynamics = false;
	for(auto str : strData) {
		if (bCatchedbSelectiveDynamics && bCatchednIons) {
			break;
		}

		if (!bCatchednIons && nullptr != strstr(str, "number of dos")) {
			sscanf(str, "%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%*s%zu", &nIons);
			bCatchednIons = true;
			continue;
		}

		if (!bCatchedbSelectiveDynamics &&
				nullptr != strstr(str, " selective dynamics")) {
			bSelectiveDynamics = true;
			bCatchedbSelectiveDynamics = true;
			continue;
		}
	}
//	printf("nIons = %zu\n", nIons);


	//exit(EXIT_SUCCESS);
	return;
}
