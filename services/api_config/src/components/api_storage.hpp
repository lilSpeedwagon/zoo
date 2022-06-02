#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <common/include/component_base.hpp>

#include <models/api_config.hpp>

namespace api_config::components {

struct Metadata {
    uint64_t id{};
    std::string author{};
    std::chrono::time_point<std::chrono::system_clock> created{};
    std::chrono::time_point<std::chrono::system_clock> updated{};
};

template<typename T>
struct StoredData {
    T data{};
    Metadata metadata{};
};

using ApiConfigData = StoredData<models::ApiConfig>;

class ApiConfigStorage final
    : public common::components::ComponentBase {
public:
    static constexpr const char* kName = "api-config-storage";

    ApiConfigStorage();
    virtual ~ApiConfigStorage();

    const char* Name() const override;

    void Init() override;

    ApiConfigData Insert(const models::ApiConfig& api);
    ApiConfigData Update(const models::ApiConfig& api);
    ApiConfigData Delete(const std::string& name);
    std::optional<ApiConfigData> Get(const std::string& name) const;

private:
    uint64_t GetNextId();

    std::unordered_map<std::string, ApiConfigData> apis_;
    uint64_t id_counter_;
};

} // namespace api_config::components