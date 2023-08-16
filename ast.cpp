#include "ast.hh"

string regs[6] = {"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"};
std::map<string, string> instr = { {"MULT_INT", "imull"}, {"PLUS_INT", "addl"}, {"MINUS_INT", "subl"}};
int top_rstack=0;
int roc=0;

extern int label_cnt;

seq_astnode::seq_astnode(vector<statement_astnode *> seq_nodes){
    this->ast_node = seq_astNode;
	this->seq_nodes = seq_nodes;
}

assignS_astnode::assignS_astnode(exp_astnode* lhs, exp_astnode* rhs){
    this->ast_node = assignS_astNode;
	this->lhs = lhs;
	this->rhs = rhs;
}

return_astnode::return_astnode(exp_astnode* ret){
    this->ast_node = return_astNode;
	this->ret = ret;
}

proccall_astnode::proccall_astnode(identifier_astnode *fname, vector<exp_astnode *> params){
    this->ast_node = proccall_astNode;
	this->fname = fname;
	this->params = params;
}

printf_astnode::printf_astnode(string str_addr, vector<exp_astnode *> params){
	this->str_addr = str_addr;
    this->ast_node = printf_astNode;
	this->params = params;
	this->size = 4*(1+ params.size());
}

if_astnode::if_astnode(exp_astnode* cond, statement_astnode* then_st,	statement_astnode* else_st){
    this->ast_node = if_astNode;
	this->cond = cond;
	this->then_st = then_st;
	this->else_st = else_st;
}

while_astnode::while_astnode(exp_astnode* cond,statement_astnode* loop_st){
    this->ast_node = while_astNode;
	this->cond = cond;
	this->loop_st = loop_st;
}

for_astnode::for_astnode(exp_astnode* init,exp_astnode* guard,exp_astnode* step,statement_astnode* body){
    this->ast_node = for_astNode;
	this->init = init;
	this->guard = guard;
	this->step = step;
	this->body = body;
}

op_binary_astnode::op_binary_astnode(string op, exp_astnode* left, exp_astnode* right, datatype type){
    this->ast_node = op_binary_astNode;
	this->op = op;
	this->left = left;
	this->right = right;
	this->lval = false;
	this->type = type;
}

op_unary_astnode::op_unary_astnode(string op, exp_astnode* operand){
    this->ast_node = op_unary_astNode;
	this->op = op;
	this->operand = operand;
	if(op == "DEREF") this->lval = true;
	else {this->lval = false;}

	if(op == "TO_FLOAT") this->type = createtype("float");
	else if(op == "TO_INT") this->type = createtype("int");
	else if(op == "UMINUS" || op == "PP") this->type = operand->type;
	else if(op == "NOT") this->type = createtype("int");
	else if(op == "ADDRESS") this->type = createtype(&(operand->type));
	else if(op == "DEREF") this->type = *(operand->type.base);
}

assignE_astnode::assignE_astnode(exp_astnode* lhs,exp_astnode* rhs){
    this->ast_node = assignE_astNode;
	this->lhs = lhs;
	this->rhs = rhs;
	this->lval = false;
	this->type = lhs->type;
}

funcall_astnode::funcall_astnode(identifier_astnode *fname, vector<exp_astnode *> params, datatype ret_type){
    this->ast_node = funcall_astNode;
	this->fname = fname;
	this->params = params;
	this->lval = false;
	this->type = ret_type;
}

intconst_astnode::intconst_astnode(int num){
    this->ast_node = intconst_astNode;
	this->num = num;
	if(num==0) is_null=true;
	else is_null=false;
	this->lval = false;
	this->type = createtype("int");
	this->rchild = false;
}


stringconst_astnode::stringconst_astnode(string str){
    this->ast_node = stringconst_astNode;
	this->str = str;
	this->lval = false;
	this->type= createtype("string");
}


identifier_astnode::identifier_astnode(string id, datatype type){
	this->ast_node = identifier_astNode;
	this->id = id;
	this->lval = true;
	this->type = type;
	this->rchild = false;
}

