# chronological backtracking

design choices and implications:
- reason clause and unassigning: (reason clauses can become invalid when reassigning negation or unassigning, triggering further unassigning)
  - ✅ full unassigning: (assigning in levels and backtracking)
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

- propagation schema:
  - ✅ assignment | -> { normalizing clause -> propagating clause -> assignment }
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
    - reason clause placeholder

- assignment trail: (just for unassigning when backtracking)
  - assignments before level index k: in level < k
  - new assignments: pushed back to the trail in any level
  - backtracking to k - 1: assignments in level < k shifted, assignments in level >= k unassigned

- propagation queue: (normalization of clauses of newly assigned/reassigned variable)
  - { level, variable: { level } } heap with lowest level in the front:
    - take front variable out to propagate
    - skip variable level not matching (invalid due to reassigning/backtracking)
  - normalize each watched clause of current variable with { level, variable: { level } }:
    - change watching literals
    - add new assign/reassign (current propagating level <= assignment level < next decision level) to the queue:
      - current variable itself can't be assigned or reassigned unless analysis
      - others variable reassigned existing item in propagation queue invalidates
    - when analysis involved:
      - filling reason clause can cause reassigning self or reassigning a variable as analyze level reason
      - adding learnt clause can cause unassigning and assigning self again
      - both lead to level unmatching after current normalization and break from current variable
  - existing variables in the queue:
    - unassigned after backtracking, level >= next decision level, variable.level undefined or variable.level < level
    - reassigned with lower level, variable.level < level


control flow:
- adding a list of new clauses cause:
  - each new clause immediately normalized and watched, possibly assigning/reassigning/UNSAT/backtracking/analysis
  - analysis adds a learnt clause, backtracking first unassigning the previous conflict clause, and assigning negation
  - assignments propagate in order after all clauses are added
  - new clauses can be added any time, also during propagation

- atomic operations:
  - normalization of a clause, and:
    - assigning/reassigning, pushing to propagation queue
    - exiting with UNSAT
    - backtracking, unassigning trail and assigning
    - analysis, possibly reassigning and pushing to propagation queue, adding learnt clause (-> backtracking)
  - assigning a decision variable, increasing next decision level, and pushing to propagation queue
  - assigning an arbitrary variable with current level and reason clause placeholder, pushing to propagation queue

- procedure:
  > solve:
    > propagate
  > normalize(clause):
    - skip -> next_clause
    - assign(level, literal)
    - reassign(level, variable)
    - exit(UNSAT)
    - backtrack(level), assign(level, literal)
    - analyze:
      - reassign(level, variable)
      - add(learnt):
        > backtrack(level), assign(level, literal)
      > -> next_variable
  > add(clause):
    - empty
      - exit(UNSAT)
    - unit:
      - exit(UNSAT)
      - assign(level, literal)
      - reassign(level, variable)
    - more than two:
      - skip -> next_clause
      - normalize(clause)
  > propagate:
    - get_next_variable
      - propagate_variable(level, variable)
        > watched_clauses: (loop)
          - skip -> next_clause
          - normalize(clause):
            - continue
              - unfilter
              - filter
            - break
      - get_new
  > get_new:
    - user:
      - assignment: (loop)
        > assign(level, literal)
      - clauses: (loop)
        > add(clause)
      > propagate
    - decide:
      > assign(level, literal)
      > propagate

- lazy reassignment: (keeping topological ordering for analysis without additional trails of levels)
  - new assignment (no matter which actual level) pushed back to trail (ordering is correct) (unchanged)
  - reassignment level stored as lazy level and reason updated, but propagation works on old level
  - before unassigning at backtracking, if there's lazy level, it is applied and pushed back to trail

sorting variants: (clause size >= 2)
(not yet watched)
- new clause before solving (root level T, F, others U)
  > skip if there is T, filter F -> all U
- new clause during solving (any level T, F, others U)
  > skip if there is root level T, filter root level F
  > UU, UT, TU, TT, TF (a <= b), FT (b <= a)
  - FU, FT (b > a) swap -> UF, TF (a > b)
  - UF, TF (a > b) find max level F from the second on
  - FF find two T, U from the others, or find one T, U and max F, or two max F
