# step 5 (04.15)

> implement addVar in minisatup; fix `bool failed(int lit)`

- minisatup create var with `ensureVar` during `add`

- debug failure: `cvc5 -i --check-unsat-cores test/regress/cli/regress0/arrays/issue4546-2.smt2`
  Internal error detected SolverEngine::checkUnsatCore(): produced core was satisfiable.

- debugging the function `bool failed(int lit)`, maybe it's incorrect

- replace LSet with `std::set` for better debugging and value checking

(begin cmake config problems)

- remove installed files in /usr/local and rebuild, now cvc5 build failed, couldn't find `minisatup.h`

- cvc5/src/CMakeLists.txt: try adding this
    add_dependencies(cvc5-obj MinisatUP::MinisatUP)
    target_include_directories(cvc5-obj SYSTEM PRIVATE MinisatUP::MinisatUP)
    target_link_libraries(cvc5 PRIVATE MinisatUP::MinisatUP)
  or following the original format using BUILD_INTERFACE and something alike, doesn't work

- cvc5/src/CMakeLists.txt: change to
    `target_link_libraries(cvc5-obj PRIVATE MinisatUP::MinisatUP)`
  the original was `cvc5` not `cvc5-obj` but the latter requires the header to be included
  this line includes the library and the header at the same time, because the header is attached to the library by:
  install/lib/cmake/MinisatUP/MinisatUPTargets.cmake:
    add_library(MinisatUP::MinisatUP STATIC IMPORTED)
    set_target_properties(MinisatUP::MinisatUP PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
    )
  if only to include the header but not the library:  
    get_target_property(MINISATUP_INCLUDE_DIRS MinisatUP::MinisatUP INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(MyApp PRIVATE ${MINISATUP_INCLUDE_DIRS})

- symbol lookup error again, both lines are needed
  cvc5/src/CMakeLists.txt: change to
    add_dependencies(cvc5-obj MinisatUP::MinisatUP)
    get_target_property(MINISATUP_INCLUDE_DIR MinisatUP::MinisatUP INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(cvc5-obj SYSTEM PRIVATE ${MINISATUP_INCLUDE_DIR})
    target_link_libraries(cvc5 PRIVATE MinisatUP::MinisatUP)

(end cmake config problems)

- debug function `bool failed(int lit)` with `std::set`:
    lit = -25
    intToLit(-25) = 49
    conflict = {48}
  returns false, failed, so should return true
  change to:
    bool failed(int lit) {
        return conflict.has(intToLit(-lit));
    }

- when `l0` is root-falsified and `l1 & l2 & ... & lk` are causing `!l0`, then have:
  `l1 & l2 & ... & lk -> !l0`, which equals
  `!l1 | !l2 | ... | !lk | !l0`
  conflict is the set of all literals on the trace negated
  so here it needs negation

- find more problems with `make check`

- bug literal doesn't exist:
  `assume` with `ensureVar` as well
  passed

- run tests again:
  
  99% tests passed, 9 tests failed out of 4128

  Label Time Summary:
  api capi      =   1.00 sec*proc (7 tests)
  api cppapi    =  49.88 sec*proc (70 tests)
  regress0      = 2843.62 sec*proc (2348 tests)
  regress1      = 5301.86 sec*proc (1429 tests)
  regress2      = 2557.61 sec*proc (142 tests)
  unit          = 147.55 sec*proc (132 tests)

  Total Test time (real) = 936.38 sec

  The following tests FAILED:
  1456 - regress0/prop/cadical_bug1.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug1.smt2   --debug-check-models
  1457 - regress0/prop/cadical_bug2.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug2.smt2   --debug-check-models
  1458 - regress0/prop/cadical_bug3.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug3.smt2   --debug-check-models
  1459 - regress0/prop/cadical_bug4.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug4.smt2
  1460 - regress0/prop/cadical_bug5.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug5.smt2   --debug-check-models
  1461 - regress0/prop/cadical_bug6.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug6.smt2
  1462 - regress0/prop/cadical_bug7.smt2 (Failed) cvc5 -i --sat-solver=cadical cadical_bug7.smt2   --debug-check-models
  3286 - regress1/sets/proj-issue668.smt2 (Failed) cvc5 proj-issue668.smt2   --debug-check-models
  4091 - unit/api/cpp/api_solver_black (Subprocess aborted)

  all with option --sat-solver=cadical and error:
  void Minisat::SolverInterface::connect_external_propagator(MinisatUP::ExternalPropagator*): Assertion `false' failed.

### questions

- why does cvc5 need to link minisatup explicitly when cvc5-obj is already linking it, assuming cvc5 is linked from cvc5-obj?

- if --sat-solver=cadical is not added, why is cadical still used in other tests?
  in `BVSolverBitblast::initSatSolver()`: `SatSolverFactory::createCadical(...)` is by default, propagator is not initialized
  in `PropEngine::PropEngine(...)`: `SatSolverFactory::createCadicalCDCLT(...)` needs an option, while `SatSolverFactory::createCDCLTMinisat(...)` is by default and in `d_satSolver->initialize`, propagator is initialized

### suggestions

- (wrong) cvc5: target_link_libraries to cvc5-obj, not cvc5, they are different build objects
