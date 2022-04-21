#pragma once

#include <optional>
#include <string>

#include <common/include/format.hpp>
#include <common/include/json.hpp>
#include <common/include/utils/errors.hpp>

namespace common::config {

/// @class Loads a static configuration data from the specified JSON
/// file. May be fully or partically serialized to a custom structs.
class Config {
public:
    Config();
    Config(const json::json& data);

    static Config FromJson(const json::json& data);
    static Config FromFile(const std::string& path);

    /// @brief Tries to serialize JSON config to the custom type T.
    /// To serialize user structs make sure you implemented function
    /// void from_json(const json&, T& config);
    /// @throws depends on the implementation of from_json().
    template<typename T>
    T Get() const {
        return data_.get<T>();
    }

    /// @brief Tries to lookup nested JSON structure by the specified path
    /// and serialize it to the type T. Custom path delimiter may be 
    /// provided as a second argument.
    /// @returns config of type T or std::nullopt if nothing found.
    /// @throws std::runtime_error if path is invalid.
    template<typename T>
    std::optional<T> GetPath(const std::string& path,
                             const char delimiter = '/') const {
        if (path.empty()) {
            return Get<T>();
        }
        auto nested_json = GetJsonByPath(path, delimiter);
        if (nested_json == nullptr) {
            return std::nullopt;
        }
        return nested_json->get<T>();
    }

private:
    const json::json* GetJsonByPath(const std::string& path,
                                    const char delimiter) const;

    json::json data_;
};

} // namespace common::config