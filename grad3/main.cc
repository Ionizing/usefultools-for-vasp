#include <outcar.hpp>
#include <cxxopts.hpp>

#define _RESET   "\033[0m"
#define _BLACK   "\033[30m"      /* Black */
#define _RED     "\033[31m"      /* Red */
#define _GREEN   "\033[32m"      /* Green */
#define _YELLOW  "\033[33m"      /* Yellow */
#define _BLUE    "\033[34m"      /* Blue */
#define _MAGENTA "\033[35m"      /* Magenta */
#define _CYAN    "\033[36m"      /* Cyan */
#define _WHITE   "\033[37m"      /* White */
#define _BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define _BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define _BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define _BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define _BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define _BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define _BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define _BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

enum COLOR {
  RED, GREEN
};

std::string colorized_str(std::string str, const COLOR color) {
  std::string out;
  switch (color) {
    case RED     : out = _RED + str + _RESET;
                break;
    case GREEN   : out = _GREEN + str + _RESET;
                break;
    default: ;
  }
  return out;
}

const char* const GREEN_TOTEN = _GREEN "TOTEN"   _RESET;
const char* const GREEN_LGDE  = _GREEN "Lg|dE|"  _RESET;
const char* const GREEN_SCFS  = _GREEN "SCF"     _RESET;
const char* const GREEN_AVGF  = _GREEN "Avg|F|"  _RESET;
const char* const GREEN_MAXF  = _GREEN "Max|F|"  _RESET;
const char* const GREEN_INDEX = _GREEN "AtomInd" _RESET;
const char* const GREEN_VOL   = _GREEN "Vol."    _RESET;
const char* const GREEN_MAG   = _GREEN "Magmom"  _RESET;
const char* const GREEN_TIME  = _GREEN "Time"    _RESET;

using ionizing::OUTCAR;
using ionizing::string_printf;
using ionizing::VecStr;
using std::string;

