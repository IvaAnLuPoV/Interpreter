#include <stack>
#include <vector>
#include <limits>
#include <cassert>
#include <fstream>
#include <iostream>
#include "Trie.cpp"
#include "Token.cpp"
#include "Commands.cpp"
#include "Variable.cpp"
#include "Operators.cpp"
#include "SyntaxTree.cpp"

int read_number(std::string::iterator& it, const std::string::iterator& last){
	int ret = 0;
	while(it != last && '0' <= *it && *it <= '9'){
		ret = ret * 10 + (*it - '0');
		++ it;
	}
	-- it;
	return ret;
}

bool check_symbol(char x){
	return ('a' <= x && x <= 'z') || ('A' <= x && x <= 'Z') || x == '_';
}
std::vector<Token*>* tokenize(std::string::iterator first, std::string::iterator last, Trie<Variable> &variables);

int* read_variable(std::string::iterator &it, const std::string::iterator& last, Trie<Variable> &variables){
	assert(check_symbol(*it));
	std::string str;
	std::vector<std::size_t> vec = {0};
	while(it != last && check_symbol(*it) || ('0' <= *it && *it <= '9')){
		str += *it; ++ it;
	}
	while(it != last && *it == '['){
		auto a = it + 1;
		auto b = it;
		int bal = 0;
		while(true){
			if(*b == '[') ++ bal;
			if(*b == ']') -- bal;
			if(bal == 0) break;
			if(b == last) throw std::logic_error("Invalid expression.");
			++ b;
		}
		vec.push_back(SyntaxTree(*tokenize(a, b, variables)).evaluate());
		it = b + 1;
	}
	-- it;
	return (*variables.query(str))[vec];
}

int* read_variable(const std::string::iterator &first, const std::string::iterator& last, Trie<Variable> &variables){
	std::string::iterator it = first;
	assert(check_symbol(*it));
	std::string str;
	std::vector<std::size_t> vec = {0};
	while(it != last && check_symbol(*it) || ('0' <= *it && *it <= '9')){
		str += *it; ++ it;
	}
	while(it != last && *it == '['){
		auto a = it + 1;
		auto b = it; 
		while(*b != ']'){
			if(b == last) throw std::logic_error("Invalid expression.");
			++ b;
		}
		vec.push_back(SyntaxTree(*tokenize(a, b, variables)).evaluate());
		it = b + 1;
	}
	return (*variables.query(str))[vec];
}

std::vector<Token*>* tokenize(std::string::iterator first, std::string::iterator last, Trie<Variable> &variables){
	std::vector<Token*> token_sequence;
	std::stack<Token*> operators;
	for(auto it = first; it != last; ++ it){
		if(*it == ' ') continue;
		if('0' <= *it && *it <= '9'){
			token_sequence.push_back(new Token(read_number(it, last)));
		}
		else if(check_symbol(*it)){
			token_sequence.push_back(new Token(read_variable(it, last, variables)));
		}
		else{
			if(*it == '('){
				operators.push(new Token("("));
			}
			else if(*it == ')'){
				while(!operators.empty() && !(operators.top() -> symbol() == "(")){
					token_sequence.push_back(operators.top());
					operators.pop();
				}
				operators.pop();
			}
			else{
				std::string cur(1, *it);
				auto nit = it; ++ nit;
				if(nit != last){
					cur += *nit;
					if(OperatorSelector::getSelector() -> selectOperator(cur) == nullptr){
						cur.pop_back();
					}
					else ++ it;
				}
				auto op = OperatorSelector::getSelector() -> selectOperator(cur);
				if(op == nullptr){
					throw std::logic_error("Unknown operator.");
				}
				while(!operators.empty() && operators.top() -> priority() >= op -> priority() && op -> associativity() == LR){
					token_sequence.push_back(operators.top());
					operators.pop();
				}
				operators.push(new Token(op));
			}
		}
	}
	while(!operators.empty()){
		token_sequence.push_back(operators.top());
		operators.pop();
	}
	return new std::vector<Token*>(token_sequence);
}

void print_tokens(std::vector<Token*>* token_sequence){
	for(auto x : *token_sequence){
		std::cout << x -> print() << " ";
	} std::cout << std::endl;
}

