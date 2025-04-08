#include "Calculator.h"

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>

// Validates an integer input from the user for menus selection
int getInteger(const std::string &prompt)
{
    std::string input;
    int value;
    while (true) 
    {
        std::cout << prompt;
        std::getline(std::cin, input);
        try 
        {
            value = std::stoi(input);
            return value;
        } 
        catch (const std::exception&) 
        {
            std::cout << "Invalid input. Please enter a valid integer." << std::endl;
        }
    }
}

// validates a double input from the user for TVM solver inputs
double getDouble(const std::string &prompt) 
{
    std::string input;
    double value;
    while (true) 
    {
        std::cout << prompt;
        std::getline(std::cin, input);
        try
        {
            value = std::stod(input);
            return value;
        } 
        catch (const std::exception&) 
        {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
}

int main()
{
    Calculator calc;
    int menuOption = 0;
    std::string inputExpression = "";

    do
    {
        std::cout << "\n=== Main Menu ===" << std::endl;
        std::cout << "1. Standard Calculator" << std::endl; // Option for standard calculations
        std::cout << "2. Finance TVM Calculator" << std::endl; // Option for financial calculations
        std::cout << "3. Settings Menu" << std::endl; // Option to configure settings
        std::cout << "0. Exit" << std::endl;

        menuOption = getInteger("Select an option: ");

        double result;
        double *ptrResult = &result; // Pointer for prev answer recall

        try
        {
            switch(menuOption)
            {
                case 1:
                    // Standard Calculator Menu
                    std::cout << "Enter expressions ('ans' to recall or 'back' to return): " << std::endl << std::endl;
                    do
                    {
                        inputExpression.clear();
                        std::getline(std::cin, inputExpression);
                        if (inputExpression == "back") break;
                        if (inputExpression.find("ans") != std::string::npos)
                        {
                            std::string prevResult = std::to_string(*ptrResult);
                            int position = 0;
                            while ((position = inputExpression.find("ans", position)) != std::string::npos)
                            {
                                inputExpression.replace(position, 3, prevResult);
                            }
                        }
                        try 
                        {
                            result = calc.evaluateExpression(inputExpression);
                            std::cout << "Result: " << result << std::endl;
                        }
                        catch (const std::invalid_argument& e)
                        {
                            std::cout << "Invalid input: " << e.what() << std::endl;
                        }
                        catch (const std::runtime_error& e)
                        {
                            std::cout << "Calculation error: " << e.what() << std::endl;
                        }
                        catch (const std::exception& e)
                        {
                            std::cout << "Unexpected error: " << e.what() << std::endl;
                        }
                        
                        std::cout << "Enter expression ('ans' to recall or 'back' to return): " << std::endl << std::endl;
                    }
                    while (inputExpression != "back");
                    break;
                case 2:
                    // Finance TVM Calculator Menu
                {
                    int tvmOption = 0;
                    do 
                    {
                        std::cout << "\n=== TVM Solver Menu ===" << std::endl;
                        std::cout << "1. Calculate Future Value (FV)" << std::endl;
                        std::cout << "2. Calculate Present Value (PV)" << std::endl;
                        std::cout << "3. Calculate Payment (PMT)" << std::endl;
                        std::cout << "4. Calculate Interest Rate (I/Y)" << std::endl;
                        std::cout << "5. Calculate Number of Periods (N)" << std::endl;
                        std::cout << "0. Return to Main Menu" << std::endl;
                        
                        tvmOption = getInteger("Select a TVM option: ");
                
                        try {
                            switch (tvmOption) 
                            {
                                case 1: 
                                { // Future Value (FV)
                                    double pv = getDouble("Enter Present Value (PV): ");
                                    double pmt = getDouble("Enter Payment (PMT): ");
                                    double i = getDouble("Enter Interest Rate (I/Y): % ") / 100.0;
                                    double n = getDouble("Enter Number of Periods (N): ");
                                    std::cout << "Future Value (FV): " << calc.calculateFV(pv, pmt, i, n) << std::endl;
                                    break;
                                }
                                case 2: 
                                { // Present Value (PV)
                                    double fv = getDouble("Enter Future Value (FV): ");
                                    double pmt = getDouble("Enter Payment (PMT): ");
                                    double i = getDouble("Enter Interest Rate (I/Y): % ") / 100.0;
                                    double n = getDouble("Enter Number of Periods (N): ");
                                    std::cout << "Present Value (PV): " << calc.calculatePV(fv, pmt, i, n) << std::endl;
                                    break;
                                }
                                case 3: 
                                { // Payment (PMT)
                                    double pv = getDouble("Enter Present Value (PV): ");
                                    double fv = getDouble("Enter Future Value (FV): ");
                                    double i = getDouble("Enter Interest Rate (I/Y): % ") / 100.0;
                                    double n = getDouble("Enter Number of Periods (N): ");
                                    std::cout << "Payment (PMT): " << calc.calculatePMT(pv, fv, i, n) << std::endl;
                                    break;
                                }
                                case 4: 
                                { // Interest Rate (I/Y)
                                    double pv = getDouble("Enter Present Value (PV): ");
                                    double fv = getDouble("Enter Future Value (FV): ");
                                    double pmt = getDouble("Enter Payment (PMT): ");
                                    double n = getDouble("Enter Number of Periods (N): ");
                                    double interest = calc.calculateInterest(pv, fv, pmt, n);
                                    std::cout << "I/Y (%): " << interest * 100 << "%" << std::endl;
                                    break;
                                }
                                case 5: 
                                { // Number of Periods (N)
                                    double pv = getDouble("Enter Present Value (PV): ");
                                    double fv = getDouble("Enter Future Value (FV): ");
                                    double pmt = getDouble("Enter Payment (PMT): ");
                                    double i = getDouble("Enter Interest Rate (I/Y): % ") / 100.0;
                                    std::cout << "Number of Periods (N): " << calc.calculateNumberOfPeriods(pv, fv, pmt, i) << std::endl;
                                    break;
                                }
                                case 0:
                                    std::cout << "Returning to Main Menu." << std::endl;
                                    break;
                                default:
                                    std::cout << "Invalid TVM option. Please try again." << std::endl;
                            }
                        } 
                        catch (const std::invalid_argument& e) {
                            std::cout << "Invalid input: " << e.what() << std::endl;
                        } 
                        catch (const std::runtime_error& e) {
                            std::cout << "Calculation error: " << e.what() << std::endl;
                        } 
                        catch (const std::exception& e) {
                            std::cout << "Unexpected error: " << e.what() << std::endl;
                        }
                    } 
                    while (tvmOption != 0);
                    break;
                }
                case 3:
                    // Settings Menu
                {
                    int settingsMenuOption = 0;
                    do 
                    {
                        std::cout << "\n=== Settings Menu ===" << std::endl;
                        std::cout << "1. Toggle Radian/Degree Mode (Current: " << (calc.settingRadianMode ? "Radian" : "Degree") << ")" << std::endl;
                        std::cout << "2. Toggle Save History (Current: " << (calc.settingSaveHistory ? "Enabled" : "Disabled") << ")" << std::endl;
                        std::cout << "3. Set Taylor Series Terms (Current: " << calc.settingTaylorTerms << ")" << std::endl;
                        std::cout << "4. Set Initial Guess for Interest Rate (Current: " << calc.settingInitialGuessInterest << ")" << std::endl;
                        std::cout << "5. Set Initial Guess for Number of Periods (Current: " << calc.settingInitialGuessPeriods << ")" << std::endl;
                        std::cout << "6. Set Error Threshold (Current: " << calc.settingErrorThreshold << ")" << std::endl;
                        std::cout << "0. Return to Main Menu" << std::endl;

                        settingsMenuOption = getInteger("Select an option: ");

                        switch (settingsMenuOption)
                        {
                            case 1:
                                calc.settingRadianMode = !calc.settingRadianMode;
                                std::cout << "Radian/Degree mode toggled to: " << (calc.settingRadianMode ? "Radian" : "Degree") << std::endl;
                                break;
                            case 2:
                                calc.settingSaveHistory = !calc.settingSaveHistory;
                                std::cout << "Save History toggled to: " << (calc.settingSaveHistory ? "Enabled" : "Disabled") << std::endl;
                                break;
                            case 3:
                            {
                                int terms = getInteger("Enter the number of Taylor Series terms (minimum 1): ");
                                if (terms > 0)
                                {
                                    calc.settingTaylorTerms = terms;
                                    std::cout << "Taylor Series terms set to: " << terms << std::endl;
                                }
                                else
                                {
                                    std::cout << "Invalid input. Taylor Series terms must be at least 1." << std::endl;
                                }
                                break;
                            }
                            case 4:
                            {
                                double initialGuessInterest = getDouble("Enter the initial guess for interest rate calculations (e.g., 0.05 for 5%): ");
                                if (initialGuessInterest > 0)
                                {
                                    calc.settingInitialGuessInterest = initialGuessInterest;
                                    std::cout << "Initial guess for interest rate set to: " << initialGuessInterest << std::endl;
                                }
                                else
                                {
                                    std::cout << "Invalid input. Initial guess must be greater than 0." << std::endl;
                                }
                                break;
                            }
                            case 5:
                            {
                                double initialGuessPeriods = getDouble("Enter the initial guess for number of periods calculations (e.g., 10): ");
                                if (initialGuessPeriods > 0)
                                {
                                    calc.settingInitialGuessPeriods = initialGuessPeriods;
                                    std::cout << "Initial guess for number of periods set to: " << initialGuessPeriods << std::endl;
                                }
                                else
                                {
                                    std::cout << "Invalid input. Initial guess must be greater than 0." << std::endl;
                                }
                                break;
                            }
                            case 6:
                            {
                                double errorThreshold = getDouble("Enter the error threshold for calculations (e.g., 0.0000001 or 1.0E-7): ");
                                if (errorThreshold > 0)
                                {
                                    calc.settingErrorThreshold = errorThreshold;
                                    std::cout << "Error threshold set to: " << errorThreshold << std::endl;
                                }
                                else
                                {
                                    std::cout << "Invalid input. Error threshold must be greater than 0." << std::endl;
                                }
                                break;
                            }
                            case 0:
                                std::cout << "Returning to Main Menu." << std::endl;
                                break;
                            default:
                                std::cout << "Invalid option. Please try again." << std::endl;
                        }
                    } 
                    while (settingsMenuOption != 0);
                    break;
                }
                case 0:
                    std::cout << "Exiting program." << std::endl;
                    break;
                default:
                    std::cout << "Invalid option. Please try again." << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "An unexpected error occurred: " << e.what() << std::endl;
        }

        std::cout << std::endl;
    }
    while(menuOption != 0);

    return 0;
}
