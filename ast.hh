#ifndef AST
#define AST

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <cstdarg>

# include "type.hh"
using namespace std;

enum typeExp {seq_astNode, assignS_astNode, return_astNode, printf_astNode, proccall_astNode, if_astNode, while_astNode, for_astNode, op_binary_astNode, op_unary_astNode, assignE_astNode, funcall_astNode, intconst_astNode, floatconst_astNode, stringconst_astNode, identifier_astNode, arrayref_astNode, member_astNode, arrow_astNode};

class abstract_astnode {
    public:
    // abstract_astnode();
    typeExp ast_node;
    virtual void print() = 0;
    virtual void printcode() = 0;
};

class statement_astnode : public abstract_astnode{
    public:
    int next_label;
    int repeat_label;
    int return_offset;
    virtual void print() = 0;
    virtual void printcode() = 0;
};

class exp_astnode : public abstract_astnode{
    public:
    bool lval;
    bool rchild;
    string address;
    bool is_null;
    datatype type;
    int label;
    int param_size;
    int loc_size;
    virtual void print() = 0;
    virtual void printcode() = 0;
};



class empty_astnode : public statement_astnode{
    public:
    //empty_astnode();
    void print();
    void printcode();
};

class seq_astnode : public statement_astnode{
    public:
	vector<statement_astnode *> seq_nodes;
    seq_astnode(vector<statement_astnode *> seq_nodes);
    void print();
    void printcode();
};

class assignS_astnode : public statement_astnode{
    public:
	exp_astnode* lhs;
	exp_astnode* rhs;
    assignS_astnode(exp_astnode* lhs, exp_astnode* rhs);
    void print();
    void printcode();
};

class return_astnode : public statement_astnode{
    public:
	exp_astnode* ret;
    return_astnode(exp_astnode* ret);
    void print();
    void printcode();
};

class if_astnode : public statement_astnode{
    public:
	exp_astnode* cond;
	statement_astnode* then_st;
	statement_astnode* else_st;
    int else_label;
    if_astnode(exp_astnode* cond, statement_astnode* then_st,	statement_astnode* else_st);
    void print();
    void printcode();
};
class while_astnode : public statement_astnode{
    public:
	exp_astnode* cond;
	statement_astnode* loop_st;
    int repeat_label;
    while_astnode(exp_astnode* cond,statement_astnode* loop_st);
    void print();
    void printcode();
};
class for_astnode : public statement_astnode{
    public:
	exp_astnode* init;
	exp_astnode* guard;
	exp_astnode* step;
	statement_astnode* body;
    for_astnode(exp_astnode* init,exp_astnode* guard,exp_astnode* step,statement_astnode* body);
    void print();
    void printcode();
};

class op_binary_astnode : public exp_astnode{
    public:
	string op;
	exp_astnode* left;
	exp_astnode* right;
    op_binary_astnode(string op, exp_astnode* left, exp_astnode* right, datatype type);
    void print();
    void printcode();
};
class op_unary_astnode : public exp_astnode{
    public:
	string op;
	exp_astnode* operand;
    op_unary_astnode(string op, exp_astnode* operand);
    void print();
    void printcode();
};
class assignE_astnode : public exp_astnode{
    public:
    exp_astnode* lhs;
	exp_astnode* rhs;
    assignE_astnode(exp_astnode* lhs,exp_astnode* rhs);
    void print();
    void printcode();
	
};

class intconst_astnode : public exp_astnode{
    public:
	int num;
    intconst_astnode(int num);
    void print();
    void printcode();
};

class stringconst_astnode : public exp_astnode{
    public:
	string str;
    stringconst_astnode(string str);
    void print();
    void printcode();
};

class ref_astnode : public exp_astnode{
    public:
};

class identifier_astnode : public ref_astnode{
    public:
	string id;
    identifier_astnode(string id, datatype type);
    //identifier_astnode(string id);
    void print();
    void printcode();
};

class arrayref_astnode : public ref_astnode{
    public:
	exp_astnode* base;
	exp_astnode* offset;
    arrayref_astnode(exp_astnode* base, exp_astnode* offset);
    void print();
    void printcode();
};

class member_astnode : public ref_astnode{
    public:
	exp_astnode* member_of;
	identifier_astnode* member;
    member_astnode(exp_astnode* member_of, identifier_astnode* member);
    void print();
    void printcode();
};

class arrow_astnode : public ref_astnode{
    public:
	exp_astnode* pointer_to_member_of;
	identifier_astnode* member;
    arrow_astnode(exp_astnode* pointer_to_member_of, identifier_astnode* member);
    void print();
    void printcode();
};

class funcall_astnode : public exp_astnode{
    public:
    identifier_astnode *fname;
	vector<exp_astnode *> params;
    funcall_astnode(identifier_astnode *fname, vector<exp_astnode *> params, datatype ret_type);
    void print();
    void printcode();
	
};

class proccall_astnode : public statement_astnode{
    public:
    identifier_astnode *fname;
	vector<exp_astnode *> params;
    int size;
    int loc_size;
    proccall_astnode(identifier_astnode *fname, vector<exp_astnode *> params);
    void print();
    void printcode();
};

class printf_astnode : public statement_astnode{
    public:
    int size;
    string str_addr;
	vector<exp_astnode *> params;
    printf_astnode(string str_addr, vector<exp_astnode *> params);
    void print();
    void printcode();
};

op_binary_astnode * binary_comp_check(string optr, exp_astnode* lhs, exp_astnode* rhs);
op_unary_astnode * unary_comp_check(string optr, exp_astnode* rhs);
exp_astnode *assign_comp_check(datatype lhs, exp_astnode* rhs);

#endif