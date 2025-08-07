# chronological backtracking

data structures:
- literal:
  - variable
    - assignment (T, U, F)
  - watching clauses:
    - (T, U, F) two-watching normal clauses
    - (F) full-watching reason clauses

- variable:
  - assignment (positive, unassigned, negative)
  - assignment level
  - assignment reason:
    - decision
    - unit clause
    - reason clause
    - *reason clause placeholder

- level stack:
  - number of assignments on each level
  - trailing levels with 0 assignments are deallocated


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
- UNSAT

[length = 1] (level a)
(static)
- T (a == 0)

(propagating)
- F: UNSAT
- U: assign new
- T (a > 0): reassign

[length = 2] (2-watching literals level a, b)
(static)
- UU
- UT
- TF (a <= b)
- TU
- TT

(propagating)
- FF/P (a > b, others F <= b): reassign negation (on b)
  - TF (a' = b, others F <= b, a' < a)
- FF/C (a = b, others F <= b): analyze, producing learned clause FF/P, or UNSAT
  - FF/P (a' > b', others F <= b', a' = a/b, b' < a/b): reassign negation (on b')
    - TF (a'' = b', others F <= b', a'' < a')
  - FF/C (a = b, others F <= b): still conflict pointing to learned clause until propagated, no need to analyze again
- UF (others F <= b): assign new (on b)
  - TF (a = b, others F <= b)
- TF/P (a > b, others F <= b): reassign (on b)
  - TF (a' = b, others F <= b, a' < a)

(normalizing)
- FF:
  (replace two F with T or U)
  - UU
  - UT
  - TU
  - TT
  (replace one F with U, replace one F with F in max level)
  - UF (others F <= b): assign new
  (replace one F with T, replace one F with F >= a)
  - TF (a <= b, others F) as TF (a <= b)
  (replace one F with T, replace one F with F in max level)
  - TF/P (a > b, others F <= b): reassign
  (replace two F in max level)
  - FF/P (a > b, others F <= b): reassign negation
  - FF/C (a = b, others F <= b): analyze

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
  - UF (others F <= b): assign new

- TF (a > b):
  (replace F with T or U)
  - TU
  - TT
  (replace F with F >= a)
  - TF (a <= b, others F) as TF (a <= b)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign


adding new clause:
[length = 0]
UNSAT

[length = 1]
- F: UNSAT
- U: assign new
- T (a = 0)
- T (a > 0): reassign

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


watched clauses of a literal state:
(U)
- UU
- TU
(T)
- TF (a <= b)
- TU
- TT
(F)
- TF (a <= b)
- TF (a = b, others F <= b, reason)

- FF/P (a > b, others F <= b): reassign negation (on b)
- FF/C (a = b, others F <= b): analyze, producing learned clause FF/P, or UNSAT
- UF (others F <= b): assign new (on b)
- TF/P (a > b, others F <= b): reassign (on b)


watched clauses state transition:
assign new: (k)
(U)
- UU:
  - FF: normalize
    - FF/P (a > b, others F <= b): reassign negation (l >= k)
    - FF/C (a = b, others F <= b): analyze (l >= k), reassign negation (l' < l)
    - UF (others F <= b): assign new (l >= k)
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - FU: normalize
    - UF (others F <= b): assign new (l >= k)
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

reassign: (k)
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

reassign negation: (k)
(T)
- TF (a <= b):
  - FF: normalize
    - FF/P (a > b, others F <= b): reassign negation (l >= k)
    - FF/C (a = b, others F <= b): analyze (l >= k), reassign negation (l' < l)
    - UF (others F <= b): assign new (l >= k)
    - TF/P (a > b, others F <= b): reassign (l >= k)
- TU:
  - FU: normalize
    - UF (others F <= b): assign new (l >= k)
- TT:
  - FF: normalize
    - FF/P (a > b, others F <= b): reassign negation (l >= k)
    - FF/C (a = b, others F <= b): analyze (l >= k), reassign negation (l' < l)
    - UF (others F <= b): assign new (l >= k)
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - FT: normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
  - TF (a <= b)
  - TF (a > b): normalize
    - TF/P (a > b, others F <= b): reassign (l >= k)
(F)
- TF (a <= b):
  - TT
- TF (a = b, others F <= b, reason):
  - unassign (from l >= k), TT watched

unassign: (from k)
(T)
- TF (a <= b):
  - UF: normalize
    - UF (others F <= b): assign new (l >= k)
- TU
  - UU
- TT
  - UU
  - UT
  - TU
(F)
- TF (a <= b)
  - TU
- TF (a = b, others F <= b, reason)
  - unassign (from l >= k), TU watched


summary:
assign new: (k)
- reassign negation (l >= k)
- analyze (l >= k), reassign negation (l' < l)
- assign new (l >= k)
- reassign (l >= k)

reassign: (k)
- reassign (l >= k)

reassign negation: (k)
- reassign negation (l >= k)
- analyze (l >= k), reassign negation (l' < l)
- assign new (l >= k)
- reassign (l >= k)
- unassign (from l >= k)

unassign: (from k)
- assign new (l >= k)
- unassign (from l >= k)
