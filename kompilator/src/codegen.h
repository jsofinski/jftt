#pragma once

typedef long long i64;

void print_lines();
void initArray(int id, i64 start, i64 end);
void addVariable(int id);
void assertVariableExists(int id);

class Node {
public:
    virtual void codegen();
};

class Identifier : public Node {
public:
    Identifier(i64 id);
    Identifier(i64 id, Node* value);

    virtual void codegen();
    virtual void codegenGetIndex();
    i64 id;
    Node* value;
};

class Program : public Node {
public:
    Program(Node* commands);

    virtual void codegen();

    Node* commands;
};
class Declarations : public Node {
public:
    Declarations();
};
class Commands : public Node {
public:
    Commands(Node* commands, Node* command);

    virtual void codegen();
    Node* commands;
    Node* command;
};
class Assign : public Node {
public:
    Assign(Identifier* id, Node* expression);

    virtual void codegen();
    Identifier* id;
    Node* expression;
};
class Expression : public Node {
public:
    Expression(Node* bvalue, int symbol, Node* cvalue);
    virtual void codegen();
    Node* bvalue;
    int symbol;
    Node* cvalue;
};

class Condition : public Node {
public:
    Condition(Node* bvalue, int symbol, Node* cvalue);
    virtual void codegen();
    Node* bvalue;
    int symbol;
    Node* cvalue;
};

class IfThen : public Node {
public:
    IfThen(Condition* condition, Node* commands, Node* elseBody);

    virtual void codegen();
private:
    Condition* condition;
    Node* commands;
    Node* elseBody;
};
class While : public Node {
public:
    While(Condition* condition, Node* commands);
    Condition* condition;
    Node* commands;

    virtual void codegen();
};
class Repeat : public Node {
public:
    Repeat(Node* commands, Condition* condition);
    Condition* condition;
    Node* commands;

    virtual void codegen();
};
class For : public Node {
public:
    For(Identifier* identifier, Node* fromValue, Node* toValue, Node* commands, int step);
    Identifier* identifier;
    Node* fromValue;
    Node* toValue;
    Node* commands;
    int step;
    
    virtual void codegen();
};
class Read : public Node {
public:
    Read(Identifier* identifier);
    
    virtual void codegen();
    Identifier* identifier;
};
class Write : public Node {
public:
    Write(Node* value);
    Write(i64 intval);

    virtual void codegen();
private:
    i64 intval;
    Node* value;
};
class Num : public Node {
public:
    Num(i64 value);

    virtual void codegen();
    i64 value;
};
    