#define CATCH_CONFIG_MAIN
#define UNIT_TEST
// #define OUTCAR_DEBUG

#include <outcar.hpp>
#include <catch.hpp>

using namespace ionizing;

OUTCAR outcar;

TEST_CASE("parse_elems debug") {
  VecStr str_vec {
    "   VRHFIN =Cu: d10 p1                ",
    "   VRHFIN =C: s2p2                   ",
    "   VRHFIN =H: ultrasoft test         " };
  VecStr split_result {
    "VRHFIN", "=Cu:", "d10", "p1" };
  REQUIRE(split_result == split(str_vec[0]));

  string elem_name = split(str_vec[0])[1];

  elem_name.pop_back();
  REQUIRE(elem_name == "=Cu");
  
  elem_name.erase(0, 1);
  REQUIRE(elem_name == "Cu");
}

TEST_CASE("parse_elems test") {
  VecStr str_vec {
    "   VRHFIN =Cu: d10 p1                ",
    "   VRHFIN =C: s2p2                   ",
    "   VRHFIN =H: ultrasoft test         " };

  VecStr result {
    "Cu",
    "C",
    "H" };

  REQUIRE(result == outcar.parse_elems(str_vec));
}

TEST_CASE("Read OUTCAR to one string and VecStr") {
  std::ifstream ifs("./unit_test/test1/OUTCAR");
  string content = outcar.file_to_string(ifs);

  WHEN("into one string") {
    string tail(content.end() - 7, content.end());
    string head(content.begin(), content.begin() + 10);
    REQUIRE(tail == "125489\n");
    REQUIRE(head == " vasp.5.3.");
  }

  WHEN("into VecStr") {
    VecStr vec_str = outcar.string_to_vecstr(content);
    string head {" vasp.5.3.5 31Mar14 (build Nov 09 2015 12:05:13) complex                        "};
    string tail {"                 Voluntary context switches:       125489"};

    REQUIRE(vec_str.front() == head);
    REQUIRE(vec_str.back() == tail);
  }

  WHEN("from raw file") {
    std::ifstream ifs2("./unit_test/test3/OUTCAR");
    VecStr elem_name { "Cu", "C", "H" };

    REQUIRE(outcar.test_parse_elem(ifs2) == elem_name);
  }
}

TEST_CASE("Parse Lattice Vector") {
    VecStr raw_lines {
      " A1 = (   9.0750000000,  -9.0750000000,   0.0000000000)",
      " A2 = (   9.0750000000,   9.0750000000,   0.0000000000)",
      " A3 = (   0.0000000000,   0.0000000000,  29.0400000000)"
    };
    
    Mat33d result;
    result << 9.0750000000,  -9.0750000000,   0.0000000000,
              9.0750000000,   9.0750000000,   0.0000000000,
              0.0000000000,   0.0000000000,  29.0400000000;

  WHEN("private member") {
    REQUIRE(outcar.parse_lattice_vectors(raw_lines) == result);

    VecStr broken_lines_1 {raw_lines};
    VecStr broken_lines_2 {raw_lines};
    broken_lines_1.pop_back();
    broken_lines_2[2][1] = 'a';

    REQUIRE_THROWS(outcar.parse_lattice_vectors(broken_lines_1));
    REQUIRE_THROWS(outcar.parse_lattice_vectors(broken_lines_2));
  }

  WHEN("from raw file") {
    std::ifstream ifs("./unit_test/test3/OUTCAR");
    string content = outcar.file_to_string(ifs);
    VecStr contentVector = outcar.string_to_vecstr(content);

    REQUIRE(outcar.parseLatticeVectors(contentVector) == result);
  }
}

