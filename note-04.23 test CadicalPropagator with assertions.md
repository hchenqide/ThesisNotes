# step 9 (04.23)

> test CadicalPropagator with assertions

- `Assert(info.assignment == 0)` in `notify_assignment` fails
  in cadical fixed assignments and assignments will both be notified and cvc5 will call `notify_assignment` in `notify_fixed_assignment`
  and fixed assignments are notified much earlier (when adding the unit clause) than normal assignments (batched)
  change the assertion to rule out fixed assignments that are already notified:
  `Assert(info.assignment == 0 || (info.assignment == lit && info.is_fixed))`

- `Assert(info.level_intro == 0)` in `notify_fixed_assignment` fails
  I thought a fixed assignment is an actual root level assignment, so when a variable is introduced in a higher user level, it will be dependent on the assumption of activation literal so there wouldn't be a fixed assignment
    user pop: 1 -> 0
    disable activation lit: 7
    set inactive: 8
    notif::backtrack: 0
    notif::fixed assignment: 7
    Fatal failure within virtual void cvc5::internal::prop::CadicalPropagator::notify_fixed_assignment(int)
  the fixed assignment here is from the activation literal, activation literals are added as a unit along with other active variables at `user_pop` and `is_active` will be set false
  but why is it hit here if it is no longer active?
  it is notified when adding unit 8 and there's even a backtrack to level 0
  there's this clause added before:
    notif::fixed assignment: 3
    addClause (0): 7 ~3 ~8 0
  so it triggered 7 to be propagated during adding unit 8 in user_pop

- in `user_pop` set `is_active` false for all variables to be inactivated before adding them as unit clauses, assertions unchanged

- this test now passed, but another test `test/regress/cli/regress0/prop/cadical_bug2.smt2` failed at the same assertion in `notify_fixed_assignment`
    user push: 0 -> 1
    solve start
    assume activation lit: ~11
    ...
    value: 3: 0
    value: 5: 1
    ...
    value: 10: 1
    value: 7: 1
    addClause (0): 11 ~10 ~7 0
    ...
    cb::reason: ~8, 11 ~8 3 ~5 0
    ...
    notif::fixed assignment: 11
  so looks like there's a backtrack against the assumptions and it should cause unsat
  the new clause `11 ~10 ~7 0` caused conflict analysis on current level 1 which asked the reason for `-8` (I don't know why), and then backtracked to level 0 and propagated the negative of the assumption of level 1 which is 11

- in this case, suppose the level of before is l <= assumption size, literal a = assumptions[l-1], and the level after analysis is k < l, then ~a is to be propagated after analysis
  if k > 0, then a is not fixed, so if it causes a fixed assignment, then current decision level must be 0
  here, the assignment must be the negation of the activation literal of the user level when it is introduced
  when `info.level_intro == 0` doesn't hold, and the literal to be notified is not from an activation variable, then the activation variable of the introduction level of this literal must already be assigned true and fixed, but this should have already caused unsat
  so, the literal must be the negation of an activation literal

- update the assertion as:
  `Assert(info.level_intro == 0 || (info.level_intro > 0 && d_decisions.size() == 0 && slit == d_activation_literals[info.level_intro - 1]))`
  all cadical tests passed

discoveries:
- there are also units added at user level 0 but only got notified at user level 1 in cadical
