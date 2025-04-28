// env.hpp
#ifndef ENV_HPP
#define ENV_HPP

#include <string>
#include <fstream>
#include <map>

class EnvLoader {
private:
    std::map<std::string, std::string> env_vars;

public:
    EnvLoader(const std::string& filename = ".env") {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                env_vars[key] = value;
            }
        }
    }

    std::string get(const std::string& key) const {
        auto it = env_vars.find(key);
        return (it != env_vars.end()) ? it->second : "";
    }
};

#endif