TEST_CASE("Parse KPoints") {
  string NKPTS_line = "   k-points           NKPTS =     20   k-points in BZ     NKDIM =     20   number of bands    NBANDS=     81";
  VecStr KPoints_lines {
       "0.06250000  0.06250000  0.06250000       0.016",
       "0.18750000  0.06250000  0.06250000       0.047",
       "0.31250000  0.06250000  0.06250000       0.047",
       "0.43750000  0.06250000  0.06250000       0.047",
       "0.18750000  0.18750000  0.06250000       0.047",
       "0.31250000  0.18750000  0.06250000       0.094",
       "0.43750000  0.18750000  0.06250000       0.094",
       "0.31250000  0.31250000  0.06250000       0.047",
       "0.43750000  0.31250000  0.06250000       0.094",
       "0.43750000  0.43750000  0.06250000       0.047",
       "0.18750000  0.18750000  0.18750000       0.016",
       "0.31250000  0.18750000  0.18750000       0.047",
       "0.43750000  0.18750000  0.18750000       0.047",
       "0.31250000  0.31250000  0.18750000       0.047",
       "0.43750000  0.31250000  0.18750000       0.094",
       "0.43750000  0.43750000  0.18750000       0.047",
       "0.31250000  0.31250000  0.31250000       0.016",
       "0.43750000  0.31250000  0.31250000       0.047",
       "0.43750000  0.43750000  0.31250000       0.047",
       "0.43750000  0.43750000  0.43750000       0.016",
  };

  MatX3d kpoint_result;
  kpoint_result.resize(20, 3);
  kpoint_result << 
         0.06250000, 0.06250000, 0.06250000,
         0.18750000, 0.06250000, 0.06250000,
         0.31250000, 0.06250000, 0.06250000,
         0.43750000, 0.06250000, 0.06250000,
         0.18750000, 0.18750000, 0.06250000,
         0.31250000, 0.18750000, 0.06250000,
         0.43750000, 0.18750000, 0.06250000,
         0.31250000, 0.31250000, 0.06250000,
         0.43750000, 0.31250000, 0.06250000,
         0.43750000, 0.43750000, 0.06250000,
         0.18750000, 0.18750000, 0.18750000,
         0.31250000, 0.18750000, 0.18750000,
         0.43750000, 0.18750000, 0.18750000,
         0.31250000, 0.31250000, 0.18750000,
         0.43750000, 0.31250000, 0.18750000,
         0.43750000, 0.43750000, 0.18750000,
         0.31250000, 0.31250000, 0.31250000,
         0.43750000, 0.31250000, 0.31250000,
         0.43750000, 0.43750000, 0.31250000,
         0.43750000, 0.43750000, 0.43750000;
  
  WHEN("Normal case") {
    REQUIRE(outcar.parse_nkpts(NKPTS_line) == 20);
    REQUIRE(outcar._incar._NKPTS == 20);
    REQUIRE(outcar.parse_kpoints(KPoints_lines) == kpoint_result);
  }

  WHEN("Negative NKPTS") {
    const string line = "   k-points           NKPTS =     -8   k-points in BZ     NKDIM =     20   number of bands    NBANDS=     81";
    REQUIRE_THROWS(outcar.parse_nkpts(line));
    REQUIRE(-1 == outcar._incar._NKPTS);
  }

  WHEN("Invalid NKPTS line prefix") {
    const string line = "  k-points           NKPTS =     -8   k-points in BZ     NKDIM =     20   number of bands    NBANDS=     81";
    REQUIRE_THROWS(outcar.parse_nkpts(line));
    REQUIRE(-1 == outcar._incar._NKPTS);
  }

  WHEN("Missing NKPTS data") {
    const string line = "   k-points           NKPTS =   ";
    REQUIRE_THROWS(outcar.parse_nkpts(line));
    REQUIRE(-1 == outcar._incar._NKPTS);
  }

  WHEN("KPoint array damaged") {
    outcar.parse_nkpts(NKPTS_line);
    KPoints_lines.pop_back();
    REQUIRE_THROWS(outcar.parse_kpoints(KPoints_lines));
  }

  WHEN("KPoint array contains invalid charactor") {
    KPoints_lines[19][11] = 'a';
    outcar.parse_nkpts(NKPTS_line);
    REQUIRE_THROWS(outcar.parse_kpoints(KPoints_lines));
  }

  WHEN("from raw file") {
    std::ifstream ifs("./unit_test/test2/OUTCAR");
    string content = outcar.file_to_string(ifs);
    VecStr contentVector = outcar.string_to_vecstr(content);

    REQUIRE(kpoint_result == outcar.parseKPoints(contentVector));
  }
}

