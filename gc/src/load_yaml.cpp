#include "gc/load_yaml.hpp"

#include "yaml-cpp/yaml.h"


namespace gc {

auto load_graph(const YAML::Node& config)
    -> Graph
{
    auto graph = config["graph"];
    for (auto node : graph["nodes"])
    {
        auto name = node["name"].as<std::string>();
        auto type = node["type"].as<std::string>();
        auto init = std::vector<int>{};
        if (auto init_ = node["init"])
        {
            for (auto element : init_)
                init.push_back(element.as<int>());
        }
        // auto attr = std::map<std::string, std::string>{};
        // if (auto attr_ = node["attr"])
        // {
        //     for (auto element : attr_)
        //     {
        //         attr.emplace(element.first.as<std::string>(),
        //                      element.second.as<std::string>());
        //     }
        // }
    }
    return {}; // TODO
}

} // namespace gc
