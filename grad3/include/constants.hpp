#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <numeric>
#include <complex>

namespace ionizing {
  static constexpr double AVOGADRO    =   6.0221367E23;             // Avogadro constant
  static constexpr double AU_TO_A     =   0.529177249;              // a.u. to angstrom
  static constexpr double BOHR_TO_ANG =   0.529177249;              // Bohr radius to angstrom
  static constexpr double C_LIGHT     = 137.037;                    // Light speed in a.u.
  static constexpr double CAL_TO_J    =   4.1840;                   // Calorie in joule
  static constexpr double DEBYE       =   3.336E-30;                // Coulomb m
  static constexpr double EV_TO_J     =   1.60217733E-19;           // eV to J
  static constexpr double H_PLANCK    =   6.6260755E-34;            // Planck constant J s
  static constexpr double HATREE_TO_J =   4.3597482E18;             // Hatree to joule
  static constexpr double K_B_EV      =   8.6173857E-5;             // Boltzmann constant in eV/K
  static constexpr double M_ELECT     =   9.10938356E-31;           // Mass of electron
  static constexpr double M_PROTON    =   1.672621898E-27;          // Mass of proton
  static constexpr double M_AU        =   1.660539040E-27;          // Unit mass in a.u.
  static constexpr double PI          =   3.14159265358979323846;   // Pi
  static constexpr double RY_TO_EV    =  13.605693009;              // Rydberg to eV

  static constexpr double EV_TO_KCAL     = EV_TO_J * AVOGADRO / 1000 / CAL_TO_J;
  static constexpr double HBAR           = H_PLANCK / PI / 2;
  static constexpr double HATREE_TO_KCAL = HATREE_TO_J * AVOGADRO / 100 / CAL_TO_J;
  static constexpr double K_B            = K_B_EV * EV_TO_J;         // Boltzmann constant in J/K
  static constexpr double PIx2           = 2 * PI;                   // Pi * 2
  static constexpr double HBAR2D2ME      = RY_TO_EV * AU_TO_A * AU_TO_A;
                             
  static constexpr std::complex<double> IMAGE_UNIT{0, 1};
  static constexpr std::complex<double> PIx2_COMPLEX(0, PIx2);
}                            
#endif // CONSTANTS_H        