TEST_CASE("INCAR Parameters in OUTCAR") {
  string ediff_line         = "   EDIFF  = 0.1E-05   stopping-criterion for ELM";
  string ediffg_line        = "   EDIFFG = -.3E-01   stopping-criterion for IOM";
  string encut_line         = "   ENCUT  =  500.0 eV  36.75 Ry    6.06 a.u.  30.47 23.11 54.70*2*pi/ulx,y,z ";
  string ibrion_line        = "   IBRION =      1    ionic relax: 0-MD 1-quasi-New 2-CG";
  string isif_line          = "   ISIF   =      3    stress and relaxation";
  string ispin_line         = "   ISPIN  =      1    spin polarized calculation?";
  string lnoncollinear_line = "   LNONCOLLINEAR =      F non collinear calculations";
  string lsorbit_line       = "   LSORBIT =      F    spin-orbit coupling";
  string lorbit_line        = "   LORBIT =      0    0 simple, 1 ext, 2 COOP (PROOUT)";
  string nbands_line        = "   k-points           NKPTS =      9   k-points in BZ     NKDIM =      9   number of bands    NBANDS=     98";
  string nelmin_line        = "   NELM   =     60;   NELMIN=  6; NELMDL= -5     # of ELM steps""   NELM   =     60;   NELMIN=  6; NELMDL= -5     # of ELM steps";
  string nions_line         = "   number of dos      NEDOS =    301   number of ions     NIONS =     27";
  string nsw_line           = "   NSW    =    200    number of steps for IOM";
  string nkpts_line         = nbands_line;
  
  WHEN("parse_ediff") {
    REQUIRE(1e-6 == outcar.parse_ediff(ediff_line));
    REQUIRE(1e-6 == outcar._incar._EDIFF);
  }
  WHEN("parse_ediffg") {
    REQUIRE(-3e-2 == outcar.parse_ediffg(ediffg_line));
    REQUIRE(-3e-2 == outcar._incar._EDIFFG);
  }
  WHEN("parse_encut") {
    REQUIRE(500.0 == outcar.parse_encut(encut_line));
    REQUIRE(500.0 == outcar._incar._ENCUT);
  }
  WHEN("parse_ibrion") {
    REQUIRE(1 == outcar.parse_ibrion(ibrion_line));
    REQUIRE(1 == outcar._incar._IBRION);
  }
  WHEN("parse_isif") {
    REQUIRE(3 == outcar.parse_isif(isif_line));
    REQUIRE(3 == outcar._incar._ISIF);
  }
  WHEN("parse_ispin") {
    REQUIRE(1 == outcar.parse_ispin(ispin_line));
    REQUIRE(1 == outcar._incar._ISPIN);
  }
  WHEN("parse_lnoncollinear") {
    REQUIRE(0 == outcar.parse_lnoncollinear(lnoncollinear_line));
    REQUIRE(0 == outcar._incar._LNONCOLLINEAR);
  }
  WHEN("parse_lsorbit") {
    REQUIRE(0 == outcar.parse_lsorbit(lsorbit_line));
    REQUIRE(0 == outcar._incar._LSORBIT);
  }
  WHEN("parse_lorbit") {
    REQUIRE(0 == outcar.parse_lorbit(lorbit_line));
    REQUIRE(0 == outcar._incar._LORBIT);
  }
  WHEN("parse_nbands") {
    REQUIRE(98 == outcar.parse_nbands(nbands_line));
    REQUIRE(98 == outcar._incar._NBANDS);
  }
  WHEN("parse_nelmin") {
    REQUIRE(6 == outcar.parse_nelmin(nelmin_line));
    REQUIRE(6 == outcar._incar._NELMIN);
  }
  WHEN("parse_nions") {
    REQUIRE(27 == outcar.parse_nions(nions_line));
    REQUIRE(27 == outcar._incar._NIONS);
  }
  WHEN("parse_nsw") {
    REQUIRE(200 == outcar.parse_nsw(nsw_line));
    REQUIRE(200 == outcar._incar._NSW);
  }
  WHEN("parse_nkpts") {
    REQUIRE(9 == outcar.parse_nkpts(nkpts_line));
    REQUIRE(9 == outcar._incar._NKPTS);
  }

  std::ifstream ifs("./unit_test/test2/OUTCAR");
  string content       = outcar.file_to_string(ifs);
  VecStr contentVector = outcar.string_to_vecstr(content);
  outcar.parseINCAR(contentVector);

  INCAR incar_result;
  incar_result._EDIFF         = 1e-6;
  incar_result._EDIFFG        = -1e-2;
  incar_result._ENCUT         = 400.0;
  incar_result._IBRION        = 1;
  incar_result._ISIF          = 3;
  incar_result._ISPIN         = 1;
  incar_result._LNONCOLLINEAR = 0;
  incar_result._LORBIT        = 0;
  incar_result._LSORBIT       = 0;
  incar_result._NBANDS        = 81;
  incar_result._NELMIN        = 2;
  incar_result._NIONS         = 32;
  incar_result._NSW           = 100;
  incar_result._NKPTS         = 20;

  WHEN("EDIFF")
    REQUIRE(incar_result._EDIFF         == outcar._incar._EDIFF        );
  WHEN("EDIFFG")
    REQUIRE(incar_result._EDIFFG        == outcar._incar._EDIFFG       );
  WHEN("ENCUT")
    REQUIRE(incar_result._ENCUT         == outcar._incar._ENCUT        );
  WHEN("IBRION")
    REQUIRE(incar_result._IBRION        == outcar._incar._IBRION       );
  WHEN("ISIF")
    REQUIRE(incar_result._ISIF          == outcar._incar._ISIF         );
  WHEN("ISPIN")
    REQUIRE(incar_result._ISPIN         == outcar._incar._ISPIN        );
  WHEN("LNONCOLLINEAR")
    REQUIRE(incar_result._LNONCOLLINEAR == outcar._incar._LNONCOLLINEAR);
  WHEN("LORBIT")
    REQUIRE(incar_result._LORBIT        == outcar._incar._LORBIT       );
  WHEN("LSORBIT")
    REQUIRE(incar_result._LSORBIT       == outcar._incar._LSORBIT      );
  WHEN("NBANDS")
    REQUIRE(incar_result._NBANDS        == outcar._incar._NBANDS       );
  WHEN("NELMIN")
    REQUIRE(incar_result._NELMIN        == outcar._incar._NELMIN       );
  WHEN("NIONS")
    REQUIRE(incar_result._NIONS         == outcar._incar._NIONS        );
  WHEN("NSW")
    REQUIRE(incar_result._NSW           == outcar._incar._NSW          );
  WHEN("NKPTS")
    REQUIRE(incar_result._NKPTS         == outcar._incar._NKPTS        );
}


