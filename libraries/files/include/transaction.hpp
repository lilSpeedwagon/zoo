#pragma once

#include <filesystem>

#include <common/include/transactions.hpp>


namespace files::transaction {

class FileTransaction : public common::transactions::Transaction {
public:
    /// @brief Ctor
    /// @param path path to the file which must be covered with a transaction 
    /// @param create_if_missing whether it is required to create an empty file if missing (false by default)
    FileTransaction(const std::filesystem::path& path, bool create_if_missing = false);
    virtual ~FileTransaction();

    /// @brief Start transaction.
    void Begin() override;

    /// @brief Complete transaction.
    void Commit() override;

    /// @brief Rollback transaction.
    void Rollback() override;

    /// @brief Whether the transaction was commited before.
    bool IsCommitted() const override;

private:
    enum class State {
        Unknown,
        InProgress,
        Commited,
        RolledBack,
    };

    void RollbackImpl();

    std::filesystem::path path_;
    std::filesystem::path backup_path_;
    State state_;
    bool create_if_missing_;
};


} // namespace files::transaction
