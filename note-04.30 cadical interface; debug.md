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
  revert the assertion and early exit for conflicts within assumptions, keep the original analyzeFinal

(05.01)

- error with many tests with `--check-proofs --proof-granularity=theory-rewrite --proof-check=lazy`
    Fatal failure within std::shared_ptr<cvc5::internal::ProofNode> cvc5::internal::prop::PropPfManager::getProof(bool) at cvc5/src/prop/prop_proof_manager.cpp:248  conflictProof
  `MinisatUPSolver::getProof()` just returns nullptr
  in cvc5 internal minisat, d_pfManager is used to output proof, and it's integrated in minisat
  it's complicated and I don't think I need to do it

- `-i --sat-solver=cadical --check-unsat-cores cadical_bug5.smt2`
    Fatal failure within void cvc5::internal::SolverEngine::checkUnsatCore() at /mnt/e/Freiburg/25s/thesis/cvc5/src/smt/solver_engine.cpp:1646
    Internal error detected SolverEngine::checkUnsatCore(): produced core was satisfiable
  the output is sat but it is supposed to be unsat
  trace shows that the last solve has notified assignment {1,-2} again
  debug shows the trail only has the two assignment, so it was cleared for some reason
  before the third solve, the sat solver is created again
  this is because with `--check-unsat-cores`, a new engine is created just to check the unsat core
  `void PropEngine::getUnsatCore(std::vector<Node>& core)` only gets unsat core from assumptions or proof, but proof doesn't work yet
  so originally with option `--check-unsat-cores`, only cvc5 internal minisat is used even with explicit `--sat-solver=cadical`
  add option `-t unsat-core`, outputs:
    PropEngine::getUnsatCore: via unsat assumptions
  debug, unsat returns when assumption 9 failed
  -8 -4 -9 were assigned just after solve start along with other inactive literals
    notif::assignments: { 18 17 16 -8 -4 -9 15 14 13 } (level: 0, level_user: 0)
  after checking, -8 is propagated by 16, which should not happen
    addClause (0): 13 ~8 ~16 0
    new propagations: 2
    new propagation: ~16
    new propagation: 14
    propagate: ~16 (current assignment: 16)
    external_clause: ~8
    external_clause: ~16
    external_clause: 0
  here 13 should be part of external clause but is not added
  this is because at propagate false literal appears and the reason clause is requested but I forgot to append activation literal
  fixed
