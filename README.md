# cgrad

[![Build Status](https://travis-ci.org/Ionizing/usefultools-for-vasp.svg?branch=master)](https://travis-ci.org/Ionizing/usefultools-for-vasp)

```bash
An OUTCAR monitor for VASP relaxation calculations.
  Author: Ionizing PeterSmith_9@outlook.com
  Acknoledgement: renh, zqj
  If any issues come up or you have any feature requests, open issues at:
  https://github.com/Ionizing/usefultools-for-vasp/issues I'll appreciate it very much. ^_^

Usage:
  ./cgrad [OPTION...]

      --magmom           Output each ion step's magmom
      --volume           Output each ion step's lattice volume
      --poscar           Save each ion step's atom pos info as POSCAR frames
      --molden           Save each ion step's atom pos info as one molden
                         file
  -e, --without-entropy  Output TOTEN without entropy
      --example          Print an example usage of cgrad
  -h, --help             print help
      --direct           Use direct coordinate rather than Cartesian when
                         saving POSCAR
      --skip arg         Skip the head steps when saving as molden
  -o, --outcar arg       Specify OUTCAR file
      --prefix arg       Specify POSCAR file prefix when saving as POSCAR
                         frames
      --dir arg          Specify sub-directory in which POSCAR frames saved
```
## Make

`cd` into `grad3` directory and `make`.

`make test` is recommended, if you trust my code, just `make` and skip the test XD;

If `g++` compiler is not found, substitute it with `clang++`, `icpc`, etc.

The result of `make test` shows [here](./grad3/test_result.md)

**Your compiler is required to support `C++11` standard (full of C++11, i.e. gcc ver > 4.9).**

## Acknoledgement
Supervisors: [renh](https://github.com/renh), [zqj](https://github.com/QijingZheng);
Libraries used: [Eigen3](https://eigen.tuxfamily.org), [cxxopts](https://github.com/jarro2783/cxxopts), [Catch2](https://github.com/catchorg/Catch2)
