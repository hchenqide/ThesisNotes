// cadical.h without comments

#ifndef _cadical_hpp_INCLUDED
#define _cadical_hpp_INCLUDED

#include <cstdint>
#include <cstdio>
#include <vector>

namespace CaDiCaL
{
    enum Status
    {
        SATISFIABLE = 10,
        UNSATISFIABLE = 20,
        UNKNOWN = 0,
    };

    enum State
    {
        INITIALIZING = 1, 
        CONFIGURING = 2,  
        STEADY = 4,       
        ADDING = 8,       
        SOLVING = 16,     
        SATISFIED = 32,   
        UNSATISFIED = 64, 
        DELETING = 128,   

        READY = CONFIGURING | STEADY | SATISFIED | UNSATISFIED,
        VALID = READY | ADDING,
        INVALID = INITIALIZING | DELETING
    };

    class File;
    class Testing;
    struct Internal;
    struct External;

    class Learner;
    class FixedAssignmentListener;
    class Terminator;
    class ClauseIterator;
    class WitnessIterator;
    class ExternalPropagator;
    class Tracer;
    class InternalTracer;
    class FileTracer;
    class StatTracer;

    class Solver
    {

    public:

        Solver();
        ~Solver();

        static const char *signature(); 

        void add(int lit);

        void clause(int);                      
        void clause(int, int);                 
        void clause(int, int, int);            
        void clause(int, int, int, int);       
        void clause(int, int, int, int, int);  
        void clause(const std::vector<int> &); 
        void clause(const int *, size_t);      

        bool inconsistent();

        void assume(int lit);

        int solve();

        int val(int lit);

        bool flip(int lit);

        bool flippable(int lit);

        bool failed(int lit);

        void connect_terminator(Terminator *terminator);
        void disconnect_terminator();

        void connect_learner(Learner *learner);
        void disconnect_learner();

        void connect_fixed_listener(FixedAssignmentListener *fixed_listener);
        void disconnect_fixed_listener();

        void connect_external_propagator(ExternalPropagator *propagator);
        void disconnect_external_propagator();

        void add_observed_var(int var);

        void remove_observed_var(int var);

        void reset_observed_vars();

        bool is_decision(int lit);

        void force_backtrack(size_t new_level);

        void constrain(int lit);

        bool constraint_failed();

        int propagate();
        void get_entrailed_literals(std::vector<int> &implicants);

        int lookahead(void);

        struct CubesWithStatus
        {
            int status;
            std::vector<std::vector<int>> cubes;
        };

        CubesWithStatus generate_cubes(int, int min_depth = 0);

        void reset_assumptions();
        void reset_constraint();

        const State &state() const { return _state; }

        int status() const
        {
            if (_state == SATISFIED)
                return 10;
            else if (_state == UNSATISFIED)
                return 20;
            else
                return 0;
        }

        static const char *version(); 

        void copy(Solver &other) const;

        int vars();

        void reserve(int min_max_var);

#ifndef NTRACING

        void trace_api_calls(FILE *file);
#endif

        static bool is_valid_option(const char *name);

        static bool is_preprocessing_option(const char *name);

        static bool is_valid_long_option(const char *arg);

        int get(const char *name);

        void prefix(const char *verbose_message_prefix);

        bool set(const char *name, int val);

        bool set_long_option(const char *arg);

        static bool is_valid_configuration(const char *);

        bool configure(const char *);

        void optimize(int val);

        bool limit(const char *arg, int val);
        bool is_valid_limit(const char *arg);

        int active() const;          
        int64_t redundant() const;   
        int64_t irredundant() const; 

        int simplify(int rounds = 3);

        void terminate();

        bool frozen(int lit) const;
        void freeze(int lit);
        void melt(int lit); 

        int fixed(int lit) const;

        void phase(int lit);
        void unphase(int lit);

        bool trace_proof(FILE *file, const char *name); 
        bool trace_proof(const char *path);             

        void flush_proof_trace(bool print = false);

        void close_proof_trace(bool print = false);

        void connect_proof_tracer(Tracer *tracer, bool antecedents,
                                  bool finalize_clauses = false);
        void connect_proof_tracer(InternalTracer *tracer, bool antecedents,
                                  bool finalize_clauses = false);
        void connect_proof_tracer(StatTracer *tracer, bool antecedents,
                                  bool finalize_clauses = false);
        void connect_proof_tracer(FileTracer *tracer, bool antecedents,
                                  bool finalize_clauses = false);

        void conclude();

        bool disconnect_proof_tracer(Tracer *tracer);
        bool disconnect_proof_tracer(StatTracer *tracer);
        bool disconnect_proof_tracer(FileTracer *tracer);

