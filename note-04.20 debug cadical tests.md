### step 8 (04.20)

> debug cadical tests

- `notify_fixed_assignment()` not implemented in minisatup, but in cvc5 `MinisatUPPropagator::notify_assignment`, set `is_fixed` at decision level 0

- next error: (test/regress/cli/regress0/prop/cadical_bug1.smt2)
    Fatal failure within virtual void cvc5::internal::prop::MinisatUPPropagator::notify_assignment(const std::vector<int>&) at cvc5/src/prop/minisatup.cpp:123  `Assert(info.is_active);`
  I added this assertion because I thought inactive variables are all fixed at units, but these units are added as clauses and will still be notified at the next solver call, so this assertion is removed

- next error: (test/regress/cli/regress0/prop/cadical_bug5.smt2)
    cvc5: minisat/minisat/core/Solver.cc:865: Minisat::lbool Minisat::Solver::search(int): Assertion `value(l) == l_Undef' failed
  after `external_propagator->cb_propagate`, I changed `if (value(l) != l_Undef) { continue; }` to `assert(value(l) == l_Undef);` when a new literal to propagate is already assigned, here it returned a literal which is assigned true

- in `MinisatUPPropagator::next_propagation()` skip true literals

- output:
    sat
    (error "Expected result unsat but got sat")
  unable to locate debug
  add "--err=./build/bin/cvc5.err" as option and inspect trace
  but the file is empty, but I built with debug option

- tried skip true literals in minisat solve, but still the same error

- tried with internal minisat, solve() is called exactly 3 times and outputs are sat - unsat - unsat
  set breakpoint at `solve()` which has no assumption (if set at `_solve()`, there will be several other calls with assumptions)
  before the second `solve()`, the state is:
  MinisatUPPropagator:
    d_assignments: 2, 5, 6 (1, -2, 3)
    d_active_vars: 1, ..., 11 (theroy atoms: 3 ... 10)
    d_active_vars_control: 10
    d_activation_literals: 22 (11)
  Minisat:
    num_clauses: 6
    next_var: 11
    notify_assignment_index: 3
    notify_backtrack: true ??
