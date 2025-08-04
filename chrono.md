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
  - A unassigned, unassigned (others unassigned or false)
  - B true~a, false~b (a <= b, others false <= b)
  - C true~a, unassigned (others unassigned or false)
  - D true~a, true~b (a <= b, others true >= b, unassigned or false)

assign a variable at level k: (k not necessarily current decision level) (all subsequent assignments can only be on the same level k)
  - A unassigned, unassigned (others unassigned or false):
    - A unassigned, unassigned (others unassigned or false): end
    - unassigned
  - B true~a, false~b (a <= b, others false <= b): not possible
  - C true~a, unassigned (others unassigned or false):
    - B true~a, false~b (a <= b, others false <= b): end
    - true~a, false~b (a > b, others false <= b): reassign propagate
      - B true~a, false~b (a = b, others false <= b): end
    - C true~a, unassigned (others unassigned or false): end
    - D true~a, true~b (a <= b, others true >= b, unassigned or false):
  - D true~a, true~b (a <= b, others true >= b, unassigned or false):
    - D true~a, true~b (a <= b, others true >= b, unassigned or false): end
    - E true~a, true~b (a <= b, others true, unassigned or false): end
  - E true~a, true~b (a <= b, others true, unassigned or false):
    - E true~a, true~b (a <= b, others true, unassigned or false): end

reassign a variable:


reassign a variable with negation:
