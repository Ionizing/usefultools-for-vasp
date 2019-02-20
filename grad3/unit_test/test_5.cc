#define CATCH_CONFIG_MAIN
#define UNIT_TEST

#include <outcar.hpp>
#include <catch.hpp>

using namespace ionizing;

OUTCAR outcar{"./test1/OUTCAR"};

std::ifstream ifs{"./test5/OUTCAR"};
string content       = outcar.file_to_string(ifs);
VecStr contentVector = outcar.string_to_vecstr(content);

TEST_CASE("Elements") {
  VecStr elem_result = {
    "Mn",
    "O" };
  std::vector<int> ions_per_type{22, 26};

  REQUIRE(outcar.parseElems(contentVector) == elem_result);
  REQUIRE(outcar._atomsPerElem             == ions_per_type);
}

TEST_CASE("KPoints"){
  REQUIRE_NOTHROW(outcar.parseKPoints(contentVector));
  REQUIRE(16 == outcar._kpoints.rows());
}

TEST_CASE("INCAR") {
  INCAR result = outcar.parseINCAR(contentVector);

  WHEN("EDIFF")
    REQUIRE(result._EDIFF          == 1E-05);
  WHEN("EDIFFG")
    REQUIRE(result._EDIFFG         == -0.01);
  WHEN("ENCUT")
    REQUIRE(result._ENCUT          == 500.0);
  WHEN("IBRION")
    REQUIRE(result._IBRION         == 2);
  WHEN("ISIF")
    REQUIRE(result._ISIF           == 2);
  WHEN("ISPIN")
    REQUIRE(result._ISPIN          == 2);
  WHEN("LNONCOLLINEAR")
    REQUIRE(result._LNONCOLLINEAR  == false);
  WHEN("LSORBIT")
    REQUIRE(result._LSORBIT        == false);
  WHEN("LORBIT")
    REQUIRE(result._LORBIT         == 0);
  WHEN("NBANDS")
    REQUIRE(result._NBANDS         == 224);
  WHEN("NELMIN")
    REQUIRE(result._NELMIN         == 2);
  WHEN("NIONS")
    REQUIRE(result._NIONS          == 48);
  WHEN("NKPTS")
    REQUIRE(result._NKPTS          == 16);
  WHEN("NSW")
    REQUIRE(result._NSW            == 500);
}

TEST_CASE("Iteration") {
  outcar.parseElems(contentVector);
  outcar.parseKPoints(contentVector);
  outcar.parseINCAR(contentVector);

  REQUIRE_NOTHROW(outcar.parseIterationVec(contentVector));
}
