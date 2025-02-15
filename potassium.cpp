#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <iomanip>

std::unordered_map<std::string, std::string> variables;

// Trim whitespace from a string
std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Check if a string is a number (integer or float)
bool isNumber(const std::string &s) {
    return std::regex_match(s, std::regex(R"(^-?\d+(\.\d+)?$)"));
}

// Extract the raw value from a variable or return itself if it's a literal
std::string resolveValue(const std::string &input) {
    std::string trimmed = trim(input);
    if (variables.find(trimmed) != variables.end()) {
        return variables[trimmed]; // Return stored variable value
    }
    return trimmed; // Otherwise, return raw value
}

// Properly tokenizes expressions while keeping quoted strings intact
std::vector<std::string> tokenizeExpression(const std::string &expr) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    char quoteType = '\0';

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        if ((c == '"' || c == '\'') && (i == 0 || expr[i - 1] != '\\')) {
            if (inQuotes && c == quoteType) {
                inQuotes = false; // End of quoted string
            } else if (!inQuotes) {
                inQuotes = true;
                quoteType = c; // Start of quoted string
            }
        }

        if (!inQuotes && c == '+') {
            tokens.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        tokens.push_back(trim(current));
    }

    return tokens;
}

// Evaluates expressions while handling numbers & strings properly
std::string evaluateExpression(const std::string &expr) {
    std::vector<std::string> tokens = tokenizeExpression(expr);
    bool isNumeric = true;
    double numericSum = 0.0;
    std::string stringResult;

    for (auto &token : tokens) {
        token = resolveValue(token); // Resolve variable names

        // Check if token is a quoted string
        if ((token.front() == '"' && token.back() == '"') || (token.front() == '\'' && token.back() == '\'')) {
            token = token.substr(1, token.size() - 2); // Remove quotes
            isNumeric = false;
        }

        if (!isNumber(token)) {
            isNumeric = false;
        }
    }

    if (isNumeric) {
        for (const auto &token : tokens) {
            numericSum += std::stod(token);
        }
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(10) << numericSum;
        std::string result = oss.str();
        result.erase(result.find_last_not_of('0') + 1, std::string::npos); // Remove trailing zeros
        if (result.back() == '.') {
            result.pop_back(); // Remove trailing decimal point if present
        }
        return result;
    } else {
        for (const auto &token : tokens) {
            stringResult += token;
        }
        return stringResult;
    }
}

// Interpret and execute a single line of code
void interpret(std::string line) {

    if (line[0] == '/' and line[1] == '/') {
        return; // Ignore comments
    }
    
    // Remove comments
    bool inQuotes = false;
    char quoteType = '\0';
    for (size_t i = 0; i < line.size(); ++i) {
        if ((line[i] == '"' || line[i] == '\'') && (i == 0 || line[i - 1] != '\\')) {
            if (inQuotes && line[i] == quoteType) {
                inQuotes = false;
            } else if (!inQuotes) {
                inQuotes = true;
                quoteType = line[i];
            }
        }
        if (!inQuotes && line[i] == '/' && i + 1 < line.size() && line[i + 1] == '/') {
            line = line.substr(0, i);
            break;
        }
    }

    std::regex printPattern(R"(pln\((.*?)\))"); // Matches pln("text") or pln(variable)
    std::regex varPattern(R"(v\s+(\w+)\s*=\s*(.*))");   // Matches let x = value
    std::smatch match;

    if (std::regex_match(line, match, printPattern)) {
        std::string value = evaluateExpression(match[1]);  // Now evaluates entire expressions
        bool inString = false;
        char stringDelimiter = '\0';
        size_t pos = 0;
        while ((pos = value.find("\\n", pos)) != std::string::npos) {
            value.replace(pos, 2, "\n");
            pos += 1; // Move past the newly inserted newline
        }
        std::cout << value << std::endl;
    } 
    else if (std::regex_match(line, match, varPattern)) {
        std::string varName = match[1];
        std::string varValue = evaluateExpression(match[2]); // Evaluate full expression
        variables[varName] = varValue; // Store the variable
    } 
    else {
        std::cerr << "Syntax Error: " << line << std::endl;
    }
}

// Run the Potassium script
void runScript(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        interpret(line);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 || std::string(argv[1]) != "run") {
        std::cerr << "Usage: potassium run <script.kscript>" << std::endl;
        return 1;
    }

    runScript(argv[2]);
    return 0;
}
