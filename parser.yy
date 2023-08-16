%skeleton "lalr1.cc"
%require  "3.0.1"
%locations

%defines 
%define api.namespace {IPL}
%define api.parser.class {Parser}

%define parse.trace

%code requires{
       #include "ast.hh"
       #include "type.hh"
       #include "symbtab.hh"
       #include "location.hh"
   namespace IPL {
      class Scanner;
   }
     
  // # ifndef YY_NULLPTR
  // #  if defined __cplusplus && 201103L <= __cplusplus
  // #   define YY_NULLPTR nullptr
  // #  else
  // #   define YY_NULLPTR 0
  // #  endif
  // # endif

}

%printer { std::cerr << $$; } STRUCT
%printer { std::cerr << $$; } MAIN
%printer { std::cerr << $$; } PRINTF
%printer { std::cerr << $$; } IDENTIFIER
%printer { std::cerr << $$; } VOID
%printer { std::cerr << $$; } INT
%printer { std::cerr << $$; } FLOAT
%printer { std::cerr << $$; } OR_OP
%printer { std::cerr << $$; } AND_OP
%printer { std::cerr << $$; } EQ_OP
%printer { std::cerr << $$; } NE_OP
%printer { std::cerr << $$; } LE_OP
%printer { std::cerr << $$; } GE_OP
%printer { std::cerr << $$; } PTR_OP
%printer { std::cerr << $$; } INC_OP
%printer { std::cerr << $$; } INT_CONSTANT
%printer { std::cerr << $$; } FLOAT_CONSTANT
%printer { std::cerr << $$; } STRING_LITERAL
%printer { std::cerr << $$; } RETURN
%printer { std::cerr << $$; } IF
%printer { std::cerr << $$; } ELSE
%printer { std::cerr << $$; } WHILE
%printer { std::cerr << $$; } FOR
%printer { std::cerr << $$; } OTHERS




%parse-param { Scanner  &scanner  }
%locations
%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <string>
   
   #include "scanner.hh"
   int nodeCount = 0;
   g_symbtab_class gst;
   int goffset=0;
   symbtab_class* local_st = new symbtab_class();
   int loffset =0;
   bool is_struct=false;
   datatype curr_ret_type;
   int string_cnt=0;
   int label_cnt =1;    
   int return_offset;
   map<string, abstract_astnode *> ast;

#undef yylex
#define yylex IPL::Parser::scanner.yylex

}




%define api.value.type variant
%define parse.assert

%start program



%token '\n'
%token <std::string> STRUCT OTHERS
%token <std::string> MAIN
%token <std::string> PRINTF
%token <std::string> IDENTIFIER 
%token <std::string> VOID 
%token <std::string> RETURN 
%token <std::string> INT FLOAT
%token <std::string> OR_OP AND_OP EQ_OP NE_OP LE_OP GE_OP
%token <std::string> PTR_OP INC_OP 
%token <std::string> STRING_LITERAL
%token <std::string> INT_CONSTANT FLOAT_CONSTANT
%token <std::string> IF ELSE WHILE FOR 

%token  '(' ')' ',' '[' ']' '*' '{' '}' ';' '=' '<' '>' '+' '-' '/' '.'
%token  '!' '&'

%nterm <int> struct_specifier declaration_list declaration 
%nterm <datatype> type_specifier
%nterm <parameter*>parameter_declaration
%nterm <vector<parameter*>>parameter_list
%nterm <fun_declarator_class*> fun_declarator
%nterm <declarator_class *> declarator
%nterm <declarator_class *> declarator_arr
%nterm <vector<declarator_class *>> declarator_list
%nterm <vector <abstract_astnode *> > translation_unit 
%nterm <abstract_astnode *> function_definition compound_statement
%nterm <vector <statement_astnode *> >statement_list
%nterm <vector <exp_astnode *> >expression_list
%nterm <statement_astnode *> statement iteration_statement 
%nterm <proccall_astnode *> procedure_call
%nterm <printf_astnode *> printf_call
%nterm <if_astnode *> selection_statement
%nterm <assignE_astnode *> assignment_expression
%nterm <exp_astnode *> primary_expression postfix_expression expression logical_and_expression equality_expression relational_expression additive_expression unary_expression multiplicative_expression
%nterm <std::string> unary_operator
%%

