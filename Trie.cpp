#include<memory>
#include<string>
#include<stdexcept>
template<class T>
class Trie{
private:
	struct Node{
		std::shared_ptr<Node> children[26];
		T* data;
		bool eow;
		Node(){
			eow = false;
			data = nullptr;
			for(int i = 0; i < 26; i ++){
				children[i] = nullptr;
			}
		}
	};
	std::shared_ptr<Node> root;
public:
	Trie() {
		root = std::make_shared<Node>();
	}
	void insert(const std::string& word, T* data){
		std::shared_ptr<Node> curr = root;
		for(auto x : word){
			if(curr -> children[x - 'a'] == nullptr){
				curr -> children[x - 'a'] = std::make_shared<Node>(Node());
			}
			curr = curr -> children[x - 'a'];
		}
		if(curr -> eow == true) throw std::logic_error("Rewriting existing entity."); 
		curr -> eow = true;
		curr -> data = data;
	}
	T* query(const std::string& word){
		std::shared_ptr<Node> curr = root;
		for(auto x : word){
			if(curr -> children[x - 'a'] == nullptr){
				throw std::logic_error("Searching for non-existent entity.");
			}
			curr = curr -> children[x - 'a'];
		}
		if(curr -> eow) return curr -> data;
		else throw std::logic_error("Searching for non-existent entity.");
	}
};
