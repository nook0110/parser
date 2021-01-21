// * source for plot builder
// *

#include "plot_source.h"

// * algebraic operations implementation

_parseData::_nodeOperator* binary_operation(_parseData::_node* first, _parseData::_node* second, _parseData::_nodeOperation operation)
{
	_parseData::_nodeOperator* result = new _parseData::_nodeOperator(first, second, operation);
	return result;
}

// * implementation of calling tree nodes as the function they contain

const float _parseData::_nodeOperator::operator()(const float x) const noexcept
{
	switch (operation)
	{
		case _nodeOperation::ADDITION:
			return((*firstOperand)(x) + (*secondOperand)(x));
			break;
		case _nodeOperation::SUBTRACTION:
			return((*firstOperand)(x) - (*secondOperand)(x));
			break;
		case _nodeOperation::MULTIPLICATION:
			return((*firstOperand)(x) * (*secondOperand)(x));
			break;
		case _nodeOperation::DIVISION:
			return((*firstOperand)(x) / (*secondOperand)(x));
			break;
		case _nodeOperation::POWER:
			return(pow((*firstOperand)(x), (*secondOperand)(x)));
			break;
		default:
			break;
	}
}

const float _parseData::_nodeFunction::operator()(const float x) const
{
	switch (this->operation)
	{
		case _nodeOperation::COS:
			return cosf((*argument)(x));
			break;	
		case _nodeOperation::CTAN:
			return 1 / tanf((*argument)(x));
			break;
		case _nodeOperation::EXP:
			return expf((*argument)(x));
			break;
		case _nodeOperation::LOG:
			return logf((*argument)(x));
			break;
		case _nodeOperation::SIN:
			return sinf((*argument)(x));
			break;
		case _nodeOperation::TAN:
			return tanf((*argument)(x));
			break;
		default:
			break;
	}
}

size_t findOperator(const _parseData::_substring& _expression, const _parseData::_nodeOperation operation)
{
	char _operation;
	switch (operation)
	{
	case _parseData::_nodeOperation::ADDITION:
		_operation = '+';
		break;
	case _parseData::_nodeOperation::DIVISION:
		_operation = '/';
		break;
	case _parseData::_nodeOperation::MULTIPLICATION:
		_operation = '*';
		break;
	case _parseData::_nodeOperation::POWER:
		_operation = '^';
		break;
	case _parseData::_nodeOperation::SUBTRACTION:
		_operation = '-';
		break;
	}
	int parCounter = 0;
	int exprBegin;
	for (exprBegin = _expression.begin; exprBegin < _expression.end && _expression[exprBegin] == ' '; ++exprBegin)
		;
	if (_expression[exprBegin] == '(')
		++parCounter;
	else if (_expression[exprBegin] == ')')
		--parCounter;
	for (size_t i = exprBegin + 1; i <= _expression.end; ++i)
	{
		if (_expression[i] == '(')
			++parCounter;
		else if (_expression[i] == ')')
			--parCounter;
		if ((parCounter == 0) && (_expression[i] == _operation))
			return i;
	}
	return 0;
}

