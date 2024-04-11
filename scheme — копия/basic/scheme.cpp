#include "scheme.h"

std::shared_ptr<Object> Interpreter::GetTokens(const std::string& str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("not end");
    }
    return obj;
}

std::string Interpreter::Run(const std::string& str) {
    if (!args_.empty()) {
        args_.clear();
    }
    auto obj = Interpreter::GetTokens(str);
    std::shared_ptr<Object> res_ast;
    if (obj == nullptr) {
        throw RuntimeError("can not calculate");
    }
    res_ast = MakeCalculation(obj);
    std::string res;
    if (res_ast == nullptr) {
        res_ast = std::make_shared<Quote>(res_ast);
    } else if (Is<Cell>(res_ast)) {
        res_ast = std::make_shared<Quote>(res_ast);
    }
    res = res_ast->Cerealize();
    return res;
}

std::shared_ptr<Object> Interpreter::MakeCalculation(std::shared_ptr<Object> obj) {
    if (Is<Boolean>(obj) || Is<Number>(obj) || Is<Quote>(obj) || Is<Symbol>(obj)) {
        return obj->Calculate();
    } else if (Is<Cell>(obj)) {
        auto first = As<Cell>(obj)->GetFirst();
        auto second = As<Cell>(obj)->GetSecond();
        if (Is<Symbol>(first)) {
            auto functor = functions_[As<Symbol>(first)->GetName()];
            if (functor == nullptr) {
                functions_[As<Symbol>(first)->GetName()] = FindFunc(As<Symbol>(first)->GetName());
                if (functions_[As<Symbol>(first)->GetName()] == nullptr) {
                    return first->Calculate();
                }
            }
            functor = functions_[As<Symbol>(first)->GetName()];
            std::vector<std::shared_ptr<Object>> a;
            while (second && Is<Cell>(second)) {  // разворачиваем в вектор
                if (Is<Cell>(As<Cell>(second)->GetFirst())) {
                    a.push_back(MakeCalculation(As<Cell>(second)->GetFirst()));
                } else {
                    a.push_back(As<Cell>(second)->GetFirst());
                }
                second = As<Cell>(second)->GetSecond();
            }
            auto res = functor->Apply(a);
            return res;
        } else if (Is<Quote>(first)) {
            auto el = first->Calculate();
            if (second != nullptr) {
                throw RuntimeError("hehe");
            }
            return el;
        } else {
            throw RuntimeError("hehe");
        }
    }
}

std::shared_ptr<Object> Interpreter::FindFunc(std::string functor) {
    if (functor == "+") {
        return std::make_shared<AddFunction>();
    } else if (functor == "-") {
        return std::make_shared<DecreaseFunction>();
    } else if (functor == "*") {
        return std::make_shared<MultiplyFunction>();
    } else if (functor == "/") {
        return std::make_shared<DivedeFunction>();
    } else if (functor == "max") {
        return std::make_shared<MaxFunction>();
    } else if (functor == "min") {
        return std::make_shared<MinFunction>();
    } else if (functor == "abs") {
        return std::make_shared<IntAbsFunction>();
    } else if (functor == "<") {
        return std::make_shared<LessFunction>();
    } else if (functor == "<=") {
        return std::make_shared<LessEqFunction>();
    } else if (functor == ">") {
        return std::make_shared<GreaterFunction>();
    } else if (functor == ">=") {
        return std::make_shared<GreaterEqFunction>();
    } else if (functor == "number?") {
        return std::make_shared<IsNumber>();
    } else if (functor == "=") {
        return std::make_shared<EqualFunction>();
    } else if (functor == "boolean?") {
        return std::make_shared<IsBool>();
    } else if (functor == "not") {
        return std::make_shared<NotFunction>();
    } else if (functor == "and") {
        return std::make_shared<AndFunction>();
    } else if (functor == "or") {
        return std::make_shared<OrFunction>();
    } else if (functor == "pair?") {
        return std::make_shared<IsPair>();
    } else if (functor == "null?") {
        return std::make_shared<IsNull>();
    } else if (functor == "list?") {
        return std::make_shared<IsList>();
    } else if (functor == "cons") {
        return std::make_shared<MakePair>();
    } else if (functor == "car") {
        return std::make_shared<GetFirst>();
    } else if (functor == "cdr") {
        return std::make_shared<GetSecond>();
    } else if (functor == "list") {
        return std::make_shared<MakeList>();
    } else if (functor == "list-ref") {
        return std::make_shared<GetListElem>();
    } else if (functor == "list-tail") {
        return std::make_shared<GetListTail>();
    } else {
        return nullptr;
    }
}
