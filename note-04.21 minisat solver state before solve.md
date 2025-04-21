# step 9 (04.21)

> understanding conditions for multiple solver calls with propagator

(after `solve()` and before the next `solve()` call)

[solver]
- at decision level 0
- `trail` contains level 0 assignments
- new clauses added and watched
- new assumptions added
- `notify_assignment_index` pointing to the next position to notify on the trail, where there might be new assignments added with unit clauses
- `notify_backtrack` is false

[propagator]
- at decision level 0
- maintains level 0 assignments
