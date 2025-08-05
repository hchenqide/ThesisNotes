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


invariant on watching literals:
(each clause is watched by 2 literals with assignments: (actual levels a, b))
- stable states:
  - UT: T/U, T/U (others T, U or F) (merged cases without falsified watching literals)
    - UU: U, U (others U or F)
    - TU: T, U (others U or F)
    - TT: T, T (others T, U or F)
    - UT...
  - TF: T, F (a <= b, others F <= b)
- unstable states:
  - FFP: F, F (a > b, others F <= b): reassign negation propagate
    - TF
  - FFC: F, F (a = b, others F <= b): analyze, producing learned clause FFP
    - FFP, FFC
      - TF
  - UF: U, F (others F <= b): assign propagate
    - TF
  - TFP: T, F (a > b, others F <= b): reassign propagate
    - TF

adding external clause:
- FFP: reassign negation propagate
- FFC: analyze
- UF: assign propagate
- UU
- TFP: reassign propagate
- TF
- TU
- TT

assign propagate: (subsequent assignments on the same level)
- UT: (after searching and reordering)
  (still two T/U watching, others T, U, F)
  - UT
  (at least one F watching, others F <= b)
  - FFP: reassign negation propagate
  - FFC: analyze
  - UF: assign propagate
  - TFP: reassign propagate
  - TF
- TF

reassign propagate: (new level < current level)
- UT
- TF:
  - TFP: reassign propagate
  - TF

reassign negation propagate: (new level < current level)
- UT: (after searching and reordering)
  (still two T/U watching, others T, U, F)
  - UT
  (at least one F watching, others F <= b)
  - FFP: reassign negation propagate
  - FFC: analyze
  - UF: assign propagate
  - TFP: reassign propagate
  - TF

