#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <vector>
#include <sstream>

struct SymbolToken {
    std::string name;

    SymbolToken(std::string n) : name(n){};
    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    };
};

struct BoolToken {
    bool state;

    BoolToken(bool b) : state(b){};
    bool operator==(const BoolToken& other) const {
        return state == other.state;
    };
};

struct QuoteToken {
    QuoteToken() = default;
    bool operator==(const QuoteToken&) const {
        return true;
    };
};

struct DotToken {
    DotToken() = default;
    bool operator==(const DotToken&) const {
        return true;
    };
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    // ConstantToken(int val) : value(val) {}
    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    };
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BoolToken>;

class Tokenizer {
public:
    explicit Tokenizer(std::istream* in) : input_(in) {
        Next();
    };

    bool IsEnd() {
        return input_->eof();
    };

    void Next();

    Token GetToken() {
        return curr_token_;
    };

private:
    std::istream* input_;
    Token curr_token_;
};
