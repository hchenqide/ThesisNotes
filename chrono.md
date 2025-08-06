# chronological backtracking

- each variable has its actual level
- trace is a list of assignments that happened on each decision levels, but each assignment has an actual level
- repropagate_queue is a list of variables that are reassigned and need to be repropagated
- decision variables?

- analyze produces a clause that reassigns negation
 trail is no longer needed, no backtracking, only propagation queue for updating assignments
- level is still needed even though the order is arbitrary, levels can be optimized if no assignments left on this level
- keep the number of variables on each level

- once a variable is assigned, it can never get unassigned
- support unassigning a variable? (deleting a clause)


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
- FF/P (a > b, others F <= b): reassign negation
- FF/C (a = b, others F <= b): analyze, producing learned clause FFP, or UNSAT
- UF (others F <= b): assign new
- TF/P (a > b, others F <= b): reassign

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


- in one temporary propagation queue, all assignments/reassignments are on the same level, reassignments are on a lower level than original level
- which to propagate when all exist?
- if one variable has multiple propagations, only the one on the least level will remain, and there wouldn't be propagations of variables on the same level with opposite assignments
- each variable has states: static, enqueued(assign new, reassign, reassign negation), propagating
- a clause whose both watching variables are static is static, one of them enqueued or propagating is static or normalizing
- variable states transition:
  - static -> enqueued (with updated reason clause)
  - enqueued -> propagating (setting other variables from static to enqueued)
  - propagating -> static (all watched clauses of this variable are now static)
- clauses are either static or normalizing
- when a variable is propagating, a watched normalizing clause will become static, or propagate and immediately become static, the next watched clauses remain normalizing, previous watched clauses can become normalizing but watched by another propagating variable. so a propagating clause wouldn't last


clause state transition:
assign new:
[length = 1]
(static)
- T
[length >= 2]
(static)
- UU:
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
- UT:
  - FT: normalize
  - UT
  - TT
- TF (a <= b):
  - TF (a <= b)
- TU:
  - TF (a <= b)
  - TF (a > b): normalize
  - TU
  - TT
- TT:
  - TT
(normalizing)
- FF:
  - FF: normalize
- FU:
  - FF: normalize
  - FU: normalize
  - FT: normalize
- FT:
  - FT: normalize
- UF:
  - FF: normalize
  - UF: normalize
  - TF (a <= b)
  - TF (a > b): normalize
- TF (a > b):
  - TF (a > b): normalize

reassign: (lower level)
(static)
- UU:
  - UU
- UT:
  - UT
- TF (a <= b):
  - TF (a <= b)
  - TF (a > b): normalize
- TU
  - TU
- TT
  - TT
(normalizing)
- FF:
  - FF: normalize
- FU:
  - FU: normalize
- FT:
  - FT: normalize
- UF:
  - UF: normalize
- TF (a > b):
  - TF (a <= b)
  - TF (a > b): normalize

reassign negation: (lower level)
(static)
- UU:
  - UU
- UT:
  - UF: normalize
  - UT
- TF (a <= b):
  - FF: normalize
  - FT: normalize
  - TF (a <= b)
  - TT
- TU:
  - FU: normalize
  - TU
- TT
  - FF: normalize
  - FT: normalize
  - TF (a <= b)
  - TF (a > b): normalize
  - TT
(normalizing)
- FF:
  - FF: normalize
  - FT: normalize
  - TF (a <= b)
  - TF (a > b): normalize
  - TT
- FU:
  - FU: normalize
  - TU
- FT:
  - FF: normalize
  - FT: normalize
  - TF (a <= b)
  - TF (a > b): normalize
  - TT
- UF:
  - UF: normalize
  - UT
- TF (a > b):
  - FF: normalize
  - FT: normalize
  - TF (a > b): normalize
  - TT


- all normalize cases can be accessed from the false branch of an assignment


unassign:
- UU:
  - UU
- UT:
  - UU
  - UT
- TF (a <= b):
  - UF: normalize
  - TF (a <= b)
  - TU
