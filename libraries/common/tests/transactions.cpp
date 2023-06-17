#include <memory>

#include <catch2/catch.hpp>

#include <common/include/transactions.hpp>


namespace common::tests::transactions {

namespace {

enum class TransactionState {
    Unknown,
    Started,
    Commited,
    RolledBack,
};

class CustomTransaction : public common::transactions::Transaction {
public:
    CustomTransaction() {
        state = TransactionState::Unknown;
    }
    virtual ~CustomTransaction() = default;

    virtual void Begin() {
        state = TransactionState::Started;
    }

    virtual void Commit() {
        state = TransactionState::Commited;
    }

    virtual void Rollback() {
        state = TransactionState::RolledBack;
    }

    virtual bool IsCommitted() const {
        return state == TransactionState::Commited;
    }

    // static method exposing internal state for test purposes
    static TransactionState State() {
        return state;
    }

private:
    static TransactionState state;
};

// definition of static member
TransactionState CustomTransaction::state = TransactionState::Unknown;

using TransactionGuard = common::transactions::TransactionGuard<CustomTransaction>;

} // namespace

TEST_CASE("GuardCommit", "[Transactions]") {
    {
        TransactionGuard guard;
        CHECK(CustomTransaction::State() == TransactionState::Started);
        guard.Commit();
        CHECK(CustomTransaction::State() == TransactionState::Commited);
    }

    CHECK(CustomTransaction::State() == TransactionState::Commited);
}

TEST_CASE("GuardNoCommit", "[Transactions]") {
    {
        TransactionGuard guard;
        CHECK(CustomTransaction::State() == TransactionState::Started);
    }

    CHECK(CustomTransaction::State() == TransactionState::RolledBack);
}

TEST_CASE("GuardRollbackOnException", "[Transactions]") {
    try {
        TransactionGuard guard;
        CHECK(CustomTransaction::State() == TransactionState::Started);
        throw std::exception();
    } catch (const std::exception& err) {}

    CHECK(CustomTransaction::State() == TransactionState::RolledBack);
}

} // namespace common::tests::transactions
