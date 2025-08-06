# chronological backtracking

each variable has its actual level
trace is a list of assignments that happened on each decision levels, but each assignment has an actual level
repropagate_queue is a list of variables that are reassigned and need to be repropagated
decision variables?

analyze produces a clause that reassigns negation
trail is no longer needed, no backtracking, only propagation queue for updating assignments
level is still needed even though the order is arbitrary, levels can be optimized if no assignments left on this level
keep the number of variables on each level

once a variable is assigned, it can never get unassigned
support unassigning a variable? (deleting a clause)


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
- UU (others T, U, F)
- UT (others T, U, F)
- TF (a <= b, others T, U, F)
- TU (others T, U, F)
- TT (others T, U, F)

(propagating)
- FF/P (a > b, others F <= b): reassign negation
- FF/C (a = b, others F <= b): analyze, producing learned clause FFP, or UNSAT
- UF (others F <= b): assign new
- TF/P (a > b, others F <= b): reassign

(normalizing)
- FF (others T, U, F):
  (replace two F with T or U)
  - UU
  - UT
  - TU
  - TT
  (replace one F with U, replace one F with F in max level)
  - UF (others F <= b): assign new
  (replace one F with T, replace one F with F >= a)
  - TF (a <= b, others F) as TF (a <= b, others T, U, F)
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

- UF (others T, U, F):
  (replace F with T or U)
  - UU
  - TU
  (replace F with F in max level)
  - UF (others F <= b): assign new

- TF (others T, U, F):
  - TF (a <= b, others T, U, F)
  - TF (a > b, others T, U, F): normalize

- TF (a > b, others T, U, F):
  (replace F with T or U)
  - TU
  - TT
  (replace F with F >= a)
  - TF (a <= b, others F) as TF (a <= b, others T, U, F)
  (replace F with F in max level)
  - TF/P (a > b, others F <= b): reassign

- TF (a > b, others F):
  (replace F with F >= a)
  - TF (a <= b, others F) as TF (a <= b, others T, U, F)
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
- FF (others T, U, F): normalize
- FU (others T, U, F): normalize
- FT (others T, U, F): normalize
- UF (others T, U, F): normalize
- UU
- UT
- TF (others T, U, F): normalize
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

- TF (a <= b, others T, U, F)

- TU (others T, U, F):
  - TF (others T, U, F): normalize
  - TT (others T, U, F)

- TT (others T, U, F)


reassign: (false branch)
- UU (others T, U, F)

- UT (others T, U, F)

- TF (a <= b, others T, U, F)
  - TF (a <= b, others T, U, F)
  - TF (a > b, others T, U, F): normalize

- TU (others T, U, F)

- TT (others T, U, F)


reassign negation: (false branch)
- UU (others T, U, F)

- UT (others T, U, F)
  - UF (others T, U, F): normalize

- TF (a <= b, others T, U, F)
  - FF (others T, U, F): normalize
  - FT (others T, U, F): normalize
  - TT (others T, U, F) as TT (others T, U, F)

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

- TF (a <= b, others T, U, F)
  - UF (others T, U, F): normalize
  - TF (a <= b, others T, U, F)
  - TU (others U, F) as TU (others T, U, F)

- TU (others T, U, F)
  - UU (others T, U, F)

- TT (others T, U, F)
  - UU (others T, U, F)
  - UT (others T, U, F)
  - TU (others T, U, F)


only TF (a = b, others F <= b) can be a reason clause (not necessarily be a reason clause), if it no longer holds even if relaxed, like TF (a <= b, others F <= b), TF (a <= b, others F) or TF (a <= b, others T, U, F), it can no longer be the reason clause, which causes a variable to change reason or unassign
- in assign/reassign: if new assignment/reassignment happen, the reason clause is set/updated
- in unassign: the others literals are not watched, so when they become unassigned, the affected reason clauses can't be retrieved, unless with full watching
- but during conflict analysis, it can be lazily discovered and unassigned, thus even able to resolve a conflict
- actually the assignments with invalid reason clauses can just be regarded as decisions
- three decisions on the same level that caused a conflict can cause reassignment on a higher level
- so all decision can be made on level 0, until there is a conflict then the level of second is increased to propagate the third

TF (a = b, others F <= b): (reason clause)
(reassign)
- TF (a <= b, others F <= b)
- TF (a > b, others F): normalize

(reassign negation)


todo:
1. unstable states maintance
2. reassignment on higher level