arrayref_astnode::arrayref_astnode(exp_astnode* base, exp_astnode* offset){
    this->ast_node = arrayref_astNode;
	this->base = base;
	this->offset = offset;
	this->lval = true;
	this->type = *(base->type.base);
}

member_astnode::member_astnode(exp_astnode* member_of, identifier_astnode* member){
    this->ast_node = member_astNode;
	this->member_of = member_of;
	this->member = member;
	this->lval = true;
	this->type = member->type;
}

arrow_astnode::arrow_astnode(exp_astnode* pointer_to_member_of, identifier_astnode* member){
    this->ast_node = arrow_astNode;
	this->pointer_to_member_of = pointer_to_member_of;
	this->member = member;
	this->lval = true;
	this->type = member->type;
}

void empty_astnode::printcode(){
}

void seq_astnode::printcode(){
	for (auto it = this->seq_nodes.begin(); it != seq_nodes.end(); ++it){
		(*it)->printcode();
	}
}


void print_binary(string op, string lop, string rop){
	if(op=="MULT_INT"||op=="PLUS_INT"||op=="MINUS_INT"){
		cout<<"\t"<<instr[op]<<"\t"<<lop<<", "<<rop<<endl;
	}
	else if(op=="LT_OP_INT"||op=="GT_OP_INT"||op=="GE_OP_INT"||op=="LE_OP_INT" || op=="EQ_OP_INT"||op=="NE_OP_INT"){
		label_cnt++;
		cout<<"\t"<<"cmp\t"<<lop<<", "<<rop<<endl;
		if(op=="LT_OP_INT")cout<<"\t"<<"jge .L"<<label_cnt<<endl;
		else if(op=="GT_OP_INT") cout<<"\t"<<"jle .L"<<label_cnt<<endl;
		else if(op=="GE_OP_INT")cout<<"\t"<<"jl .L"<<label_cnt<<endl;
		else if(op=="LE_OP_INT")cout<<"\t"<<"jg .L"<<label_cnt<<endl;
		else if(op=="EQ_OP_INT")cout<<"\t"<<"jne .L"<<label_cnt<<endl;
		else if(op=="NE_OP_INT")cout<<"\t"<<"je .L"<<label_cnt<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$1"<<", "<<rop<<endl;
		cout<<"\t"<<"jmp .L"<<label_cnt+1<<endl;
		cout<<".L"<<label_cnt<<":"<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$0"<<", "<<rop<<endl;
		cout<<".L"<<label_cnt+1<<":"<<endl;
		label_cnt++;
	}
	else if(op=="AND_OP"){
		label_cnt++;
		cout<<"\t"<<"cmp\t"<<"$0"<<", "<<rop<<endl;
		cout<<"\t"<<"je .L"<<label_cnt<<endl;
		cout<<"\t"<<"cmp\t"<<"$0"<<", "<<lop<<endl;
		cout<<"\t"<<"je .L"<<label_cnt<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$1"<<", "<<rop<<endl;
		cout<<"\t"<<"jmp .L"<<label_cnt+1<<endl;
		cout<<".L"<<label_cnt<<":"<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$0"<<", "<<rop<<endl;
		cout<<".L"<<label_cnt+1<<":"<<endl;
		label_cnt++;
	}
	else if(op=="OR_OP"){
		label_cnt++;
		cout<<"\t"<<"cmp\t"<<"$0"<<", "<<rop<<endl;
		cout<<"\t"<<"jne .L"<<label_cnt<<endl;
		cout<<"\t"<<"cmp\t"<<"$0"<<", "<<lop<<endl;
		cout<<"\t"<<"jne .L"<<label_cnt<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$0"<<", "<<rop<<endl;
		cout<<"\t"<<"jmp .L"<<label_cnt+1<<endl;
		cout<<".L"<<label_cnt<<":"<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$1"<<", "<<rop<<endl;
		cout<<".L"<<label_cnt+1<<":"<<endl;
		label_cnt++;
	}
	else if(op=="DIV_INT"){
		cout<<"\tmovl\t%eax, %edi\n";
		cout<<"\tmovl\t"<<rop<<", %eax\n";
		cout<<"\tcltd\n";
		if(lop=="%eax")cout<<"\tidivl\t%edi"<<endl;
		else if(lop[0]=='$'){
			cout<<"\tmovl\t"<<lop<<", %esi\n";
			cout<<"\tidivl\t"<<"%esi"<<endl;
		}
		else cout<<"\tidivl\t"<<lop<<endl;
		cout<<"\tmovl\t%eax, "<<rop<<"\n";
		if(rop!="%eax")cout<<"\tmovl\t%edi, %eax\n";
	}
}

