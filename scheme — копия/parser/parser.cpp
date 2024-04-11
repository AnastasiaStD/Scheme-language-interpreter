#include <parser.h>
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    if (std::holds_alternative<ConstantToken>(token)) {
        auto res = std::make_shared<Number>(std::get<ConstantToken>(token).GetValue());
        tokenizer->Next();
        return res;
    } else if (std::holds_alternative<SymbolToken>(token)) {
        auto res = std::make_shared<Symbol>(std::get<SymbolToken>(token).GetValue());
        tokenizer->Next();
        return res;
    } else if (std::holds_alternative<QuoteToken>(token)) {
        throw SyntaxError("do not expect quote");
    } else if (std::holds_alternative<DotToken>(token)) {
        throw SyntaxError("unexpected dot");
    } else if (std::holds_alternative<BoolToken>(token)) {
        auto res = std::make_shared<Number>(std::get<BoolToken>(token).GetValue());
        tokenizer->Next();
        return res;
    } else if (std::holds_alternative<BracketToken>(token)) {
        const auto& bracket_token = std::get<BracketToken>(token);
        if (bracket_token == BracketToken::OPEN) {
            tokenizer->Next();
            auto res = ReadList(tokenizer);
            // tokenizer->Next();
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

// bool Cycle(Tokenizer* tokenizer) {
//     if (std::holds_alternative<BracketToken>(tokenizer->GetToken())) {
//         const auto& bracketToken = std::get<BracketToken>(tokenizer->GetToken());
//         if (bracketToken == BracketToken::CLOSE) {
//             return true;
//         }
//     }
//     return false;
// }
//
// std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
//     auto ob = Object();
//     auto cell = std::make_shared<Cell>(nullptr, nullptr);
//     auto token = tokenizer->GetToken();
//     if (std::holds_alternative<ConstantToken>(token)) {
//         auto new_fir = std::make_shared<Number>(std::get<ConstantToken>(token).GetValue());
//         cell->ChangeFirst(new_fir);
//     } else if (std::holds_alternative<SymbolToken>(token)) {
//         auto new_fir = std::make_shared<Symbol>(std::get<SymbolToken>(token).GetValue());
//         cell->ChangeFirst(new_fir);
//     } else if (std::holds_alternative<QuoteToken>(token)) {
//         throw SyntaxError("do not expect quote");
//     } else if (std::holds_alternative<BoolToken>(token)) {
//         auto new_fir = std::make_shared<Number>(std::get<BoolToken>(token).GetValue());
//         cell->ChangeFirst(new_fir);
//     } else if (std::holds_alternative<DotToken>(token)) {
//         throw SyntaxError("unexpected dot");
//     } else if (std::holds_alternative<BracketToken>(token)) {
//         const auto& bracketToken = std::get<BracketToken>(token);
//         if (bracketToken == BracketToken::OPEN) {
//             tokenizer->Next();
//             auto res = ReadList(tokenizer);
//             cell->ChangeFirst(res);
//         } else {
//             return nullptr;
//         }
//     }
//
//     while (!Cycle(tokenizer)) {  // while not )
//         tokenizer->Next();
//         if (tokenizer->IsEnd()) {
//             throw SyntaxError("unexpected end");
//         }
//         auto token = tokenizer->GetToken();
//         if (std::holds_alternative<ConstantToken>(token)) {
//             auto new_sec = ReadList(tokenizer);
//             cell->ChangeSecond(new_sec);
//         } else if (std::holds_alternative<SymbolToken>(token)) {
//             throw SyntaxError("do not expect symbol");
//             //            auto new_sec = ReadList(tokenizer);
//             //            cell->ChangeSecond(new_sec);
//         } else if (std::holds_alternative<QuoteToken>(token)) {
//             throw SyntaxError("do not expect quote");
//         } else if (std::holds_alternative<BoolToken>(token)) {
//             auto new_sec = ReadList(tokenizer);
//             cell->ChangeSecond(new_sec);
//         } else if (std::holds_alternative<DotToken>(token)) {
//             tokenizer->Next();
//             auto new_sec = Read(tokenizer);
//             auto tok = tokenizer->GetToken();
//             if (!std::holds_alternative<BracketToken>(tok)) {
//                 throw SyntaxError("lol");
//             }
//             cell->ChangeSecond(new_sec);
//         } else if (std::holds_alternative<BracketToken>(token)) {
//             const auto& bracketToken = std::get<BracketToken>(token);
//             if (bracketToken == BracketToken::OPEN) {
//                 tokenizer->Next();
//                 auto res = Read(tokenizer);
//                 cell->ChangeSecond(res);
//             }
//         }
//     }
//
//     return cell;
// }