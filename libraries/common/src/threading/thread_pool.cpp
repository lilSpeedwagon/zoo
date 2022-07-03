#include <common/include/thread_pool.hpp>


namespace common::threading {

IoThreadPool::IoThreadPool(size_t pool_size) 
    : pool_size_{pool_size} {
    context_ptr_ = std::make_shared<
        boost::asio::io_context>(pool_size_);
}

IoThreadPool::~IoThreadPool() {
    Join();
}

std::shared_ptr<boost::asio::io_context> 
IoThreadPool::GetContextPtr() const {
    return context_ptr_;
}

void IoThreadPool::Run() {
    Join();
    pool_.clear();
    pool_.reserve(pool_size_);
    for(auto i = 0; i < pool_size_; i++) {
        pool_.emplace_back(
            [context = this->context_ptr_] { context->run(); });
    }
}

void IoThreadPool::RunInThisThread() {
    Run();
    context_ptr_->run();
}

void IoThreadPool::Stop() {
    if (context_ptr_->stopped()) {
        return;
    }

    context_ptr_->stop();
    Join();
}

void IoThreadPool::Join() {
    for (auto& thread : pool_) {
        thread.join();
    }
}

void IoThreadPool::Detach() {
    for (auto& thread : pool_) {
        thread.detach();
    }
}

} // namespace common::threading
