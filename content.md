Master Thesis

Connecting Minisat with cvc5 via IPASIR-UP Interface

Table of Content

- Introduction
  - Motivation
    (To test the usability of IPASIR-UP interface, provide a uniform interaction scheme with sat solvers)

  - Objectives
    (integrate minisat with cvc5 via IPASIR-UP interface, test performance, major work on minisat part)

  - Structure of the Thesis
    (TOC)

- Background and Related Work
  - Overview of SAT and SMT Solvers
    (SAT problem, SAT Solver, CDCL, existing SAT solvers, incremental solving, SMT problem, SMT solver, SAT solver interacting with SMT solver, existing SMT solvers)

  - The IPASIR and IPASIR-UP Interfaces
    (incremental sat solving, describe each interface function, user propagator)

  - Minisat
    (SAT solver, CDCL, IPASIR)

  - cvc5
    (SMT solver, multiple SAT solvers)

  - Related Work
    (existing cvc5 integration with CaDiCaL)

- Implementation
  - Development Environment and Tools
    (vscode/wsl, cmake, debugging and testing commands)

  - Extension of cvc5 for integrating Minisat
    (build cmake package of Minisat to be referenced and linked in cvc5, inherite existing interface for cadical)

  - Adaptation of MiniSat to IPASIR-UP
    (implementing IPASIR-UP on Minisat, solving time adding clause, lazy explanation, IPASIR-UP interfaces, cadical interfaces)

- Correctness and Performance Testing
  - Correctness
    (minisat fuzzer, drup checker, idrup checker, cvc5 make check, found bugs, cvc5/cadical bugs)

  - Performance
    (benchmarks, comparision with cadical and internal minisat, chronological backtracking optimization in minisat)

- Summary and Future Work
  (the integration, limitations, native proof support, an interface for other sat solvers)

- References:
  - 


questions:
- How many chapters and how many pages minimum for a master thesis? Existing thesis for reference.
- How deep the levels or how many sections?
- In which format to write the thesis? And how about version controlling? No latex. Google docs.
