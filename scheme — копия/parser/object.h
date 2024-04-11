#pragma once

#include <memory>
#include <string>
#include <utility>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
private:
    int value_;

public:
    Number(int val) : value_(val){};
    int GetValue() const {
        return value_;
    };
};

class Symbol : public Object {
private:
    std::string name_;

public:
    Symbol(std::string str) : name_(str){};
    const std::string& GetName() const {
        return name_;
    };
};

class Cell : public Object {
private:
    std::pair<std::shared_ptr<Object>, std::shared_ptr<Object>> cell_;

public:
    Cell(std::shared_ptr<Object> head, std::shared_ptr<Object> tail)
        : cell_(std::make_pair(head, tail)){};
    std::shared_ptr<Object> GetFirst() const {
        return cell_.first;
    };
    std::shared_ptr<Object> GetSecond() const {
        return cell_.second;
    };
    void ChangeFirst(std::shared_ptr<Object> obj) {
        cell_.first = obj;
    }

    void ChangeSecond(std::shared_ptr<Object> obj) {
        cell_.second = obj;
    }
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
};
