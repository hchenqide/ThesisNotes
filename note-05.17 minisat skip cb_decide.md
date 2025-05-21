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
  unit/api/c/capi_solver_black
  unit/api/c/capi_uncovered_black

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

- api/cpp/issues/proj-issue421, rewrite in smt2
    (set-logic QF_ALL)
    (define-sort SeqBV4 () (Seq (_ BitVec 4)))
    (declare-fun _x49 () SeqBV4)
    (declare-fun _x51 () Real)
    (define-fun t65 () SeqBV4 (seq.rev _x49))
    (define-fun t69 () Real (tan _x51))
    (assert (<= t69 _x51))
    (check-sat-assuming ((not (seq.prefixof t65 _x49))))
  minisat and cadical outputs unknown
  minisatup stuck at check model in cb_decide and never returns
  breakpoints always hits gmp so there's some calculation in a permanent loop

(05.20)

  `transcendental_purify_arg_21` is assigned with a large fraction in minisatup, but with simple number in cadical

(05.21)

- rebase on main

- replace minisatup with cadical and run make check for baseline

  12 failed

  regress1/strings/issue5611-deq-norm-emp.smt2
  regress1/sets/finite-type/sets-card-color-sat.smt2

  unit/api/cpp/api_solver_black
  unit/api/c/capi_solver_black
  unit/api/c/capi_uncovered_black

  7 timeout
    regress1/nl/nl_uf_lalt.smt2
    regress1/quantifiers/issue5735-subtypes.smt2
    regress0/nl/dd.sin-cos-346-b-chunk-0210.smt2
    regress1/sygus/proj-issue165.smt2
    regress1/arith/issue7252-arith-sanity.smt2
    regress2/arith/real2int-test.smt2
    regress0/arith/issue1399.smt2
