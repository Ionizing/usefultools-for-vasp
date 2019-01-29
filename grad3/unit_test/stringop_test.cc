#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <stringops.hpp>

TEST_CASE("String Trimming") {
  const char* str_org = "\t \v\nabc def g hi \n \v";
  const char* str_ltrim = "abc def g hi \n \v";
  const char* str_rtrim = "\t \v\nabc def g hi";
  const char* str_trim = "abc def g hi";

  REQUIRE(str_ltrim == ionizing::ltrim_copy(str_org));
  REQUIRE(str_rtrim == ionizing::rtrim_copy(str_org));
  REQUIRE(str_trim  == ionizing::trim_copy(str_org));

  REQUIRE("" == ionizing::trim_copy(""));
}

TEST_CASE("String split & join") {
  using ionizing::VecStr;
  using ionizing::split;
  using ionizing::join;
  WHEN("Splitting with specified delimiter") {
    const char* str_org = ",,a,b,,,c,d,,";
    VecStr str_vec = {"", "", "a", "b", "", "", "c", "d", "", ""};
    REQUIRE(str_vec == split(str_org, ','));
    REQUIRE(str_org == join(split(str_org, ','), ','));

    VecStr str_vec_empty;
    REQUIRE(str_vec_empty == split("", ','));
    REQUIRE("" == join(split("", ','), ','));

    const char* str_org_2 = "abc def ghi";
    VecStr str_vec_unchanged = {"abc def ghi"};
    REQUIRE(str_vec_unchanged == split(str_org_2, ','));
    REQUIRE(str_org_2 == join(split(str_org_2, ','), ','));
  }
  WHEN("Splitting with default delimiter (space)") {
    const char* str_org = "  LiamHuang       Ionizing   ";
    VecStr str_vec = {"LiamHuang", "Ionizing"};
    REQUIRE(str_vec == split(str_org));

    REQUIRE("LiamHuang Ionizing" == join(split(str_org)));
  }
}

TEST_CASE("To Upper & To Lower") {
  using ionizing::toupper_copy;
  using ionizing::tolower_copy;

  const char* str_org = "A b,c.d/e\nf\vG  H,,I..J//K\n\nL\n";
  const char* str_upper = "A B,C.D/E\nF\vG  H,,I..J//K\n\nL\n";
  const char* str_lower = "a b,c.d/e\nf\vg  h,,i..j//k\n\nl\n";
  const char* str_empty = "";

  REQUIRE(str_upper == toupper_copy(str_org));
  REQUIRE(str_lower == tolower_copy(str_org));
  REQUIRE(str_empty == tolower_copy(str_empty));
  REQUIRE(str_empty == toupper_copy(str_empty));
}

TEST_CASE("Starts With & Ends With") {
  using ionizing::is_start_with;
  using ionizing::is_end_with;

  const char* str_org = "   Abc.,.,.\ndef";
  const char* prefix  = "   Abc";
  const char* postfix =       ".,.,.\ndef";
  const char* false_fix = " g ";

  REQUIRE((is_start_with(str_org, prefix) and !is_end_with(str_org, prefix)));
  REQUIRE((is_end_with(str_org, postfix) and !is_start_with(str_org, postfix)));
  REQUIRE_FALSE((is_start_with(str_org, false_fix) or is_end_with(str_org, false_fix)));
}

TEST_CASE("Is empty or white space") {
  using ionizing::is_empty_or_white_space;
  WHEN("string is \"\"") {
    const char* str = "";
    REQUIRE(is_empty_or_white_space(str));
  }
  WHEN("string is full of return \\v \\r and etc") {
    const char* str = "   \n\v\t ";
    REQUIRE(is_empty_or_white_space(str));
  }
}

TEST_CASE("String Printf") {
  using ionizing::string_printf;

  const char* fmt = "result = %d %s %.5f";
  int integer = 10;
  const char* str = "printf";
  float float_num = 20.34567;

  const char* result = "result = 10 printf 20.34567";
  REQUIRE(result == string_printf(fmt, integer, str, float_num));

  WHEN("test edge case") {
    const int length = 1 << 11;
    char str[length + 1];
    for (int i=0; i!=length; ++i) {
      str[i] = i % 26 + 'a';
    }

    str[length] = '\0';

    REQUIRE(str == string_printf("%s", str));
  }
}