- lazy clause (one T, the others F)
  > find the T, replace to front, find max level F from the second on
- learnt clause after analysis (first max level F, all others F):
  > find max level F the second on
(watched)
- finding another watching literal (first T, U, F, second F, others T, U, F)
  > find T, U from the others and replace the second F
  > find max level F the second on


clause state:
[length = 0]
(propagating)
- exit(UNSAT)

[length = 1] (level a)
(static)
- T (a == 0)
(propagating)
- F (a == 0): exit(UNSAT)
- F (a > 0): backtrack(a - 1), assign(0)
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


operations:
assign(k):
- U -> F(k)
- U -> T(k)

reassign(k):
- F(l > k) -> F(k)
- T(l > k) -> T(k)

backtrack(k):
- F(l > k) -> U
- F(l <= k) -> F(l)
- T(l > k) -> U
- T(l <= k) -> T(l)


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


watched clauses state transition with immediate propagation:
[assign(k): U -> F(k)]
- UU ->
  - FU (a = k): normalize
    - UF (others F <= b', b' >= k): assign(b' >= k)
      [assign(k): U -> F(k), assign(l >= k): U -> F(l)]
      [assign(k): U -> F(k), assign(l >= k): U -> T(l)]
  - UF (b = k): normalize
    - ...FU (a = k)
- UT ->
  - FT (a = k): normalize
    - TF/P (a > b', others F <= b', b' >= k): reassign(b' >= k)
      [assign(k): U -> F(k), reassign(l >= k): F(l' > l) -> F(l)]
      [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l)]
- TU ->
  - TF (a <= b, b = k)
  - TF (a > b, b = k): normalize
    - ...FT (a = k)

[assign(k): U -> F(k), assign(l >= k): U -> F(l)]
- FU (a = k) ->
  - FF (a = k, b >= k): normalize
    - FF/P (a' > b', others F <= b', b' >= k): backtrack(a' - 1 >= k), assign(b' >= k)
      [assign(k): U -> F(k), assign(l >= k): U -> F(l), backtrack(>= k), assign(l' >= k)]
    - FF/C (a' = b', others F <= b', b' >= k):
      - exit(UNSAT)
      - analyze, producing learned clause
        - FF/P (a' > b'', others F <= b''): backtrack(a' - 1 >= k - 1), assign(b'')
          [assign(k): U -> F(k), assign(l >= k): U -> F(l), backtrack(>= k - 1), assign(l')]
    - UF (others F <= b', b' >= k): assign(b' >= k)
      [assign(k): U -> F(k), assign(l >= k): U -> F(l), assign(l' >= k): U -> F(l')]
      [assign(k): U -> F(k), assign(l >= k): U -> F(l), assign(l' >= k): U -> T(l')]
    - TF/P (a > b', others F <= b', b' >= k): reassign (b' >= k)
      [assign(k): U -> F(k), assign(l >= k): U -> F(l), reassign(l' >= k): F(l'' > l') -> F(l')]
      [assign(k): U -> F(k), assign(l >= k): U -> F(l), reassign(l' >= k): T(l'' > l') -> T(l')]
- UF (b = k) ->
  - FF (a >= k, b = k): normalize
    - ...FF (a = k, b >= k)

[assign(k): U -> F(k), assign(l >= k): U -> T(l)]
- FU (a = k) ->
  - FT (a = k, b >= k): normalize
    - TF (a' > b', others F <= b', b' >= k): reassign(b' >= k)
      [assign(k): U -> F(k), assign(l >= k): U -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
      [assign(k): U -> F(k), assign(l >= k): U -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]
- UF (b = k) ->
  - TF (a >= k, b = k): normalize
    - ...FT (a = k, b >= k)

[assign(k): U -> F(k), reassign(l >= k): F(l' > l) -> F(l)]
-> [assign(k): U -> F(k)]

[assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l)]
- FT (a = k, b > k) ->
  - FT (a = k, b >= k): normalize
    - TF/P (a > b', others F <= b', b' >= k): reassign(b' >= k)
      [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
      [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]
- TF (a > k, b = k) ->
  - TF (a >= k, b = k): normalize
    - ...FT (a = k, b >= k)

[assign(k): U -> F(k), assign(l >= k): U -> F(l), backtrack(>= k), assign(l' >= k)]


[assign(k): U -> F(k), assign(l >= k): U -> F(l), backtrack(>= k - 1), assign(l')]


[assign(k): U -> F(k), assign(l >= k): U -> F(l), assign(l' >= k): U -> F(l')]
-> [assign(k): U -> F(k), assign(l >= k): U -> F(l)]

[assign(k): U -> F(k), assign(l >= k): U -> F(l), assign(l' >= k): U -> T(l')]
-> [assign(k): U -> F(k), assign(l >= k): U -> T(l)]

[assign(k): U -> F(k), assign(l >= k): U -> F(l), reassign(l' >= k): F(l'' > l') -> F(l')]
-> [assign(k): U -> F(k), assign(l >= k): U -> F(l)]

[assign(k): U -> F(k), assign(l >= k): U -> F(l), reassign(l' >= k): T(l'' > l') -> T(l')]
-> [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l)]

[assign(k): U -> F(k), assign(l >= k): U -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
-> [assign(k): U -> F(k), assign(l >= k): U -> T(l)]

[assign(k): U -> F(k), assign(l >= k): U -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]
-> [assign(k): U -> F(k), assign(l >= k): U -> T(l)]

[assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
-> [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l)]

[assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]
-> [assign(k): U -> F(k), reassign(l >= k): T(l' > l) -> T(l)]


[reassign(k): F(l > k) -> F(k)]
- TF (a <= l)
  - TF (a <= k)
  - TF (a > k): normalize
    - TF (a > b, others F <= b, b >= k): reassign(b >= k)
      [reassign(k): F(l > k) -> F(k), reassign(l >= k): F(l' > l) -> F(l)]
      [reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l)]

[reassign(k): F(l > k) -> F(k), reassign(l >= k): F(l' > l) -> F(l)]
-> [reassign(k): F(l > k) -> F(k)]

[reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l)]
- TF (a > k)
  - TF (l = k)
  - TF (l > k): normalize
    - TF (a > b, others F <= b, b >= k): reassign(b >= k)
      [reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
      [reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]

[reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): F(l'' > l') -> F(l')]
-> [reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l)]

[reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l), reassign(l' >= k): T(l'' > l') -> T(l')]
-> [reassign(k): F(l > k) -> F(k), reassign(l >= k): T(l' > l) -> T(l)]


summary:
- assign(k) F:
  - assign(>= k) F:
    - backtrack(>= k), assign(>= k):
      - ...
    - exit(UNSAT)
    - backtrack(>= k - 1), assign(<=> k):
      - ...
    - assign(>= k) F:
        -> assign(k) F, assign(>= k) F
      assign(>= k) T:
        -> assign(k) F, assign(>= k) T
    - reassign(>= k) F:
        -> assign(k) F, assign(>= k) F
      reassign(>= k) T:
        -> assign(k) F, reassign(>= k) T
    assign(>= k) T:
    - reassign(>= k) F:
        -> assign(k) F, assign(>= k) T
      reassign(>= k) T:
        -> assign(k) F, assign(>= k) T
  - reassign(>= k) F:
      -> assign(k) F
    reassign(>= k) T:
    - reassign(>= k) F:
        -> assign(k) F, reassign(>= k) T
      reassign(>= k) T:
        -> assign(k) F, reassign(>= k) T
- reassign(k) F:
  - reassign(>= k) F:
      -> reassign(k) F
    reassign(>= k) T:
    - reassign(>= k) F:
        -> reassign(k) F, reassign(>= k) T
      reassign(>= k) T:
        -> reassign(k) F, reassign(>= k) T
