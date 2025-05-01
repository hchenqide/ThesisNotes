### 05.01

> minisat handle units in add_clause_lazy

- mark the reason clause as undef
  throw this unit as exception and break analyze
  backtrack to level 0 and assign this unit


> minisat understanding `analyze`, `litRedundant`, `analyzeFinal`

**analyze**

(at conflict level with conflict clause, output learnt clause and backtrack level)
(conflict clause must contain at least two false literals of conflict level)

- variables:
  - seen: map<Var, bool>
  - p: Lit
  - pathC: int
  - trail: vector<Lit>
  - index: size_t
  - out_learnt: vector<Lit>
  - analyze_toclear: vector<Lit>

- out_learnt leave empty entry at front for the asserting literal

- loop: with current conflict clause
  - for each literal in the clause if not `seen` and not root
    - mark as `seen`
    - if of conflict level, increase `pathC`
    - else add to `out_learnt`
  - find the next literal `p` marked `seen` in `trail` backwards
  - get the reason clause of `p` as the conflict clause to expand
  - unmark `seen` of `p`
  - decrease `pathC`
  (from the initial conflict clause, each loop expands one clause and add all literals in out_learnt except literals of conflict level)
  (pathC counts the number of unexpanded literals of conflict level)
  (loop stops when all but one literal on the conflict level is expanded)
  (until here only reason clauses of literals in conflict level are expanded)

- set this last literal as the first element for `out_learnt`
  (now `out_learnt` contains one negated literal from conflict level and all other literals from lower levels, this negated literal is to be propagated, the backtrack level depends on the level of the other literals after simplification)
  (now `seen` contains all literals in `out_learnt` of lower levels)

- copy current `out_learnt` to `analyze_toclear` which are to be unset in `seen` after analyze

(if the reason for an assignment is undef, then it is either a root level unit, or a decision)

- (ccmin_mode == 2) keep literals in `out_learnt` that either have reason undef or not redundant by `litRedundant`
  (ccmin_mode == 1) only check depth one, not recursive

- if `out_learnt` is unit, backtrack to level 0
  else swap the literal with the highest level to the second position and backtrack to this level

- unmark `seen` of literals in `analyze_toclear`

**litRedundant**

(literal in current learned clause, which is already marked `seen`)
(doesn't add new literals in learned clause)

- variables:
  - p: Lit
  - analyze_stack: stack<{int, Lit}>
  - seen
    - seen_undef
    - seen_source
    - seen_removable
    - seen_failed
  - analyze_toclear

- loop: with current literal `p`, it's reason clause `c` and index `i`, analyze_stack
  (stack keeps the index of iteration of each clause, can be just implemented with recursion)

(in general, it just checks if the literal `p` can be fully implied by other seen literals)
(so actually, if p is redunant, then its ancestors are also redunant, but here it's not memorized? no it is)

closure conditions: (given current `seen` set of literals)
- if all parents of a literal are seen or root, then it's removable
  if all parents of a literal are seen or root or removable, then it's also removable, ...
- if one parent of a literal is decision which is not seen, then it's not removable
  if one parent of a literal is not removable, then it's also not removable, ...

it's checked recursively and memorized in `seen`

> suggestion: `seen_removable` can override `seen_source` so when checking the next it can immediately return, and it can be defined recursively

**analyzeFinal**

(get the conflict clause from a literal that's false but is supposed to be true)

- variables:
  - out_conflict: set<Lit>
  - seen

- set p to be `seen`
- iterate over trail backwards until root level assignments, skipping literals that are not `seen`, expanding `seen` to the literals in the reason


> handle units in reasonLazy

during `analyze`:
- expanding on conflict level
- expanding checking redundant

during `analyzeFinal`:
