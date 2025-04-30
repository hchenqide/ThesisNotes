### 04.30

> implement cadical interfaces in minisat

- `terminate()`
  minisat already contains `bool asynch_interrupt`, `interrupt()` and `clearInterrupt()`
  and `withinBudget()` will check the interrupt
  cvc5 calls the same `interrupt()` without `volatile`

- `connect_terminator()`
  check terminator in `withinBudget()`

- `phase()`
  `setPolarity()`

- `fixed(int)`
> issue: cvc5 calls it with a variable and returns bool, but cadical calls it with a literal and returns {1,0,-1}
  here returns actual boolean like in cvc5

- `val()`
  needs to change according to the new semantics, but in cvc5 it's still old semantics, so here just keep it as is

> put assumptions after external propagation

> debug

- error `--check-unsat-cores test/regress/cli/regress0/prop/cadical_bug5.smt2`:
    Fatal failure within virtual int cvc5::internal::prop::MinisatUPPropagator::cb_propagate() at cvc5/src/prop/minisatup.cpp:493  d_decisions.size() >= current_user_level()
  I added this assertion, because assumptions used to be used for decisions before propagation in minisat, now it's changed
  remove the assertion

- error `--sygus-inst test/regress/cli/regress2/quantifiers/issue10805-syqi-no-arr-const.smt2`:
  /minisat/minisat/core/Solver.cc:282: Minisat::Lit Minisat::Solver::pickBranchLit(): Assertion `value(l) == l_Undef' failed
  value(l) = l_False
  skip already assigned variables in cb_decide
> now it can run correctly but is much slower than cadical

- error `--check-unsat-cores test/regress/cli/regress1/bv/fuzz18.smtv1.smt2`
  /minisat/minisat/core/Solver.cc:806: Minisat::lbool Minisat::Solver::search(int): Assertion `learnt_clause[0] == ~assumptions[conflict_level-1]' failed
  same error with `--check-unsat-cores test/regress/cli/regress2/strings/proj-439-cyclic-str-ipc.smt2` which is simpler
  still complicated
  add debug print vec of literals in minisat
    conflict level 10, assumptions size 10, backtrack level 9
    assumptions: -3 -4 5 6 -7 8 -9 26 44 84
    learnt clause: -45 -27 -12
    trail of level 10: 84 45 49 ... 743
  84 and 45 are closely related
    with clause at the beginning: `addClause (0): 84 ~45 ~5 0`, and 5 is assumed before 84
  so in the learned clause, the propagation literal might not be the decision literal of the conflict level, though this is kind of rare
  revert the assertion and early exit for conflicts within assumptions, keep the original analyeFinal
