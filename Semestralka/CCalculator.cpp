#include <cstring>
#include "COperand.h"
#include "CCalculator.h"
#include "CCparser.h"
#include "CInteger.h"
#include "CLongInteger.h"
#include "CDecimal.h"

void CCalculator::run()
{
    string input;
    string result;

    while(true)
    {
        input = readInput();
        if(input == "variable")
            continue;
        if(input == "quit")
            break;
        result = calculate(input);
        display(result);
        saveHistory(input, result);
    }
}

string CCalculator::readInput()
{
    string input;

    cout << ">";
    getline(cin, input, '\n');
    removeWhiteSpaces(input);

    //adding new variable
    if(input.find('=') != string::npos)
    {
        m_history.push_back(input);
        createNewVariable(input);
        return "variable";
    }

    return input;
}

void CCalculator::removeWhiteSpaces(string &str)
{
    for (auto it = str.begin(); it < str.end(); it++)
        while (*it == ' ')
            str.erase(it);
}

void CCalculator::createNewVariable(const string &input)
{
    char * token;
    token = strtok((char*)input.c_str(), "=");
    string name(token);
    token = strtok(NULL, "=");
    string val(token);
    m_variables.emplace_back(CVariable(name, val, determineType(val)));
}

string CCalculator::calculate(const string & input)
{
    CParser parser;
    vector<string> parsedInput;
    stack<COperand*> operandStack;

    if(!input.empty())
    {
        parsedInput.clear();
        parsedInput = parser.parse(input) ;

        for(const auto & token : parsedInput)
        {
            if(!isOperator(token))
                pushToStack(token, operandStack);
            else
            {
                COperand *rVal = operandStack.top();
                operandStack.pop();
                COperand *lVal = operandStack.top();
                operandStack.pop();

                COperand *result = performOperation(lVal, rVal, token);
                operandStack.push(result);

                delete rVal;
                delete lVal;
            }
        }
    }
}

void CCalculator::display(const string & result) const
{
    cout << "----------" << endl;
    cout << result << endl;
}

void CCalculator::saveHistory(const string & input, const string & result)
{
    string tmp;

    tmp += input;
    tmp += " = ";
    tmp += result;
    m_history.push_back(tmp);
}

EValType CCalculator::determineType(const string & number) const
{
    if(number.find('.') != string::npos || number.find('e') != string::npos)
        return VAL_DEC;

    if(number.size() > 6)
        return VAL_LONGINT;

    return VAL_INT;
}

bool CCalculator::isOperator(const string & op) const
{
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
}

void CCalculator::pushToStack(const string &operand, stack<COperand *> &stack) const
{
    switch (determineType(operand))
    {
        case VAL_INT: {
            COperand *p = new CInteger(operand);
            stack.push(p);
            break;
        }
        case VAL_LONGINT: {
            COperand *p = new CLongInteger(operand);
            stack.push(p);
            break;
        }
        case VAL_DEC: {
            COperand *p = new CDecimal(operand);
            stack.push(p);
            break;
        }
    }
}

COperand *CCalculator::performOperation(COperand * lVal, COperand * rVal, const string & op)
{
    if(op == "+")
        return *lVal + *rVal;

    if(op == "-")
        return *lVal - *rVal;

    if(op == "*")
        return*lVal * *rVal;

    if(op == "/")
        return *lVal / *rVal;

//    if(op == "%")
//        return *lVal % *rVal;
}