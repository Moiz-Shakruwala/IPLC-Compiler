
#include "scanner.hh"
#include "parser.tab.hh"
#include <fstream>
using namespace std;

extern g_symbtab_class gst;
g_symbtab_class gstfun, gststruct; 
string filename;
extern std::map<string,abstract_astnode*> ast;

int main(int argc, char **argv)
{
	using namespace std;
	fstream in_file, out_file;
	

	in_file.open(argv[1], ios::in);

	IPL::Scanner scanner(in_file);

	IPL::Parser parser(scanner);

#ifdef YYDEBUG
	parser.set_debug_level(1);
#endif
parser.parse();
// create gstfun with function entries only

for (const auto &entry : gst.Entries)
{
	if (entry.second.varfun == "fun")
	gstfun.Entries.insert({entry.first, entry.second});
}
// create gststruct with struct entries only

for (const auto &entry : gst.Entries)
{
	if (entry.second.varfun == "struct")
	gststruct.Entries.insert({entry.first, entry.second});
}
// start the JSON printing

cout<<"\t.file\t\""<<argv[1]<<"\"\n";

for (auto it = gstfun.Entries.begin(); it != gstfun.Entries.end(); ++it)

{

	//cout<<"\t.text\n";
	if(!it->second.symbtab->strs.empty())cout<<"\t.section\t.rodata\n";
	for(auto str_i=it->second.symbtab->strs.begin(); str_i!=it->second.symbtab->strs.end();str_i++){
		cout<<".LC"<<str_i->first<<":\n";
		cout<<"\t.string "<<str_i->second<<endl;
	}
	//ast[it->first]->print();
	cout<<"\t.text\n";
	cout << "\t.globl\t"<<it->first<<endl;
	cout << "\t.type\t"<<it->first <<", @function"<<endl;
	cout << it->first << ": " << endl;
	cout << "\tpushl\t%ebp\n";
	cout << "\tmovl\t%esp, %ebp\n";
	cout << "\tsubl\t$"<<it->second.loc_size<<", %esp\n";
	ast[it->first]->printcode();
	if(!it->second.symbtab->has_rval){
		cout << "\tleave\n";
		cout << "\tret\n";
	}
}
// 	cout << "]" << endl;
// 	cout << "}" << endl;

// 	fclose(stdout);
}

// cout << "{\"globalST\": " << endl;
// gst.printgst();
// cout << "," << endl;

// cout << "  \"structs\": [" << endl;
// for (auto it = gststruct.Entries.begin(); it != gststruct.Entries.end(); ++it)

// {   cout << "{" << endl;
// 	cout << "\"name\": " << "\"" << it->first << "\"," << endl;
// 	cout << "\"localST\": " << endl;
// 	it->second.symbtab->print();
// 	cout << "}" << endl;
// 	if (next(it,1) != gststruct.Entries.end()) 
// 	cout << "," << endl;
// }
// cout << "]," << endl;
// cout << "  \"functions\": [" << endl;

