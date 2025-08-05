# chronological backtracking

each variable has its actual level
trace is a list of assignments that happened on each decision levels, but each assignment has an actual level
repropagate_queue is a list of variables that are reassigned and need to be repropagated
decision variables?

analyze produces a clause that reassigns negation
trail is no longer needed, no backtracking, only propagation queue for updating assignments
level is still needed even though the order is arbitrary, levels can be optimized if no assignments left on this level

once a variable is assigned, it can never get unassigned
support unassigning a variable? (deleting a clause)


states of unit clause:
(stable)
- T
(unstable)
- F: UNSAT
- U: assign new


states of 2-watching literals: (actual levels a, b)
(stable)
- UU (others T, U, F)
- UT (others T, U, F)
- TF (a <= b, others F)
- TU (others T, U, F)
- TT (others T, U, F)

(unstable normalized)
- FF/P (a > b, others F <= b): reassign negation
- FF/C (a = b, others F <= b): analyze, producing learned clause FFP, or UNSAT
- UF (others F <= b): assign new
- TF/P (a > b, others F <= b): reassign

normalize:
- FF (others T, U, F):
  (replace two F with T or U)
  - UU
  - UT
  - TU
  - TT
  (replace one F with U, replace one F with F in max level)
  - UF (others F <= b): assign new
  (replace one F with T, replace one F with F >= a)
  - TF (a <= b, others F)
  (replace one F with T, replace one F with F in max level)
  - TF/P (a > b, others F <= b): reassign
  (replace two F in max level)
  - FF/P (a > b, others F <= b): reassign negation
  - FF/C (a = b, others F <= b): analyze

- FF (others T, F):
  (replace two F with T)
  - TT
  (replace one F with T, replace one F with F >= a)
  - TF (a <= b, others F)
  (replace one F with T, replace one F with F in max level)
  - TF/P (a > b, others F <= b): reassign
  (replace two F in max level)
  - FF/P (a > b, others F <= b): reassign negation
  - FF/C (a = b, others F <= b): analyze

- FU (others T, U, F):
  (swap)
  - UF (others T, U, F): normalize

- FT (others T, U, F):
  (swap)
  - TF (others T, U, F): normalize

- FT (others T, F):
  (swap)
  - TF (others T, F): normalize

- UF (others T, U, F):
  (replace F with T or U)
  - UU
  - TU
  (replace F with F in max level)
  - UF (others F <= b): assign new

- TF (others T, U, F):
  (replace F with T or U)
  - UT
  - TT
  (replace F with F >= a)
  - TF (a <= b, others F)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign

- TF (others T, F):
  (replace F with T)
  - TT
  (replace F with F >= a)
  - TF (a <= b, others F)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign

- TF (a > b, others F):
  (replace F with F >= a)
  - TF (a <= b, others F)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign


adding external clause:
(length = 0)
- UNSAT

(length = 1)
- F: UNSAT
- U: assign new
- T

(length >= 2)
- FF (others T, U, F): normalize
- FU (others T, U, F): normalize
- FT (others T, U, F): normalize
- UF (others T, U, F): normalize
- UU
- UT
- TF (others T, U, F): normalize
- TU
- TT


in one temporary propagation queue, all assignments/reassignments are on the same level, reassignments are on a lower level than original level
which to propagate when all exist?


assign new: (false branch)
- UU (others T, U, F):
  - FF (others T, U, F): normalize
  - FU (others T, U, F): normalize
  - FT (others T, U, F): normalize
  - UF (others T, U, F): normalize
  - UT (others T, U, F)
  - TF (others T, U, F): normalize
  - TU (others T, U, F)
  - TT (others T, U, F)

- UT (others T, U, F):
  - FT (others T, U, F): normalize
  - TT (others T, U, F)

- TF (a <= b, others F)

- TU (others T, U, F):
  - TF (others T, U, F): normalize
  - TT (others T, U, F)

- TT (others T, U, F)


reassign: (false branch)
- UU (others T, U, F)

- UT (others T, U, F)

- TF (a <= b, others F)
  - TF (a <= b, others F)
  - TF (a > b, others F): normalize

- TU (others T, U, F)

- TT (others T, U, F)


reassign negation: (false branch)
- UU (others T, U, F)

- UT (others T, U, F)
  - UF (others T, U, F): normalize

- TF (a <= b, others F)
  - FF (others T, F): normalize
  - FT (others T, F): normalize
  - TT (others T, F) < (others T, U, F)

- TU (others T, U, F)
  - FU (others T, U, F): normalize

- TT (others T, U, F)
  - FF (others T, U, F): normalize
  - FT (others T, U, F): normalize
  - TF (others T, U, F): normalize


unassign: (both branches)
- UU (others T, U, F)

- UT (others T, U, F)
  - UU (others T, U, F)

- TF (a <= b, others F)
  - UF (others U, F): normalize
  - TF (others U, F): normalize
  - TU (others U, F) < (others T, U, F)

- TU (others T, U, F)
  - UU (others T, U, F)

- TT (others T, U, F)
  - UU (others T, U, F)
  - UT (others T, U, F)
  - TU (others T, U, F)
