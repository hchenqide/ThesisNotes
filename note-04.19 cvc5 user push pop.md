### step 7

> understanding cvc5 user push/pop
> 
> reference: https://cs.stanford.edu/~preiner/talks/Preiner-Shonan23.pdf


`MinisatUPSolver` (also `CadicalSolver`):

[properties]

- `d_nextVarIdx`: incremental index for variables, starting from 1


`MinisatUPPropagator` (also `CadicalPropagator`):

[properties]

- `d_active_vars`: all variables from user level 0 - current in flat vector
  `d_active_vars_control`: the begin index for each user level starting from 1, empty if user level is 0
> guess: `add_observed_var`/`remove_observed_var` are only for user push/pop to introduce new variables in a certain push level which will be discarded after pop
> suggestion: sat solvers natively support user push levels (removable variables and clauses), otherwise there will be a lot of garbage variables left in the solver

- `d_activation_literals`: activation literals for each user level to be added to the clauses of the level
> suggestion: if activation literal is the first literal added to a user level, it can be just referenced from `d_active_vars`

- `d_var_info`: info for each variable, extended when adding new variables, never shrinked, variables never reused
> suggestion: there may be a lot of gaps unused if implemented with std::vector, and variables should be able to reuse, if sat solvers could support

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
  - if the variable is fixed, save it for re-adding later
  - re-add the fixed variables in the current level
