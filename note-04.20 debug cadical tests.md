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

- continue with the next tests, error: (test/regress/cli/regress1/sets/proj-issue668.smt2)
    Fatal failure within void cvc5::internal::SharedTermsDatabase::addSharedTerm(cvc5::internal::TNode, cvc5::internal::TNode, cvc5::internal::theory::TheoryIdSet) at cvc5/src/theory/shared_terms_database.cpp:99   Check failure   theories != (*find).second
  it's called from _solve - renotify_fixed

(04.21)

- searched cvc5 github about tracing and found on wiki:
  `-vvv` flag is needed for verbosity
  updated debug config with following arguments:
    "-vvv",
    "--err=./build/bin/cvc5.err",
  updated command line:
  cvc5$:
  `./build/bin/cvc5 --vvv --err=./build/bin/cvc5.err --out=./build/bin/cvc5.out -i --sat-solver=cadical test/regress/cli/regress0/prop/cadical_bug5.smt2`
  some traces are printed but not all
  these printed traces are using `verbose(1) <<`, etc
  checked config:
  ./build/bin/cvc5 --show-config
    debug code    : yes
    statistics    : yes
    tracing       : yes
    muzzled       : no
    assertions    : yes
  and when I debug, the trace function won't be entered at all

- `Trace()` is controlled by macro `CVC5_TRACING` in `cvc5/src/base/output.h`
  `-DCVC5_TRACING` is in compile command
  there is this tag check `cvc5::internal::TraceChannel.isOn(tag)`
  `TraceChannel.on()` handled in options `enableTraceTag()`
  with this option
  `-t cadical::propagator`
  updated command line:
  cvc5$:
  `./build/bin/cvc5 -t cadical::propagator --err=./build/bin/cvc5.err --out=./build/bin/cvc5.out -i --sat-solver=cadical test/regress/cli/regress0/prop/cadical_bug5.smt2`
  now we have trace in cvc5.err

- replace `MinisatUPSolver` with `MinisatSatSolver` in `SatSolverFactory` so we can compare traces between MinisatUP and CaDiCal, build and run:
  cvc5$:
  `./build/bin/cvc5 -t cadical::propagator --err=stdout -i --sat-solver=cadical test/regress/cli/regress0/prop/cadical_bug5.smt2 > ../notes/test_cadical_bug5_1_cadical.out`
  `./build/bin/cvc5 -t cadical::propagator --err=stdout -i test/regress/cli/regress0/prop/cadical_bug5.smt2 > ../notes/test_cadical_bug5_1_minisatup.out`

- modify cadical_bug5.smt2 and comment out the first check-sat to simplify the results
  the simplified smt2 is too simple and result doesn't show anything

- modify cadical_bug5.smt2 and comment out all set-info so it wouldn't abort
  not too different as the original outputs

- debug while checking output

  addClause (0): 10 0
  user push: 0 -> 1
  ...
  propagate: ~10

  assignment 10 not notified ??

  debug adding clause 10 0
  add_tmp.sz = 3 ??
  add_tmp has literals [8, 9, 10]

  [8, 9] was added during `add_external_clause`, and `add_tmp` was not cleared

- minisatup clear `add_tmp` after solve

### extra (04.21)

> understanding conditions for multiple solver calls with propagator

(after `solve()` and before the next `solve()` call)

[solver]
- at decision level 0
- `trail` contains level 0 assignments
- new clauses added and watched
- new assumptions added
- `notify_assignment_index` pointing to the next position to notify on the trail, where there might be new assignments added with unit clauses
- `notify_backtrack` is false

[propagator]
- at decision level 0
- maintains level 0 assignments

discovery:
- after solve we don't need to backtrack propagator by ourselves, because `resetTrail()` will be called to reset propagator to level 0
