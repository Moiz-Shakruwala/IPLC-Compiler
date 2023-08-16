#include "symbtab.hh"

variable::variable(datatype typespec, declarator_class *dcltr, int offset){
	
	this->type = typespec;
	for(auto it = dcltr->stack.rbegin();it!=dcltr->stack.rend();++it){
		datatype *temp = new datatype();
		*temp = this->type;
		if((*it) == "*")this->type = createtype(temp);
		else this->type = createtype(temp, stoi(*it));
	}
	this->id = dcltr->id;
	this->offset = offset;
	this->loc_param = "local";
}

parameter::parameter(datatype typespec, declarator_class* dcltr){
	this->type = typespec;
	for(auto it = dcltr->stack.rbegin();it!=dcltr->stack.rend();++it){
		datatype *temp = new datatype();
		*temp = this->type;
		if((*it) == "*")this->type = createtype(temp);
		else this->type = createtype(temp, stoi(*it));
	}
	this->id = dcltr->id;
	this->loc_param = "param";
}

fun_symbol::fun_symbol(datatype ret_type, string id , symbtab_class *symbtab){
	this->varfun="fun";
	this->symbtab = symbtab;
	this->id = id;
	this->ret_type = ret_type;
}

struct_symbol::struct_symbol(string id , int size, symbtab_class *symbtab){
	this->varfun="struct";
	this->symbtab = symbtab;
	this->id = id;
	this->size = size;
}

fun_declarator_class::fun_declarator_class(string id, vector<parameter*> params){
	this->id = id;
	this->params = params;
}

void g_symbtab_class::printgst(){
	cout<<"["<<endl;
	for(auto it = Entries.begin();it!= Entries.end();++it){
		if (it->second.varfun == "fun"){
			cout<<"["<<endl;
			cout<< "\"" <<it->first<< "\"" << "," << endl;
			cout<< "\"" << "fun" << "\"" << "," << endl;
			cout<< "\"" << "global"<< "\"" << "," << endl;
			cout<< 0 << "," << endl;
			cout<< 0 << "," << endl;
			cout<< "\"" << it->second.ret_type.type_str()<< "\""; //<< "," << endl;
			cout<<"]"<<endl;
		}
		else{
			cout<<"["<<endl;
			cout<< "\"" <<it->first<< "\"" << "," << endl;
			cout<< "\"" << "struct" << "\"" << "," << endl;
			cout<< "\"" << "global"<< "\"" << "," << endl;
			cout<< it->second.size << "," << endl;
			cout<< "\"" << "-" << "\"" << "," << endl;
			cout<< "\"" << "-" << "\""; //<< "," << endl;
			cout<<"]"<<endl;
		}
		
		if (next(it,1) != Entries.end()) 
		cout << "," << endl;
	}
	cout<<endl<<"]"<<endl;
}

void symbtab_class::print(){
	cout<<"["<<endl;
	for(auto it = symbols.begin();it!= symbols.end();++it){
		cout<<"["<<endl;
		cout<< "\"" <<it->first<< "\"" << "," << endl;
		cout<< "\"" << "var" << "\"" << "," << endl;
		cout<< "\"" << it->second.loc_param<< "\"" << "," << endl;
		cout<< it->second.type.size<< "," << endl;
		cout<< it->second.offset<< "," << endl;
		cout<< "\"" << it->second.type.type_str()<< "\"";
		cout<<"]"<<endl;
		if (next(it,1) != symbols.end()) 
		cout << "," << endl;
	}
	cout<<endl<<"]"<<endl;
}
