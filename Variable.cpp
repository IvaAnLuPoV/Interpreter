#include <vector>
#include <stdexcept>
class Variable{
private:
	std::vector<std::size_t> dimensions;
	std::vector<int> data;
public:
	Variable(const std::vector<std::size_t> &dimensions) : dimensions(dimensions) {
		int p = 1;
		for(auto x : dimensions){
			p *= x;
		}
		data.resize(p);
	}
	int* operator[](const std::vector<std::size_t>& pos){
		if(dimensions.size() != pos.size()){
			std::cout << "dim: ";
			for(auto x : dimensions){
				std::cout << x << " ";
			} std::cout << std::endl;
			std::cout << "pos: ";
			for(auto x : pos){
				std::cout << x << " ";
			} std::cout << std::endl;
			throw std::logic_error("Non-existent position in an array.");
		}
		for(int i = 0; i < pos.size(); i ++){
			if(pos[i] < 0 || dimensions[i] <= pos[i]) throw std::logic_error("Non-existent position in an array.");
		}
		int p = 1, idx = 0;
		for(int i = pos.size() - 1; i >= 0; i --){
			idx += p * pos[i];
			p *= dimensions[i];
		}
		return &data[idx];
	}
};
