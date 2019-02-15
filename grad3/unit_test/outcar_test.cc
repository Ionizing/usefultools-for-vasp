#define CATCH_CONFIG_MAIN
#define UNIT_TEST

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
  std::ifstream ifs("./unit_test/test2/OUTCAR");
  string content        = outcar.file_to_string(ifs);
  VecStr contentVector  = outcar.string_to_vecstr(content);

  WHEN("parse_magmom") {
    const char* magmom_str = " number of electron    1026.0000000 magnetization     135.0000000";
    const char* magmom_err = " number of electron    1026.0000000 magnetization";
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
    MatX3d forcepos_res;
    forcepos_res.resize(4, 3);
    forcepos_res <<  0.000716,  0.000716,  0.000716,
                    -0.000716, -0.000716,  0.000716,
                    -0.000716,  0.000716, -0.000716,
                     0.000716, -0.000716, -0.000716;

    REQUIRE_THROWS(outcar.parse_atom_force_pos(forcepos_vec));
    outcar._incar._NIONS = 4;
    REQUIRE(forcepos_res == outcar.parse_atom_force_pos(forcepos_vec));
  }

}
