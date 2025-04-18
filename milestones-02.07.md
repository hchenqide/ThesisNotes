#+TITLE: Adding MiniSAT to cvc5 via IPASIR-UP

* implement missing parts fo ipasir-up interface
for cvc like
   - observed: only notify observed variables
   - possible new variables -- only during add_observed?
   - guarantee that only observed variables are added
     but not required for minisat, only critical for
     inprocessing [and makes testing easier]
* connect minisat to cvc, experiments
   [Conjecture: for non-incremental no assumptions]

* (Optional) Optimization:
  - IDRUP
  - assumptions (requires IDRUP)
Even more optional:
  - readd VE with every observed being observed
    + only not-eliminated can be observed
  - connect fuzzer to cadical
  - chrono BT


Final milestone:
* writing thesis
* presentation
