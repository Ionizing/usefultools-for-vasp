#define CATCH_CONFIG_MAIN

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