program:
        main_definition
       | translation_unit main_definition

translation_unit:
          struct_specifier{}
       |  function_definition{}
       | translation_unit function_definition{}
       | translation_unit struct_specifier

struct_specifier:
          STRUCT IDENTIFIER {
                 if(gst.Entries.find("struct "+ $2)!= gst.Entries.end()) error(@$, "multiple definitions of struct: struct " + $2);

                 is_struct = true;
                 struct_symbol temp("struct "+$2, loffset, local_st);
                 gst.Entries.insert({"struct "+$2, temp});
                 local_st = gst.Entries.find("struct "+$2)->second.symbtab; 
                 } '{' declaration_list '}' ';' 
       {
              gst.Entries.find("struct "+$2)->second.size = loffset;                   
              local_st = new symbtab_class();
              loffset =0;
       }

fun_declarator:
         IDENTIFIER '(' parameter_list ')'
       {
              $$ = new fun_declarator_class($1, $3);
       }
       | IDENTIFIER '(' ')'
       {
              vector<parameter*> temp;
              $$ = new fun_declarator_class($1, temp);
       }

function_definition:
          type_specifier {} fun_declarator {
                 if(gst.Entries.find($3->id)!= gst.Entries.end()) error(@$, "multiple definitions of function: " + $3->id);

                 is_struct=false;
                 int p_offset = 8;
                 
                 for(auto it = $3->params.rbegin(); it!=$3->params.rend(); ++it){
                     if(local_st->symbols.find((*it)->id)!= local_st->symbols.end()){
                            error(@$, "multiple definitions of parameter: " + (*it)->id);
                     }
                     (*it)->offset = p_offset;
                     p_offset+=(*it)->type.size;
                     local_st->symbols.insert({(*it)->id, **it});
                     local_st->param_types.push_back((*it)->type);
                     }
                local_st->has_rval=false;
                curr_ret_type = $1;
                fun_symbol temp($1, $3->id, local_st);
                gst.Entries.insert({$3->id, temp});
                gst.Entries.find($3->id)->second.size = p_offset-8;  
                return_offset = p_offset; 
                local_st = gst.Entries.find($3->id)->second.symbtab; 
                 } compound_statement
       { 
          
         ast.insert({$3->id, $5});
         gst.Entries.find($3->id)->second.loc_size = -loffset;
       
         local_st = new symbtab_class();
         loffset =0;
       }       

main_definition:
         INT MAIN '(' ')' {
              is_struct=false;
                            
              curr_ret_type = createtype("int");
              fun_symbol temp(curr_ret_type, "main", local_st);
              gst.Entries.insert({"main", temp});
              local_st = gst.Entries.find("main")->second.symbtab; 
         } compound_statement {
                ast.insert({"main", $6});
                gst.Entries.find("main")->second.size = -loffset;    

         local_st = new symbtab_class();
         loffset =0;
         }
         

type_specifier:
         VOID { $$ = createtype("void"); }
       | INT { $$ = createtype("int");}
       | STRUCT IDENTIFIER
       {
              string id = "struct " + $2;
              auto it = gst.Entries.find(id);
              if( it != gst.Entries.end() && it->second.varfun == "struct"){
                     $$ = createtype(id, it->second.size);
              }
              else{
                     error(@$, "struct does not exist : " + id);
              }
       }

declaration_list:
          declaration
          | 
          declaration_list declaration

