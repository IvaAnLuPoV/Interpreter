#pragma once
#include <memory>
#include <sstream>
#include <stdexcept>
#include "Operators.cpp"
enum Type{
	VARIABLE,
	CONSTANT,
	OPERATOR
};
class Token{
private:
	enum Type m_type;
	int* m_value; const Operator* m_op;
public:
	Token(int* variable){
		m_value = variable;
		m_type = VARIABLE;
	} // constructor for variable
	Token(int constant){
		m_value = new int(constant);
		m_type = CONSTANT;
	} // constructor for constant
	Token(const std::string& m_op){
		this -> m_op = OperatorSelector::getSelector() -> selectOperator(m_op);
		m_type = OPERATOR;
	} // constructor for operator (using symbol)
	Token(const Operator* m_op){
		this -> m_op = m_op;
		m_type = OPERATOR;
	} // constructor for operator (using operator)
	inline std::string symbol() const {
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return m_op -> symbol();
	}
	inline int priority() const {
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return m_op -> priority();
	}
	inline int* value() { return m_value; }
	inline Type type() const { return m_type; }
	inline bool is_bracket(){
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return m_op -> is_bracket();
	}
	inline bool is_closing(){
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return !(dynamic_cast<const bracketOperator*>(m_op) -> is_open());
	}
	inline int arity() const {
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return m_op -> m_arity;
	}
	int apply(int l, int r = 0){
		if(m_type != OPERATOR) throw std::logic_error("Not an operator.");
		return m_op -> apply(l, r);
	}
	std::string print() const {
		if(m_type == OPERATOR) return m_op->symbol();
		else{
			std::ostringstream oss;
			oss << static_cast<const void*>(m_value);
			return oss.str();
		}
	}
};
