# meeting 04.25

- val() semantic
  https://github.com/arminbiere/cadical/blob/development/src/cadical.hpp :309

- terminiate sync/async
  void terminate() {}
  (async) volatile bool flag, reset flag and clear assumptions? backtrack?
  void connect_terminator(Terminator *terminator) {}
  (sync) check in every loop before decision, if returns true, terminate()

- phase
  `user_pol` in minisat

- propagate - assume order
  call cb_propagate before picking next assumption for a new level

- lazy propagation unit
  backtrack and set, skip analyze
  https://github.com/m-fleury/glucose/blob/main/core/Solver.cc strongBacktrack (chronological backtracking)
