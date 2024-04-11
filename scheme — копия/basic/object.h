#pragma once

#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include "error.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::string Cerealize() = 0;
    virtual std::shared_ptr<Object> Clone() = 0;
    virtual std::shared_ptr<Object> Calculate() = 0;
    virtual std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) = 0;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
};

class Boolean : public Object {
private:
    bool state_;

public:
    Boolean(bool s) : state_(s){};

    bool GetValue() {
        return state_;
    }
    std::string Cerealize() override {
        if (state_) {
            return "#t";
        }
        return "#f";
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Boolean>(Boolean(this->state_));
    };
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) override {
        throw SyntaxError("cay not apply");
    }
    std::shared_ptr<Object> Calculate() override {
        return std::make_shared<Boolean>(Boolean(state_));
    }
};

class Quote : public Object {
private:
    std::shared_ptr<Object> object_;

public:
    Quote(std::shared_ptr<Object> ob) : object_(ob){};

    std::shared_ptr<Object> GetObject() {
        return object_;
    }

    std::string Cerealize() override {
        std::string ch = "(";
        if (object_ == nullptr) {
            return "()";
        }
        auto res = object_->Cerealize();
        ch += res;
        ch += ")";
        return ch;
    }
    std::shared_ptr<Object> Clone() override {
        throw SyntaxError("  ");
    }
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) override {
        throw SyntaxError("cay not apply");
    }

    std::shared_ptr<Object> Calculate() override {
        return std::make_shared<Quote>(object_);
    }
};

class Number : public Object {
private:
    int value_;

public:
    Number(int val) : value_(val){};
    int GetValue() const {
        return value_;
    };
    std::shared_ptr<Object> Calculate() override {
        return std::make_shared<Number>(Number(value_));
    }
    std::string Cerealize() override {
        return std::to_string(value_);
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Number>(Number(this->value_));
    };
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) override {
        throw SyntaxError("cay not apply");
    }
};

class Symbol : public Object {
private:
    std::string name_;

public:
    Symbol(std::string str) : name_(str){};
    const std::string& GetName() const {
        return name_;
    };
    std::string Cerealize() override {
        return name_;
    }
    std::shared_ptr<Object> Calculate() override {  // возвращает функцию
        return std::make_shared<Symbol>(Symbol(name_));
    }
    std::shared_ptr<Object> Clone() override {
        return std::make_shared<Symbol>(Symbol(this->name_));
    };
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) override {
        throw SyntaxError("cay not apply");
    }
};

class Cell : public Object {

public:
    std::pair<std::shared_ptr<Object>, std::shared_ptr<Object>> cell_;
    Cell(std::shared_ptr<Object> head, std::shared_ptr<Object> tail)
        : cell_(std::make_pair(head, tail)){};
    std::shared_ptr<Object> GetFirst() const {
        return cell_.first;
    };
    std::shared_ptr<Object> GetSecond() const {
        return cell_.second;
    };

    void ChangeFirst(std::shared_ptr<Object> f) {
        cell_.first = f;
    }
    void ChangeSecond(std::shared_ptr<Object> s) {
        cell_.second = s;
    }

    std::string Cerealize() override {
        if (!cell_.first) {
            return "()";  // когда у нас вообще такая штука получается?
        }
        if (!cell_.second) {
            std::string res;
            res += cell_.first->Cerealize();
            // res += ")";
            return res;
        }
        std::string res_first;
        std::string res_second;
        std::string res;
        res_first += cell_.first->Cerealize();
        if (auto num = std::dynamic_pointer_cast<Cell>(cell_.second)) {
            res_second += " ";
            res_second += cell_.second->Cerealize();
        } else {
            res_second += " . ";
            res_second += cell_.second->Cerealize();
        }
        res = res_first + res_second;
        //        res += " ";
        //        res += cell_.second->Cerealize();
        //        res += ")";
        return res;
    }
    std::shared_ptr<Object> Clone() override {
        if (cell_.first == nullptr) {
            return nullptr;
        }
        auto first_clone = cell_.first->Clone();
        auto second_clone = cell_.second->Clone();
        auto cell_clone = std::make_shared<Cell>(Cell(first_clone, second_clone));
        if (auto nested_cell = dynamic_cast<Cell*>(first_clone.get())) {
            cell_clone->cell_.first = nested_cell->Clone();
        }
        return cell_clone;
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& v) override {
        throw SyntaxError("cay not apply");
    }
    std::shared_ptr<Object> Calculate() override {
        auto first = cell_.first->Calculate();
        if (auto symbol = dynamic_cast<Symbol*>(first.get())) {
            // найти соответствующую функцию и применить ее ко второму элементу пары
        } else if (auto nested_cell = dynamic_cast<Cell*>(first.get())) {
            // вызвать метод Calculate для вложенной ячейки и применить его результат к второму
            // элементу пары
            auto result = nested_cell->Calculate();
        } else {
            throw SyntaxError("invalid expression");
        }
    }
};

class AddFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override {
        double sum = 0;
        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Number>(el)) {
                sum += num->GetValue();
            } else {
                throw RuntimeError("Invalid argument type for addition");
            }
        }
        return std::make_shared<Number>(sum);
    }
    std::string Cerealize() override {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() override {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() override {
        throw SyntaxError("can't calculate");
    };
};

class DecreaseFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        double sum = 0;
        if (args.empty()) {
            throw RuntimeError("empty arg_vec for -");
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    sum -= num->GetValue();
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Number>(sum);
    }
};

class MultiplyFunction : public Object {
public:
    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        double sum = 1;
        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Number>(el)) {
                sum *= num->GetValue();
            } else {
                throw RuntimeError("Invalid argument type for addition");
            }
        }
        return std::make_shared<Number>(sum);
    }
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };
};

class DivedeFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        double sum = 0;
        if (args.empty()) {
            throw RuntimeError("empty arg_vec for -");
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    sum /= num->GetValue();
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Number>(sum);
    }
};

class MaxFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            throw RuntimeError("empty arg_vec for -");
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum = num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum < num->GetValue()) {
                        sum = num->GetValue();
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("Invalid argument type for max/min");
        }
        return std::make_shared<Number>(sum);
    }
};

class MinFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            throw RuntimeError("empty arg_vec for -");
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum = num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum > num->GetValue()) {
                        sum = num->GetValue();
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("Invalid argument type for max/min");
        }
        return std::make_shared<Number>(sum);
    }
};

class IsSymbol : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            throw RuntimeError("no arguments");
        }
        bool is_symbol = true;
        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Symbol>(el)) {
                continue;
            } else {
                is_symbol = false;
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
};

class IsNumber : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            throw RuntimeError("no arguments");
        }
        bool is_numb = true;
        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Number>(el)) {
                continue;
            } else {
                is_numb = false;
                return std::make_shared<Boolean>(is_numb);
            }
        }
        return std::make_shared<Boolean>(is_numb);
    }
};

class IsBool : public Object {
    std::string Cerealize() override {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() override {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() override {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) override {
        if (args.empty()) {
            throw RuntimeError("no arguments");
        }
        bool is_numb = true;
        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Boolean>(el)) {
                continue;
            } else {
                is_numb = false;
                return std::make_shared<Boolean>(is_numb);
            }
        }
        return std::make_shared<Boolean>(is_numb);
    }
};

class IntAbsFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        size_t sum = 0;
        if (args.empty() || (args.size() > 1)) {
            throw RuntimeError("empty arg_vec for -");
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            if (num->GetValue() < 0) {
                sum = (-1) * num->GetValue();
            } else {
                sum = num->GetValue();
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Number>(sum);
    }
};

class EqualFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum == num->GetValue()) {
                        continue;
                    } else {
                        return std::make_shared<Boolean>(false);
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Boolean>(true);
    }
};

class GreaterFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum > num->GetValue()) {
                        continue;
                    } else {
                        return std::make_shared<Boolean>(false);
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Boolean>(true);
    }
};

class GreaterEqFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum >= num->GetValue()) {
                        continue;
                    } else {
                        return std::make_shared<Boolean>(false);
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Boolean>(true);
    }
};

class LessFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum < num->GetValue()) {
                        continue;
                    } else {
                        return std::make_shared<Boolean>(false);
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Boolean>(true);
    }
};

class LessEqFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        long long sum = 0;
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Number>(args[0])) {
            sum += num->GetValue();
            for (size_t i = 1; i < args.size(); ++i) {
                if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                    if (sum <= num->GetValue()) {
                        continue;
                    } else {
                        return std::make_shared<Boolean>(false);
                    }
                } else {
                    throw RuntimeError("Invalid argument type for addition");
                }
            }
        } else {
            throw RuntimeError("unvalid arg");
        }
        return std::make_shared<Boolean>(true);
    }
};

class NotFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 1) {
            throw RuntimeError("а что отрацато-то? ну или слишком много аргументов");
        }
        if (Is<Number>(args[0])) {
            return std::make_shared<Boolean>(false);
        }
        if (auto num = std::dynamic_pointer_cast<Boolean>(args[0])) {
            return std::make_shared<Boolean>(!(num->GetValue()));
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            if (num->GetObject() == nullptr) {
                return std::make_shared<Boolean>(false);
            } else {
                return std::make_shared<Boolean>(true);
            }
        }

        throw RuntimeError("не те аргументы");
    }
};

class AndFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }

        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Number>(el)) {
                continue;
            } else if (auto num = std::dynamic_pointer_cast<Boolean>(el)) {
                if (num->GetValue() == false) {
                    return std::make_shared<Boolean>(false);
                }
            } else if (auto num = std::dynamic_pointer_cast<Quote>(el)) {
                if (num->GetObject() == nullptr) {
                    return std::make_shared<Boolean>(false);
                } else {
                    continue;
                }
            }
        }
        auto final = args[args.size() - 1];
        if (auto num = std::dynamic_pointer_cast<Boolean>(final)) {
            if (num->GetValue() == false) {
                return std::make_shared<Boolean>(false);
            } else {
                return std::make_shared<Boolean>(true);
            }
        } else if (auto num = std::dynamic_pointer_cast<Number>(final)) {
            return std::make_shared<Number>(num->GetValue());
        } else if (auto num = std::dynamic_pointer_cast<Symbol>(final)) {
            return std::make_shared<Symbol>(num->GetName());
        } else if (auto num = std::dynamic_pointer_cast<Quote>(final)) {
            return std::make_shared<Quote>(num->GetObject());
        }
    }
};

class OrFunction : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            return std::make_shared<Boolean>(false);
        }

        for (auto el : args) {
            if (auto num = std::dynamic_pointer_cast<Number>(el)) {
                continue;
            } else if (auto num = std::dynamic_pointer_cast<Boolean>(el)) {
                if (num->GetValue() == true) {
                    return std::make_shared<Boolean>(true);
                }
            } else if (auto num = std::dynamic_pointer_cast<Quote>(el)) {
                if (num->GetObject() != nullptr) {
                    return std::make_shared<Boolean>(true);
                } else {
                    continue;
                }
            }
        }
        auto final = args[args.size() - 1];
        if (auto num = std::dynamic_pointer_cast<Boolean>(final)) {
            if (num->GetValue() == true) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(false);
            }
        } else if (auto num = std::dynamic_pointer_cast<Number>(final)) {
            return std::make_shared<Number>(num->GetValue());
        } else if (auto num = std::dynamic_pointer_cast<Symbol>(final)) {
            return std::make_shared<Symbol>(num->GetName());
        } else if (auto num = std::dynamic_pointer_cast<Quote>(final)) {
            return std::make_shared<Quote>(num->GetObject());
        }
    }
};

class IsPair : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 1) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            auto p = num->GetObject();
            if (p != nullptr) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(false);
            }
        } else if (auto num = std::dynamic_pointer_cast<Cell>(args[0])) {
            if (num->GetFirst() != nullptr) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(false);
            }
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class IsNull : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.size() > 1) {
            throw RuntimeError("no or too many arguments");
        }
        if (args.empty()) {
            return std::make_shared<Boolean>(true);
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            auto p = num->GetObject();
            if (p != nullptr) {
                return std::make_shared<Boolean>(false);
            } else {
                return std::make_shared<Boolean>(true);
            }
        } else if (auto num = std::dynamic_pointer_cast<Cell>(args[0])) {
            if (num->GetFirst() != nullptr) {
                return std::make_shared<Boolean>(false);
            } else {
                return std::make_shared<Boolean>(true);
            }
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class IsList : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 1) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            auto p = num->GetObject();
            if (p == nullptr) {
                return std::make_shared<Boolean>(true);
            } else {
                auto first = As<Cell>(p)->GetFirst();
                auto second = As<Cell>(p)->GetSecond();
                if (Is<Cell>(second)) {
                    while (second && Is<Cell>(second)) {
                        if (Is<Cell>(second)) {
                            second = As<Cell>(second)->GetSecond();
                        } else {
                            return std::make_shared<Boolean>(false);
                        }
                    }
                    if (second != nullptr) {
                        return std::make_shared<Boolean>(false);
                    }
                    return std::make_shared<Boolean>(true);
                }
                return std::make_shared<Boolean>(false);
            }
        } else if (auto num = std::dynamic_pointer_cast<Cell>(args[0])) {
            if (num->GetFirst() == nullptr) {
                return std::make_shared<Boolean>(true);
            } else {
                return std::make_shared<Boolean>(true);
            }
        } else {
            return std::make_shared<Boolean>(false);
        }
    }
};

class MakePair : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            throw RuntimeError("no or too many arguments");
        }
        auto first = args[0];
        std::shared_ptr<Object> second = nullptr;
        for (size_t i = 1; i < args.size(); ++i) {
            second = args[i];
        }
        return std::make_shared<Cell>(first, second);
    }
};