void assignS_astnode::printcode(){
	this->rhs->printcode();
	if(this->lhs->ast_node==arrayref_astNode){
		
		cout<<"\tmovl\t %eax, %edi\n";
		this->lhs->printcode();
		cout<<"\tpopl\t"<<"%eax"<<endl;
		cout<<"\taddl\t%ebp, %eax\n";
		cout<<"\tmovl\t%edi, "<<"(%eax)"<<"\n";
	}
	else if(this->lhs->ast_node==arrow_astNode){
		
		cout<<"\tmovl\t %eax, %edi\n";
		this->lhs->printcode();
		cout<<"\tmovl\t%esi, "<<"%eax"<<endl;
		cout<<"\tmovl\t%edi, "<<"(%eax)"<<"\n";
	}
	else if(this->lhs->ast_node==op_unary_astNode){
		cout<<"\tmovl\t %eax, %edi\n";
		this->lhs->printcode();
		cout<<"\tmovl\t%esi, "<<"%eax"<<endl;
		cout<<"\tmovl\t%edi, "<<"(%eax)"<<"\n";
	}
	else{
		cout<<"\tmovl\t%eax, "<<this->lhs->address<<"\n";
	}
}

void return_astnode::printcode(){
	this->ret->printcode();

	cout<<"\tmovl\t%eax, "<<this->return_offset<<"(%ebp)"<<endl;
	cout << "\tleave\n";
	cout << "\tret\n";	
}

void proccall_astnode::printcode(){
	// for(auto it =this->params.begin(); it!=this->params.end(); it++){
	// 	(*it)->printcode();
	// 	cout<<"\tpushl\t%eax"<<endl;
	// }
	// cout<<"\tcall\t"<<this->fname->id<<"\n";
	// cout<<"\taddl\t$"<<this->size<<", %esp\n";
	roc=0; //return_offset_correction
	cout<<"\tsubl\t$4"<<", %esp\n";
	for(int i=0; i<=top_rstack; i++){
		cout<<"\tpushl\t"<<regs[i]<<endl;
		roc+=4;
	}
	cout<<"\tsubl\t$4"<<", %esp\n";
	for(auto it =this->params.begin(); it!=this->params.end(); it++){
		(*it)->printcode();
		cout<<"\tpushl\t%eax"<<endl;
	}
	cout<<"\tcall\t"<<this->fname->id<<"\n";
	cout<<"\taddl\t$"<<this->size<<", %esp\n";
	cout<<"\tpopl\t%edi\n";
	cout<<"\tmovl\t%edi, "<< to_string(roc)+"(%esp)"<<endl;
	for(int i=top_rstack;i>=0 ;i--){
		cout<<"\tpopl\t"<<regs[i]<<endl;
	}
	cout<<"\taddl\t$4"<<", %esp\n";
}

void printf_astnode::printcode(){
	for(auto it =this->params.rbegin(); it!=this->params.rend(); it++){
		(*it)->printcode();
		cout<<"\tpushl\t%eax"<<endl;
	}
	cout<<"\tpushl\t"<<this->str_addr<<endl;
	cout<<"\tcall\tprintf\n";
	cout<<"\taddl\t$"<<this->size<<", %esp\n";
}

void if_astnode::printcode(){
	this->cond->printcode();
	cout<<"\tcmpl\t$0, "<<regs[top_rstack]<<endl;
	cout<<"\tje .L"<<this->else_label<<endl;
	this->then_st->printcode();
	cout<<"\tjmp .L"<<this->next_label<<endl;
	cout<<".L"<<this->else_label<<":"<<endl;
	this->else_st->printcode();
	cout<<".L"<<this->next_label<<":"<<endl;
}

void while_astnode::printcode(){
	cout<<".L"<<this->repeat_label<<":"<<endl;
	this->cond->printcode();
	cout<<"\tcmpl\t$0, "<<regs[top_rstack]<<endl;
	cout<<"\tje .L"<<this->next_label<<endl;
	this->loop_st->printcode();
	cout<<"\tjmp .L"<<this->repeat_label<<endl;
	cout<<".L"<<this->next_label<<":"<<endl;
}

void for_astnode::printcode(){
	this->init->printcode();
	cout<<".L"<<this->repeat_label<<":"<<endl;
	this->guard->printcode();
	cout<<"\tcmpl\t$0, "<<regs[top_rstack]<<endl;
	cout<<"\tje .L"<<this->next_label<<endl;
	this->body->printcode();
	this->step->printcode();
	cout<<"\tjmp .L"<<this->repeat_label<<endl;
	cout<<".L"<<this->next_label<<":"<<endl;

}

void op_binary_astnode::printcode(){
	if(this->right->ast_node==identifier_astNode||this->right->ast_node==intconst_astNode||this->right->ast_node==member_astNode){
		this->left->printcode();
		if(this->left->type.type==POINTER && this->right->ast_node==intconst_astNode){
			string temp = "$"+to_string(4*stoi(this->right->address.substr(1, this->right->address.length())));
			print_binary(this->op, temp, regs[top_rstack]);
		}
		else print_binary(this->op, this->right->address, regs[top_rstack]);
		//cout<<"\t"<<instr[this->op]<<"\t"<<this->right->address<<", "<<regs[top_rstack]<<endl;
	}
	else if(this->right->ast_node==funcall_astNode){
		this->right->printcode();
		this->left->printcode();
		print_binary(this->op, this->right->address, regs[top_rstack]);
		cout<<"\taddl\t$4"<<", %esp\n";
		//cout<<"\t"<<instr[this->op]<<"\t"<<this->right->address<<", "<<regs[top_rstack]<<endl;
	}
	else if(this->left->label < this->right->label && this->left->label<3){
		string temp = regs[top_rstack];
		regs[top_rstack] = regs[top_rstack+1];
		regs[top_rstack+1] = temp;
		this->right->printcode();
		string R = regs[top_rstack];
		top_rstack+=1;
		this->left->printcode();
		print_binary(this->op, R, regs[top_rstack]);
		//cout<<"\t"<<instr[this->op]<<"\t"<<R<<", "<<regs[top_rstack]<<endl;
		top_rstack-=1;
		temp = regs[top_rstack];
		regs[top_rstack] = regs[top_rstack+1];
		regs[top_rstack+1] = temp;

	}
	else if(this->left->label>=this->right->label && this->right->label<3){
		this->left->printcode();
		string R = regs[top_rstack];
		top_rstack+=1;
		this->right->printcode();
		print_binary(this->op, regs[top_rstack], R);
		//cout<<"\t"<<instr[this->op]<<"\t"<<regs[top_rstack]<<", "<<R<<endl;
		top_rstack-=1;
	}
	else{
		this->right->printcode();
		cout<<"\tpushl\t"<<regs[top_rstack]<<endl;
		this->left->printcode();
		print_binary(this->op, "(%esp)", regs[top_rstack]);
		//cout<<"\t"<<instr[this->op]<<"\t"<<"(%esp)" <<", "<<regs[top_rstack]<<endl;
		cout<<"\taddl\t$4, %esp\n";

	}

}

void op_unary_astnode::printcode(){
	if(this->op=="UMINUS"){
		this->operand->printcode();
		cout<<"\timull\t$-1, "<<regs[top_rstack]<<endl;
	}
	else if(this->op=="PP"){
		this->operand->printcode();
		cout<<"\taddl\t$1, "<<this->operand->address<<endl;
	}
	else if(this->op=="NOT"){
		this->operand->printcode();
		label_cnt++;
		cout<<"\t"<<"cmpl\t$0"<<", "<<regs[top_rstack]<<endl;
		cout<<"\t"<<"je .L"<<label_cnt<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$0"<<", "<<regs[top_rstack]<<endl;
		cout<<"\t"<<"jmp .L"<<label_cnt+1<<endl;
		cout<<".L"<<label_cnt<<":"<<endl;
		cout<<"\t"<<"movl"<<"\t"<<"$1"<<", "<<regs[top_rstack]<<endl;
		cout<<".L"<<label_cnt+1<<":"<<endl;
		label_cnt++;
	}
	else if(this->op=="ADDRESS"){
		cout<<"\tleal\t"<<this->operand->address<<", "<<regs[top_rstack]<<endl;
	}
	else if(this->op=="DEREF"){
		this->operand->printcode();
		cout<<"\tmovl\t"<<regs[top_rstack]<<", "<<"%esi"<<endl;
		cout<<"\tmovl\t("<<regs[top_rstack]<<"), "<<regs[top_rstack]<<endl;
	}
}

void assignE_astnode::printcode(){
	this->rhs->printcode();
	cout<<"\tmovl\t %eax, "<<this->lhs->address<<"\n";
}

void funcall_astnode::printcode(){
	roc=0; //return_offset_correction
	cout<<"\tsubl\t$4"<<", %esp\n";
	for(int i=0; i<=top_rstack; i++){
		cout<<"\tpushl\t"<<regs[i]<<endl;
		roc+=4;
	}
	cout<<"\tsubl\t$4"<<", %esp\n";
	for(auto it =this->params.begin(); it!=this->params.end(); it++){
		(*it)->printcode();
		cout<<"\tpushl\t%eax"<<endl;
	}
	cout<<"\tcall\t"<<this->fname->id<<"\n";
	cout<<"\taddl\t$"<<this->param_size<<", %esp\n";
	this->address="(%esp)";
	cout<<"\tpopl\t%edi\n";
	cout<<"\tmovl\t%edi, "<< to_string(roc)+"(%esp)"<<endl;
	for(int i=top_rstack;i>=0 ;i--){
		cout<<"\tpopl\t"<<regs[i]<<endl;
	}
	if(!this->rchild){
		cout<<"\tpopl\t"<<regs[top_rstack] <<"\n";
		//cout<<"\taddl\t$4"<<", %esp\n";
	}	
}

void intconst_astnode::printcode(){
	if(!this->rchild) cout<<"\tmovl\t"<<this->address<<", "<< regs[top_rstack] <<"\n";
}

void stringconst_astnode::printcode(){
}

void identifier_astnode::printcode(){
	if(!this->rchild) cout<<"\tmovl\t"<<this->address<<", "<< regs[top_rstack] <<"\n";
}

void arrayref_astnode::printcode(){
	this->offset->printcode();
	cout<<"\timull\t$"<<this->offset->type.size<<", "<<regs[top_rstack]<<endl;
	cout<<"\taddl\t$"<< (this->base->address.substr(0, this->base->address.length()-6 ))<<", "<<regs[top_rstack]<<endl;
	cout<<"\tpushl\t"<<regs[top_rstack]<<endl;
	cout<<"\taddl\t"<< "%ebp"<<", "<<regs[top_rstack]<<endl;
	cout<<"\tmovl\t("<< regs[top_rstack]<<"), "<<regs[top_rstack]<<endl;
}

void member_astnode::printcode(){
	if(!this->rchild) cout<<"\tmovl\t"<<this->address<<", "<< regs[top_rstack] <<"\n";	
}

void arrow_astnode::printcode(){
	this->pointer_to_member_of->printcode();
	cout<<"\taddl\t$"<<this->address<<", "<<regs[top_rstack] <<"\n";
	cout<<"\tmovl\t"<<regs[top_rstack]<<", %esi"<<endl;
	cout<<"\tmovl\t("<<regs[top_rstack]<<"), "<<regs[top_rstack] <<"\n";
}



void printAst(const char *astname, const char *fmt...) // fmt is a format string that tells about the type of the arguments.
{   
	typedef vector<exp_astnode *>* pv;
	va_list args;
	va_start(args, fmt);
	if ((astname != NULL) && (astname[0] != '\0'))
	{
		cout << "{ ";
		cout << "\"" << astname << "\"" << ": ";
	}
	cout << "{" << endl;
	while (*fmt != '\0')
	{
		if (*fmt == 'a')
		{
			char * field = va_arg(args, char *);
			abstract_astnode *a = va_arg(args, abstract_astnode *);
			cout << "\"" << field << "\": " << endl;
			
			a->print();
		}
		else if (*fmt == 's')
		{
			char * field = va_arg(args, char *);
			char *str = va_arg(args, char *);
			cout << "\"" << field << "\": ";

			cout << str << endl;
		}
		else if (*fmt == 'i')
		{
			char * field = va_arg(args, char *);
			int i = va_arg(args, int);
			cout << "\"" << field << "\": ";

			cout << i;
		}
		else if (*fmt == 'f')
		{
			char * field = va_arg(args, char *);
			double f = va_arg(args, double);
			cout << "\"" << field << "\": ";
			cout << f;
		}
		else if (*fmt == 'l')
		{
			char * field = va_arg(args, char *);
			pv f =  va_arg(args, pv);
			cout << "\"" << field << "\": ";
			cout << "[" << endl;
			for (int i = 0; i < (int)f->size(); ++i)
			{
				(*f)[i]->print();
				if (i < (int)f->size() - 1)
					cout << "," << endl;
				else
					cout << endl;
			}
			cout << endl;
			cout << "]" << endl;
		}
		++fmt;
		if (*fmt != '\0')
			cout << "," << endl;
	}
	cout << "}" << endl;
	if ((astname != NULL) && (astname[0] != '\0'))
		cout << "}" << endl;
	va_end(args);
}


void empty_astnode::print(){
	cout<<"\"empty\"";
}

void seq_astnode::print(){
	printAst("\0", "l", "seq", seq_nodes );
}

void assignS_astnode::print(){
	printAst("assignS", "aa", "left", lhs, "right", rhs);
}

void return_astnode::print(){
	printAst("", "a", "return", ret);
}

void proccall_astnode::print(){
	printAst("proccall", "al", "fname", fname, "params", params);
}

void printf_astnode::print(){
	identifier_astnode *temp = new identifier_astnode("printf", createtype("void"));
	printAst("proccall", "al", "fname", temp, "params", params);
}

void if_astnode::print(){
	printAst("if", "aaa", "cond", cond, "then", then_st, "else", else_st);
}

void while_astnode::print(){
	printAst("while", "aa","cond", cond, "stmt", loop_st);
}

void for_astnode::print(){
	printAst("for", "aaaa","init", init, "guard", guard, "step", step, "body", body);
}

void op_binary_astnode::print(){
	cout<<"label: "<<this->label<<endl;
	string temp = "\"" + op + "\"";
	const char *opc = temp.c_str();
	printAst("op_binary", "saa", "op", opc, "left", left, "right", right);
}

void op_unary_astnode::print(){
	string temp = "\"" + op + "\"";
	const char *opc = temp.c_str();
	printAst("op_unary", "sa", "op", opc, "child", operand);
}

void assignE_astnode::print(){
	printAst("assignE", "aa", "left", lhs, "right", rhs);
}

void funcall_astnode::print(){
	cout<<"is_right: "<<this->rchild<<endl;
	printAst("funcall", "al", "fname", fname, "params", params);
}

void intconst_astnode::print(){
	cout<<"label: "<<this->label<<endl;
	cout<<"is_right: "<<this->rchild<<endl;
	printAst("\0", "i","intconst",this->num);
}

