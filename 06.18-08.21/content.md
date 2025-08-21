Master Thesis

(note)
- "MiniSat"

Title
From MiniSat to MiniSatUP and its Integration with cvc5 via IPASIR-UP
x MiniSatUP with IPASIR-UP and cvc5 Integration 

Abstract
(story, explain the title, how far it reached)
"the second implementation of IPASIR-UP"
(each point will be explained in a paragraph in introduction)
(less than one page, no citation)
(after writing introduction)


Table of Content

- Introduction: 2 pages
  (longer abstract)

  Motivation
  (To test the usability of IPASIR-UP interface, provide a uniform interaction scheme with sat solvers)

  Objectives and Achievements
  (integrate minisat with cvc5 via IPASIR-UP interface, fuzzing, performance, major work on minisat part)

  Structure of the Thesis
  (TOC)

- Background and Related Work
  - Overview of SAT Solvers
    (SAT problem: 1 page, CDCL, existing SAT solver Minisat "core, simp, only work on core", incremental solving with IPASIR)

  - Overview of SMT Solvers
    (SMT problem, SMT solver, SAT solver interacting with SMT solver, existing SMT solver cvc5)

  - IPASIR-UP Interface
    (describe each interface function: 4-5 pages, user propagator)

  - Fuzzing Sat Solvers

  - Related Work: 2 pages
    (Nicolaj, Bjorner, earlier one without Katalin, one with) (search for more citations, find motivations)
    (existing cvc5 integration with CaDiCaL, )

- Implementation
(start here)
  - Adaptation of MiniSat to IPASIR-UP
    (principles: minimum change)
    (with some code, like the goto part) (how much changes of lines)
    (implementing IPASIR-UP on Minisat, solving time adding clause, lazy explanation, IPASIR-UP interfaces, cadical interfaces)

  - Extension of cvc5 for integrating Minisat
    (a graph showing cvc5 with cadical and minisatup, no code)
    (fixed assignments (out-of-order only for units))
    (build cmake package of Minisat to be referenced and linked in cvc5, inherite existing interface for cadical)

  - (Optional) Development Environment and Tools
    (vscode/wsl, cmake, debugging and testing commands)

- Correctness and Performance Testing
  - Correctness
    (minisat fuzzer (bugs before and after cvc5 integration), drup checker, idrup checker, cvc5 make check, found bugs, cvc5/cadical bugs)
    (bugs classificaiton (severity, difficulty to find/fix))

  - Performance
    (benchmarks, comparision with cadical and internal minisat, chronological backtracking optimization in minisat)
    (mention the cadical bug with cvc5 with user propagator "a clause as a better reason will be missed after backtracking" with chronological backtracking)

- Summary and Future Work
  (the integration, limitations, cvc5 proof interface, or a proof interface for IPASIR-UP, for more solvers)

- References: 1-2 pages
  - 


questions:
- How many chapters and how many pages minimum for a master thesis? (around 40) Existing thesis for reference.
- How deep the levels or how many sections? (chapter - section 2 levels)
- In which format to write the thesis? And how about version controlling? latex for vscode (for citation) (overleaf with git)
  fachschaft template
