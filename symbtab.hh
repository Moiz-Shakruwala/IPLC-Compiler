# include <iostream>
# include <string>
# include <vector>
# include <map>

# include "type.hh"

using namespace std;

class declarator_class{
    public:
    string id;
    string ptr_arr;
    vector<string> stack;
};

class symbol{
    public:
    string loc_param;
    datatype type;
    string id;
    int offset;
};

class variable : public symbol{
    public:
    variable(datatype typespec, declarator_class *dcltr, int offset);
};

class parameter: public symbol{
    public:
    parameter(datatype typespec, declarator_class* dcltr);
};

class fun_declarator_class{
    public:
    string id;
    vector<parameter*> params;
    fun_declarator_class(string id, vector<parameter*> params);
};

class symbtab_class{
    public:
    map<string, symbol> symbols;
    vector<datatype> param_types;
    map<int, string> strs;
    bool has_rval;
    void print();
};

class fs_symbol{
    public:
    string varfun;
    string id;
    symbtab_class *symbtab;
    int size;
    int loc_size;
    datatype ret_type;
};

class fun_symbol: public fs_symbol{
    public:
    fun_symbol(datatype ret_type, string id, symbtab_class *symbtab);
};

class struct_symbol: public fs_symbol{
    public:
    struct_symbol(string id, int size, symbtab_class *symbtab);
};

class g_symbtab_class {
    public:
    map<string, fs_symbol> Entries;
    void printgst();
};