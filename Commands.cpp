#include <string>
#include <iostream>
#include "SyntaxTree.cpp"
enum CommandType{ IF, ELSE, WHILE, READ, PRINT, GOTO, DONE, EXPRESSION };
class Command{
protected:
	enum CommandType m_command_type;
	std::string m_text;
public:
	Command(CommandType m_command_type, const std::string &m_text) : m_command_type(m_command_type), m_text(m_text) {}
	CommandType command_type() const { return m_command_type; }
	virtual void print() const { std::cout << m_text << std::endl; }
	virtual ~Command() = default;
};
class IfCommand : public Command {
private:
	int m_else = -1, m_endif = -1;
	std::string* m_expression;
public:
	IfCommand(CommandType command_type, const std::string &m_text, std::string* m_expression) : Command(command_type, m_text), m_expression(m_expression) {}
	void print() const override { std::cout << m_text << " else = " << m_else << " endif = " << m_endif << std::endl; }
	std::string* get_expression(){ return m_expression; }
	void set_else(int n_else){ m_else = n_else; }
	void set_endif(int n_endif){ m_endif = n_endif; }
	int get_else() const { return m_else; }
	int get_endif() const { return m_endif; }
	~IfCommand() { delete m_expression; }
};

class ElseCommand : public Command{
private:
	int m_endif = -1;
public:
	ElseCommand(CommandType command_type, const std::string &m_text) : Command(command_type, m_text) {}
	void print() const override { std::cout << "ELSE endif = " << m_endif << std::endl; }
	void set_endif(int n_endif){ m_endif = n_endif; }
	int get_endif(){ return m_endif; }
};

class WhileCommand : public Command {
private:
	int m_done;
	std::string* m_expression;
public:
	WhileCommand(CommandType command_type, const std::string &m_text, std::string* m_expression) : Command(command_type, m_text), m_expression(m_expression) {}
	std::string* get_expression() { return m_expression; }
	void print() const override { std::cout << m_text << " done = " << m_done << std::endl; }
	void set_done(int n_done){ m_done = n_done; }
	int get_done() const { return m_done; }
	~WhileCommand() { delete m_expression; }
};

class DoneCommand : public Command {
private:
	int m_while;
public:
	DoneCommand(CommandType command_type, const std::string &m_text, int m_while) : Command(command_type, m_text), m_while(m_while) {}
	void print() const override { std::cout << m_text << " while = " << m_while << std::endl; }
	void set_while(int n_while){ m_while = n_while; }
	int get_while() const { return m_while; }
};

class ReadCommand : public Command {
private:
	std::string* m_expression;
public:
	ReadCommand(CommandType command_type, const std::string &m_text, std::string* m_expression) : Command(command_type, m_text), m_expression(m_expression) {}
	std::string* get_expression() { return m_expression; }
	void print() const override { std::cout << m_text << " variable = " << *m_expression << std::endl; }
	~ReadCommand() { delete m_expression; }
};

class PrintCommand : public Command {
private:
	std::string* m_expression;
public:
	PrintCommand(CommandType command_type, const std::string &m_text, std::string* m_expression) : Command(command_type, m_text), m_expression(m_expression) {}
	std::string* get_expression() { return m_expression; }
	~PrintCommand() { delete m_expression; }
};

class GotoCommand : public Command {
private:
	std::string m_label;
public:
	GotoCommand(CommandType command_type, const std::string &m_text, const std::string &m_label) : Command(command_type, m_text), m_label(m_label) {}
	std::string get_label() const { return m_label; }
};

class ExpressionCommand : public Command {
private:
	std::string *m_expression;
public:
	ExpressionCommand(CommandType command_type, const std::string &m_text, std::string* m_expression) : Command(command_type, m_text), m_expression(m_expression) {}
	std::string* get_expression() { return m_expression; }
	~ExpressionCommand() { delete m_expression; }
};
