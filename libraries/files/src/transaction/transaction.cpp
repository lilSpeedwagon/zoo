#include <transaction.hpp>

#include <fstream>

#include <common/include/format.hpp>
#include <common/include/logging.hpp>


namespace files::transaction {

namespace {

static const std::string kFileCopyPostfix = ".backup";

}

FileTransaction::FileTransaction(const std::filesystem::path& path, bool create_if_missing)
    : path_(path), backup_path_(path.string() + kFileCopyPostfix),
      state_(State::Unknown), create_if_missing_(create_if_missing) {}

FileTransaction::~FileTransaction() {}

void FileTransaction::Begin() {
    LOG_TRACE() << "Begin file transaction on " << path_;

    if (state_ == State::InProgress) {
        throw std::logic_error("Cannot begin already started transaction");
    }

    // if backup exists - the previous transaction failed
    // need to restore state from the backup
    if (std::filesystem::exists(backup_path_)) {
        LOG_WARNING() << common::format::Format("Backup found for {}, restoring previous transaction state", path_);
        RollbackImpl();
    }

    // create file if required
    if (create_if_missing_ && !std::filesystem::exists(path_)) {
        std::ofstream file(path_); // touch file to create
    }

    std::filesystem::copy_file(path_, backup_path_);
    state_ = State::InProgress;
}

void FileTransaction::Commit() {
    LOG_TRACE() << "Committing transaction on " << path_;
    if (state_ != State::InProgress) {
        throw std::logic_error("Cannot commit not started or completed transaction");
    }
    
    bool is_deleted = std::filesystem::remove(backup_path_);
    if (!is_deleted) {
        LOG_WARNING() << "Missing backup file when rolling back transaction";
    }
    state_ = State::Commited;
}

void FileTransaction::Rollback() {
    LOG_TRACE() << "Rolling back transaction on " << path_;
    if (state_ != State::InProgress) {
        throw std::logic_error("Cannot rollback not started or completed transaction");
    }
    RollbackImpl();
}

void FileTransaction::RollbackImpl() {
    auto options = std::filesystem::copy_options::overwrite_existing;
    std::filesystem::copy_file(backup_path_, path_, options);
    bool is_deleted = std::filesystem::remove(backup_path_);
    if (!is_deleted) {
        LOG_WARNING() << "Missing backup file when rolling back transaction";
    }
    state_ = State::RolledBack;
}

bool FileTransaction::IsCommitted() const {
    return state_ == State::Commited;
}

} // namespace files::transaction
