#include <transactions.hpp>

#include <cassert>


namespace common::transactions {

TransactionGuard::TransactionGuard(std::shared_ptr<Transaction> transaction_ptr)
    : transaction_ptr_(transaction_ptr) {
    assert(transaction_ptr_ != nullptr);
    transaction_ptr_->Begin();
}

TransactionGuard::~TransactionGuard() {
    if (!transaction_ptr_->IsCommitted()) {
        transaction_ptr_->Rollback();
    }
}

void TransactionGuard::Commit() {
    transaction_ptr_->Commit();
}

} // namespace common::transactions