_parseData::_nodeexpr parseSubexpression(const _parseData::_substring& substring)
{
	const size_t end = substring.end;
	_parseData::_substring resultString(substring); // resultString.begin = substring.begin
	size_t& subEnd = resultString.end;
	size_t subBegin;
	for (subBegin = substring.begin; subBegin <= end && substring[subBegin] == ' '; ++subBegin)
		;
	switch (substring[subBegin])
	{
	case 'x':
	{
		for (subEnd = subBegin + 1; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
			;
		--subEnd;
		_parseData::_node* result = new _parseData::_node();
		return _parseData::_nodeexpr(resultString, result);
	}
	case '(':
	{
		size_t parPosition;
		int parCounter = 0;
		for (parPosition = subBegin; parCounter != 1 || substring[parPosition] != ')'; ++parPosition)
		{
			if (parPosition == end)
				throw std::exception();
			if (substring[parPosition] == '(')
				++parCounter;
			else if (substring[parPosition] == ')')
				--parCounter;
		}
		for (subEnd = parPosition + 1; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
			;
		--subEnd;
		_parseData::_substring inside(substring.expression, subBegin + 1, parPosition - 1);
		_parseData::_node* parsedNode = parseExpression(inside);
		return _parseData::_nodeexpr(resultString, parsedNode);
	}
	case '-':
	{
		subEnd = subBegin;
		_parseData::_nodeConst* result = new _parseData::_nodeConst(-1);
		return _parseData::_nodeexpr(resultString, result);
	}
	}
	if(isWord(substring, subBegin, "sqrt"))
	{
		_parseData::_substring squareArg = substring;
		squareArg.begin = subBegin + 4;
		_parseData::_nodeexpr squareArgFunc = parseSubexpression(squareArg);
		subEnd = squareArgFunc.expression.end;
		_parseData::_nodeConst* power = new _parseData::_nodeConst(0.5);
		_parseData::_nodeOperator* sqrt = binary_operation(squareArgFunc.node, power, _parseData::_nodeOperation::POWER);
		return _parseData::_nodeexpr(resultString, sqrt);
	}
	else if(isWord(substring, subBegin, "sin"))
	{
		_parseData::_substring sinArg = substring;
		sinArg.begin = subBegin + 3;
		_parseData::_nodeexpr sinArgFunc = parseSubexpression(sinArg);
		subEnd = sinArgFunc.expression.end;
		_parseData::_nodeFunction* sin = new _parseData::_nodeFunction(sinArgFunc.node, _parseData::_nodeOperation::SIN);
		return _parseData::_nodeexpr(resultString, sin);
	}
	else if(isWord(substring, subBegin, "cos"))
	{
		_parseData::_substring cosArg = substring;
		cosArg.begin = subBegin + 3;
		_parseData::_nodeexpr cosArgFunc = parseSubexpression(cosArg);
		subEnd = cosArgFunc.expression.end;
		_parseData::_nodeFunction* cos = new _parseData::_nodeFunction(cosArgFunc.node, _parseData::_nodeOperation::COS);
		return _parseData::_nodeexpr(resultString, cos);
	}
	else if(isWord(substring, subBegin, "ctg"))
	{
		_parseData::_substring ctgArg = substring;
		ctgArg.begin = subBegin + 3;
		_parseData::_nodeexpr ctgArgFunc = parseSubexpression(ctgArg);
		subEnd = ctgArgFunc.expression.end;
		_parseData::_nodeFunction* ctan = new _parseData::_nodeFunction(ctgArgFunc.node, _parseData::_nodeOperation::CTAN);
		return _parseData::_nodeexpr(resultString, ctan);
	}
	else if(isWord(substring, subBegin, "tan"))
	{
		_parseData::_substring tgArg = substring;
		tgArg.begin = subBegin + 3;
		_parseData::_nodeexpr tgArgFunc = parseSubexpression(tgArg);
		subEnd = tgArgFunc.expression.end;
		_parseData::_nodeFunction* tan = new _parseData::_nodeFunction(tgArgFunc.node, _parseData::_nodeOperation::TAN);
		return _parseData::_nodeexpr(resultString, tan);
	}
	else if(isWord(substring, subBegin, "exp"))
	{
		_parseData::_substring expArg = substring;
		expArg.begin = subBegin + 3;
		_parseData::_nodeexpr expArgFunc = parseSubexpression(expArg);
		subEnd = expArgFunc.expression.end;
		_parseData::_nodeFunction* exp = new _parseData::_nodeFunction(expArgFunc.node, _parseData::_nodeOperation::EXP);
		return _parseData::_nodeexpr(resultString, exp);
	}
	else if(isWord(substring, subBegin, "log"))
	{
		_parseData::_substring logArg = substring;
		logArg.begin = subBegin + 3;
		_parseData::_nodeexpr logArgFunc = parseSubexpression(logArg);
		subEnd = logArgFunc.expression.end;
		_parseData::_nodeFunction* log = new _parseData::_nodeFunction(logArgFunc.node, _parseData::_nodeOperation::LOG);
		return _parseData::_nodeexpr(resultString, log);
	}
	// probably expression is a number
	std::string string_number = "";
	size_t i;
	for (i = subBegin; i <= end && (substring[i] == 46 || (substring[i] >= 48 && substring[i] <= 57)); ++i)
	{
		string_number.push_back(substring[i]);
	}
	if (string_number.length() == 0)
		throw std::exception();
	for (subEnd = i; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
		;
	--subEnd;
	float number = std::stof(string_number);
	_parseData::_nodeConst* constNumber = new _parseData::_nodeConst(number);
	return _parseData::_nodeexpr(resultString, constNumber);
}

bool isWord(_parseData::_substring string, size_t wordBegin, std::string word)
{
	for(size_t i = 0; i < word.length(); ++i)
	{
		if (wordBegin + i > string.end)
			return false;
		if (string[wordBegin + i] != word[i])
			return false;
	}
	return true;
}

_parseData::_node* parseExpression(const _parseData::_substring& expr)
{
	const size_t begin = expr.begin;
	const size_t end = expr.end;
	_parseData::_nodeexpr firstExpr = parseSubexpression(expr); // first math subexpression
	if (firstExpr.expression.end == end) // if the first expr is the whole string
		return firstExpr.node;
	size_t operatorPlace = findOperator(expr, _parseData::_nodeOperation::ADDITION);
	if (operatorPlace && operatorPlace != end)
	{
		_parseData::_node* left = parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1));
		_parseData::_node* right = parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _parseData::_nodeOperation::ADDITION));
	}
	operatorPlace = findOperator(expr, _parseData::_nodeOperation::SUBTRACTION);
	if (operatorPlace && operatorPlace != end)
	{
		_parseData::_node* left = parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1));
		_parseData::_node* right = parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _parseData::_nodeOperation::SUBTRACTION));
	}
	operatorPlace = findOperator(expr, _parseData::_nodeOperation::MULTIPLICATION);
	if (operatorPlace && operatorPlace != end)
	{
		_parseData::_node* left = parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1));
		_parseData::_node* right = parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _parseData::_nodeOperation::MULTIPLICATION));
	}
	operatorPlace = findOperator(expr, _parseData::_nodeOperation::DIVISION);
	// can actually throw division by zero exception
	if (operatorPlace && operatorPlace != end)
	{
		_parseData::_node* left = parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1));
		_parseData::_node* right = parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _parseData::_nodeOperation::DIVISION));
	}
	if (expr[firstExpr.expression.end + 1] != '^')
	{
		// can throw an exception if the expression cannot be parsed
		// returns a product of the first subexpr and the rest of the expression
		_parseData::_node* restFunction = parseExpression(_parseData::_substring(expr.expression,
			firstExpr.expression.end + 1,
			end));
		return binary_operation(firstExpr.node, restFunction, _parseData::_nodeOperation::MULTIPLICATION);
	}
	operatorPlace = findOperator(expr, _parseData::_nodeOperation::POWER);
	if (operatorPlace && operatorPlace != end)
	{
		_parseData::_node* left = parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1));
		_parseData::_node* right = parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _parseData::_nodeOperation::POWER));
	}
}

#include <iostream>

int main()
{
	std::string expr;
	std::getline(std::cin, expr);
	_parseData::_node* tree = parseExpression(_parseData::_substring(expr));
	tree->printBT("", false);
	float x;
	do
	{
		std::cin >> x;
		std::cout << (*tree)(x) << std::endl;
	} while (x != 0);
	return 0;
}