std::pair<std::string, std::vector<std::size_t>> init_variable(std::string::iterator first, std::string::iterator last){
	assert(check_symbol(*first));
	auto it = first;
	std::string str;
	std::vector<std::size_t> vec = {1};
	while(it != last && check_symbol(*it) || ('0' <= *it && *it <= '9')){
		str += *it; ++ it;
	}
	while(it != last){
		assert(*it == '[');
		++ it;
		vec.push_back(read_number(it, last));
		it += 2;
	}
	return std::make_pair(str, vec);
}

bool check_prefix(std::string::iterator first, std::string::iterator last, const std::string &prefix){
	for(int i = 0; i < prefix.size(); i ++){
		if(first + i == last) return false;
		if(*(first + i) != prefix[i]) return false;
	}
	return true;
}

void print_program(std::vector<Command*> program){
	int idx = 0;
	for(auto line : program){
		std::cout << idx << ": "; line -> print(); ++ idx;
	}
}

void read_line(std::istream* &input_stream, Trie<int> &labels, Trie<Variable> &variables, std::vector<Command*> &program, std::stack<std::pair<Command*, int>> &multiline_commands){
	std::string current_line;
	getline(*input_stream, current_line);
	if(current_line.empty()) return;
	if(check_prefix(current_line.begin(), current_line.end(), "LABEL")){
		auto [label, _] = init_variable(current_line.begin() + 6, current_line.end());
		labels.insert(label, new int(program.size()));
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "GOTO")){
		auto [label, _] = init_variable(current_line.begin() + 5, current_line.end());
		program.push_back(new GotoCommand(GOTO, current_line, label));
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "LET")){
		auto [variable, dimensions] = init_variable(current_line.begin() + 4, current_line.end());
		variables.insert(variable, new Variable(dimensions));
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "PRINT")){
		std::string expression = current_line.substr(6, current_line.size() - 6);
		program.push_back(new PrintCommand(PRINT, current_line, new std::string(expression)));
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "READ")){
		std::string expression = current_line.substr(5, current_line.size() - 5);
		program.push_back(new ReadCommand(READ, current_line, new std::string(expression)));
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "IF")){
		std::string expression = current_line.substr(3, current_line.size() - 3);
		auto temp = new IfCommand(IF, current_line, new std::string(expression));
		program.push_back(temp);
		multiline_commands.push({temp, program.size()});
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "ELSE")){
		if(multiline_commands.empty() || multiline_commands.top().first -> command_type() != IF || dynamic_cast<const IfCommand*>(multiline_commands.top().first) -> get_else() != -1){
			throw std::logic_error("ELSE without a preceeding IF.");
		}
		auto temp = new ElseCommand(ELSE, "");
		program.push_back(temp);
		dynamic_cast<IfCommand*>(multiline_commands.top().first) -> set_else(program.size() - 1);
		multiline_commands.push({temp, program.size()});
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "ENDIF")){
		if(multiline_commands.empty() || (multiline_commands.top().first -> command_type() != IF && multiline_commands.top().first -> command_type() != ELSE)){
			throw std::logic_error("ENDIF without a preceeding IF/ELSE.");
		}
		if(multiline_commands.top().first -> command_type() == ELSE){
			dynamic_cast<ElseCommand*>(multiline_commands.top().first) -> set_endif(program.size());
			multiline_commands.pop();
		}
		dynamic_cast<IfCommand*>(multiline_commands.top().first) -> set_endif(program.size() - 1);
		multiline_commands.pop();
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "WHILE")){
		std::string expression = current_line.substr(6, current_line.size() - 6);
		auto temp = new WhileCommand(WHILE, current_line, new std::string(expression));
		program.push_back(temp);
		multiline_commands.push({temp, program.size()});
	}
	else if(check_prefix(current_line.begin(), current_line.end(), "DONE")){
		if(multiline_commands.empty() || multiline_commands.top().first -> command_type() != WHILE){
			throw std::logic_error("DONE without a preceeding WHILE.");
		}
		dynamic_cast<WhileCommand*>(multiline_commands.top().first) -> set_done(program.size());
		program.push_back(new DoneCommand(DONE, current_line, multiline_commands.top().second - 1));
		multiline_commands.pop();
	}
	else{
		program.push_back(new ExpressionCommand(EXPRESSION, current_line, new std::string(current_line)));
	}
}

