# step 10 (04.24)

> fix bugs with `make check` replacing minisat with minisatup

- `test/regress/cli/regress2/bug812.smt2` error minisat IntMap access out of bounds
  in `pickBranchLit` there's a typo mixing lit and l
    int lit = external_propagator->cb_decide();
    Lit l = intToLit(lit);
    if (value(lit) == l_Undef)
  fixed

- `test/regress/cli/regress0/prop/cadical_bug6.smt2` assertion failure `info.level_intro == 0`
  forgot to sychronize assertions made in `CadicalPropagator` with `MinisatupPropagator`
  fixed

- `test/regress/cli/regress0/prop/cadical_bug7.smt2` with `--debug-check-models` flag, model check failure
  compare trace with cadical
  after sat, minisatup immediately notified backtrack 0 and undid some assignments so I guess the checker couldn't get the assignments
  revert notify backtrack immediately after solve in minisat

- `test/regress/cli/regress0/prop/cadical_bug5.smt2` with `--check-unsat-cores` flag, Minisat::Solver::search(int): Assertion `learnt_clause[0] == ~assumptions[conflict_level-1]' failed
  I added this part to return unsat early if a conflict happens against assumptions
  check assumptions (size: 13) from trace:
    solve start
    assume activation lit: ~13
    assume: 1 1 3 7 9 1 1 1 12 1 1 1
  so there are a lot of repetitive assumptions
  and here it's a conflict analysis triggered by an external clause with conflict level 13 and backtrack level 10
  the learned clause (size: 6) is
    4 -12 13 -3 -9 -7 0
  this is just a normal propagation, not important
  check the conflict clause (size: 2) from trace
    -8 -16 0
  check the assignments of -8 and -16 from trace
    notif::decision: new level 11
    notif::decision: new level 12
    notif::decision: new level 13
    ...
    notif::assignment: [p] 8 (level_intro: 0, existing_assignment: 0)
    ...
    notif::backtrack: 13
    ...
    notif::assignment: [p] 16 (level_intro: 1, existing_assignment: 0)
  so they are both assigned on level 13
  and because level 11 - 13 are all not actual levels, so the conflict analysis goes back to level 10
fix:
  when the next assumption literal is true, remove it from the assumption list without entering a new level, because it is implied from the assumptions before

- now the error becomes: Internal error detected SolverEngine::checkUnsatCore(): produced core was satisfiable
  tried to compare with cadical with option `--sat-solver=cadical`, still the same result as minisatup
  in `cvc5/src/smt/set_defaults.cpp` if there's `--check-unsat-cores` option, minisat will replace cadical
> guess: with cadical the unsat core will also be wrong so they made it default with minisat

