# ifndef TYPE
# define TYPE

# include <iostream>
# include <string>

using namespace std;

//enum base_types{VOID_TYPE, INT_TYPE, FLOAT_TYPE };
enum type_types{BASE, ARRAY, POINTER};

class datatype{
    public:
    type_types type;
    bool is_struct;
    string base_type;
    datatype *base;
    string type_str();
    int size;
    int arr_size;
};

datatype createtype(string base_type);
datatype createtype(string base_type, int size);
datatype createtype(datatype *type, int size);
datatype createtype(datatype *type);

#endif