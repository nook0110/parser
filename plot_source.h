// * plot builder source header
// *

#ifndef _PLOT_SOURCE_H_
#define _PLOT_SOURCE_H_

// * includes
#include <string> //? std::string
#include <functional> //? std::function
#include <cmath> //? std math functions
#include <iostream>

// * type aliasing
struct _plotData
{
	using function = std::function<float(float)>;
};

// * details of parser implementation

static char *Operations[] =
{
	"ADDITION",
	"SUBTRACTION",
	"MULTIPLICATION",
	"DIVISION",
	"POWER",
	"IDENTICAL",
	"CONSTANT",
	"SIN",
	"COS",
	"TAN",
	"CTAN",
	"EXP",
	"LOG",
};

struct _parseData
{
	enum _nodeOperation
	{
		ADDITION=0,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION,
		POWER,
		IDENTICAL,
		CONSTANT,
		SIN,
		COS,
		TAN,
		CTAN,
		EXP,
		LOG,
		//even the sky,
		//Allah himself...
	};

	struct _substring
	{
		std::string expression;
		size_t begin;
		size_t end;
		_substring()
		{}
		_substring(const std::string& _expression)
			: expression(_expression), begin(0), end(expression.length() - 1)
		{}
		_substring(const std::string& _expression, const size_t _begin, const size_t _end)
			: expression(_expression), begin(_begin), end(_end)
		{}
		char operator[](const size_t i)
		{
			return expression[i];
		}
		const char operator[](const size_t i) const
		{
			return expression[i];
		}
	};
	// * binary tree containing the internal structure of the expression
	// * parent class containing identical function as itself
	class _node
	{
		public:
			_nodeOperation operation;
			_node()
			{
				operation = _nodeOperation::IDENTICAL;
			}
			virtual const float operator()(const float x) const { return x; }

			virtual void printBT(const std::string& prefix, bool isLeft)
			{
				std::cout << prefix;

				std::cout << (isLeft ? "|---" : "^---");

				// print the value of the node
				std::cout << "X" << std::endl;
			}
	};
	// * child class containing constant function
	class _nodeConst : public _node
	{
		private:
			float const_value;
		public:
			_nodeConst(float _value)
				: const_value(_value)
			{
				this->operation = _nodeOperation::CONSTANT;
			}
			virtual const float operator()(const float x) const noexcept { return const_value; }

			virtual void printBT(const std::string& prefix, bool isLeft)
			{
				std::cout << prefix;

				std::cout << (isLeft ? "|---" : "^---");

				// print the value of the node
				std::cout << const_value << std::endl;
			}
	};
	// * child class containing different algebraic functions
	class _nodeFunction : public _node
	{
		private:
			_node* argument;
		public:
			_nodeFunction(_node* _argument, _nodeOperation FUNCTION_TYPE)
				: argument(_argument)
			{
				this->operation = FUNCTION_TYPE;
			}
			virtual const float operator()(const float x) const;

			virtual void printBT(const std::string& prefix, bool isLeft)
			{
				std::cout << prefix;

				std::cout << (isLeft ? "|---" : "^---");

				// print the value of the node
				std::cout << Operations[operation] << std::endl;

				// enter the next tree level - left and right branch
				argument->printBT(prefix + (isLeft ? "|   " : "    "), true);
			}
	};
	// * child class containing binary ariphmetic operations
	class _nodeOperator : public _node
	{
		private:
			_node* firstOperand;
			_node* secondOperand;
		public:
			_nodeOperator(_node* _first, _node* _second)
				: firstOperand(_first), secondOperand(_second)
			{}
			_nodeOperator(_node* _first, _node* _second, _nodeOperation OPERATION_TYPE)
				: firstOperand(_first), secondOperand(_second)
			{
				this->operation = OPERATION_TYPE;
			}
			virtual const float operator()(const float x) const noexcept;

			virtual void printBT(const std::string& prefix, bool isLeft)
			{
				std::cout << prefix;

				std::cout << (isLeft ? "|---" : "^---");

				// print the value of the node
				std::cout << Operations[operation] << std::endl;

				// enter the next tree level - left and right branch
				firstOperand->printBT(prefix + (isLeft ? "|   " : "    "), true);
				secondOperand->printBT(prefix + (isLeft ? "|   " : "    "), false);
			}
	};
	struct _nodeexpr
	{
		_parseData::_substring expression;
		_parseData::_node* node;
		_nodeexpr()
		{}
		_nodeexpr(const _parseData::_substring& _expression, _parseData::_node* _node)
			: expression(_expression), node(_node)
		{}
	};
};


// * some functions declarations
size_t findOperator(const _parseData::_substring& _expression, const _parseData::_nodeOperation operation);
_parseData::_node* parseExpression(const _parseData::_substring& expr);
_parseData::_nodeexpr parseSubexpression(const _parseData::_substring& substring);
bool isWord(_parseData::_substring string, size_t wordBegin, std::string word);

// * ifndef _PLOT_SOURCE_H_
#endif