TEST_CASE("Parse Iteration") {

  WHEN("parse_magmom") {
    const char* magmom_str = " number of electron    1026.0000000 magnetization     135.0000000";
    const char* magmom_err = " number of electron    1026.0000000 magnetization";
    REQUIRE_THROWS(outcar.parse_magmom(magmom_str));
    outcar._incar._ISPIN = 2;
    REQUIRE(135.00 == outcar.parse_magmom(magmom_str));
    REQUIRE(135.00 == outcar.tmpIteration._magmom);
    REQUIRE_THROWS(outcar.parse_magmom(magmom_err));
  }

  WHEN("parse_lattice_volume") {
    const char* volume_str = "  volume of cell :      425.26";
    const char* voluem_err = "  volume of cell :";
    REQUIRE(425.26 == outcar.parse_lattice_volume(volume_str));
    REQUIRE(425.26 == outcar.tmpIteration._volume);
    REQUIRE_THROWS(outcar.parse_lattice_volume(voluem_err));
  }

  WHEN("parse_lattice") {
    VecStr lattice_vecstr {
      "     7.519999981  0.000000000  0.000000000     0.132978724  0.000000000  0.000000000",
      "     0.000000000  7.519999981  0.000000000     0.000000000  0.132978724  0.000000000",
      "     0.000000000  0.000000000  7.519999981     0.000000000  0.000000000  0.132978724" 
    };
    Mat33d lattice_result;
    lattice_result << 7.519999981, 0.000000000, 0.000000000,
                      0.000000000, 7.519999981, 0.000000000,
                      0.000000000, 0.000000000, 7.519999981;
    REQUIRE(lattice_result == outcar.parse_lattice(lattice_vecstr));
  }

  WHEN("parse_atom_force_pos") {
    VecStr forcepos_vec {
        "     0.52935      0.52935      0.52935         0.000716      0.000716      0.000716",
        "     6.96491      3.21778      4.27649        -0.000716     -0.000716      0.000716",
        "     3.21778      4.27649      6.96491        -0.000716      0.000716     -0.000716",
        "     4.27649      6.96491      3.21778         0.000716     -0.000716     -0.000716"};
    MatX3d force_res;
    force_res.resize(4, 3);
    force_res <<  0.000716,  0.000716,  0.000716,
                 -0.000716, -0.000716,  0.000716,
                 -0.000716,  0.000716, -0.000716,
                  0.000716, -0.000716, -0.000716;

    REQUIRE_THROWS(outcar.parse_atom_force_pos(forcepos_vec));
    outcar._incar._NIONS = 4;
    REQUIRE(force_res == outcar.parse_atom_force_pos(forcepos_vec));
  }

  WHEN("parse_toten") {
    const char* toten_str = "  energy  without entropy=    -1059.00022771  energy(sigma->0) =    -1059.00022771";
    const char* toten_err = "energy  without entropy=    -1059.00022771  energy(sigma->0) =    -1059.00022771";
    REQUIRE(-1059.00022771 == outcar.parse_toten(toten_str));
    REQUIRE(-1059.00022771 == outcar.tmpIteration._totalEnergy_sigma_0);
    REQUIRE(outcar.parse_toten(toten_err));
  }

  WHEN("parse_cpu_time") {
    const char* cputime_str = "     LOOP+:  cpu time  428.5440: real time  468.0720";
    const char* cputime_err = "     LOOP+:  cpu tim  428.5440: real time  468.0720";
    REQUIRE(428.5440 == outcar.parse_cpu_time(cputime_str));
    REQUIRE(428.5440 == outcar.tmpIteration._cpuTime);
    REQUIRE_THROWS(outcar.parse_cpu_time(cputime_err));
  }

  WHEN("calc_atom_force") {
    Mat33d atom_force_dirs;
    atom_force_dirs << 1, 2, 3,
                       4, 5, 6,
                       7, 8, 9;
    Vecd atom_force_res;
    atom_force_res.resize(3);
    for (long i=0 ;i!=atom_force_dirs.rows(); ++i) {
      atom_force_res(i) = atom_force_dirs.row(i).norm();
    }

    REQUIRE(atom_force_res == outcar.calc_atom_force(atom_force_dirs));
  }

  std::ifstream ifs("./unit_test/test6/OUTCAR");
  string content        = outcar.file_to_string(ifs);
  VecStr contentVector  = outcar.string_to_vecstr(content);

  WHEN("parse_iteration") {
    int it_begin  = 0;
    int it_end    = contentVector.size();
    static const string IT_START_PREFIX = 
      "--------------------------------------- Iteration";
    static const string IT_END_PREFIX   = "     LOOP+";

    for (size_t i=0; i!=contentVector.size(); ++i) {
      if (is_start_with(contentVector[i], IT_START_PREFIX)) {
        it_begin = i;
        continue;
      } else if (is_start_with(contentVector[i], IT_END_PREFIX)) {
        it_end   = i + 1;
        break;
      } else {}
    }

    VecStr it_lines {
      contentVector.begin() + it_begin, contentVector.begin() + it_end };

    /*
     * outcar._incar._ISPIN = 1;
     * REQUIRE_THROWS(outcar.parse_iteration(it_lines));
     */
    outcar._incar._NIONS = 135;
    outcar._incar._ISPIN = 2;
    OUTCAR::IonIteration iteration;
    REQUIRE_NOTHROW(iteration = outcar.parse_iteration(it_lines));
    REQUIRE( -1059.00022771 == iteration._deltaE);
    REQUIRE(    -0.0000203  == iteration._magmom);
    REQUIRE(  4553.41       == iteration._volume);
    REQUIRE(  2094.2404     == iteration._cpuTime);
    REQUIRE( -1059.00022771 == iteration._totalEnergy);
  }

  WHEN("parse_iteration_vec") {
    OUTCAR::VecIt it_vec_result;
    outcar._incar._NIONS = 135;
    outcar._incar._ISPIN = 2;
    REQUIRE_NOTHROW(it_vec_result = outcar.parse_iteration_vec(contentVector));
    REQUIRE(17 == it_vec_result.size());

    REQUIRE(it_vec_result.back()._cpuTime == 428.5440);
    REQUIRE(it_vec_result.back()._totalEnergy == -1062.01695778);
  }
}
