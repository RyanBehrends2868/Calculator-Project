#include "Calculator.h"

#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <fstream> 

Calculator::Calculator(bool radianMode, bool saveHistory, int taylorTerms, double initialGuessInterest, double initialGuessPeriods, double errorThreshold)
{
    settingRadianMode = radianMode;
    settingSaveHistory = saveHistory;
    settingTaylorTerms = taylorTerms;
    settingInitialGuessInterest = initialGuessInterest;
    settingInitialGuessPeriods = initialGuessPeriods;
    settingErrorThreshold = errorThreshold;
}

/*---------------
Tokenize Function
-----------------*/
std::vector<Calculator::Token> Calculator::tokenize(const std::string& inputExpression) const
{
    std::vector<Token> tokens; // Stores the tokens
    std::string currentToken; // For multi digit or letter tokens 
    bool expectNumber = true; // Finite state for detecting unary minus vs. binary subtraction
    
    for (int index = 0; index < inputExpression.length(); index++)
    {
        currentToken.clear();
        char character = inputExpression[index];
        
        // Skip white space
        if (std::isspace(character)) continue;

        // Digits: use currentToken to build multi digit token
        if (std::isdigit(character) || character == '.')
        {
            while (index < inputExpression.length() && (std::isdigit(inputExpression[index]) || inputExpression[index] == '.')) // While expression[index] is still a digit or decimal append to currentToken and increment index
            {
                currentToken += inputExpression[index++];
                // Check if currentToken contains multiple '.'
                if (currentToken.find('.') != currentToken.rfind('.'))
                {
                    throw std::invalid_argument("Invalid number format: multiple decimal points in " + currentToken);
                }
            }
            index--; // undo last while loop increment because for loop will increment

            tokens.push_back(Token{NUMBER, currentToken});
            expectNumber = false; // After a number, expect an operator
        }

        // Alphabetic: try to match to function or pi after using currentToken to build word
        else if(std::isalpha(character))
        {
            while (index < inputExpression.length() && std::isalpha(inputExpression[index]))
            {
                currentToken += inputExpression[index++];
            }
            index--; // undo last while loop increment because for loop will increment
            // Match word to known function or pi
            if (currentToken == "pi" || currentToken == "Pi" || currentToken == "PI")
            {
                tokens.push_back(Token{NUMBER, "3.141592653589793"});
                expectNumber = false; // After a number, expect an operator
            }
            else if (currentToken == "sin" || currentToken == "cos" || currentToken == "tan")
            {
                tokens.push_back(Token{FUNCTION, currentToken});
                expectNumber = true; // After a Function, expect a number
            }
            else
            {
                throw std::invalid_argument("Unrecognized function: " + currentToken);
            }
        }
        // Operators
        else if (character == '^' || character == '*' || character == '/' || character == '+' || character == '-')
        {
            if (character == '-' && expectNumber) // if expecting a number and instead get a -, append unary minus
            {
                tokens.push_back(Token{OPERATOR, "u-"});
            }
            else
            {
                tokens.push_back(Token{OPERATOR, std::string(1, character)});
            }
            expectNumber = true; // After an Operator, expect a number
        }
        // Left Parenthesis
        else if (character == '(')
        {
            tokens.push_back(Token{LEFT_PAREN, "("});
            expectNumber = true; // After left parenthesis, expect a number
        }
        // Right Parenthesis
        else if (character == ')')
        {
            tokens.push_back(Token{RIGHT_PAREN, ")"});
            expectNumber = false; // After right parenthesis, expect an operator
        }
        else
        {
            throw std::invalid_argument(std::string("Unrecognized character: ") + character);
        }
    }
    return tokens;
}

/*
Additional Functions for Shunting Yard Algorithm
*/ 
int Calculator::getPrecedence(const std::string& operation) const
{
    if (operation == "+" || operation == "-") return 1;
    if (operation == "*" || operation == "/") return 2;
    if (operation == "^") return 3;
    if (operation == "u-") return 4;
    return 0;     
}

bool Calculator::isLeftAssociative(const std::string& operation) const
{
    return (operation != "^" && operation != "u-");
}

