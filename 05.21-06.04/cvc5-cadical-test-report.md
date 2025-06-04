# Errors in the tests of cvc5 with cadical replacing minisat

1. Replace `MinisatSatSolver` with `CadicalSolver` in `SatSolverFactory::createCDCLTMinisat` in cvc5, with the patch below: (based on origin/HEAD 8f0fed3a4370ed13c03df4dfc3d95021e94fcce1 Date: Thu May 29 2025 19:06:27 GMT+0200)

```diff
diff --git a/src/prop/sat_solver_factory.cpp b/src/prop/sat_solver_factory.cpp
index 6737265846..97d5943c6c 100644
--- a/src/prop/sat_solver_factory.cpp
+++ b/src/prop/sat_solver_factory.cpp
@@ -14,6 +14,7 @@
  */
 
 #include "prop/sat_solver_factory.h"
+#include "options/smt_options.h"
 
 #include "prop/cadical.h"
 #include "prop/cryptominisat.h"
@@ -26,6 +27,11 @@ namespace prop {
 CDCLTSatSolver* SatSolverFactory::createCDCLTMinisat(
     Env& env, StatisticsRegistry& registry)
 {
+  if (!env.getOptions().smt.produceProofs) {
+    CadicalSolver* res = new CadicalSolver(env, registry, "");
+    res->setResourceLimit(env.getResourceManager());
+    return res;
+  }
   return new MinisatSatSolver(env, registry);
 }
```

2. Build cvc5 and run make check

```sh
./configure.sh debug --auto-download
cd build
make -j$(( $(nproc) + 1 ))
make check
```

output:
```
...
The following tests FAILED:
	 52 - regress0/arith/issue1399.smt2 (Failed)
	1078 - regress0/nl/dd.sin-cos-346-b-chunk-0210.smt2 (Failed)
	2428 - regress1/arith/issue7252-arith-sanity.smt2 (Failed)
	2797 - regress1/nl/nl_uf_lalt.smt2 (Failed)
	3042 - regress1/quantifiers/issue5735-subtypes.smt2 (Failed)
	3283 - regress1/sets/finite-type/sets-card-color-sat.smt2 (Failed)
	3410 - regress1/strings/issue5611-deq-norm-emp.smt2 (Failed)
	3714 - regress1/sygus/proj-issue165.smt2 (Failed)
	3812 - regress2/arith/real2int-test.smt2 (Failed)
	4097 - unit/api/c/capi_solver_black (Failed)
	4106 - unit/api/c/capi_uncovered_black (Failed)
	4119 - unit/api/cpp/api_solver_black (Failed)
```

summary:
- 12 failed
  - 7 timeouts
    - 52 - regress0/arith/issue1399.smt2 (Failed)
    - 1078 - regress0/nl/dd.sin-cos-346-b-chunk-0210.smt2 (Failed)
    - 2428 - regress1/arith/issue7252-arith-sanity.smt2 (Failed)
    - 2797 - regress1/nl/nl_uf_lalt.smt2 (Failed)
    - 3042 - regress1/quantifiers/issue5735-subtypes.smt2 (Failed)
    - 3714 - regress1/sygus/proj-issue165.smt2 (Failed)
    - 3812 - regress2/arith/real2int-test.smt2 (Failed)
  - 2 errors
    - 3283 - regress1/sets/finite-type/sets-card-color-sat.smt2 (Failed)
    - 3410 - regress1/strings/issue5611-deq-norm-emp.smt2 (Failed)
  - 3 errors with api
    - 4097 - unit/api/c/capi_solver_black (Failed)
    - 4106 - unit/api/c/capi_uncovered_black (Failed)
    - 4119 - unit/api/cpp/api_solver_black (Failed)

3. Another error: [issue 11867](https://github.com/cvc5/cvc5/issues/11867)
