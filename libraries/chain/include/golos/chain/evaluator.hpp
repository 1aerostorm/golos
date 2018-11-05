#pragma once

#include <golos/protocol/exceptions.hpp>
#include <golos/protocol/operations.hpp>

#define ASSERT_REQ_HF_IN_DB(HF, FEATURE, DATABASE) \
    GOLOS_ASSERT(DATABASE.has_hardfork(HF), unsupported_operation, \
        "${feature} is not enabled until HF ${hardfork}", \
        ("feature",FEATURE)("hardfork",BOOST_PP_STRINGIZE(HF)));

#define ASSERT_REQ_HF(HF, FEATURE) \
    ASSERT_REQ_HF_IN_DB(HF, FEATURE, _db)

namespace golos {
    namespace chain {

        class database;

        template<typename OperationType=golos::protocol::operation>
        class evaluator {
        public:
            virtual void apply(const OperationType &op) = 0;

            virtual int get_type() const = 0;
        };

        template<typename EvaluatorType, typename OperationType=golos::protocol::operation>
        class evaluator_impl : public evaluator<OperationType> {
        public:
            typedef OperationType operation_sv_type;
            // typedef typename EvaluatorType::operation_type op_type;

            evaluator_impl(database &d)
                    : _db(d) {
            }

            virtual void apply(const OperationType &o) final override {
                auto *eval = static_cast< EvaluatorType * >(this);
                const auto &op = o.template get<typename EvaluatorType::operation_type>();
                eval->do_apply(op);
            }

            virtual int get_type() const override {
                return OperationType::template tag<typename EvaluatorType::operation_type>::value;
            }

            database &db() {
                return _db;
            }

        protected:
            database &_db;
        };

    }
}

#define DEFINE_EVALUATOR(X) \
class X ## _evaluator : public golos::chain::evaluator_impl< X ## _evaluator > \
{                                                                           \
   public:                                                                  \
      typedef X ## _operation operation_type;                               \
                                                                            \
      X ## _evaluator( database& db )                                       \
         : golos::chain::evaluator_impl< X ## _evaluator >( db )          \
      {}                                                                    \
                                                                            \
      void do_apply( const X ## _operation& o );                            \
};
