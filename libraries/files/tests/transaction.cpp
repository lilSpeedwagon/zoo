#include <filesystem>
#include <fstream>
#include <string>

#include <catch2/catch.hpp>

#include <files/include/transaction.hpp>


// Tests from this section cause a side-effect:
// temporary file for IO operations is created.

namespace common::tests::binary {

namespace {

static const std::filesystem::path kFilePath = "./test_temp.bin";
static const std::filesystem::path kBackupPath = std::filesystem::path(kFilePath.string() + ".backup");
static const std::string kMessage = "hello";

class TransactionFixtureWithFile {
public:
    TransactionFixtureWithFile() {
        std::ofstream file(kFilePath); // create file
    }
    ~TransactionFixtureWithFile() {
        // cleanup temp files if exist
        std::filesystem::remove(std::filesystem::path(kFilePath));
        std::filesystem::remove(std::filesystem::path(kBackupPath));
    }
};

class TransactionFixtureWithoutFile {
public:
    TransactionFixtureWithoutFile() = default;
    ~TransactionFixtureWithoutFile() {
        // cleanup temp files if exist
        std::filesystem::remove(std::filesystem::path(kFilePath));
        std::filesystem::remove(std::filesystem::path(kBackupPath));
    }
};

} // namespace

TEST_CASE_METHOD(TransactionFixtureWithFile, "Commit", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    CHECK(!transaction.IsCommitted());

    transaction.Begin();
    CHECK(std::filesystem::exists(kFilePath));
    CHECK(std::filesystem::exists(kBackupPath));
    CHECK(!transaction.IsCommitted());
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Commit();
    {
        std::ifstream file(kFilePath);
        std::string content;
        file >> content;
        CHECK(content == kMessage);
    }
    CHECK(std::filesystem::exists(kFilePath));
    CHECK(!std::filesystem::exists(kBackupPath));
    CHECK(transaction.IsCommitted());
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "CommitnNotStarted", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    CHECK_THROWS_WITH(transaction.Commit(), "Cannot commit not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "CommitTwice", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Commit();
    CHECK_THROWS_WITH(transaction.Commit(), "Cannot commit not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "CommitRolledBack", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Rollback();
    CHECK_THROWS_WITH(transaction.Commit(), "Cannot commit not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "Rollback", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Rollback();
    {
        std::ifstream file(kFilePath);
        std::string content;
        file >> content;
        CHECK(content == "");
    }
    CHECK(std::filesystem::exists(kFilePath));
    CHECK(!std::filesystem::exists(kBackupPath));
    CHECK(!transaction.IsCommitted());
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "RollbackTwice", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Rollback();
    CHECK_THROWS_WITH(transaction.Rollback(), "Cannot rollback not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "RollbackCommitted", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Commit();
    CHECK_THROWS_WITH(transaction.Rollback(), "Cannot rollback not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "RollbackNotStarted", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    CHECK_THROWS_WITH(transaction.Rollback(), "Cannot rollback not started or completed transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "BeginStarted", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    transaction.Begin();
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    CHECK_THROWS_WITH(transaction.Begin(), "Cannot begin already started transaction");
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "BeginAgainAfterCommit", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    for (size_t i = 0; i < 3; i++) {
        transaction.Begin();
        {
            std::ofstream file(kFilePath);
            file << kMessage;
        }

        transaction.Commit();
        {
            std::ifstream file(kFilePath);
            std::string content;
            file >> content;
            CHECK(content == kMessage);
        }
    }
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "BeginAgainAfterRollback", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    
    for (size_t i = 0; i < 3; i++) {
        transaction.Begin();
        {
            std::ofstream file(kFilePath);
            file << kMessage;
        }

        transaction.Rollback();
        {
            std::ifstream file(kFilePath);
            std::string content;
            file >> content;
            CHECK(content == "");
        }
    }
}

TEST_CASE_METHOD(TransactionFixtureWithoutFile, "BeginMissingFile", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath);
    CHECK_THROWS_AS(transaction.Begin(), std::filesystem::filesystem_error);
}

TEST_CASE_METHOD(TransactionFixtureWithoutFile, "TransactionWithFileCreation", "[Transaction]") {
    files::transaction::FileTransaction transaction(kFilePath, true);
    CHECK(!std::filesystem::exists(kFilePath));
    transaction.Begin();
    CHECK(std::filesystem::exists(kFilePath));
    {
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    transaction.Commit();
    {
        std::ifstream file(kFilePath);
        std::string content;
        file >> content;
        CHECK(content == kMessage);
    }
}

TEST_CASE_METHOD(TransactionFixtureWithFile, "RestoreAfterFail", "[Transaction]") {
    // Start a transaction and interrupt it in the middle
    {
        files::transaction::FileTransaction transaction(kFilePath);
        transaction.Begin();
        std::ofstream file(kFilePath);
        file << kMessage;
    }

    CHECK(std::filesystem::exists(kFilePath));
    CHECK(std::filesystem::exists(kBackupPath));

    // Start a new transaction.
    // Ensure that the state before the previous transaction is restored.
    {
        files::transaction::FileTransaction transaction(kFilePath);
        transaction.Begin();
        CHECK(std::filesystem::exists(kFilePath));
        CHECK(std::filesystem::exists(kBackupPath));
        std::ifstream file(kFilePath);
        std::string content;
        file >> content;
        CHECK(content == "");
    }
}

} // namespace common::tests::binary
