#include <common/include/config.hpp>

#include <fstream>

namespace common::config {

namespace {

std::vector<std::string> GetPathTokens(
    const std::string& path, const char delimiter) {
    if (path.empty()) {
        return {};
    }

    std::vector<std::string> path_tokens{};
    auto begin_it = path.cbegin();
    auto end_it = std::find(begin_it, path.cend(), delimiter);
    for(;;) {
        if (std::distance(begin_it, end_it) <= 1) {
            throw std::runtime_error("invalid config path");
        }
        path_tokens.emplace_back(begin_it, end_it);

        if (end_it == path.cend()) {
            break;
        }
        begin_it = std::next(end_it);
        end_it = std::find(begin_it, path.cend(), delimiter);
    }
    return path_tokens;
}

} // namespace

Config::Config() : data_() {}

Config::Config(const json::json& data) : data_(data) {}

Config Config::FromJson(const json::json& data) {
    return Config(data);
}

Config Config::FromFile(const std::string& path) {
    std::ifstream stream(path, std::ios::in);
    if (!stream.is_open() || !stream.good()) {
        throw std::runtime_error(format::Format(
            "Cannot open config file: {}", utils::errors::GetLastError()));
    }

    json::json data;
    stream >> data;
    return Config(data);
}

const json::json* Config::GetJsonByPath(const std::string& path,
                                        const char delimiter) const {
    auto path_tokens = GetPathTokens(path, delimiter);

    auto current_ptr = &data_;
    for (const auto& token : path_tokens) {
        try {
            current_ptr = &(current_ptr->at(token));
        } catch (const json::json::type_error&) {
            return nullptr;
        } catch (const json::json::out_of_range&) {
            return nullptr;
        }
    }

    return current_ptr;
}

} // namespace common::config