void stringconst_astnode::print(){
	//const char *strc = str.c_str();
	cout<<"some string"<<endl;
	//printAst("\0", "s","stringconst","some string");
}

void identifier_astnode::print(){
	cout<<"label: "<<this->label<<endl;
	cout<<"is_right: "<<this->rchild<<endl;
	string temp = "\"" + id + "\"";
	const char *idc = temp.c_str();
	printAst("\0", "s","identifier",idc);
}

void arrayref_astnode::print(){
	printAst("arrayref", "aa", "array", base , "index", offset);
}

void member_astnode::print(){
	printAst("member", "aa", "struct", member_of, "field", member);
}

void arrow_astnode::print(){
	printAst("arrow", "aa", "pointer", pointer_to_member_of, "field", member);
}


op_binary_astnode * binary_comp_check(string optr, exp_astnode* lhs, exp_astnode* rhs){
	op_binary_astnode *ret;
	if(optr == "MULT" || optr == "DIV" ||optr == "PLUS" ||optr == "MINUS"){
		if(lhs->type.type == BASE && rhs->type.type == BASE){
			if(lhs->type.base_type == "int" && rhs->type.base_type=="int"){
				ret = new op_binary_astnode(optr+"_INT",lhs, rhs, createtype("int"));
			}
			else if(lhs->type.base_type == "float" && rhs->type.base_type=="float" ){
				ret = new op_binary_astnode(optr+"_FLOAT", lhs, rhs, createtype("float"));
			}
			else if(lhs->type.base_type == "int" && rhs->type.base_type=="float" ){
				op_unary_astnode *temp = new op_unary_astnode("TO_FLOAT", lhs);
				ret = new op_binary_astnode(optr+"_FLOAT",temp, rhs, createtype("float"));
			}
			else if(lhs->type.base_type == "float" && rhs->type.base_type=="int" ){
				op_unary_astnode *temp = new op_unary_astnode("TO_FLOAT", rhs);
				ret = new op_binary_astnode(optr+"_FLOAT",lhs, temp, createtype("float"));
			}
		}
		else if(optr == "PLUS"){
			if(lhs->type.type != BASE && rhs->type.type == BASE && rhs->type.base_type=="int"  ){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, lhs->type);
			}
			else if(rhs->type.type != BASE && lhs->type.type == BASE &&lhs->type.base_type=="int" ){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, rhs->type);
			}
		}
		else if(optr == "MINUS"){
			if(lhs->type.type != BASE && rhs->type.type == BASE && rhs->type.base_type=="int"  ){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, lhs->type);
			}
			else if(lhs->type.base->type_str() == rhs->type.base->type_str()){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, lhs->type);
			}
		}
		else{
			return NULL;
		}
	}
	
	else if(optr=="LT_OP"||optr=="GT_OP"||optr=="GE_OP"||optr=="LE_OP" || optr=="EQ_OP"||optr=="NE_OP"){
		if(lhs->type.type == BASE && rhs->type.type == BASE){
			if(lhs->type.base_type == "int" && rhs->type.base_type=="int"){
				ret = new op_binary_astnode(optr+"_INT",lhs, rhs, createtype("int"));
			}
			else if(lhs->type.base_type == "float" && rhs->type.base_type=="float" ){
				ret = new op_binary_astnode(optr+"_FLOAT", lhs, rhs, createtype("int"));
			}
			else if(lhs->type.base_type == "int" && rhs->type.base_type=="float" ){
				op_unary_astnode *temp = new op_unary_astnode("TO_FLOAT", lhs);
				ret = new op_binary_astnode(optr+"_FLOAT",temp, rhs, createtype("int"));
			}
			else if(lhs->type.base_type == "float" && rhs->type.base_type=="int" ){
				op_unary_astnode *temp = new op_unary_astnode("TO_FLOAT", rhs);
				ret = new op_binary_astnode(optr+"_FLOAT",lhs, temp, createtype("int"));
			}
			else{
				return NULL;
			}
		}
		else if(optr=="LT_OP"||optr=="GT_OP"||optr=="GE_OP"||optr=="LE_OP"){
			if(lhs->type.base->type_str() == rhs->type.base->type_str()){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, createtype("int"));
			}
		}
		else if(optr=="EQ_OP"||optr=="NE_OP"){
			if(lhs->type.type!=BASE && rhs->type.type!=BASE && lhs->type.base->type_str() == rhs->type.base->type_str()){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, createtype("int"));
			}
			else if((lhs->type.type!=BASE && rhs->ast_node==intconst_astNode && rhs->is_null) || (rhs->type.type!=BASE && lhs->ast_node==intconst_astNode && lhs->is_null)){
				ret = new op_binary_astnode(optr+"_INT", lhs, rhs, createtype("int"));
			}
		}
		
	}
	else if(optr=="AND_OP"||optr=="OR_OP"){
		if(!lhs->type.is_struct && !rhs->type.is_struct){
			ret = new op_binary_astnode(optr, lhs, rhs, createtype("int"));
		}
	}
	else{
		return NULL;
	}

	if(lhs->ast_node==identifier_astNode||lhs->ast_node==intconst_astNode ||lhs->ast_node==funcall_astNode||lhs->ast_node==member_astNode){
		lhs->rchild=false;
		lhs->label = 1;
	}
	if(rhs->ast_node==identifier_astNode||rhs->ast_node==intconst_astNode ||rhs->ast_node==funcall_astNode||rhs->ast_node==member_astNode){
		rhs->rchild=true;
		rhs->label = 0;
	}
	if(lhs->label == rhs->label) ret->label = lhs->label+1;
	else ret->label = max(lhs->label, rhs->label);
	return ret;

}