/*---------------------------------------------------------------
Convert to Reverse Polish Notation (RPN): Shunting Yard Algorithm
----------------------------------------------------------------*/
std::vector<Calculator::Token> Calculator::convertToRPN(const std::vector<Calculator::Token>& tokenExpression) const
{
    std::vector<Calculator::Token> outputStack;
    std::vector<Calculator::Token> operatorStack;

    for (const Token& token : tokenExpression)
    {
        if (token.type == NUMBER)
        {
            outputStack.push_back(token);
        }
        else if (token.type == FUNCTION)
        {
            operatorStack.push_back(token);
        }
        else if (token.type == OPERATOR)
        {
            std::string operator1 = token.value;
            while (!operatorStack.empty() && operatorStack.back().type != LEFT_PAREN &&
                    // for right associative operator 1: operatorStack operation is > precedence than operator 1
                    // or for left associative operator 1: operatorStack operation is == precedence to operator 1) 
                  (getPrecedence(operatorStack.back().value) > getPrecedence(operator1) ||
                  (getPrecedence(operatorStack.back().value) == getPrecedence(operator1) && isLeftAssociative(operator1))))
            {
                outputStack.push_back(operatorStack.back());
                operatorStack.pop_back();
            }
            operatorStack.push_back(token);
        }
        else if (token.type == LEFT_PAREN)
        {
            operatorStack.push_back(token);
        }
        else if (token.type == RIGHT_PAREN)
        {
            // push from operatorStack to outputStack until left parenthesis is found
            while(!operatorStack.empty() && operatorStack.back().type != LEFT_PAREN)
            {
                outputStack.push_back(operatorStack.back());
                operatorStack.pop_back();
            }
            if (operatorStack.empty()) 
            {
                throw std::invalid_argument("Mismatched parenthesis, missing: '('");
            }
            operatorStack.pop_back(); // discard left parenthesis
        }
        else
        {
            throw std::invalid_argument("Unknown Token Type for value: " + token.value); 
        }
    }
    // Empty the remaining operator evalStack
    while(!operatorStack.empty())
    {
        if (operatorStack.back().type == LEFT_PAREN)
        {
            throw std::invalid_argument("Mismatched parenthesis, missing: ')'");
        }
        outputStack.push_back(operatorStack.back());
        operatorStack.pop_back();
    }
    return outputStack;
}

/*-----------------------------------------
Evaluate Reverse Polish Notation Expression
------------------------------------------*/
double Calculator::evaluateRPN(const std::vector<Calculator::Token>& rpnExpression) const
{
    std::vector<double> evalStack;
    // If token number push output
    // If operator, check unary or binary, pop numbers from output, push result
    // If function, pop number from output, push result
    // return last remaining number, if more than 1 error
    for (const Token& token : rpnExpression)
    {
        if (token.type == NUMBER)
        {
            evalStack.push_back(std::stod(token.value));
        }
        else if (token.type == OPERATOR)
        {
            if (token.value == "u-") // Unary minus
            {
                if (evalStack.empty()) throw std::invalid_argument("Invalid expression: not enough operands");
                double num = evalStack.back();
                evalStack.pop_back();
                evalStack.push_back(-num);
            }
            else
            {
                if (evalStack.size() < 2) throw std::invalid_argument("Invalid expression: not enough operands");
                double num2 = evalStack.back();
                evalStack.pop_back();
                double num1 = evalStack.back();
                evalStack.pop_back();
                
                if (token.value == "+") evalStack.push_back(num1 + num2);
                else if (token.value == "-") evalStack.push_back(num1 - num2);
                else if (token.value == "*") evalStack.push_back(num1 * num2);
                else if (token.value == "/")
                {
                    if (num2 == 0) throw std::runtime_error("Division by zero");
                    evalStack.push_back(num1 / num2);
                }
                else if (token.value == "^") evalStack.push_back(std::pow(num1, num2));
            }
        }
        else if (token.type == FUNCTION)
        {
            if (evalStack.empty()) throw std::invalid_argument("Invalid expression: not enough operands");
            double num = evalStack.back();
            evalStack.pop_back();
            
            if (token.value == "sin") evalStack.push_back(Calculator::calcSin(num));
            else if (token.value == "cos") evalStack.push_back(Calculator::calcCos(num));
            else if (token.value == "tan") evalStack.push_back(Calculator::calcTan(num));
        }
    }
    
    if (evalStack.size() != 1) throw std::invalid_argument("Invalid expression: too many operands");
    return evalStack.back();
}

/*@@@@@@@@@@@@@@@@
evaluateExpression
@@@@@@@@@@@@@@@@@@*/
double Calculator::evaluateExpression(const std::string& inputExpression) const
{
    // Tokenize the expression, convert tokens to RPN and evaluate the RPN expression.
    const std::vector<Calculator::Token> tokenExpression = tokenize(inputExpression);
    const std::vector<Calculator::Token> rpnExpression = convertToRPN(tokenExpression);
    double result = evaluateRPN(rpnExpression);
    
    // Save history to a file
    if (settingSaveHistory) saveHistory(inputExpression, tokenExpression, rpnExpression, "calculation_history.txt", result);

    // Adjust result close to zero before returning
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}

/*--------------------
Trignometric Functions
---------------------*/
double Calculator::factorial(const int num) const 
{
    if (num <= 1) return 1;
    double result = 1;
    for (int index = 2; index <= num; ++index) 
    {
        result *= index;
    }
    return result;
}

double Calculator::reduceAngle(const double angle) const 
{
    double reduced = angle;
    const double pi = 3.141592653589793;
    // Bring angle to within [-pi, pi]
    while (reduced > pi || reduced < -pi) 
    {
        if (reduced > pi) 
        {
            reduced -= 2 * pi;
        }
        else if (reduced < -pi) 
        {
            reduced += 2 * pi;
        }
    }
    return reduced;
}