int main(int argc, char* argv[]) {
  cxxopts::Options options{argv[0], 
_YELLOW  
"An OUTCAR monitor for VASP relaxation calculations.\n\
  Author: Ionizing PeterSmith_9@outlook.com\n\
  Acknoledgement: renh, zqj\n\
  If any issues come up or you have any feature requests, open issues at: \n\
  https://github.com/Ionizing/usefultools-for-vasp/issues \
I'll appreciate it very much. ^_^\n" _RESET};

  bool is_output_magmom   = false,
       is_output_volume   = false,
       is_output_poscars  = false,
       is_output_molden   = false,
       is_without_entropy = false,
       is_print_example   = false,
       is_print_help      = false,
       is_direct          = false,
       is_clean_format    = false;

  int  molden_skip        = 0;

  std::string outcar_name  {"OUTCAR"};
  std::string frame_prefix {"POSCAR_frame"};
  std::string frame_sub_dir{"poscar_frames"};

  options
    .allow_unrecognised_options()
    .add_options()
    ("magmom", "Output each ion step's magmom",
     cxxopts::value<bool>(is_output_magmom))
    ("volume", "Output each ion step's lattice volume",
     cxxopts::value<bool>(is_output_volume))
    ("poscar", "Save each ion step's atom pos info as POSCAR frames",
     cxxopts::value<bool>(is_output_poscars))
    ("molden", "Save each ion step's atom pos info as one molden file",
     cxxopts::value<bool>(is_output_molden))
    ("e, without-entropy", "Output TOTEN without entropy",
     cxxopts::value<bool>(is_without_entropy))
    ("example", "Print an example usage of cgrad",
     cxxopts::value<bool>(is_print_example))
    ("h, help", "print help",
     cxxopts::value<bool>(is_print_help))
    ("direct", "Use direct coordinate rather than Cartesian when saving POSCAR",
     cxxopts::value<bool>(is_direct))
    ("c, clean-format", "Output in clean format without color",
     cxxopts::value<bool>(is_clean_format))
    ("skip", "Skip the head steps when saving as molden",
     cxxopts::value<int>(molden_skip))
    ("o, outcar", "Specify OUTCAR file",
     cxxopts::value<std::string>(outcar_name))
    ("prefix", "Specify POSCAR file prefix when saving as POSCAR frames",
     cxxopts::value<std::string>(frame_prefix))
    ("dir", "Specify sub-directory in which POSCAR frames saved",
     cxxopts::value<std::string>(frame_sub_dir));

  auto result = options.parse(argc, argv);

// #define MAIN_DEBUG
#ifdef MAIN_DEBUG
  std::cout << "is_direct = " << is_direct << std::endl
            << "is_print_help = " << is_print_help << std::endl
            << "is_print_example = " << is_print_example << std::endl
            << "is_output_magmom = " << is_output_magmom << std::endl
            << "is_output_volume = " << is_output_volume << std::endl
            << "is_output_poscars  = " << is_output_poscars << std::endl
            << "is_output_molden = " << is_output_molden << std::endl
            << "is_clean_format = " << is_clean_format << std::endl
            << "is_without_entropy = " << is_without_entropy << std::endl
            << "molden_skip = " << molden_skip << std::endl
            << "outcar_name = " << outcar_name << std::endl
            << "frame_prefix = " << frame_prefix << std::endl
            << "frame_sub_dir = " << frame_sub_dir << std::endl;
#endif




  if (is_print_help) {
    std::cout << options.help();
    return 0;
  }

  if (is_print_example) {
    string str = string_printf("\
Example:\n\
    %s --magmom --volume --poscar --molden -e --skip=0 -o ./OUTCAR --prefix=POSCAR_frame --dir=frames",
        argv[0]);
    std::cout << str << std::endl;
    return 0;
  }

  OUTCAR outcar{outcar_name.c_str()}; 

  VecStr        elems  = outcar.getElems();
  OUTCAR::VecIt it_vec = outcar.getIterationVec();

  static const string fmt_complex = string_printf("%%3d \
%s %%11.5f  \
%s %%4.1f  \
%s %%3d  \
%s %%6.3f  \
%s %%6.3f  \
%s %%3d%%2s%%c  \
%s %%5.2f",
      GREEN_TOTEN,
      GREEN_LGDE,
      GREEN_SCFS,
      GREEN_MAXF,
      GREEN_AVGF,
      GREEN_INDEX,
      GREEN_TIME);

  static const string fmt_clean = string_printf("  %%4d\
 %%11.5f\
   %%4.1f\
  %%3d\
 %%6.3f\
 %%6.3f\
 %%3d %%2s %%c\
  %%5.2f");

  const string fmt = (is_clean_format) ? fmt_clean : fmt_complex;
  if (is_clean_format) {
    string line = "#nStep       TOTEN Lg|dE| nSCF Max|F| Avg|F| i_max|F|   Time";
    if (is_output_magmom) {
      line += "    Magmom";
    } else {}
    if (is_output_volume) {
      line += "    Volume";
    } else {}
    std::cout << line << std::endl;
  }

  for (int i=0; i!=static_cast<int>(it_vec.size()); ++i) {
    double toten = is_without_entropy ? it_vec[i]._totalEnergy_sigma_0 : it_vec[i]._totalEnergy;
    std::string line;
      line = string_printf(fmt.c_str(),
          i + 1, toten,
          std::log10(std::abs(it_vec[i]._deltaE)),
          it_vec[i]._nSCF,
          it_vec[i]._maxForce,
          it_vec[i]._averageF,
          it_vec[i]._maxIndex, it_vec[i]._maxAtomElem.c_str(), it_vec[i]._maxDirection,
          it_vec[i]._cpuTime / 60);

      if (is_output_magmom) {
        if (!is_clean_format) {
          line += string_printf(" %s %9.4f", GREEN_MAG, it_vec[i]._magmom);
        } else {
          line += string_printf(" %9.4f", it_vec[i]._magmom);
        }
      }

      if (is_output_volume) {
        if (!is_clean_format) {
          line += string_printf(" %s %7.3f", GREEN_VOL, it_vec[i]._volume);
        } else {
          line += string_printf(" %9.4f", it_vec[i]._volume);
        }
      }

    std::cout << line << std::endl;
  } // end for

  if (is_output_molden) {
    outcar.saveAsMolden(it_vec, "animate.molden", molden_skip);
  }

  if (is_output_poscars) {
    outcar.saveAsPoscar(it_vec, frame_prefix.c_str(), 
        frame_sub_dir.c_str(), is_direct);
  }
  return 0;
}