        static void usage(); 

        static void configurations(); 

        void statistics(); 
        void resources();  

        void options(); 

        bool traverse_clauses(ClauseIterator &) const;
        bool traverse_witnesses_backward(WitnessIterator &) const;
        bool traverse_witnesses_forward(WitnessIterator &) const;

        const char *read_dimacs(FILE *file, const char *name, int &vars,
                                int strict = 1);

        const char *read_dimacs(const char *path, int &vars, int strict = 1);

        const char *read_dimacs(FILE *file, const char *name, int &vars,
                                int strict, bool &incremental,
                                std::vector<int> &cubes);

        const char *read_dimacs(const char *path, int &vars, int strict,
                                bool &incremental, std::vector<int> &cubes);

        const char *write_dimacs(const char *path, int min_max_var = 0);

        const char *write_extension(const char *path);

        static void build(FILE *file, const char *prefix = "c ");

    private:

        bool adding_clause;
        bool adding_constraint;

        State _state; 

        Internal *internal; 
        External *external; 

        friend class Testing; 

#ifndef NTRACING

        bool close_trace_api_file; 
        FILE *trace_api_file;      

        static bool tracing_api_through_environment;

        void trace_api_call(const char *) const;
        void trace_api_call(const char *, int) const;
        void trace_api_call(const char *, const char *) const;
        void trace_api_call(const char *, const char *, int) const;
#endif

        void transition_to_steady_state();

        friend class App;
        friend class Mobical;
        friend class Parser;

        const char *read_solution(const char *path);

#ifndef PRINTF_FORMAT
#ifdef __MINGW32__
#define __USE_MINGW_ANSI_STDIO 1
#define PRINTF_FORMAT __MINGW_PRINTF_FORMAT
#else
#define PRINTF_FORMAT printf
#endif
#endif

#define CADICAL_ATTRIBUTE_FORMAT(FORMAT_POSITION,            \
                                 VARIADIC_ARGUMENT_POSITION) \
    __attribute__((format(PRINTF_FORMAT, FORMAT_POSITION,    \
                          VARIADIC_ARGUMENT_POSITION)))

        void section(const char *);     
        void message(const char *, ...) 
            CADICAL_ATTRIBUTE_FORMAT(2, 3);

        void message();               
        void error(const char *, ...) 
            CADICAL_ATTRIBUTE_FORMAT(2, 3);

        void verbose(int level, const char *, ...)
            CADICAL_ATTRIBUTE_FORMAT(3, 4);

        const char *read_dimacs(File *, int &, int strict, bool *incremental = 0,
                                std::vector<int> * = 0);

        int call_external_solve_and_check_results(bool preprocess_only);

        void dump_cnf();
        friend struct DumpCall; 

        ExternalPropagator *get_propagator();
        bool observed(int lit);
        bool is_witness(int lit);

        friend struct LemmaCall;
        friend struct ObserveCall;
        friend struct DisconnectCall;
        friend class MockPropagator;
    };

    class Terminator
    {
    public:
        virtual ~Terminator() {}
        virtual bool terminate() = 0;
    };

    class Learner
    {
    public:
        virtual ~Learner() {}
        virtual bool learning(int size) = 0;
        virtual void learn(int lit) = 0;
    };

    class FixedAssignmentListener
    {
    public:
        virtual ~FixedAssignmentListener() {}

        virtual void notify_fixed_assignment(int) = 0;
    };

    class ExternalPropagator
    {

    public:
        bool is_lazy = false; 
        bool are_reasons_forgettable =
            false; 

        virtual ~ExternalPropagator() {}

        virtual void notify_assignment(const std::vector<int> &lits) = 0;
        virtual void notify_new_decision_level() = 0;
        virtual void notify_backtrack(size_t new_level) = 0;

        virtual bool cb_check_found_model(const std::vector<int> &model) = 0;

        virtual int cb_decide() { return 0; };

        virtual int cb_propagate() { return 0; };

        virtual int cb_add_reason_clause_lit(int propagated_lit)
        {
            (void)propagated_lit;
            return 0;
        };

        virtual bool cb_has_external_clause(bool &is_forgettable) = 0;

        virtual int cb_add_external_clause_lit() = 0;
    };

    class ClauseIterator
    {
    public:
        virtual ~ClauseIterator() {}
        virtual bool clause(const std::vector<int> &) = 0;
    };

    class WitnessIterator
    {
    public:
        virtual ~WitnessIterator() {}
        virtual bool witness(const std::vector<int> &clause,
                             const std::vector<int> &witness,
                             uint64_t id = 0) = 0;
    };

} 

#endif
