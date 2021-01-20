// * source for plot builder
// *

#include "plot_source.h"

// * functors implementation
lambdaAddition::lambdaAddition(const _plotData::function& _left, const _plotData::function& _right)
	: left{ _left }, right{ _right }
	{}
float lambdaAddition::operator()(float x) const noexcept
{
	return left(x) + right(x);
}

//
lambdaSubtraction::lambdaSubtraction(const _plotData::function& _left, const _plotData::function& _right)
	: left{ _left }, right{ _right }
{}
float lambdaSubtraction::operator()(float x) const noexcept
{
	return left(x) - right(x);
}
//
lambdaMultiplication::lambdaMultiplication(const _plotData::function& _left, const _plotData::function& _right)
	: left{ _left }, right{ _right }
{}
float lambdaMultiplication::operator()(float x) const noexcept
{
	return left(x) * right(x);
}

//
lambdaDivision::lambdaDivision(const _plotData::function& _left, const _plotData::function& _right)
	: left{ _left }, right{ _right }
{}
float lambdaDivision::operator()(float x) const
{
	return left(x) / right(x);
}

//
lambdaPower::lambdaPower(const _plotData::function& _left, const _plotData::function& _right)
	: left{ _left }, right{ _right }
{}
float lambdaPower::operator()(float x) const noexcept
{
	return pow(left(x), right(x));
}

//
lambdaSuper::lambdaSuper(const _plotData::function& _func, const _plotData::function& _arg)
	: func{ _func }, arg{ _arg }
{}
float lambdaSuper::operator()(float x) const noexcept
{
	return func(arg(x));
}

//
lambdaConstant::lambdaConstant(float x)
	: constant(x)
{}
float lambdaConstant::operator()(float x) const noexcept
{
	return constant;
}

//
lambdaId::lambdaId()
{}
float lambdaId::operator()(float x) const noexcept
{
	return x;
}

size_t findOperator(const _parseData::_substring& _expression, const _parseData::_operator operation)
{
	char _operation;
	switch (operation)
	{
	case _parseData::_operator::ADDITION:
		_operation = '+';
		break;
	case _parseData::_operator::DIVISION:
		_operation = '/';
		break;
	case _parseData::_operator::MULTIPLICATION:
		_operation = '*';
		break;
	case _parseData::_operator::POWER:
		_operation = '^';
		break;
	case _parseData::_operator::SUBTRACTION:
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

_parseData::_functionexpr parseSubexpression(const _parseData::_substring& substring)
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
		return _parseData::_functionexpr(resultString, _plotData::function(lambdaId()));
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
		_plotData::function parsedFunction = parseExpression(inside);
		return _parseData::_functionexpr(resultString, parsedFunction);
	}
	case '-':
	{
		subEnd = subBegin;
		return _parseData::_functionexpr(resultString, lambdaConstant(-1));
	}
	case 's':
	{
		if (subBegin + 2 > end)
			throw std::exception();
		if (substring[subBegin + 1] == 'q')
		{
			if(isWord(substring, subBegin, "sqrt"))
			{
			_parseData::_substring squareArg = substring;
			squareArg.begin = subBegin + 4;
			_parseData::_functionexpr squareArgFunc = parseSubexpression(squareArg);
			subEnd = squareArgFunc.expression.end;
			_plotData::function resultFunc = lambdaPower(squareArgFunc.function, lambdaConstant(0.5));
			return _parseData::_functionexpr(resultString, resultFunc);
			}
		}
		else if (substring[subBegin + 1] == 'i')
		{
			if(isWord(substring, subBegin, "sin"))
			{
			_parseData::_substring sinArg = substring;
			sinArg.begin = subBegin + 3;
			_parseData::_functionexpr sinArgFunc = parseSubexpression(sinArg);
			subEnd = sinArgFunc.expression.end;
			_plotData::function resultFunc = lambdaSuper(static_cast<float(*)(float)>(&sinf), sinArgFunc.function);
			return _parseData::_functionexpr(resultString, resultFunc);
			}
		}
		else
			throw std::exception();
	}
	case 'c':
	{
		if(isWord(substring, subBegin, "cos"))
		{
		_parseData::_substring cosArg = substring;
		cosArg.begin = subBegin + 3;
		_parseData::_functionexpr cosArgFunc = parseSubexpression(cosArg);
		subEnd = cosArgFunc.expression.end;
		_plotData::function resultFunc = lambdaSuper(static_cast<float(*)(float)>(&cosf), cosArgFunc.function);
		return _parseData::_functionexpr(resultString, resultFunc);
		}
		else if(isWord(substring, subBegin, "ctg"))
		{
		_parseData::_substring ctgArg = substring;
		ctgArg.begin = subBegin + 3;
		_parseData::_functionexpr ctgArgFunc = parseSubexpression(ctgArg);
		subEnd = ctgArgFunc.expression.end;
		_plotData::function resultFunc = lambdaDivision(lambdaConstant(1), lambdaSuper(static_cast<float(*)(float)>(&tanf), ctgArgFunc.function));
		return _parseData::_functionexpr(resultString, resultFunc);
		}
	}
	case 't':
	{
		if(isWord(substring, subBegin, "tan"))
		{
		_parseData::_substring tgArg = substring;
		tgArg.begin = subBegin + 3;
		_parseData::_functionexpr tgArgFunc = parseSubexpression(tgArg);
		subEnd = tgArgFunc.expression.end;
		_plotData::function resultFunc = lambdaSuper(static_cast<float(*)(float)>(&tanf), tgArgFunc.function);
		return _parseData::_functionexpr(resultString, resultFunc);
		}
	}
	case 'e':
	{
		if(isWord(substring, subBegin, "exp"))
		{
		_parseData::_substring expArg = substring;
		expArg.begin = subBegin + 3;
		_parseData::_functionexpr expArgFunc = parseSubexpression(expArg);
		subEnd = expArgFunc.expression.end;
		_plotData::function resultFunc = lambdaSuper(static_cast<float(*)(float)>(&expf), expArgFunc.function);
		return _parseData::_functionexpr(resultString, resultFunc);
		}
	}
	case 'l':
	{
		if(isWord(substring, subBegin, "log"))
		{
		_parseData::_substring lnArg = substring;
		lnArg.begin = subBegin + 3;
		_parseData::_functionexpr lnArgFunc = parseSubexpression(lnArg);
		subEnd = lnArgFunc.expression.end;
		_plotData::function resultFunc = lambdaSuper(static_cast<float(*)(float)>(&logf), lnArgFunc.function);
		return _parseData::_functionexpr(resultString, resultFunc);
		}
	}
	default: // probably expression is a number
	{
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
		return _parseData::_functionexpr(resultString, _plotData::function(lambdaConstant(number)));
	}
	}
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

