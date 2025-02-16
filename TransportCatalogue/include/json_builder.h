#pragma once

#include <string>
#include <vector>
#include "json.h"

namespace json {

class DictItemContext;
class ArrayItemContext;
class KeyValueContext;

class Builder {
public:
    Builder();
    Node Build();

    KeyValueContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    Node::Value& GetCurrentValue();
    const Node::Value& GetCurrentValue() const;

    void AssertNewObjectContext() const;
    void AddObject(Node::Value value, bool one_shot);
};

class DictItemContext {
public:
    DictItemContext(Builder& builder) 
        : builder_(builder) {}

    KeyValueContext Key(std::string key);
    Builder& EndDict();

private:
    Builder& builder_;
};

class ArrayItemContext {
public:
    ArrayItemContext(Builder& builder) 
        : builder_(builder) {}

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndArray();

private:
    Builder& builder_;
};

class KeyValueContext {
public:
    KeyValueContext(Builder& builder) 
        : builder_(builder) {}

    DictItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();

private:
    Builder& builder_;
};

}  // namespace json
