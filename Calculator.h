#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <vector>

class Calculator
{
public: 
    // Settings
    bool settingRadianMode;
    bool settingSaveHistory;
    int settingTaylorTerms;
    double settingInitialGuessInterest;
    double settingInitialGuessPeriods;
    double settingErrorThreshold;
    // Constructor with defaults
    Calculator(bool radianMode = true, bool saveHistory = false, int taylorTerms = 10, double initialGuessInterest = .05, double initialGuessPeriods = 10, double errorThreshold = 1e-28);

    double evaluateExpression(const std::string& inputExpression) const; // tokenize -> convertToRPN -> evaluateRPN

    // Finance Calculator Time Value of Money (TVM) Solver
    // n = number of periods, i = interest rate per period, pv = present value, pmt = payment, fv = future value
    double calculateFV(double pv, double pmt, double i, double n) const;
    double calculatePV(double fv, double pmt, double i, double n) const;
    double calculatePMT(double pv, double fv, double i, double n) const;
    // uses iterative method with Newton-Raphson formula
    double calculateInterest(double pv, double fv, double pmt, double n) const; 
    double calculateNumberOfPeriods(double pv, double fv, double pmt, double i) const;

private:
    enum TokenType
    {
        NUMBER,
        OPERATOR,
        FUNCTION,
        LEFT_PAREN,
        RIGHT_PAREN
    };

    struct Token
    {
        TokenType type;
        std::string value;        
        // Constructor initializes token with TokenType and value
        Token(TokenType type, const std::string& value) : type(type), value(value) {}
    };

    // Main Functions to process input expression
    std::vector<Token> tokenize(const std::string& inputExpression) const; // Converts input string to tokens for Shunting Yard algorith
    std::vector<Token> convertToRPN(const std::vector<Token>& tokenExpression) const; // Shunting Yard algorith to produce Reverse Polish Notation (RPN)
    double evaluateRPN(const std::vector<Token>& rpnExpression) const; // Evaluates the Reverse Polish Notation expression

    // Helper Functions to process input expression
    bool isLeftAssociative(const std::string& op) const;
    int getPrecedence(const std::string& op) const;

    // Trigonometric and Mathematical Helper Functions
    double factorial(const int n) const;
    double reduceAngle(const double angle) const;  // Reduces angle to [-π, π] range
    
    // Trigonometric Functions using Taylor Series approximation
    double calcSin(const double angle) const;  
    double calcCos(const double angle) const;  
    double calcTan(const double angle) const;  

    // Save history to a file
    void saveHistory(const std::string& inputExpression, 
                     const std::vector<Token>& tokens, 
                     const std::vector<Token>& rpnExpression, 
                     const std::string& filename, 
                     double result) const;
};

#endif