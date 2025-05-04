### 05.03

> debug

- use internal minisat with `--check-proofs` flag
    if(env.getOptions().smt.checkProofs) {
      return new MinisatSatSolver(env, registry);
    }
  the option `--sub-cbqi` alone would also check proof

- error occurs many times:
    Fatal failure within void cvc5::internal::SharedTermsDatabase::addSharedTerm(cvc5::internal::TNode, cvc5::internal::TNode, cvc5::internal::theory::TheoryIdSet) at cvc5/src/theory/shared_terms_database.cpp:99 theories != (*find).second
  add trace tag `-t register`
  with test `-i test/regress/cli/regress1/strings/issue6072-inc-no-const-reg.smt2`
    SharedTermsDatabase::addSharedTerm
  occurs before assignment
  set breakpoint
  called when adding a new literal as well
  constructed a minimal example with the error:
    ; COMMAND-LINE: -i
    ; EXPECT: sat
    ; EXPECT: sat
    (set-logic ALL)
    (declare-fun s () String)
    (assert (= (str.len s) 0))
    (push)
    (check-sat)
    (pop)
    (check-sat)
  the same term is visited another time
  before `visit`, there is a check `alreadyVisited`, but the second time the check passed
  add trace tag `-t register::internal`
> guess: it's a bug from cvc5
  test with cadical
  construct a test case where the assertion (= (str.len s) 0) is only fixed in user-level 1
    (set-logic ALL)
    (declare-fun s () String)
    (declare-fun a () Bool)
    (declare-fun b () Bool)
    (assert (or (not a) b))
    (assert (or (not b) a))
    (assert (or (not a) (not b)))
    (assert (or a (= (str.len s) 0)))
    (push)
    (check-sat)
    (pop)
    (check-sat)
  but it's still already fixed at level 0
  and cadical wouldn't produce fix variables because it just decides -a at the beginning so there's no backtrack
  inverse a
    (set-logic ALL)
    (declare-fun a () Bool)
    (declare-fun b () Bool)
    (assert (or (not a) b))
    (assert (or (not b) a))
    (assert (or a b))
    (push)
    (check-sat)
    (pop)
    (check-sat)
  now there's fixed assignment at user level 1, but there's no renotify
  because `is_theory_atom` is false
    (set-logic ALL)
    (declare-fun s () String)
    (declare-fun a () Bool)
    (declare-fun b () Bool)
    (assert (or (not a) b))
    (assert (or (not b) a))
    (assert (or a b))
    (assert (or (not a) (= (str.len s) 0)))
    (push)
    (check-sat)
    (pop)
    (check-sat)
  now there's renotify
  and there's the same error
  tested it with cvc5 online but it runs well, did they already fix it?
  maybe it's production version with assertions disabled
