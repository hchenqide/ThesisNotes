### step 7 (04.19 - 04.20)

> understanding cvc5 user push/pop
> 
> reference: https://cs.stanford.edu/~preiner/talks/Preiner-Shonan23.pdf


`MinisatUPSolver` (also `CadicalSolver`):

[properties]

- `d_nextVarIdx`: incremental index for variables, starting from 1


`MinisatUPPropagator` (also `CadicalPropagator`):

[types]

- `VarInfo`:
  - `is_theory_atom`: 

[properties]

- `d_var_info`: info for each variable, extended when adding new variables, never shrinked, variables never reused
> suggestion: there may be a lot of gaps unused if implemented with std::vector, and variables should be able to reuse, if sat solvers could support

- `d_active_vars`: all variables from user level 0 - current in flat vector
  `d_active_vars_control`: the begin index for each user level starting from 1, empty if user level is 0
> guess: `add_observed_var`/`remove_observed_var` are only for user push/pop to introduce new variables in a certain push level which will be discarded after pop
> suggestion: sat solvers natively support user push levels (removable variables and clauses), otherwise there will be a lot of garbage variables left in the solver
  `d_activation_literals`: activation literals for each user level to be added to the clauses of the level
> suggestion: if activation literal is the first literal added to a user level, it can be just referenced from `d_active_vars`

- `d_assignments`: notified assignments in one certain user level, empty before solver call
  `d_assignment_control`: the begin index for assignments at each decision level

- `d_decisions`: the decision literals at each decision level

[methods]

- `add_clause`: add a clause at current user level
  - sort out fixed literals
  - prepend activation literal
  - add to solver directly, or append to new clause queue

- `add_new_var`:
  - solver add observed variable
  - add to `d_active_vars`
  - create an entry in `d_var_info`

- `user_push`: (before sat solver `solve()` call, in decision level 0)
  - push next level begin index in `d_active_vars_control`
  - (outside) create new activation literal

- `user_pop`: (after sat solver `solve()` call, in decision level 0)
  - pop active variables and remove observation (adding unit clauses assigning these variables)
  - if the variable is fixed and is theory_atom, save it to be re-added later
  - re-add the fixed variables in the current level
> in the original code this part is confusing, the actual procedure should be:
  - pop active variables and remove observation
> question: why do we need to renotify fixed variables, if they are fixed? and why do we need to keep the `level_fixed`, if they are all fixed above `level_intro` and destroyed after pop?

(IPASIRUP)

- `notify_backtrack`:
  - pop decisions
  - pop assignments
> in the original code this is not done:
  - put fixed assignments at the back again (or filtering out the other assignments and resize)

- `cb_check_found_model`:
> guess: it is not necessary because the check can still be done after solver returns sat, but it is good because if check failed, new clauses can be just added in the next round keeping the decision state
