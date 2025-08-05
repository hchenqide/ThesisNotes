# chronological backtracking

cases:
- assign a new variable at a lower level
- reassign a variable at a lower level
- reassign a variable with negation at a lower level
(reassignment always happen at a lower level than current level)


each variable has its actual level
trace is a list of assignments that happened on each decision levels, but each assignment has an actual level
repropagate_queue is a list of variables that are reassigned and need to be repropagated
decision variables?

analyze produces a clause that reassigns negation
trail is no longer needed, no backtracking, only propagation queue for updating assignments
level is still needed even though the order is arbitrary, levels can be optimized if no assignments left on this level


states of unit clause:
(stable)
- T
(unstable)
- F: UNSAT
- U: assign propagate

states of 2-watching literals: (actual levels a, b)
(stable)
- UU (others T, U, F)
- UT (others T, U, F)
- TF (a <= b, others F <= b)
- TU (others T, U, F)
- TT (others T, U, F)
(unstable normalized)
- FF/P (a > b, others F <= b): reassign negation propagate
- FF/C (a = b, others F <= b): analyze, producing learned clause FFP, or UNSAT
- UF (others F <= b): assign propagate
- TF/P (a > b, others F <= b): reassign propagate
(normalization)
- FF (others T, U, F):
  (replace two F with T or U)
  - UU
  - UT
  - TU
  - TT
  (replace one F with T or U, replace one F with F in max level)
  - UF (others F <= b): assign propagate
  - TF/P (a > b, others F <= b): reassign propagate
  - TF (a <= b, others F <= b)
  (replace one F with F in max level, replace one F with F in second max level)
  - FF/P (a > b, others F <= b): reassign negation propagate
  - FF/C (a = b, others F <= b): analyze
- FU (others T, U, F):
  (replace F with T or U)
  - UU
  - TU
  (replace F with F in max level, swap)
  - UF (others F <= b): assign propagate
- FT (others T, U, F):
  (replace F with T or U)
  - UT
  - TT
  (replace F with F in max level, swap)
  - TF/P (a > b, others F <= b): reassign propagate
  - TF (a <= b, others F <= b)
- UF (others T, U, F):
  (replace F with T or U)
  - UU
  - TU
  (replace F with F in max level)
  - UF (others F <= b): assign propagate
- TF (others T, U, F):
  (replace F with T or U)
  - UT
  - TT
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign propagate
  - TF (a <= b, others F <= b)
- TF (others F):
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign propagate
  - TF (a <= b, others F <= b)

normalize:
(find two T, U)
- found one: find one T, U
- find two max F
(find one T, U)
- found one: stop
- find max F
(find max F)
- found max F: stop


adding external clause:
(length = 0)
- UNSAT
(length = 1)
- F: UNSAT
- U: assign propagate
- T
(length >= 2)
- FF: normalize (find-2)
- FU: normalize (swap, find-1)
- FT: normalize (swap, find-1)
- UF: normalize (find-1)
- UU
- UT
- TF: normalize (find-1)
- TU
- TT


in one temporary propagation queue, all assignments/reassignments are on the same level, reassignments are on a lower level than original level


assign propagate: (only false branch of new assignments affected)
- UU (others T, U, F):
  - FF (others T, U, F): normalize (find-2)
  - FU (others T, U, F): normalize (swap, find-1)
  - FT (others T, U, F): normalize (swap, find-1)
  - UF (others T, U, F): normalize (find-1)
  - UT (others T, U, F)
  - TF (others T, U, F): normalize (find-1)
  - TU (others T, U, F)
  - TT (others T, U, F)
- UT (others T, U, F):
  - FT (others T, U, F): normalize (swap, find-1)
  - TT (others T, U, F)
- TF (a <= b, others F <= b)
- TU (others T, U, F):
  - TF (others T, U, F): normalize (find-1)
  - TT (others T, U, F)
- TT (others T, U, F)


reassign propagate:
- UU (others T, U, F)
- UT (others T, U, F)
- TF (a <= b, others F <= b)
  - TF (others F): normalize (find-max)
- TU (others T, U, F)
- TT (others T, U, F)


reassign negation propagate:
- UU (others T, U, F)
- UT (others T, U, F)
  - UF (others T, U, F): normalize (find-1)
- TF (a <= b, others F <= b)


- TU (others T, U, F)
- TT (others T, U, F)




- UT: (after searching and reordering)
  (still two T/U watching, others T, U, F)
  - UT
  (at least one F watching, others F <= b)
  - FFP: reassign negation propagate
  - FFC: analyze
  - UF: assign propagate
  - TFP: reassign propagate
  - TF
- TF:
  - FFP: reassign negation propagate
  - FFC: analyze
  - TFP: reassign propagate
  - TF
  - UT
