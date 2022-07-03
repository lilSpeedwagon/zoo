#pragma once

#include <thread>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/core/noncopyable.hpp>

namespace common::threading {

/// @class Threadpool intended for async work
/// with boost async I/O context.
class IoThreadPool : private boost::noncopyable {
public:
    IoThreadPool(size_t pool_size = 1);
    ~IoThreadPool();

    std::shared_ptr<boost::asio::io_context> GetContextPtr() const;
    
    void Run();
    void RunInThisThread();
    void Stop();
    void Join();
    void Detach();

private:
    const size_t pool_size_;
    std::vector<std::thread> pool_;
    std::shared_ptr<boost::asio::io_context> context_ptr_;
};


} // namespace common::threading