### 05.09

> meeting

-
  volatile    bool                asynch_interrupt;

-
  assert(level(a) >= level(b));  // level(a) > level(b) is possible
  (backtrack to level b and propagate)

-
  try {
      analyze(confl, learnt_clause, backtrack_level);
  } catch (Lit l) {
      for (int i = 0; i < analyze_toclear.size(); i++) seen[var(analyze_toclear[i])] = 0;
      cancelUntil(0);  // chronological? cvc5 doesn't do
      uncheckedEnqueue(l);
      continue;
  }

-
  ClauseLearner
  learned clauses from analyze
  proof?

> implementations (05.11)

- make asynch_interrupt volatile

- lazy clause handle level(a) > level(b)

- added clause learner
