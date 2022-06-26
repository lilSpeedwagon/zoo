#include "request.hpp"

#include <exception>

#include <common/include/utils/algo.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

namespace api_config::utils {

models::ApiConfigId GetId(const http::Request& request) {
    auto params = http::utils::GetParams(request);
    auto id_opt = common::utils::algo::GetOptional(params, "id");
    if (!id_opt.has_value()) {
        throw http::exceptions::BadRequest("Parameter 'id' not found");
    }

    // negative numbers
    if (id_opt->at(0) == '-') {
        throw http::exceptions::BadRequest("Parameter 'id' is invalid");
    }
    
    try {
        return models::ApiConfigId(std::stoul(id_opt.value().data()));
    } catch (const std::logic_error& ex) {
        throw http::exceptions::BadRequest("Parameter 'id' is invalid");
    }
}

} // namespace api_config::utils