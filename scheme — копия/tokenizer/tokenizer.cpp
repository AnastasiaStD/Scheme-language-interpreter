#include <tokenizer.h>
#include "error.h"

std::vector<Token> Read(const std::string& string) {
    std::istringstream iss(string);
    Tokenizer tokenizer(&iss);

    std::vector<Token> tokens;

    while (!tokenizer.IsEnd()) {
        tokenizer.Next();
        tokens.push_back(tokenizer.GetToken());
    }
    return tokens;
}

void Tokenizer::Next() {
    char ch = input_->get();
    if (input_->eof()) {
        curr_token_ = QuoteToken{};
        return;
    }
    if (ch == '\'') {
        curr_token_ = QuoteToken();
    } else if (isspace(ch)) {
        Next();
        return;
    } else if (ch == '(') {
        curr_token_ = BracketToken::OPEN;
    } else if (ch == ')') {
        curr_token_ = BracketToken::CLOSE;
    } else if (ch == '.') {
        curr_token_ = DotToken();
    } else if (std::isdigit(ch)) {
        std::string num_str;
        num_str += ch;
        while (std::isdigit(input_->peek()) && !input_->eof()) {
            ch = input_->get();
            num_str += ch;
        }
        if (input_->eof()) {
            input_->unget();
        }
        int value = std::stoi(num_str);
        curr_token_ = ConstantToken{value};
        return;
    } else if (ch == '-' && input_->peek() == ' ') {
        std::string symbol;
        symbol += ch;
        input_->get();
        curr_token_ = SymbolToken{symbol};
        return;
    } else if (ch == '-' && std::isdigit(input_->peek())) {
        std::string num_str;
        num_str += ch;
        while (std::isdigit(input_->peek()) && !input_->eof()) {
            ch = input_->get();
            num_str += ch;
        }
        if (input_->eof()) {
            input_->unget();
        }
        int value = std::stoi(num_str);
        curr_token_ = ConstantToken{value};
        return;
    } else if (ch == '+' && std::isdigit(input_->peek())) {
        std::string num_str;
        while (std::isdigit(input_->peek()) && !input_->eof()) {
            ch = input_->get();
            num_str += ch;
        }
        if (input_->eof()) {
            input_->unget();
        }
        int value = std::stoi(num_str);
        curr_token_ = ConstantToken{value};
        return;
    } else if (ch == '+' && input_->peek() == ' ') {
        std::string symbol;
        symbol += ch;
        input_->get();
        curr_token_ = SymbolToken{symbol};
        return;
    } else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || ch == '=' || ch == '*' ||
               ch == '#' || ch == '-' || ch == '+') {
        std::string symbol;
        symbol += ch;
        while ((std::isalnum(input_->peek()) || input_->peek() == '<' || input_->peek() == '>' ||
                input_->peek() == '=' || input_->peek() == '/' || input_->peek() == '*' ||
                input_->peek() == '#' || input_->peek() == '?' || input_->peek() == '!' ||
                input_->peek() == '+' || input_->peek() == '-') &&
               !input_->eof()) {
            ch = input_->get();
            symbol += ch;
        }
        if (input_->eof()) {
            input_->unget();
        }
        curr_token_ = SymbolToken{symbol};
        return;
    } else if (ch == '#f' || ch == '#t') {
        if (ch == '#f') {
            curr_token_ = BoolToken{false};
            return;
        } else {
            curr_token_ = BoolToken{true};
            return;
        }
    } else {
        throw SyntaxError("Invalid syntax");
    }
};