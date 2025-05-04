### 05.04

> debug

- errors with `--lang=sygus2`
  --lang=sygus2 --sygus-si=none --sygus-out=status --sygus-fair=direct phone-1-long.sy
  Unexpected output difference
  simplify the test case
    ; COMMAND-LINE: --lang=sygus2 --sygus-si=none --sygus-out=status --sygus-fair=direct
    ; EXPECT: feasible
    (set-logic SLIA)
    (synth-fun f ((name String)) String
        ((ntString String))
        ((ntString String (name
                          (str.substr name 0 0)))))
    (constraint (= (f "") ""))
    (check-synth)
  sat but outputs `fail`
  after solve `d_theoryProxy->isModelUnsound()` returns true
  `TheoryEngine::d_modelUnsound` is true
  set breakpoint at `TheoryEngine::setModelUnsound`
  called by `QuantifiersEngine::check` during `d_proxy->theoryCheck` at `cb_check_found_model`
  clause `5 6 0` added
  add tag `-t quant-engine-debug`
  cadical also has such behaviour
  and cadical after solve `TheoryEngine::d_modelUnsound` is also true, so it's not because of this
  in `SygusSolver::checkSynth` comment suggests that `getSynthSolutions` should return true
  this calls `SynthEngine::getSynthSolutions`, then calls `SynthConjecture::getSynthSolutions`, then `SynthConjecture::getSynthSolutionsInternal` which failed with `d_hasSolution` false
  debug with cadical, set breakpoint on `d_hasSolution`
  set true at `SynthConjecture::doCheck` at final `cb_check_found_model`
  but in minisatup this is not set
  add tag `-t sygus-engine-debug`
  compare log
  in cadical there are more lines after `...empty model, fail.`
    Checking 1 active conjectures...
  this variable seems to be the only difference
    new var: 4 (level: 0, is_theory_atom: 0, in_search: 0)
    phase: 4
    addClause (0): 4 ~4 0
  in minisat it's assigned as -4, with
    value: 4: 0
    Enumerator ee_4 is inactive.
  in cadical it's 4
    getEnumeratedValues
    value: 4: 1
      * Value is : ee_4 -> (str.substr name 0 0) 
  minisat doesn't assign according to the phase, but this shouldn't be an error
> guess: this should also be an error for cvc5
  check minisat phase
    else if (user_pol[next] != l_Undef)
        return mkLit(next, user_pol[next] == l_True);
  while
    setPolarity(intToVar(lit), lit > 0 ? l_True : l_False);
  so here the polarity is just wrong
  fixed
  now it outputs feasible and the traces with cadical are the same