declaration:
          type_specifier declarator_list ';'
       {
              for(auto it = $2.begin(); it!=$2.end(); ++it){
                     if($1.base_type=="void" && ((*it)->stack.size()==0 ||(*it)->stack.back()!="*")) error(@$, "can not declare type \"void\"");
                     if(local_st->symbols.find((*it)->id)!= local_st->symbols.end()){
                            error(@$, "multiple definitions of" + (*it)->id);
                     }
                     symbol *temp;
                     if (is_struct){
                            temp = new variable($1, (*it) , loffset);
                            loffset+=temp->type.size;
                     }
                     else{
                            temp = new variable($1, (*it) , 0);
                            temp->offset = loffset - temp->type.size;
                            loffset-=temp->type.size;
                     }
                     local_st->symbols.insert({(*it)->id, *temp});
              }
       }

declarator_list: 
          declarator
       {
         vector<declarator_class *>temp;
         $$ = temp;
         $$.push_back($1);
       }
       |  declarator_list ',' declarator
       {
         $$ = $1;
         $$.push_back($3);
       }    

declarator:
          declarator_arr
       {
          $$ = $1;
       }
       | '*' declarator
       {
           $$ = $2;
           $$->stack.push_back("*");
       }

declarator_arr:
         IDENTIFIER
       {
              $$ = new declarator_class();
              $$->id = $1;
       }
       | declarator_arr '[' INT_CONSTANT ']' 
       {
              $$ = $1;
              $$->stack.push_back($3);
       }

parameter_list:
         parameter_declaration
       {
       	$$.push_back($1);
       }
       | parameter_list ',' parameter_declaration
       {
       	$$ = $1;
              $$.push_back($3);
       }

parameter_declaration:
         type_specifier declarator
       {
       	$$ = new parameter($1, $2);
       }

compound_statement:
         '{' '}'
       {
              vector<statement_astnode*> temp;
              $$ = new seq_astnode(temp);
       }
       | '{' statement_list '}'
       {
       	$$ = new seq_astnode($2);
       }
       | '{' declaration_list statement_list '}' 
       {
              $$ = new seq_astnode($3);
       }

statement_list:
         statement
       {
              vector<statement_astnode*> temp;
              temp.push_back($1);
              $$ = temp;
       }
       | statement_list statement
       {
              vector<statement_astnode*> temp;
              temp = $1;
              temp.push_back($2);
              $$ = temp;
          
       }

statement:
          ';'
       {
           $$ = new empty_astnode();
           $$->next_label = label_cnt++;
       }
       | '{' statement_list '}' 
       {
          $$ = new seq_astnode($2);
          $$->next_label = label_cnt++;
       }
       | selection_statement 
       {
              $$ = $1;
              $$->next_label = label_cnt++;
       }
       | iteration_statement 
       {
              $$ = $1;
              $$->next_label = label_cnt++;
       }
       | assignment_expression ';'
       {
              $$ = new assignS_astnode($1->lhs, $1->rhs);
              $$->next_label = label_cnt++;
       }
       | procedure_call 
       {
              $$ = $1;
              $$->next_label = label_cnt++;
       }
       | printf_call
       {
              $$ = $1;
              $$->next_label = label_cnt++;
       }
       | RETURN expression ';'   
       {
              if(assign_comp_check(curr_ret_type, $2)==NULL) error(@$,"return type incompatible with given return expression");
              $$ = new return_astnode($2);
              $$->next_label = label_cnt++;
              $$->return_offset = return_offset;
              local_st->has_rval = true;
       }

assignment_expression: 
          unary_expression '=' expression
       {
              if(!$1->lval) error(@$, "lvalue of lhs does not exsit");
              
              exp_astnode * temp;
              temp = assign_comp_check($1->type, $3);
              if(temp==NULL) error(@$,"lhs type incompatible with rhs type");
              $$ = new assignE_astnode($1, temp);    
       }

expression:
          logical_and_expression
          {
              $$=$1;
          }
          | expression OR_OP logical_and_expression
          {
              $$ = binary_comp_check("OR_OP", $1, $3);        
              if($$ == NULL) error(@$, "incompatible operator OR_OP");
          }

