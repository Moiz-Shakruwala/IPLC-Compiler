#include "type.hh"
#include "symbtab.hh"

string datatype::type_str(){
	string my_str= "";
	datatype *iter_type = new datatype;
	*iter_type = *this;
	while(true){
		if(iter_type->type==BASE) {
			my_str= iter_type->base_type+my_str;
			break;
		}
		else if(iter_type->type == POINTER){
			my_str= "*"+my_str;
		}
		else if(iter_type->type == ARRAY){
			if(my_str[0]=='*') my_str="("+my_str+")"+"["+ to_string(iter_type->arr_size) +"]";
			else my_str=my_str+"["+ to_string(iter_type->arr_size) +"]";
		}
		iter_type = iter_type->base;
	}
	return my_str;	
}

datatype createtype(string base_type){
	datatype temp;
	temp.type = BASE;
	temp.base_type = base_type;
	if(base_type == "int" || base_type == "float") temp.size = 4;
	else if (base_type == "void") temp.size = 0;
	temp.is_struct=false;
    return temp;
}

datatype createtype(string base_type, int size){
	datatype temp;
	temp.type = BASE;
	temp.base_type = base_type;
	temp.size = size;
	temp.is_struct=true;
	return temp;
}

datatype createtype(datatype *type_in, int size){
	datatype temp;
	temp.type = ARRAY;
	temp.base_type = type_in->base_type ;
	temp.arr_size = size;
	temp.base = type_in;
	temp.size = type_in->size*size;
	temp.is_struct=false;
	return temp;
}

datatype createtype(datatype *type_in){
    datatype temp;
	temp.type = POINTER;
	temp.base_type = type_in->base_type;
	temp.base = type_in;
	temp.size = 4;
	temp.is_struct=false;
	return temp;
}