void process(int start_line, Trie<int> &labels, Trie<Variable> &variables, std::vector<Command*> &program, std::stack<std::pair<Command*, int>> &multiline_commands){
	for(int line = start_line; line < program.size(); line ++){
		if(program[line] -> command_type() == IF){
			IfCommand* curr = dynamic_cast<IfCommand*>(program[line]);
			try{
				if(!SyntaxTree(*tokenize((*curr -> get_expression()).begin(), (*curr -> get_expression()).end(), variables)).evaluate()){
					if(curr -> get_else() != -1){
						line = curr -> get_else();
					}
					else line = curr -> get_endif();
				}
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		}
		else if(program[line] -> command_type() == ELSE){
			ElseCommand* curr = dynamic_cast<ElseCommand*>(program[line]);
			line = curr -> get_endif();
		}
		else if(program[line] -> command_type() == WHILE){
			WhileCommand* curr = dynamic_cast<WhileCommand*>(program[line]);
			try{
				auto token_sequence = tokenize((*curr -> get_expression()).begin(), (*curr -> get_expression()).end(), variables);
				auto temp = SyntaxTree(*token_sequence).evaluate();
				if(!temp){
					line = curr -> get_done();
				}
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		}
		else if(program[line] -> command_type() == READ){
			ReadCommand* curr = dynamic_cast<ReadCommand*>(program[line]);
			try{
				int value = 0;
				while(true){
					char x = std::cin.get();
					if('0' <= x && x <= '9'){
						value = 10 * value + (x - '0');
					}
					if(x == ' ' || x == '\n') break;
				}
				*read_variable((curr -> get_expression()) -> begin(), (curr -> get_expression()) -> end(), variables) = value;
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		
		}
		else if(program[line] -> command_type() == PRINT){
			PrintCommand* curr = dynamic_cast<PrintCommand*>(program[line]);
			try{
				auto token_sequence = tokenize((*curr -> get_expression()).begin(), (*curr -> get_expression()).end(), variables);
				std::cout << SyntaxTree(*token_sequence).evaluate() << std::endl;
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		
		}
		else if(program[line] -> command_type() == GOTO){
			GotoCommand* curr = dynamic_cast<GotoCommand*>(program[line]);
			try{
				line = *labels.query(curr -> get_label());
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		
		}
		else if(program[line] -> command_type() == DONE){
			DoneCommand* curr = dynamic_cast<DoneCommand*>(program[line]);
			try{
				line = curr -> get_while() - 1;
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		}
		else if(program[line] -> command_type() == EXPRESSION){
			ExpressionCommand* curr = dynamic_cast<ExpressionCommand*>(program[line]);
			try{
				std::string* expr = curr -> get_expression();
				SyntaxTree(*tokenize(expr -> begin(), expr -> end(), variables)).evaluate();
			}
			catch(const std::logic_error& e){
				std::cerr << "Caught error: " << e.what() << std::endl;
				std::cerr << "while evaluating "; curr -> print();
			}
		}
	}
}

int main(int argc, char** argv){
	bool repl = true;
	Trie<int> labels;
	Trie<Variable> variables;
	std::ifstream program_file;
	std::istream* input_stream = &std::cin;
	if(argc == 2){
		repl = false;
		program_file.open(argv[1]);	
		if(!program_file){
			throw std::logic_error("Couldn't open supplied program file.");
		}
		input_stream = &program_file;
	}
	if(repl == false){
		std::vector<Command*> program;
		std::stack<std::pair<Command*, int>> multiline_commands;
		while(true){
			if((*input_stream).eof()) break;
			read_line(input_stream, labels, variables, program, multiline_commands);
		}
		if(!multiline_commands.empty()){
			throw std::logic_error("Incomplete IF/WHILE.");
		}
		process(0, labels, variables, program, multiline_commands);
	}
	else{
		std::vector<Command*> program;
		std::stack<std::pair<Command*, int>> multiline_commands;
		int last_executed = 0;
		while(true){
			if((*input_stream).eof()) break;
			read_line(input_stream, labels, variables, program, multiline_commands);
			if(multiline_commands.empty()){
				process(last_executed, labels, variables, program, multiline_commands);
				last_executed = program.size();
			}
		}
	}
}
