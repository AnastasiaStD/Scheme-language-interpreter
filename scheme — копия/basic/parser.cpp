#include <parser.h>
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    if (std::holds_alternative<ConstantToken>(token)) {
        auto res = std::make_shared<Number>(std::get<ConstantToken>(token).GetValue());
        tokenizer->Next();
        return res;
    } else if (std::holds_alternative<SymbolToken>(token)) {
        if (std::get<SymbolToken>(token).GetValue() == "quote") {
            tokenizer->Next();
            auto f = Read(tokenizer);
            auto res = std::make_shared<Quote>(f);
            return res;
        } else {
            auto res = std::make_shared<Symbol>(std::get<SymbolToken>(token).GetValue());
            tokenizer->Next();
            return res;
        }
    } else if (std::holds_alternative<QuoteToken>(token) && !(tokenizer->IsEnd())) {
        tokenizer->Next();
        auto f = Read(tokenizer);
        auto res = std::make_shared<Quote>(f);
        return res;
    } else if (std::holds_alternative<DotToken>(token)) {
        throw SyntaxError("unexpected dot");
    } else if (std::holds_alternative<BoolToken>(token)) {
        auto res = std::make_shared<Boolean>(std::get<BoolToken>(token).GetValue());
        tokenizer->Next();
        return res;
    } else if (std::holds_alternative<BracketToken>(token)) {
        const auto& bracket_token = std::get<BracketToken>(token);
        if (bracket_token == BracketToken::OPEN) {
            tokenizer->Next();
            auto res = ReadList(tokenizer);
            return res;
        } else {
            throw SyntaxError("can not identify token hoho");
        }
    } else {
        throw SyntaxError("can not identify token hehe ");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    if (tokenizer->IsEnd()) {
        throw SyntaxError("is end");
    }
    if (std::holds_alternative<BracketToken>(token) &&
        std::get<BracketToken>(token) == BracketToken::CLOSE) {
        tokenizer->Next();
        return nullptr;
    } else {
        auto car = Read(tokenizer);
        token = tokenizer->GetToken();
        if (std::holds_alternative<DotToken>(token)) {
            tokenizer->Next();
            auto cdr = Read(tokenizer);
            token = tokenizer->GetToken();
            if (std::holds_alternative<BracketToken>(token) &&
                std::get<BracketToken>(token) == BracketToken::CLOSE) {
                tokenizer->Next();
                return std::make_shared<Cell>(car, cdr);
            } else {
                throw SyntaxError("expected closing bracket");
            }
        } else {
            auto cdr = ReadList(tokenizer);
            return std::make_shared<Cell>(car, cdr);
        }
    }
}
