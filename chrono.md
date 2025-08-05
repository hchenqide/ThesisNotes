# chronological backtracking in MiniSat

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


invariant on watching literals:
(each clause is watched by 2 literals with assignments: (actual levels a, b))
- stable states:
  - UU: U, U (others U or F)
  - TF: T, F (a <= b, others F <= b)
  - TU: T, U (others U or F)
  - TT: T, T (others T, U or F)
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
- FFP
- FFC
- UF
- UU
- TFP
- TF
- TU
- TT

assign propagate: (subsequent assignments on the same level)
- UU:
  - FFP: reassign negation propagate
  - FFC: analyze
  - UF: assign propagate
  - UU
- TF
- TU:
  - TFP: reassign propagate
  - TF
  - TU
  - TT
- TT
  - TT

reassign propagate: (new level < current level)
- UU
- TF:
  - TFP: reassign propagate
  - TF
- TU
- TT

reassign negation propagate: (new level < current level)
- UU:
  - UU
  - TU
- TF:
  - FFP: reassign negation propagate
  - FFC: analyze
  - TFP: reassign propagate
  - TF
  - TT
- TU:
  - UF:
  - UU
  - TU
  - TT
- TT:
  - FFP: reassign negation propagate
  - FFC: analyze
  - TFP: reassign propagate
  - TF
  - TT
