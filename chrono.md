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


invariant on watching literals:
- each clause is watched by 2 literals with assignments: (actual levels a, b)
  - A: U, U (others U or F)
  - B: T, F (a <= b, others F <= b)
  - C: T, U (others U or F)
  - D: T, T (others T, U or F)

adding external clause:
- F, F (a > b, others F <= b): reassign negation propagate
  - B: end
- F, F (a = b, others F <= b): analyze

- U, F (others F <= b): assign propagate


assign propagate: (subsequent assignments on the same level)
- A: U, U (others U or F):
  - .. F, F (a > b, others F <= b): reassign negation propagate
    - B: T, F (a = b, others F <= b): end
  - .. F, F (a = b, others F <= b): analyze

  - .. U, F (others F <= b): assign propagate
    - B: T, F (a = b, others F <= b): end
  - A: U, U (others U or F): end
- B: T, F (a <= b, others F <= b): not possible
- C: T, U (others U or F):
  - B: T, F (a <= b, others F <= b): end
  - .. T, F (a > b, others F <= b): reassign propagate
    - B: T, F (a = b, others F <= b): end
  - C: T, U (others U or F): end
  - D: T, T (others T, U or F): end
- D: T, T (others T, U or F):
  - D: T, T (others T, U or F): end

reassign propagate:


reassign negation propagate:
