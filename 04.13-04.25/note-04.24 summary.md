### step 1-10 (04.11 - 04.25)

> summary

#### done

- create minisatup solver in cvc5, link with minisat, and debug
- implement ipasir interface in minisatup, test and bug fixes, only cadical tests failed (in other words, minisatup as bitvector implementation in place of cadical passed all tests, which doesn't use ipasirup)
- implement ipasirup interface, understand CadicalPropagator in cvc5, add comments and assertions, debug cadical tests

#### todo

- run full `make check` with minisatup replacing both minisat and cadical

- implement set resource limit
- implement lazily adding unit clause (test/regress/cli/regress1/strings/issue6101.smt2)

#### concepts

- fixed variable
- user push/pop, user level, activation literal, active variable

#### discovered issues in CadicalPropagator