- TU:
  - UU
  - TU
- TT:
  - UU
  - UT
  - TU


reassign: (higher level)
(static)
- UU:
  - UU
- UT:
  - UT
- TF (a <= b):
  - TF (a <= b)
  - TF (a > b): normalize
- TU
  - TU
- TT
  - TT
(normalizing)
- FF:
  - FF: normalize
- FU:
  - FU: normalize
- FT:
  - FT: normalize
- UF:
  - UF: normalize
- TF (a > b):
  - TF (a <= b)
  - TF (a > b): normalize


reason clause state:
(valid)
- TF (a = b, others F <= a/b) < TF (a <= b)
(maybe valid)
- TF (a <= b)
(invalid)
- TU
- TT


reason clause state transition: (target variable)
assign new:
- TF (a = b, others F <= a/b):
  - TF (a = b, others F <= a/b) -> unchanged
- TF (a <= b):
  - TF (a <= b) -> maybe valid
- TU:
  - TF (a <= b) -> maybe valid
  - TF (a > b): normalize
    - TU: invalid
    - TT: invalid
    - TF (a <= b) -> maybe valid
    - TF/P (a > b, others F <= b): reassign
      - TF (a = b, others F <= a/b) -> unchanged
  - TU (others U, F) -> invalid
- TT:
  - TT -> invalid

reassign: (lower level)
- TF (a = b, others F <= a/b):
  (target variable)
  -> changed
  (others)
  - TF (a = b, others F <= a/b) -> unchanged
  - TF (a > b, others F <= a): normalize
    - TF (a = b, others F <= a/b) -> unchanged
    - TF (a > b, others F <= b): reassign
      - TF (a = b, others F <= a/b) -> unchanged
- TF (a <= b):
  - TF (a <= b) -> maybe valid
  - TF (a > b): normalize
    - TU -> invalid
    - TT -> invalid
    - TF (a <= b) -> maybe valid
    - TF/P (a > b, others F <= b): reassign
      - TF (a = b, others F <= a/b) -> unchanged
- TU:
  - TU -> invalid
- TT:
  - TT -> invalid

reassign negation: (lower level)
- TF (a = b, others F <= a/b):
  - FF -> changed
  - FT -> changed
  - TF (a = b, others T < a/b, F <= a/b) < TF (a <= b) -> invalid
  - TT (a > b, others T < a, F <= a) -> invalid
- TF (a <= b):
  - FF -> changed
  - FT -> changed
  - TF (a <= b) -> maybe valid
  - TT -> invalid
- TU:
  - FU -> changed
  - TU: invalid
- TT
  - FF -> changed
  - FT -> changed
  - TF (a <= b) -> maybe valid
  - TF (a > b): normalize
    - TU -> invalid
    - TT -> invalid
    - TF (a <= b) -> maybe valid
    - TF/P (a > b, others F <= b): reassign
      - TF (a = b, others F <= a/b) -> unchanged
  - TT -> invalid

unassign:
- TF (a = b, others F <= a/b):
  - UF -> dropped
  - TF (a = b, others U, F <= a/b) < TF (a <= b) -> invalid
  - TU (others U, F <= a) < TU (others U, F) -> invalid
- TF (a <= b):
  - UF -> dropped
  - TF (a <= b) -> maybe valid
  - TU -> invalid
- TU:
  - UU -> dropped
  - TU -> invalid
- TT:
  - UU -> dropped
  - UT -> dropped
  - TU -> invalid


- unassigning happens through reason clauses (deleted or invalid)
- an invalid reason can happen to become valid again, so unassigning should happen lazily, or just reassigning to higher level without unassigning

- during conflict analysis, invalid reason clauses can be lazily discovered and dropped
- actually the assignments with invalid reason clauses can just be regarded as decisions
- three decisions on the same level that caused a conflict can cause reassignment on a higher level
- so all decision can be made on level 0, until there is a conflict then the level of second is increased to propagate the third