class GetFirst : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 1) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            auto p = num->GetObject();
            if (p == nullptr) {
                throw RuntimeError("can not understand");
            } else if (auto n = std::dynamic_pointer_cast<Cell>(p)) {
                return n->GetFirst();
            }
        }
    }
};

class GetSecond : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 1) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            auto p = num->GetObject();
            if (p == nullptr) {
                throw RuntimeError("can not understand");
            } else if (auto n = std::dynamic_pointer_cast<Cell>(p)) {
                return n->GetSecond();
            }
        }
    }
};

class MakeList : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty()) {
            return nullptr;
        }
        auto first = args[0];
        if (args.size() == 1) {
            return std::make_shared<Cell>(first, nullptr);
        }
        auto second = std::make_shared<Cell>(nullptr, nullptr);
        auto ter = second;
        for (size_t i = 1; i < args.size(); i += 2) {
            if (auto num = std::dynamic_pointer_cast<Number>(args[i])) {
                auto f = std::make_shared<Number>(num->GetValue());
                ter->ChangeFirst(f);
            } else if (auto sym = std::dynamic_pointer_cast<Symbol>(args[i])) {
                ter->ChangeFirst(std::make_shared<Symbol>(sym->GetName()));
            } else if (auto boolean = std::dynamic_pointer_cast<Boolean>(args[i])) {
                ter->ChangeFirst(std::make_shared<Boolean>(boolean->GetValue()));
            }

            auto new_cell = std::make_shared<Cell>(args[i + 1], nullptr);
            ter->ChangeSecond(new_cell);
            ter = new_cell;
        }
        return std::make_shared<Cell>(first, second);
    }
};

class GetListElem : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 2) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            if (auto n = std::dynamic_pointer_cast<Number>(args[1])) {
                int ind = n->GetValue();
                auto ast = num->GetObject();
                if (auto cell = std::dynamic_pointer_cast<Cell>(ast)) {
                    auto fin = cell->GetSecond();
                    int i = 1;
                    if (ind == 0) {
                        return cell->GetFirst();
                    }
                    while (fin != nullptr) {
                        if (auto cur = std::dynamic_pointer_cast<Cell>(fin)) {
                            if (i == ind) {
                                if (auto ans = std::dynamic_pointer_cast<Number>(cur->GetFirst())) {
                                    return std::make_shared<Number>(ans->GetValue());
                                } else if (auto ans = std::dynamic_pointer_cast<Boolean>(
                                               cur->GetFirst())) {
                                    return std::make_shared<Boolean>(ans->GetValue());
                                } else if (auto ans =
                                               std::dynamic_pointer_cast<Symbol>(cur->GetFirst())) {
                                    return std::make_shared<Symbol>(ans->GetName());
                                } else {
                                    throw RuntimeError("кринжанула");
                                }
                            }
                            ++i;
                            fin = cur->GetSecond();
                        }
                        if (ind > i) {
                            throw RuntimeError("кринжанула");
                        }
                    }
                } else {
                    if (ind != 0) {
                        throw RuntimeError("invslid index");
                    } else {
                        return ast;
                    }
                }
            }
        }
    }
};

class GetListTail : public Object {
    std::string Cerealize() {
        throw SyntaxError("can't cerealize func");
    };
    std::shared_ptr<Object> Clone() {
        throw SyntaxError("can't clone func");
    };
    std::shared_ptr<Object> Calculate() {
        throw SyntaxError("can't calculate");
    };

    std::shared_ptr<Object> Apply(const std::vector<std::shared_ptr<Object>>& args) {
        if (args.empty() || args.size() > 2) {
            throw RuntimeError("no or too many arguments");
        }
        if (auto num = std::dynamic_pointer_cast<Quote>(args[0])) {
            if (auto n = std::dynamic_pointer_cast<Number>(args[1])) {
                int ind = n->GetValue();
                auto ast = num->GetObject();
                if (auto cell = std::dynamic_pointer_cast<Cell>(ast)) {
                    auto fin = cell->GetSecond();
                    int i = 1;
                    if (ind == 0) {
                        return ast;
                    }

                    while (fin != nullptr) {
                        if (auto cur = std::dynamic_pointer_cast<Cell>(fin)) {
                            if (i == ind) {
                                return fin;
                            }
                            ++i;
                            fin = cur->GetSecond();
                        }
                        if (ind - 1 == i) {
                            return nullptr;
                        } else if (ind > i) {
                            throw RuntimeError("кринжанула");
                        }
                    }
                } else {
                    if (ind != 0) {
                        throw RuntimeError("invslid index");
                    } else {
                        return ast;
                    }
                }
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