op_unary_astnode * unary_comp_check(string optr, exp_astnode* rhs){
	op_unary_astnode *ret;
	if(optr == "UMINUS"){
		if(rhs->type.type_str() != "int" && rhs->type.type_str() != "float" ) return NULL;
		ret = new op_unary_astnode(optr,rhs);
	}
	else if(optr == "NOT"){
		if(rhs->type.type == BASE && rhs->type.type_str() != "int" && rhs->type.type_str() != "float" ) return NULL;
		ret = new op_unary_astnode(optr,rhs);
	}
	else if(optr == "ADDRESS"){
		if(!rhs->lval) return NULL;
		ret = new op_unary_astnode(optr,rhs);
	}
	else if(optr == "DEREF"){
		if(rhs->type.type==BASE || rhs->type.base->type_str() == "void") return NULL;
		ret = new op_unary_astnode(optr,rhs);
	}
	else{	return NULL;}

	if(rhs->ast_node==identifier_astNode||rhs->ast_node==intconst_astNode ||rhs->ast_node==funcall_astNode||rhs->ast_node==member_astNode){
		ret->label = 1;
	}
	else{
		ret->label = rhs->label;
	}
	return ret;
	
}

exp_astnode *assign_comp_check(datatype lhs, exp_astnode* rhs){
	exp_astnode *ret;
	if(lhs.type == BASE && rhs->type.type == BASE){
		if(lhs.base_type == rhs->type.base_type){
			ret = rhs;
		}
		else if(lhs.base_type == "int" && rhs->type.base_type=="float" ){
			ret= new op_unary_astnode("TO_INT", rhs);
		}
		else if(lhs.base_type == "float" && rhs->type.base_type=="int" ){
			ret = new op_unary_astnode("TO_FLOAT", rhs);
		}
		else{
			ret = NULL;
		}
	}
	else if(lhs.type_str() == "void*" && rhs->type.type !=BASE){
		ret = rhs;
	}
	else if(lhs.type == POINTER && rhs->type.type_str() == "void*" ){
		ret = rhs;
	}
	//refine this
	else if((lhs.type==POINTER && rhs->type.type!=BASE)&& lhs.base->type_str() == rhs->type.base->type_str()){
		ret=rhs;
	}
	else if(lhs.type==POINTER && rhs->ast_node==intconst_astNode && rhs->is_null){
		ret = rhs;
	}
	else{
			ret = NULL;
	}
	return ret;
}