### 06.04

> results of `make check` in production build

summary: 4085 tests, 5 unfinished below, all others passed (some skipped):
- 2791 metitarski-3-4.smt2
- 2858 proj-issue619-nconst-nl-mv.smt2
- 3042 issue5735-subtypes.smt2
- 3890 issue6057-replace-re-all.smt2
- 4063 api/cpp/issues/proj-issue421

in the previous make check with debug build, the following failed:
(timeout)
- 2791 regress1/nl/metitarski-3-4.smt2
- 2858 regress1/proj-issue619-nconst-nl-mv.smt2
- 3042 regress1/quantifiers/issue5735-subtypes.smt2
- 3890 regress2/strings/issue6057-replace-re-all.smt2
- 4063 api/cpp/issues/proj-issue421 (Unfinished)
(assertion error)
- 3588 regress1/sygus/cegis-unif-inv-eq-fair.sy
(check error)
- 4106 unit/api/c/capi_uncovered_black
- 4119 unit/api/cpp/api_solver_black
- 4097 unit/api/c/capi_solver_black

less failed tests than debug build (), might because 1.assertions not activated 2. less tests are run
