### step 2 (04.14)

> add MinisatUP to cvc5 cmake compile configuration

- locate cadical package imports in cmake
  cvc5/CMakeLists.txt:
    532: find_package(CaDiCaL REQUIRED)
  cvc5/cmake/FindCaDiCaL.cmake (entire file)
  cvc5/src/CMakeLists.txt:
    1529: add_dependencies(cvc5-obj CaDiCaL)
    1530: target_include_directories(cvc5-obj SYSTEM PRIVATE ${CaDiCaL_INCLUDE_DIR})
    1531: target_link_libraries(cvc5 PRIVATE $<BUILD_INTERFACE:CaDiCaL> $<INSTALL_INTERFACE:cadical>)
  (ignored)
  cvc5/cmake/ConfigCompetition.cmake
  cvc5/test/regress/cli/CMakeLists.txt

- add minisatup package:
  cvc5/CMakeLists.txt:
    `find_package(MiniSatUP REQUIRED)`

- in minisat, change cmake to install package, package installed in usr/local, where cvc5 can find
  (see git commit)
  `sudo cmake --install .`

- add minisatup.a in cvc5 dependencies
  cvc5/src/CMakeLists.txt:
    `target_link_libraries(cvc5 PRIVATE MinisatUP::MinisatUP)`

- build cvc5 again:
  cvc5$
  `./contrib/cibw/before_all_linux.sh`

  cmake couldn't find MinisatUP package, add this line before find_package(MiniSatUP REQUIRED)
  `list(APPEND CMAKE_PREFIX_PATH "/usr/local")`

  still couldn't find, turned out to be a typo in uppercase, this alone now works
  cvc5/CMakeLists.txt:
    `find_package(MinisatUP REQUIRED)`

- /usr/bin/ld.gold: error: cannot find -lMinisatUP::MinisatUP:
  add `EXPORT_NAME MinisatUP` to minisat cmake

- built and linked successfully

### suggestions

- cvc5: git ignore /install
