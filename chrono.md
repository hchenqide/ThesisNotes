# chronological backtracking

design choices and implications:
- reason clause and unassigning: (reason clauses can become invalid when reassigning negation or unassigning, triggering further unassigning)
 x- full unassigning: (assigning in levels and backtracking)
    > backtrack to one level before the literal's original level and unassign all, so the literal's impact all cleared
    > but unassignment might be excessive
    > only the highest one level unassigned when reassigning negation from conflict analysis, and all propagations on the highest level are due to the assignment, which is now reverted
    > unassigning or adding new clause can still cause multi-level backtracking and excessive unassignment
  - eager unassigning:
    > reason clauses watched by all false literals, when one becomes true or unassigned, the propagated literal unassigns
    > when a reason clause is unassigned, it also needs to be unwatched from other false literals, so that when it becomes reason again, it won't be watched twice
      - reason clauses kept in a set for each false literal
      - reason clauses keeping a flag for each literal whether they are currently watching
  - lazy unassigning:
    > an invalid reason clause can become valid again thus no need to unassign yet
    > analysis search stops at invalid reason clauses, unassigns all on the path, but still produces a learnt clause


data structures:
- literal:
  - assignment (T, U, F)
  - watching clauses

- variable:
  - assignment (positive, unassigned, negative)
  - assignment level
  - assignment reason:
    - decision
    - unit clause
    - reason clause
    - *reason clause placeholder

- propagation schema:
  - assignment | -> { normalizing clause -> propagating clause -> assignment }
    (immediate assignment, delayed normalization)
    (allow other assignments before normalizing clause)
    (queue of assignments yet to normalize)

  - propagating clause | -> assignment -> { normalizing clause -> propagating clause }
    (delayed assignment, immediate normalization)
    (allow other propagations before assignment)
    (queue of propagations yet to assign)
    (this might help scheduling propagations)
    (propagating clauses may be ruined due to new assignments before getting propagated)

  - propagating clause | -> assignment | -> { normalizing clause -> propagating clause }
    (hybrid)


clause state:
[length = 0]
(propagating)
- exit(UNSAT)

[length = 1] (level a)
(static)
- T (a == 0)
(propagating)
- F: exit(UNSAT)
- U: assign(0)
- T (a > 0): reassign(0)

[length = 2] (2-watching literals level a, b)
(static)
- UU
- UT
- TF (a <= b)
- TU
- TT
(propagating)
- FF/P (a > b, others F <= b): backtrack(a - 1), assign(b)
  - TF (a' = b, others F <= b)
- FF/C (a = b, others F <= b):
  - (a/b = 0): exit(UNSAT)
  - (a/b > 0): analyze, producing learned clause
    - FF/P (a > b', others F <= b'): backtrack(a - 1), assign(b')
      - TF (a' = b', others F <= b')
    - FF/C (a = b, others F <= b):
      - UU
- UF (others F <= b): assign(b)
  - TF (a = b, others F <= b)
- TF/P (a > b, others F <= b): reassign(b)
  - TF (a' = b, others F <= b)
(normalizing)
- FF:
  (replace two F with T or U)
  - UU
  - UT
  - TU
  - TT
  (replace one F with U, replace one F with F in max level)
  - UF (others F <= b): assign(b)
  (replace one F with T, replace one F with F >= a)
  - TF (a <= b, others F) as TF (a <= b)
  (replace one F with T, replace one F with F in max level)
  - TF/P (a > b, others F <= b): reassign(b)
  (replace two F in max level)
  - FF/P (a > b, others F <= b): backtrack(a - 1), assign(b)
  - FF/C (a = b, others F <= b):
    - (a/b = 0): exit(UNSAT)
    - (a/b > 0): analyze
- FU:
  (swap)
  - UF: normalize
- FT:
  (swap)
  - TF (a <= b)
  - TF (a > b): normalize
- UF:
  (replace F with T or U)
  - UU
  - TU
  (replace F with F in max level)
  - UF (others F <= b): assign(b)
- TF (a > b):
  (replace F with T or U)
  - TU
  - TT
  (replace F with F >= a)
  - TF (a <= b, others F) as TF (a <= b)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign(b)


adding new clause:
[length = 0]
- exit(UNSAT)

[length = 1]
- F: exit(UNSAT)
- U: assign(0)
- T (a = 0)
- T (a > 0): reassign(0)

[length >= 2]
- FF: normalize
- FU: normalize
- FT: normalize
- UF: normalize
- UU
- UT
- TF (a <= b)
- TF (a > b): normalize
- TU
- TT


watched clauses of a literal (T, U, F) state:
(static)
(F)
- TF (a <= b)
(U)
- UU
- UT
- TU
(T)
- UT
- TF (a <= b)
- TU
- TT

(normalizing)
(F)
- FF
- FU
- FT
- TF (a > b)
(U)
- FU
- UF
(T)
- FT
- TF (a > b)


watched clauses state transition:
assign(k):
(U -> F)
- UU ->
  - FU: normalize
    - UF (others F <= b): assign new (l >= k)
  - UF: normalize
    - UF (others F <= b): assign new (l >= k)
- UT ->
  - FT: normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
- TU ->
  - TF (a <= b)
  - TF (a > b): normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
- FU ->
  -

- UU:
  - FF: normalize
    - FF/P (a > b, others F <= b): reassign negation (l >= k)
    - FF/C (a = b, others F <= b): analyze (l >= k), reassign negation (l' < l)
    - UF (others F <= b): assign new (l >= k)
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - FU: normalize
  - FT: normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - UF: normalize
    - UF (others F <= b): assign new (l >= k)
  - UT
  - TF (a <= b)
  - TF (a > b): normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - TU
  - TT
- TU:
  - TF (a <= b)
  - TF (a > b): normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - TT

reassign(k):
(T)
- TF (a <= b):
  - TF (a <= b)
- TU:
  - TU
- TT:
  - TT
(F)
- TF (a <= b):
  - TF (a <= b)
  - TF (a > b): normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
- TF (a = b, others F <= b, reason)
  - TF (a = b, others F <= b, reason): reason unchanged
  - TF (a > b', others F <= b, b' < b, reason): normalize, reassign, reason unchanged
    - TF/P (a > b, others F <= b): reassign (l >= k)

backtrack(k):


