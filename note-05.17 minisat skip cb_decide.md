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