double Calculator::calcSin(const double angle) const 
{
    double radianAngle;
    if (settingRadianMode)
    {
        radianAngle = angle;
    }
    else
    {
        radianAngle = angle * 3.141592653589793 / 180.0; // Convert to radians if in degree mode
    }
    const double theta = reduceAngle(radianAngle);
    double result = 0;
    for (int index = 0; index < settingTaylorTerms; index++) 
    {
        int exponent = 2 * index + 1;
        result += pow(-1, index) * pow(theta, exponent) / factorial(exponent); 
    }
    // Adjust result close to zero
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}

double Calculator::calcCos(const double angle) const 
{
    double radianAngle;
    if (settingRadianMode)
    {
        radianAngle = angle;
    }
    else
    {
        radianAngle = angle * 3.141592653589793 / 180.0; // Convert to radians if in degree mode
    }
    const double theta = reduceAngle(radianAngle);
    double result = 0;
    for (int index = 0; index < settingTaylorTerms; index++) 
    {
        int exponent = 2 * index;
        result += pow(-1, index) * pow(theta, exponent) / factorial(exponent);
    }
    // Adjust result close to zero
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}

double Calculator::calcTan(const double angle) const 
{
    double cosValue = calcCos(angle);
    if (std::fabs(cosValue) < settingErrorThreshold) throw std::runtime_error("Tangent undefined at this angle");
    return calcSin(angle) / cosValue;
}

/*----------------------------------
Time Value of Money Solver Functions
------------------------------------*/

// Future Value calculation
double Calculator::calculateFV(double pv, double pmt, double i, double n) const
{
    if (i == 0)
    {
        return -(pv + pmt * n);
    }

    double result = -pv * pow(1 + i, n) - pmt * ((pow(1 + i, n) - 1) / i);
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}


// Present Value calculation
double Calculator::calculatePV(double fv, double pmt, double i, double n) const
{
    if (i == 0)
    {
        return -(fv + pmt * n);
    }

    double result = -(fv / pow(1 + i, n)) - pmt * ((1 - pow(1 + i, -n)) / i);
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}

// Payment calculation
double Calculator::calculatePMT(double pv, double fv, double i, double n) const
{
    if (i <= 0 || n <= 0)
    {
        throw std::invalid_argument("Interest rate and number of periods must be greater than zero.");
    }

    double result = (-pv * i - (fv * i) / pow(1 + i, n)) / (1 - pow(1 + i, -n));
    if (std::fabs(result) < settingErrorThreshold) result = 0;
    return result;
}

// Interest rate calculation using Newton-Raphson
double Calculator::calculateInterest(double pv, double fv, double pmt, double n) const
{
    if (n <= 0) throw std::invalid_argument("Number of periods must be greater than zero.");

    double guess = settingInitialGuessInterest;
    double diff = 1;
    double newGuess;
    int iterations = 0;
    int maxIterations = 1000;

    while (std::fabs(diff) > settingErrorThreshold && iterations < maxIterations)
    {
        double f = -pv * pow(1 + guess, n) - pmt * ((pow(1 + guess, n) - 1) / guess) - fv;
        double f_prime = -pv * n * pow(1 + guess, n - 1) - pmt * ((guess * n * pow(1 + guess, n - 1) - (pow(1 + guess, n) - 1)) / (guess * guess));

        newGuess = guess - f / f_prime;
        diff = newGuess - guess;
        guess = newGuess;
        iterations++;
    }

    if (iterations >= maxIterations)
        throw std::runtime_error("Interest rate calculation did not converge.");

    return guess;
}

// Number of Period calculation using Newton-Raphson
double Calculator::calculateNumberOfPeriods(double pv, double fv, double pmt, double i) const
{
    if (i <= 0) throw std::invalid_argument("Interest rate must be greater than zero.");

    double guess = settingInitialGuessPeriods;
    double diff = 1;
    double newGuess;
    int iterations = 0;
    int maxIterations = 1000;

    while (std::fabs(diff) > settingErrorThreshold && iterations < maxIterations)
    {
        double f = -pv * pow(1 + i, guess) - pmt * ((pow(1 + i, guess) - 1) / i) - fv;
        double f_prime = -pv * log(1 + i) * pow(1 + i, guess) - pmt * pow(1 + i, guess) * log(1 + i) / i;

        newGuess = guess - f / f_prime;
        diff = newGuess - guess;
        guess = newGuess;
        iterations++;
    }

    if (iterations == maxIterations)
        throw std::runtime_error("Number of periods calculation did not converge.");

    return guess;
}


void Calculator::saveHistory(const std::string& inputExpression, 
                              const std::vector<Calculator::Token>& tokens, 
                              const std::vector<Calculator::Token>& rpnExpression, 
                              const std::string& filename, 
                              double result) const
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Failed to open history file.");
    }

    outFile << "Expression: " << inputExpression << "\n";
    outFile << "Tokens: ";
    for (const Token& token : tokens)
    {
        outFile << token.value << " ";
    }
    outFile << "\nRPN: ";
    for (const Token& token : rpnExpression)
    {
        outFile << token.value << " ";
    }
    outFile << "\nResult: " << result << "\n\n";

    outFile.close();
}
