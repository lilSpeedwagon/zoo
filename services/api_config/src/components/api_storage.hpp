#pragma once

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include <components/include/component_base.hpp>

#include <models/api_config.hpp>


namespace api_config::components {

class ApiConfigStorage final
    : public ::components::ComponentBase {
public:
    static constexpr const char* kName = "api-config-storage";

    ApiConfigStorage();
    virtual ~ApiConfigStorage();

    const char* Name() const override;

    void Init() override;
    void Reset() override;

    models::ApiConfigData Insert(const models::ApiConfig& api);
    std::optional<models::ApiConfigData> Update(models::ApiConfigId id,
                                                const models::ApiConfig& api);
    std::optional<models::ApiConfigData> Delete(models::ApiConfigId id);
    std::optional<models::ApiConfigData> Get(models::ApiConfigId id) const;
    std::vector<models::ApiConfigData> List() const;

private:
    models::ApiConfigId GetNextId();

    std::unordered_map<models::ApiConfigId, models::ApiConfigData> apis_;
    uint64_t id_counter_;
};

} // namespace api_config::components