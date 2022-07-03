#pragma once

#include <memory>

#include <boost/core/noncopyable.hpp>

#include <common/include/thread_pool.hpp>
#include <http/include/http_server.hpp>

namespace components {

/// @class Async ComponentsEngine controller.
/// Provides http server with test/debug methods
/// for the components system.
class ComponentsController : private boost::noncopyable {
public:
    ComponentsController();
    ~ComponentsController();

    /// @brief Runs test control server is a separate thread.
    void RunAsync();

    /// @brief Stops running test control server.
    void Stop();

    bool IsRunning() const;

private:
    bool is_running_;
    common::threading::IoThreadPool pool_;
    std::shared_ptr<http::server::HttpServer> server_ptr_;
};

} // namespace components