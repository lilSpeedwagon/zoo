#pragma once

#include <memory>

#include <boost/noncopyable.hpp>


namespace common::transactions {

/// @brief Transaction mechanism interface. Subclasses must provied implementation for
/// the Begin(), Commit(), Rollback(), and IsCommited() methods. Transactions should be atomic.
/// The managed resource / process should not be observed in any intermediate state.
class Transaction {
public:
    virtual ~Transaction() = default;

    /// @brief Start transaction.
    virtual void Begin() = 0;

    /// @brief Complete transaction.
    virtual void Commit() = 0;

    /// @brief Rollback transaction.
    virtual void Rollback() = 0;

    /// @brief Whether the transaction was commited before.
    virtual bool IsCommitted() const = 0;
};

/// @brief Scope guard for transactions. Begins transaction in ctor,
/// and rollbacks it in dtor if IsCommited() returns false.
template<typename TransactionT>
class TransactionGuard : public boost::noncopyable {
public:
    template<typename ...Args>
    TransactionGuard(Args... args) 
        : transaction_ptr_(std::make_unique<TransactionT>(std::forward<Args>(args)...)) {
        transaction_ptr_->Begin();
    }
    ~TransactionGuard() {
        if (!transaction_ptr_->IsCommitted()) {
            transaction_ptr_->Rollback();
        }    
    }

    /// @brief Commits the underlying transaction.
    void Commit() {
        transaction_ptr_->Commit();
    }

private:
    TransactionGuard(TransactionGuard&&) {}
    TransactionGuard& operator=(TransactionGuard&&) {}

    std::unique_ptr<Transaction> transaction_ptr_;
};

} // namespace common::transactions
