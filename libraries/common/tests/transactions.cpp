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
    CustomTransaction() : state_(TransactionState::Unknown) {}
    virtual ~CustomTransaction() = default;

    virtual void Begin() {
        state_ = TransactionState::Started;
    }

    virtual void Commit() {
        state_ = TransactionState::Commited;
    }

    virtual void Rollback() {
        state_ = TransactionState::RolledBack;
    }

    virtual bool IsCommitted() const {
        return state_ == TransactionState::Commited;
    }

    TransactionState State() const {
        return state_;
    }

private:
    TransactionState state_;
};

} // namespace

TEST_CASE("GuardCommit", "[Transactions]") {
    auto transaction = std::make_shared<CustomTransaction>();

    {
        common::transactions::TransactionGuard guard(transaction);
        CHECK(transaction->State() == TransactionState::Started);
        guard.Commit();
        CHECK(transaction->State() == TransactionState::Commited);
    }

    CHECK(transaction->State() == TransactionState::Commited);
}

TEST_CASE("GuardNoCommit", "[Transactions]") {
    auto transaction = std::make_shared<CustomTransaction>();

    {
        common::transactions::TransactionGuard guard(transaction);
        CHECK(transaction->State() == TransactionState::Started);
    }

    CHECK(transaction->State() == TransactionState::RolledBack);
}

TEST_CASE("GuardRollbackOnException", "[Transactions]") {
    auto transaction = std::make_shared<CustomTransaction>();

    try {
        common::transactions::TransactionGuard guard(transaction);
        CHECK(transaction->State() == TransactionState::Started);
        throw std::exception();
    } catch (const std::exception& err) {}

    CHECK(transaction->State() == TransactionState::RolledBack);
}

} // namespace common::tests::transactions