_plotData::function parseExpression(const _parseData::_substring& expr)
{
	const size_t begin = expr.begin;
	const size_t end = expr.end;
	_parseData::_functionexpr firstExpr = parseSubexpression(expr); // first math subexpression
	if (firstExpr.expression.end == end) // if the first expr is the whole string
		return firstExpr.function;
	size_t operatorPlace = findOperator(expr, _parseData::_operator::ADDITION);
	if (operatorPlace && operatorPlace != end)
	{
		const _plotData::function left(parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1)));
		const _plotData::function right(parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end)));
		return _plotData::function(lambdaAddition(left, right));
	}
	operatorPlace = findOperator(expr, _parseData::_operator::SUBTRACTION);
	if (operatorPlace && operatorPlace != end)
	{
		const _plotData::function left(parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1)));
		const _plotData::function right(parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end)));
		return _plotData::function(lambdaSubtraction(left, right));
	}
	operatorPlace = findOperator(expr, _parseData::_operator::MULTIPLICATION);
	if (operatorPlace && operatorPlace != end)
	{
		const _plotData::function left(parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1)));
		const _plotData::function right(parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end)));
		return _plotData::function(lambdaMultiplication(left, right));
	}
	operatorPlace = findOperator(expr, _parseData::_operator::DIVISION);
	// can actually throw division by zero exception
	if (operatorPlace && operatorPlace != end)
	{
		const _plotData::function left(parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1)));
		const _plotData::function right(parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end)));
		return _plotData::function(lambdaDivision(left, right));
	}
	if (expr[firstExpr.expression.end + 1] != '^')
	{
		// can throw an exception if the expression cannot be parsed
		// returns a product of the first subexpr and the rest of the expression
		_plotData::function restFunction = parseExpression(_parseData::_substring(expr.expression,
			firstExpr.expression.end + 1,
			end));
		return _plotData::function(lambdaMultiplication(firstExpr.function, restFunction));
	}
	operatorPlace = findOperator(expr, _parseData::_operator::POWER);
	if (operatorPlace && operatorPlace != end)
	{
		const _plotData::function left(parseExpression(_parseData::_substring(expr.expression, begin, operatorPlace - 1)));
		const _plotData::function right(parseExpression(_parseData::_substring(expr.expression, operatorPlace + 1, end)));
		return _plotData::function(lambdaPower(left, right));
	}
}



#include<iostream>
int main()
{
	std::string expr;
	std::getline(std::cin, expr);
	_plotData::function result = parseExpression(_parseData::_substring(expr));
	float x;
	do
	{
	std::cin >> x;
	std::cout << result(x) << std::endl;
	} while (x != 0);
	system("pause");
	return 0;
}
