#include "headers.h"
#include "global.h"
#include <iostream>
#include <iomanip>
#include <tuple>

class OUTCAR {
public:
	OUTCAR(std::ifstream&);
	~OUTCAR();
	size_t shownNumOfAtoms();
	
	void GetPosInfo();	// called ONLY when POSCAR doesn't exist.
/*	
	these three functions below will be called 
	ONLY when POSCAR exists.
 */
	void inputnIons(const size_t); 
	void inputSelectiveDynamics(const bool);
	void inputdarrActive(const std::vector<std::vector<double>>);

	void GetOtherVals(); // called at last
private:
	std::vector<char*> strData;
	std::vector<std::vector<double>> darrForces;
	std::vector<std::vector<double>> darrActive;
  /* index, force, force_x, force_y, force_z  */
  std::vector<std::tuple<int, double, double, double, double>> darrUnconverged;
	size_t nNumOfElems, nNumOfAtoms, nIons;
	double dEDiff;

	size_t nIterations, nSteps;
	double dEnergy, dLastEnergy, dCPUTime,
        dDE, dMagmom, dVolume, dTotalTime,
        dAverage, dMaxForce, dFAccuracy;
	bool bSpinPolarized,
       bSelectiveDynamics;	// Defined in POSCAR.h
	
	void GetNumOfAtoms();
	void GetNumOfElems();
	void GetEDiff();
  void GetEDiffG();
};
