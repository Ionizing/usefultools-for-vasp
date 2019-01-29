#include <stringops.hpp>

namespace ionizing {
  void ltrim(string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch);
      }));
  }

  void rtrim(string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch);
      }).base(), s.end());
  }

  void trim(string &s) {
      ltrim(s);
      rtrim(s);
  }

  string ltrim_copy(string s) {
      ltrim(s);
      return s;
  }

  string rtrim_copy(string s) {
      rtrim(s);
      return s;
  }

  string trim_copy(string s) {
      trim(s);
      return s;
  }

  VecStr split(string str, const char delim) {
    std::stringstream ss(std::move(str));
    VecStr out;
    if ('\0' == delim) {
      string tmp;
      while (!ss.eof()) {
        ss >> tmp;
        if ("" != tmp) {
          out.push_back(std::move(tmp));
        }
      }
    } else {
      string tmp;
      while (std::getline(ss, tmp, delim)) {
        out.push_back(std::move(tmp));
      }

      if (*(ss.str().crbegin()) == delim) {
        out.push_back(string());
      }
    }
    return out;
  }

  string join(VecStr str_vec, const char delim) {
    string out;
    bool is_first = true;
    for (auto e : str_vec) {
        if (is_first) {
          is_first = false;
        } else {
          out.push_back(delim);
        }
        out += std::move(e);
    }

    return out;
  }

  void toupper (string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](char ch){ return std::toupper(ch);
      });
  }

  void tolower (string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](char ch){ return std::tolower(ch);
      });
  }

  string toupper_copy (string str) {
    toupper(str);
    return str;
  }

  string tolower_copy (string str) {
    tolower (str);
    return str;
  }

  bool is_start_with(const string& src, const string& val) {
    if (src.length() < val.length()) {
      return false;
    } else {
      return src.compare(0, val.length(), val) == 0;
    }
  }

  bool is_end_with (const string& src, const string& val) {
    if (src.length() < val.length()) {
      return false;
    } else {
      return src.compare(src.length() - val.length(), src.length(), val) == 0;
    }
  }

  bool is_empty_or_white_space (const string& str) {
    return std::all_of(str.begin(), str.end(), [](char ch){
        return std::isspace(ch);
      });
  }

  string string_printf(const char* fmt, ...) {
    std::va_list va;
    va_start(va, fmt);
    string out;
    _string_printf(out, fmt, va);
    va_end(va);
    return out;
  }

  void _string_printf(string& dst, const char* fmt, va_list va) {
    long int length = 1 << 10;
    long int result;
    std::unique_ptr<char[]> buffer(new char[length + 1]); 

    va_list va_bak;
    va_copy(va_bak, va);
    result = vsnprintf(buffer.get(), length, fmt, va);
    va_end(va_bak);
    while (result <= 0 or result >= length) {
      length *= 2;
      buffer.reset(new char[length + 1]);
      va_copy(va, va_bak);
      result = vsnprintf(buffer.get(), length, fmt, va);
      va_end(va);
    }
    dst.append(buffer.get(), result);
  }
}