logical_and_expression:
          equality_expression
          {
              $$=$1;
          }
          | logical_and_expression AND_OP equality_expression
          {
              $$ = binary_comp_check("AND_OP", $1, $3);
              if($$ == NULL) error(@$, "incompatible operator AND_OP");
          }

equality_expression: 
          relational_expression
          {
              $$=$1;
          }
          | equality_expression EQ_OP relational_expression
          {
              string optr = "EQ_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator " + optr);
          }
          | equality_expression NE_OP relational_expression
          {
              string optr = "NE_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator " + optr);
          }

relational_expression: 
          additive_expression
          {
              $$=$1;
          }
          | relational_expression '<' additive_expression
          {
              string optr = "LT_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr );
          }
          | relational_expression '>' additive_expression
          {
              string optr = "GT_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr);
          }
          | relational_expression LE_OP additive_expression
          {
              string optr = "LE_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr);
          }
          | relational_expression GE_OP additive_expression
          {
              string optr = "GE_OP";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr);
          }

additive_expression: 
          multiplicative_expression
          {
              $$=$1;
          }
          | additive_expression '+' multiplicative_expression
          {
              $$ = binary_comp_check("PLUS",$1, $3);
              if($$ == NULL) error(@$, "incompatible operator: PLUS");
          }
          | additive_expression '-' multiplicative_expression
          {
              $$ = binary_comp_check("MINUS",$1, $3);
              if($$ == NULL) error(@$, "incompatible operator: MINUS");
          }

multiplicative_expression: 
          unary_expression
          {
              $$ = $1;
          }
          | multiplicative_expression '*' unary_expression
          {
              string optr = "MULT";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr);
          }
          | multiplicative_expression '/' unary_expression
          {
              string optr = "DIV";
              $$ = binary_comp_check(optr,$1, $3);
              if($$ == NULL) error(@$, "incompatible operator" + optr);              
          }

unary_expression:
          postfix_expression
       {    
              $$=$1;
       }
       | unary_operator unary_expression
       {
              $$ = unary_comp_check($1, $2);
              if($$ == NULL) error(@$, "incompatible unary operator");
              if($2->label ==0)$$->label=1;
              else $$->label = $2->label;
       }

