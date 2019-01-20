#include "headers.h"

class POSCAR {
public:
	POSCAR(std::ifstream&, const size_t);
	~POSCAR();
	size_t shownIons();
	bool showbSelective_Dynamics();
	std::vector<std::vector<double>> showdarrActive();
private:
	size_t nIons;
	size_t nAtoms;
	bool bSelective_Dynamics;
	std::vector<std::vector<double>> darrActive;
	
	void inputAtoms(const size_t);
};