#pragma once
#include <string>
#include <vector>
#include <stdexcept>

enum Associativity{ LR, RL };

struct Operator{
	std::string m_symbol;
	int m_priority;
	enum Associativity m_associativity;
	int m_arity;
	Operator(const std::string& m_symbol, int m_priority, Associativity m_associativity = LR, int m_arity = 2) 
		: m_symbol(m_symbol), 
		  m_priority(m_priority), 
		  m_associativity(m_associativity),
		  m_arity(m_arity) {}
	
	inline std::string symbol() const { return m_symbol; }
	inline int priority() const { return m_priority; }
	inline int arity() const { return m_arity; }
	inline Associativity associativity() const { return m_associativity; }
	virtual int apply(int left, int right) const = 0;
    virtual bool is_bracket() const { return false; }
	virtual ~Operator() {}
};

class OperatorSelector{
private:
	std::vector<const Operator*> operators;
	OperatorSelector() = default;
	OperatorSelector(const OperatorSelector&) = delete;
	OperatorSelector& operator=(const OperatorSelector&) = delete;
public:
	static OperatorSelector* getSelector(){
		static OperatorSelector theSelector;
		return &theSelector;
	}
	void registerOperator(const Operator* op){
		if(op && !selectOperator(op -> m_symbol)){
			operators.push_back(op);
		}
	}
	const Operator* selectOperator(const std::string &m_symbol) const {
		for (const Operator* const& op : operators) {
			if (op -> m_symbol == m_symbol) {
				return op;
			}
		}
		return nullptr;
	}
};

template <typename OP>
class operatorRegistrator
{
	OP op;
public:
	operatorRegistrator() {
		OperatorSelector::getSelector()->registerOperator(&op);
	}
};

struct bracketOperator : public Operator
{
    bracketOperator(const std::string& c) : Operator(c, -1) {}
    
	int apply(int, int) const override {
		throw std::logic_error("Non-applicable operator.");
		return 0;
	}
	virtual bool is_bracket() const override { return true; }
	virtual bool match(char ch) const = 0;
	virtual bool is_open() const = 0;
};

struct openBracketOperator : public bracketOperator
{
    openBracketOperator() : bracketOperator("(") {}
    virtual bool match(char c) const override { return c == ')'; }
    virtual bool is_open() const override { return true; }
};
struct closeBracketOperator : public bracketOperator
{
public:
    closeBracketOperator() : bracketOperator(")") {}
    virtual bool match(char c) const override { return c == '('; }
    virtual bool is_open() const override { return false; }
};

static operatorRegistrator<openBracketOperator> _openBrReg;
static operatorRegistrator<closeBracketOperator> _closeBrReg;

struct assignmentOperator : public Operator{
	assignmentOperator() : Operator("=", 0, RL) {}
	int apply(int a, int b) const override {
		return b;
	}
};
static operatorRegistrator<assignmentOperator> _assignmentReg;

struct orOperator : public Operator{
	orOperator() : Operator("||", 1) {}
	int apply(int a, int b) const override {
		return a || b;
	}
};
static operatorRegistrator<orOperator> _orReg;

struct andOperator : public Operator{
	andOperator() : Operator("&&", 2) {}
	int apply(int a, int b) const override {
		return a && b;
	}
};
static operatorRegistrator<andOperator> _andReg;

struct eqOperator : public Operator{
	eqOperator() : Operator("==", 3) {}
	int apply(int a, int b) const override {
		return a == b;
	}
};
static operatorRegistrator<eqOperator> _eqReg;

struct ineqOperator : public Operator{
	ineqOperator() : Operator("!=", 3) {}
	int apply(int a, int b) const override {
		return a != b;
	}
};
static operatorRegistrator<ineqOperator> _ineqReg;

struct lessOperator : public Operator{
	lessOperator() : Operator("<", 4) {}
	int apply(int a, int b) const override {
		return a < b;
	}
};
static operatorRegistrator<lessOperator> _lessReg;

struct lessEqOperator : public Operator{
	lessEqOperator() : Operator("<=", 4) {}
	int apply(int a, int b) const override {
		return a <= b;
	}
};
static operatorRegistrator<lessEqOperator> _lessEqReg;

struct greaterOperator : public Operator{
	greaterOperator() : Operator(">", 4) {}
	int apply(int a, int b) const override {
		return a > b;
	}
};
static operatorRegistrator<greaterOperator> _greaterReg;

struct greaterEqOperator : public Operator{
	greaterEqOperator() : Operator(">=", 4) {}
	int apply(int a, int b) const override {
		return a > b;
	}
};
static operatorRegistrator<greaterEqOperator> _greaterEqReg;

struct sumOperator : public Operator{
	sumOperator() : Operator("+", 5) {}
	int apply(int a, int b) const override {
		return a + b;
	}
};
static operatorRegistrator<sumOperator> _sumReg;

struct subOperator : public Operator{
	subOperator() : Operator("-", 5) {}
	int apply(int a, int b) const override {
		return a - b;
	}
};
static operatorRegistrator<subOperator> _subReg;

struct mulOperator : public Operator{
	mulOperator() : Operator("*", 6) {}
	int apply(int a, int b) const override {
		return a * b;
	}
};
static operatorRegistrator<mulOperator> _mulReg;

struct divOperator : public Operator{
	divOperator() : Operator("/", 6) {}
	int apply(int a, int b) const override {
		return a / b;
	}
};
static operatorRegistrator<divOperator> _divReg;

struct modOperator : public Operator{
	modOperator() : Operator("%", 6) {}
	int apply(int a, int b) const override {
		return a % b;
	}
};
static operatorRegistrator<modOperator> _modReg;

struct notOperator : public Operator{
	notOperator() : Operator("!", 7, RL, 1) {}
	int apply(int a, int b = 0) const override {
		return !a;
	}
};
static operatorRegistrator<notOperator> _notReg;