postfix_expression: 
          primary_expression
       {
              $$ = $1;
       }
       | postfix_expression '[' expression ']'
       {
              if($1->type.type == BASE || $1->type.base->type_str() == "void" ) error(@$, "exp: " + $1->type.type_str() + "expression can not be array refrenced");
              if($3->type.type != BASE || $3->type.base_type != "int") error(@$,"array can only be refrenced with integer");
              $$ = new arrayref_astnode($1, $3);
              
              $$->address=$1->address;
              if($3->label==0) $$->label=1;
              else $$->label = $3->label;
       }
       |IDENTIFIER '(' ')'
       {
              auto entry = gst.Entries.find($1);
              if( entry != gst.Entries.end() && entry->second.symbtab->param_types.size() == 0){
                     identifier_astnode *temp = new identifier_astnode($1, entry->second.ret_type);
                     vector<exp_astnode*>temp2;
                     $$ = new funcall_astnode(temp, temp2, entry->second.ret_type);
                     $$->param_size = 0;
                     $$->address="(%esp)";
              }
              else{
                     error(@$, "no matching function call found");
              }
              
       }
       | IDENTIFIER '(' expression_list ')'
       {
              auto entry = gst.Entries.find($1);
              if( entry != gst.Entries.end()&& entry->second.symbtab->param_types.size() == $3.size()){
                     //done half:check for compatibility of parameter
                     vector<exp_astnode *>passed_params;
                     vector<datatype> param_types = entry->second.symbtab->param_types;
                     int len = $3.size();
                     for(int i=0;i<len;i++ ){
                            exp_astnode *temp = assign_comp_check(param_types[len-i-1], $3[i]);
                            if(temp ==NULL) error(@$, "incompatible datatype: "+ param_types[len-i-1].type_str()+", " + $3[i]->type.type_str() );
                            passed_params.push_back(temp);
                     }
                     identifier_astnode *temp = new identifier_astnode($1, gst.Entries.find($1)->second.ret_type);
                     $$ = new funcall_astnode(temp, passed_params, entry->second.ret_type);
                     $$->param_size = entry->second.size;
                     $$->address="(%esp)";
              }
              else if($1 == "printf" || $1 == "scanf"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("void"));
                     $$ = new funcall_astnode(temp, $3, createtype("void"));
              }
              else if($1 == "mod"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("int"));
                     $$ = new funcall_astnode(temp, $3, createtype("int"));
              }
              else{
                     error(@$, "no matching function call found");
              }
              
       }
       | postfix_expression '.' IDENTIFIER
       {      
              if($1->type.is_struct == false) error(@$, "expression is not a struct");
              
              symbtab_class *struct_symbtab = gst.Entries.find($1->type.base_type)->second.symbtab;
              if(struct_symbtab->symbols.find($3) == struct_symbtab->symbols.end()) error(@$, "identifier is not a local variable of the struct");
              int internal_offset = struct_symbtab->symbols.find($3)->second.offset;
              identifier_astnode *temp = new identifier_astnode($3, struct_symbtab->symbols.find($3)->second.type);  
              $$ = new member_astnode($1, temp);
              int struct_offset = stoi($1->address.substr(0, $1->address.length()-6 ));
              $$->address=to_string(struct_offset+internal_offset)+"(%ebp)";
       }
       | postfix_expression PTR_OP IDENTIFIER
       {
              

              if($1->type.type == BASE  || $1->type.base->is_struct == false) error(@$, "expression is not a pointer struct");

              symbtab_class *struct_symbtab = gst.Entries.find($1->type.base->base_type)->second.symbtab;
              if(struct_symbtab->symbols.find($3) == struct_symbtab->symbols.end()) error(@$, "identifier is not a local variable of the struct");
              int internal_offset = struct_symbtab->symbols.find($3)->second.offset;
              identifier_astnode *temp = new identifier_astnode($3, struct_symbtab->symbols.find($3)->second.type);  
              $$ = new arrow_astnode($1, temp);
              $$->address=to_string(internal_offset);
       }
       | postfix_expression INC_OP
       {
             if(!$1->lval) error(@$, "expression does not have lvalue");
             if($1->type.type==BASE && $1->type.base_type!="int" && $1->type.base_type!="float") error(@$, "the expression is neither a pointer nor an int nor a float");
             $$ = new op_unary_astnode("PP", $1);
       }
       
primary_expression:
         INT_CONSTANT
       {  
              $$ = new intconst_astnode(stoi($1));
              $$->address = "$"+$1;
       }
       | IDENTIFIER
       {
          datatype type;
          int address;
          if(local_st->symbols.find($1) != local_st->symbols.end()){
              type = local_st->symbols.find($1)->second.type;
              address = local_st->symbols.find($1)->second.offset;
          } 
          else error(@$,"identifier not a local variable");
       
          $$ = new identifier_astnode($1, type);
          $$->address = to_string(address)+"(%ebp)";
       }
       | '(' expression ')'
       {
       	$$ = $2;   
       }

unary_operator:
         '-' { $$ = "UMINUS"; }
       | '!' { $$ = "NOT"; }
       | '&' { $$ = "ADDRESS"; }
       | '*' { $$ = "DEREF"; }

selection_statement:
          IF '(' expression ')' statement ELSE statement
       {
          $$ = new if_astnode($3,$5,$7);
          $$->else_label = $5->next_label+1;
          $$->next_label = $7->next_label+1;
          label_cnt++;
          label_cnt++;
       }

