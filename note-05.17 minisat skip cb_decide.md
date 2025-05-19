### 05.17

> minisat skip cb_decide when no more needed

- error with `test/regress/cli/regress0/nl/nta/issue8160-model-purify.smt2`
  outputs unknown instead of sat
  d_found_solution is set false always on backtrack
  so this could probably also happen with cadical
  but this time `d_modelUnsound` is not set by quantifiers_engine, but nonlinear_extension
  add tag `-t nl-ext`

- same error with `test/regress/cli/regress0/bv/proj-issue343.smt2` which is simpler
  add tag `-t nl-ext-cm`
  comparing cadical and minisat, the assignments are different
  couldn't find out the problem

- minisat skip cb_decide when `trail.size() == nVars()`
  cvc5 revert changes
  now all the tests passed

- run full make check

- minisat/core/Solver.cc:932: Minisat::lbool Minisat::Solver::search(int): Assertion `!order_heap.empty() || next == lit_Undef' failed
  if order_heap is empty then next could be the last
  fixed

- 25 failed

  1
  --lang=sygus2 --sygus-unif-pi=complete --sygus-bool-ite-return-const --sygus-out=status cegis-unif-inv-eq-fair.sy
    Fatal failure within cvc5::internal::Node cvc5::internal::theory::datatypes::SygusExtension::registerSearchValue(cvc5::internal::Node, cvc5::internal::Node, cvc5::internal::Node, unsigned int, bool, bool) at cvc5/src/theory/datatypes/sygus_extension.cpp:1062  d_anchor_to_conj.find(a) != d_anchor_to_conj.end()

  1
  --rlimit-per=100 issue8854-get-model-after-unknown.smt2

  2
  unit/api/c/capi_solver_black
  unit/api/c/capi_uncovered_black

  5 timeout

  16 assertion

(05.18)

- --lang=sygus2 --sygus-unif-pi=complete --sygus-bool-ite-return-const --sygus-out=status cegis-unif-inv-eq-fair.sy
  this passed at 5.15 but failed at 5.13, change is only about disabling model check at decide
  minisat skip cb_decide when there's no more than 1 left to decide
    if (trail.size() + 1 < nVars())
  now it doesn't stop running
  it passed during make check which runs for like 4 mins

- disable the assertion

- run make check
  interrupted, tested around 1000, failed 38, all timeout

- even there's only one left to decide, cb_decide is still useful to determine the phase, so it should be kept

(05.19)

- some with --check-unsat-cores flag don't timeout, why
  some also takes a long time with cadical

- examine the test cast with `--produce-abducts uf-abduct.smt2`
  in the loop
  cb_decide is never called
  check_found_model would always add new variables

- revert the change in minisat

- notice that cadical has this `res->setResourceLimit(resmgr)` at `SatSolverFactory` which is not added in minisatup
  added

- make check
  the sygus one would still fail

  9 failed 1 not finised

  unit/api/cpp/api_solver_black
    /mnt/e/Freiburg/25s/thesis/cvc5/test/unit/api/cpp/api_solver_black.cpp:2562: Failure
    Value of: pl.hasSeenSatClause()
      Actual: false
    Expected: true
    [  FAILED  ] 1 test, listed below:
    [  FAILED  ] TestApiBlackSolver.pluginListen

  unit/api/c/capi_solver_black
    cvc5/test/unit/api/c/capi_solver_black.cpp:3782: Failure
    Value of: clause_seen
      Actual: false
    Expected: true
    [  FAILED  ] 1 test, listed below:
    [  FAILED  ] TestCApiBlackSolver.plugin_listen

  unit/api/c/capi_uncovered_black
    cvc5/test/unit/api/c/capi_uncovered_black.cpp:387: Failure
    Value of: pl.hasSeenSatClause()
      Actual: false
    Expected: true
    [  FAILED  ] 1 test, listed below:
    [  FAILED  ] TestCApiBlackUncovered.plugin_uncovered_default

  api/cpp/issues/proj-issue421
    not finished

  --lang=sygus2 --sygus-unif-pi=complete --sygus-bool-ite-return-const --sygus-out=status cegis-unif-inv-eq-fair.sy
    Fatal failure within cvc5::internal::Node cvc5::internal::theory::datatypes::SygusExtension::registerSearchValue(cvc5::internal::Node, cvc5::internal::Node, cvc5::internal::Node, unsigned int, bool, bool) at cvc5/src/theory/datatypes/sygus_extension.cpp:1062  d_anchor_to_conj.find(a) != d_anchor_to_conj.end()

  issue5735-subtypes.smt2
  --no-jh-rlv-order issue6057-replace-re-all.smt2
  --no-nl-ext-rewrite issue10003-nl-ext-rr.smt2
  --nl-ext=full --no-new-prop metitarski-3-4.smt2
  proj-issue619-nconst-nl-mv.smt2
    Timeout
