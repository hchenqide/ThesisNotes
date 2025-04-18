### step 6 (04.16)

> implement IPASIR-UP interface

- check cvc5 internal minisat implementation for the corresponding functions of ipasirup

- minisat add `intToVar()` to SolverTypes.h

- error:
  Fatal failure within void cvc5::internal::prop::MinisatUPPropagator::user_pop() at cvc5/src/prop/minisatup.cpp:795  `Assert(info.is_fixed);`

  `info.is_fixed` is only set at `FixedAssignmentListener::notify_fixed_assignment()`, which is not implemented

### questions

(minisat)

- how assumptions work to ensure when analyzing, backtrack wouldn't go to level 0
(04.17)
> when there's conflict, after analysis and backtrack to a level before, the assumed literal will be flipped and propgated until there's no conflict, then this assumption will fail at the next retrieval of assumptions. but it can be optimized
> this behaviour is verified by `test/regress/cli/regress0/arrays/issue4546-2.smt2`, with conditional breakpoint `decisionLevel() <= assumptions.size()` at `if (confl != CRef_Undef){` in `Solver::search()`. the negation of the assumed literal is given after analysis as the learned clause, and will backtrack to level 0
(04.18)
> in general, during assumption, at level l, where the assumed literal is `assumptions[l-1]`, when there is a conflict, after analysis, the destination level k < l, and the literal's negation is propagated, all assumptions starting from k will be reassumed until the lth, which is now false:
  ```c++
  // after analyze
  if (decisionLevel() < assumptions.size()) {
      assert(learnt_clause[0] == ~assumptions[decisionLevel()]);
  }
  ```
  no, this is wrong, because the new decisionLevel() k can be lower than l-1
> to simplify this, move `analyzeFinal(~p, conflict)` before analyze:
  ```c++
  if (decisionLevel() == 0) return l_False;
  if (decisionLevel() <= assumptions.size()) {
      Lit p = assumptions[decisionLevel() - 1];
      analyzeFinal(~p, conflict);
      return l_False;
  }
  ```
  no, this wouldn't work yet because ~p is not yet propagated so there's no reason clause
  it should be put after analyze before the next propagation, since it's not necessary anymore to propagate
  ```c++
  if (decisionLevel() < assumptions.size()) {
      analyzeFinal(learnt_clause[0], conflict);
      return l_False;
  }
  ```
  error running test: Fatal failure within void cvc5::internal::SolverEngine::checkUnsatCore()
    "-i",
    "--check-unsat-cores",
    "test/regress/cli/regress0/arrays/issue4546-2.smt2"
  the decision level is 4 before analysis while there's only one assumption, so in this case the new propagated literal will not be one in assumptions
  so only when the conflict is found within assumptions can this be applyed
> but if there's a conflict from a decision other than assumptions, it doesn't need to backtrack to a much earlier level and redo the assumptions again (this will require chronological backtracking)

- assumptions may increase level bypassing user_propagator
> but if there is user_propagator, will assumptions be used?
(04.18)
> yes, with test `test/regress/cli/regress0/prop/cadical_bug1.smt2`, `assume()` is called after `connect_external_propagator()` in the same solver instance
> but when an assumption is about to make, user_propagator wouldn't provide an external unit or clause?
> updated state:
  - search -> propagate
  - propagate -> analyze(conflict) | simplify
  - analyze(conflict) -> unsat | propagate
  - simplify -> unsat | assume
  - **assume -> propagate | interact**
  - interact -> add | decide
  - add -> unsat | propagate | analyze(conflict) | interact
  - decide -> sat | propagate
  - unsat
  - sat
> when there's assumption, assignments are still notified before going to the new level, but units or clauses are not requested

- can `is_decision(int)` be determined by `vardata[x].d_reason == CRef_Undef && level(x) > 0` when there are other cases that a literal can be assigned, for example, as a unit
(04.17)
> probably not, because a root level unit will still be marked as the current level non-chronically, unlike cadical
(04.18)