iteration_statement:
          WHILE '(' expression ')' statement
       {
          $$ = new while_astnode($3,$5);
          $$->repeat_label = $5->next_label;
          $$->next_label = $5->next_label+1;
          label_cnt++;
       }
       |
       FOR '(' assignment_expression ';' expression ';' assignment_expression ')' statement
       {
           if($5->type.type != BASE || $5->type.base_type=="int" || $5->type.base_type=="float")  $$ = new for_astnode($3,$5,$7, $9);
           else error(@$, "condition expression is not valid type");
           $$->repeat_label = $9->next_label;
           $$->next_label = $9->next_label+1;
           label_cnt++;
       }

expression_list: 
          {}expression
       {
              vector<exp_astnode*> temp;
              temp.push_back($2);
              $$ = temp;
       }
       |  expression_list ',' {
              
              }expression
       {
              vector<exp_astnode*> temp;
              temp = $1;
              temp.push_back($4);
              $$ = temp;
       }

procedure_call:
          IDENTIFIER '(' ')' ';'
          {
              auto it = gst.Entries.find($1);
              if( it != gst.Entries.end() && it->second.symbtab->param_types.size() ==0){
                     identifier_astnode *temp = new identifier_astnode($1, it->second.ret_type);
                     vector<exp_astnode*>temp2;
                     $$ = new proccall_astnode(temp, temp2);
                     $$->size = 0;
              }
              else if($1 == "printf" || $1 == "scanf"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("void"));
                     vector<exp_astnode*>temp2;
                     $$ = new proccall_astnode(temp, temp2);
              }
              else if($1 == "mod"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("int"));
                     vector<exp_astnode*>temp2;
                     $$ = new proccall_astnode(temp, temp2);
              }
              else{
                     error(@$, "no matching function call found");
              }
          }
          | IDENTIFIER '(' expression_list ')' ';'
          {
              auto entry = gst.Entries.find($1);
              if( entry != gst.Entries.end()&& entry->second.symbtab->param_types.size() == $3.size()){
                     //done half:check for compatibility of parameter
                     vector<exp_astnode *>passed_params;
                     vector<datatype> param_types = entry->second.symbtab->param_types;
                     int len = $3.size();
                     for(int i=0;i<len;i++ ){
                            exp_astnode *temp = assign_comp_check(param_types[len-i-1], $3[i]);
                            if(temp ==NULL) error(@$, "incompatible datatype: "+ param_types[len-i-1].type_str()+", " + $3[i]->type.type_str() );
                            passed_params.push_back(temp);
                     }
                     identifier_astnode *temp = new identifier_astnode($1, gst.Entries.find($1)->second.ret_type);
                     $$ = new proccall_astnode(temp, passed_params);
                     $$->size = entry->second.size;
              }
              else if($1 == "printf" || $1 == "scanf"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("void"));
                     $$ = new proccall_astnode(temp, $3);
              }
              else if($1 == "mod"){
                     identifier_astnode *temp = new identifier_astnode($1, createtype("int"));
                     $$ = new proccall_astnode(temp, $3);
              }
              else{
                     error(@$, "no matching function call found");
              }
          }

printf_call:
         PRINTF '(' STRING_LITERAL ')' ';'
         {
              local_st->strs[string_cnt] =$3;
              //identifier_astnode *temp = new identifier_astnode("printf", createtype("void"));
              vector<exp_astnode*>temp2;
              $$ = new printf_astnode("$.LC"+to_string(string_cnt), temp2);
              string_cnt+=1;  
         }
       |  PRINTF '(' STRING_LITERAL ',' expression_list ')' ';'
       {
              local_st->strs[string_cnt] =$3;
              //identifier_astnode *temp = new identifier_astnode("printf", createtype("void"));
              $$ = new printf_astnode("$.LC"+to_string(string_cnt), $5);
              string_cnt+=1;
       }

%%


void IPL::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cout << "Error at line " << l.begin.line <<": "<< err_message << "\n";
   exit(1);
}
