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

- error with `test/regress/cli/regress0/bug486.cvc.smt2`
  seems still has something todo with cb_decide
  a full model check is happening within cb_decide to exit early, but does it make sense
