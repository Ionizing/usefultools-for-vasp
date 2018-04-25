# Supervised by Hao Ren (renh@upc.edu.cn)

This tiny utility is interpreted from renh's `grad3` which was written with python.

## grad3 usage

```bash
usage: [[-o OUTCAR] [-p POSCAR] [-v] [-m]]
Detailed usage:
      -h --help    display help
      -o --outcar  specify OUTCAR file manually
      -p --poscar  specipy POSCAR file manually
      -m --magnet  enable magnet layout
      -v --volume  enable lattice volume layout
```

![demo](http://owucpthrj.bkt.clouddn.com/FrzGKHOF7wyFkx5AV9ffH7AX3h7o)

## Make

Delete the tail after `--std=c++11` in `LFLAGS` line, and `make` . If `clang` compiler is not found, substitute it with `g++`, `icpc`, etc.

**Your compiler is required to support `C++11` standard.**
