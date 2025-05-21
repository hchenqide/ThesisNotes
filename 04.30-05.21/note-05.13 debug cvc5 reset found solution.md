### 05.13

> debug in cvc5

- change check smt.checkProofs to smt.produceProofs which is more general in sat_solver_factory

- disable renotify_fixed
  now the test `-i test/regress/cli/regress1/strings/issue6072-inc-no-const-reg.smt2` can run without the error

- run full `make check`
  interrupted just before finishing

- timeout with `test/regress/cli/regress1/quantifiers/issue5735-subtypes.smt2`
  also in cadical

- error with `test/regress/cli/regress0/fp/issue9858.smt2`
  not done: theory need check
  breakpoint at `cb_check_found_model`, `d_found_solution` is true, so check is skipped
  `d_found_solution` is assigned true once at the first solve call
  with cadical `d_found_solution` is not assigned true at the first solve call
  so it shouldn't be true here
  also notice that `check_found_model` is called too many times
    cb::decide
      cb::check_found_model
      full check (recheck: 0)
      new propagations: 0
      done: full assignment consistent
      cb::check_found_model end: done: 1
      Found solution
    cb::decide: 0
    cb::check_found_model
    done: found partial solution
    solve done: 1
    notif::backtrack: 0
  the first check_found_model is during `cb_decide`, and `d_found_solution` is set here
> there are two problems:
  - (minisat) `cb_decide` is called even if there's no more available variables to decide, and if there's only one left it's also not necessary to call
  - (cvc5) `d_found_solution` will be reset at backtrack, but it is skipped when it's already at level 0
  always reset `d_found_solution` at backtrack
  fixed

- error with `test/regress/cli/regress0/bug486.cvc.smt2`, output should be sat, but unsat
  seems still has something todo with cb_decide
  a full model check is happening within cb_decide to exit early, but does it make sense

(05.15)

  disable the model check at cb_decide, and `d_found_solution` will become unused and remain false
  still wrong output
  if variables and clauses are added during `cb_decide`, then a new variable will be directly decided next, but it normally should be to add the external clauses and propagate first, so this change is all right, also change in minisat should be made to prevent calling cb_decide
  trace is similar to cadical, only the renotify part is missing
  so the `renotify_fixed` is still necessary because otherwise the fixed assignments will be lost
  but the propagator still keeps the assignments for the next solve
  revert disable renotify_fixed and it now outputs unsat

- run full `make check`
  didn't finish, timeout, manually interrupted
  Failed 44 occurences (search ***failed)
  1 
    [ RUN      ] TestCApiBlackSolver.plugin_listen
    /mnt/e/Freiburg/25s/thesis/cvc5/test/unit/api/c/capi_solver_black.cpp:3782: Failure
    Value of: clause_seen
      Actual: false
    Expected: true
    [  FAILED  ] TestCApiBlackSolver.plugin_listen (32 ms)
  16
    assertion failure (void cvc5::internal::SharedTermsDatabase::addSharedTerm)
  20
    timeout
  5
    cvc5 --ieval=use --term-db-mode=all dd.ricart-ieval.smt2
    cvc5 --solve-real-as-int -q issue10724-real-as-int-types.smt2
    cvc5 -q issue8160-model-purify.smt2
    cvc5 --rlimit-per=100 issue8854-get-model-after-unknown.smt2
    cvc5 issue6717-ite-rewrite.smt2
  1
    cvc5 --sygus-verify-timeout=100 --sygus-enum=smart -q sum-sq-unknown.sy
