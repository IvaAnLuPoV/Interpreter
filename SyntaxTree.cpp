// tova mozheshe da e chuden primer na Singleton class
// no se usetih tvurde kusno :(
#pragma once
#include "Operators.cpp"
#include "Token.cpp"
#include <iostream>
#include <stack>
class SyntaxTree{
private:
	struct Node{
		Token* token;
		std::vector<Node*> children;
		Node(Token* token) : token(token) {}
		inline Type type() const { return token -> type(); }
		inline int* value() { return token -> value(); }
	};
	SyntaxTree(const SyntaxTree&) = delete;
	SyntaxTree(SyntaxTree&&) = delete;
	Node* root;
	int dfs(Node* node){
		if(node -> type() == OPERATOR){
			if(node -> token -> arity() == 1){
				int a = dfs(node -> children[0]);
				return node -> token -> apply(a);
			}
			if(node -> token -> arity() == 2){
				if(node -> token -> symbol() == "="){
					int a = dfs(node -> children[0]);
					if(node -> children[1] -> type() != VARIABLE){
						 throw std::logic_error("Ill-formed expression.");
					}
					*(node -> children[1] -> value()) = a;
					return a;
				}
				else if(node -> token -> symbol() == "&&"){
					int a = dfs(node -> children[1]);
					if(!a) return false;
					int b = dfs(node -> children[0]);
					return node -> token -> apply(a, b);
				}
				else if(node -> token -> symbol() == "||"){
					int a = dfs(node -> children[1]);
					if(a) return true;
					int b = dfs(node -> children[0]);
					return node -> token -> apply(a, b);
				}
				else{
					int a = dfs(node -> children[1]);
					int b = dfs(node -> children[0]); 
					return node -> token -> apply(a, b);
				}
			}
		}
		if(node -> type() == CONSTANT){
			return *(node -> value());
		}
		if(node -> type() == VARIABLE){
			return *(node -> value());
		}
		throw std::logic_error("how did i get here??");
		return 0;
	}
	void print(Node* node){
		if(!node) return;
		if(node -> type() == VARIABLE){
			std::cout << "variable " << node -> value() << " | " << *(node -> value()) << std::endl;
		}
		if(node -> type() == CONSTANT){
			std::cout << "constant " << *(node -> value()) << std::endl;
		}
		if(node -> type() == OPERATOR){
			if(node -> token -> arity() == 1){
				std::cout << node -> token -> print() << std::endl;
				print(node -> children[0]); 
			}
			if(node -> token -> arity() == 2){
				std::cout << node -> token -> print() << std::endl;
				print(node -> children[1]);
				print(node -> children[0]); 
			}
		}
	}
public:
	SyntaxTree() : root(nullptr) {}
	SyntaxTree(const std::vector<Token*> token_sequence){
		std::stack<Node*> node_stack;
		for(auto token : token_sequence){
			if(token -> type() == OPERATOR){
				int arity = token -> arity();
				Node* curr = new Node(token);
				while(!node_stack.empty() && arity > 0){
					(curr -> children).push_back(node_stack.top());
					node_stack.pop();
					-- arity;
				}
				if(arity > 0 && node_stack.empty()) throw std::logic_error("Ill-formed expression.");
				node_stack.push(curr);
			}
			else{
				node_stack.push(new Node(token));
			}
		}
		root = node_stack.top(); node_stack.pop();
		if(!node_stack.empty()) throw std::logic_error("Ill-formed expression.");
	}
	int evaluate(){
		return dfs(root);
	}
	void print(){
		print(root);
	}
};
