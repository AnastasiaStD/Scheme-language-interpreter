#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "tokenizer.h"
#include "parser.h"
#include "object.h"

class Interpreter {
public:
    std::string Run(const std::string& str);
    std::vector<std::shared_ptr<Object>> args_;
    std::unordered_map<std::string, std::shared_ptr<Object>> functions_;
    std::shared_ptr<Object> MakeCalculation(std::shared_ptr<Object> obj);
    std::shared_ptr<Object> FindFunc(std::string);
    std::shared_ptr<Object> GetTokens(const std::string& str);
};
