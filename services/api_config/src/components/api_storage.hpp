#pragma once

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include <common/include/component_base.hpp>

#include <models/api_config.hpp>


namespace api_config::components {

class ApiConfigStorage final
    : public common::components::ComponentBase {
public:
    static constexpr const char* kName = "api-config-storage";

    ApiConfigStorage();
    virtual ~ApiConfigStorage();

    const char* Name() const override;

    void Init() override;

    models::ApiConfigData Insert(const models::ApiConfig& api);
    models::ApiConfigData Update(uint64_t id, const models::ApiConfig& api);
    models::ApiConfigData Delete(uint64_t name);
    std::optional<models::ApiConfigData> Get(uint64_t name) const;
    std::vector<models::ApiConfigData> List() const;

private:
    uint64_t GetNextId();

    std::unordered_map<uint64_t, models::ApiConfigData> apis_;
    uint64_t id_counter_;
};

} // namespace api_config::components