#include "headers.h"

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
	size_t nNumOfElems;
	size_t nNumOfAtoms;
	size_t nIons;
	double dEDiff;

	size_t nIterations;
	size_t nSteps;
  size_t nMaxForceInd;
	double dEnergy;
	double dLastEnergy;
	double dCPUTime;
	double dDE;
	double dMagmom;
	double dVolume;
	double dTotalTime;
	double dAverage;
	double dMaxForce;
	bool bSpinPolarized;
	bool bSelectiveDynamics;	// Defined in POSCAR.h
	
	void GetNumOfAtoms();
	void GetNumOfElems();
	void GetEDiff();
};
