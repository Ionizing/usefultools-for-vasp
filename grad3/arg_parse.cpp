#include "arg_parse.h"

void Parse(int argc, char** argv) {
  int c;
  static struct option long_options[] = {
    {"help",      no_argument,       0, 'h'},
    {"volume",    no_argument,       0, 'v'},
    {"magnet",    no_argument,       0, 'm'},
    {"outcar",    required_argument, 0, 'o'},
    {"poscar",    required_argument, 0, 'p'},
    {"without-entropy", no_argument, 0, 'e'},
    {"unconverged-atoms", no_argument, 0, 'u'},
    {NULL,        0,              NULL,   0}
  };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    /* Detect the end of the options.  */
  while((c = getopt_long(argc, argv, "ehmo:p:uv", 
          long_options, &option_index)) != -1){
    // printf("@22 in arg_parse.h\n");
    switch (c) {
      case 'h':
        PrintHelp();
        break;
        
      case 'v':
        layout_vol = true;
        // printf("layout_vol changed\n");
        break;
        
      case 'm':
        layout_mag = true;
        // printf("layout_mag changed\n");
        break;
        
      case 'o':
        strcpy(fname_OUTCAR, optarg);
        break;
        
      case 'p':
        strcpy(fname_POSCAR, optarg);
        break;
        
      case 'e':
        without_ent = true;
        break;

      case 'u':
        layout_unconverged_atoms = true;
        break;
        
      case '?':
        PrintHelp();
        break;
        
      default:
        std::exit(EXIT_FAILURE);
    }
  }
}

/*
char* getCmdOption(const char** begin,const char** end, char* option) {
    // char ** itr = std::find(begin, end, option);
    const char** itr = nullptr;
    for(itr = begin; itr != end; ++itr) {

    }

    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}
*/

void PrintHelp() {
/*
    printf("%s", g_str_FAIL);
    printf("Invalid input arguments\n");
    printf("%s", g_str_ENDC);

    printf("The default arguments are \'-p POSCAR -o OUTCAR\'");
    printf("Type -p \'POSCAR\' to specify POSCAR file.\n");
    printf("Type -o \'OUTCAR\' to specify OUTCAR file.\n");
    printf("Or you can specify both POSCAR and OUTCAR files.\n");
 */
  printf("usage: [[-o OUTCAR] [-p POSCAR] [-v] [-m]]\n");
  printf("Detailed usage:\n\
      -h --help    display help\n\
      -o --outcar  specify OUTCAR file manually\n\
      -p --poscar  specipy POSCAR file manually\n\
      -m --magnet  enable magnet layout\n\
      -v --volume  enable lattice volume layout\n\
      -e --without-entropy  layout Energy without entropy\n\
      -u --unconverged_atoms   layout unconverged atoms info\n");
 //	system("pause");
    exit(EXIT_FAILURE);
  return;
}

void PrintFileOpenError(const char* fname) {
  printf("%s", g_str_FAIL);
  printf("Unfortunately, %s open failed. Please check.\n", fname);
  printf("%s", g_str_ENDC);
//	system("pause");
	exit(EXIT_FAILURE);
	return;
}
