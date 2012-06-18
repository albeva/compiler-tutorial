//
// This is a lexer example from the blog series on
// How to build a compiler with LLVM
//
// You can find more on http://lightbasic.com
//
// Author: Albert Varaksin
// Licence: Public Domain
// This code is provided AS IS. The Author will not be held liable or
// responsible in any shape or form, directly or indirectly, for whatever
// issues, losses or damages using this code might cause.
//
// This code requires C++11 compatible compiler.

#include <iostream>
#include <map>
#include <unordered_map>
using namespace std;


// TokenType enum holds entries for all possible distinct tokens
// in our grammar and special control tokens
enum class TokenType {
    Invalid,        // invalid input. Useful for error handling
    Identifier,     // identifier like: foo or bar
    Assign,         // =
    Multiply,       // *
    Divide,         // /
    Plus,           // +
    Minus,          // -
    Greater,        // >
    GreaterEqual,   // >=
    Equal,          // ==
    LesserEqual,    // <=
    Lesser,         // <
    BraceOpen,      // {
    BraceClose,     // }
    ParenOpen,      // (
    ParenClose,     // )
    Comma,          // ,
    Colon,          // :
    SemiColon,      // ;
    IntegerLiteral, // 1, 23, 435, -12, 0
    FloatLiteral,   // 1.1, -45.2, -.5
    StringLiteral,  // "hello world!"
    Int,            // "int"
    Double,         // "double"
    String,         // "string"
    Function,       // "function"
    Return,         // "return"
    If,             // "if"
    Else,           // "else"
    For,            // "for"
    Continue,       // "continue"
    Break,          // "break"
    EndOfInput      // end of the input. Stop lexing further
};

std::map<TokenType, string> tokenTypeMap = {
    {TokenType::Invalid,        "<Invalid>"},
    {TokenType::Identifier,     "<Identifier>"},
    {TokenType::Assign,         "="},
    {TokenType::Multiply,       "*"},
    {TokenType::Divide,         "/"},
    {TokenType::Plus,           "+"},
    {TokenType::Minus,          "-"},
    {TokenType::Greater,        ">"},
    {TokenType::GreaterEqual,   ">="},
    {TokenType::Equal,          "=="},
    {TokenType::LesserEqual,    "<="},
    {TokenType::Lesser,         "<"},
    {TokenType::BraceOpen,      "{"},
    {TokenType::BraceClose,     "}"},
    {TokenType::ParenOpen,      "("},
    {TokenType::ParenClose,     ")"},
    {TokenType::Comma,          ","},
    {TokenType::Colon,          ":"},
    {TokenType::SemiColon,      ";"},
    {TokenType::IntegerLiteral, "<Integer Literal>"},
    {TokenType::FloatLiteral,   "<Float Literal>"},
    {TokenType::StringLiteral,  "<String Literal>"},
    {TokenType::Int,            "int"},
    {TokenType::Double,         "double"},
    {TokenType::String,         "string"},
    {TokenType::Function,       "function"},
    {TokenType::Return,         "return"},
    {TokenType::If,             "if"},
    {TokenType::Else,           "else"},
    {TokenType::For,            "for"},
    {TokenType::Continue,       "continue"},
    {TokenType::Break,          "break"},
    {TokenType::EndOfInput,     "<End-Of-Input>"}
};


// Define the lookup table for keywords. This map
// matches a keyword string to a TokenType
std::unordered_map<string, TokenType> keywordMap = {
    {"int",         TokenType::Int},
    {"double",      TokenType::Double},
    {"string",      TokenType::String},
    {"function",    TokenType::Function},
    {"return",      TokenType::Return},
    {"if",          TokenType::If},
    {"else",        TokenType::Else},
    {"for",         TokenType::For},
    {"continue",    TokenType::Continue},
    {"break",       TokenType::Break}
};


// Token structure represents a very simple Token that is returned from
// the lexer. This here holds the token TokenType and its textual value
struct Token {
    TokenType   type;
    string      value;
    int         line, column;
};


// Lexer class takes a string literal and breaks it down into
// individual tokens. It filters out white spaces, newlines, comments
// and returns any unexpected input as an Invalid token.
class Lexer
{
public:
    // create new lexer object.
    Lexer(string source) : m_source(source), m_pos(0), m_start(0)
    {}
    
    // get the next token
    Token next()
    {
        // length of the source code.
        auto len = m_source.length();
        
        // return the end token if this is an empty string
        if (len == 0) return { TokenType::EndOfInput, "" };
        
        // loop while current position index is smaller than the
        // length of the input source.
        while (m_pos < len) {
            
            // get current character
            auto ch = m_source[m_pos];
            auto next = m_pos + 1 < m_source.length() ? m_source[m_pos + 1] : '\0';
            
            // Set the current token start position and advance the position
            // to point to the next character
            m_start = m_pos++;
            
            // skip if current character is a white space or a new line
            if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') continue;
            
            // deal with comments. Comments start with //
            // so check first that next character exists and is not the end of
            // source, secondly test that next character is /
            // (m_pos points to the next character already beacues we incremented it)
            if (ch == '/' && m_pos < len && next == '/') {
                // skip everything until we encounter either end of string
                // or a new line
                while (++m_pos < len && m_source[m_pos] != '\n');
                continue;
            }
            
            // if this is a alphabetic character
            if (isalpha(ch)) return identifier();
            
            // is the character a number ?
            if (isnumber(ch)) return number();
            
            // match the possible operator
            // if none match then in default set it to invalid
            TokenType type;
            switch (ch) {
                case '=':
                    // if next char is = then this is == operator
                    // advance position by 1 as well
                    if (next == '=') {
                        type = TokenType::Equal;
                        m_pos++;
                    }
                    else type = TokenType::Assign;
                    break;
                case '*':
                    type = TokenType::Multiply;
                    break;
                case '/':
                    type = TokenType::Divide;
                    break;
                case '+':
                    type = TokenType::Plus;
                    break;
                case '-':
                    type = TokenType::Minus;
                    break;
                case '>':
                    // if next char is = then this is >= operator
                    // advance position by 1 as well
                    if (next == '=') {
                        type = TokenType::GreaterEqual;
                        m_pos++;
                    }
                    else type = TokenType::Greater;
                    break;
                case '<':
                    // if next char is = then this is <= operator
                    // advance position by 1 as well
                    if (next == '=') {
                        type = TokenType::LesserEqual;
                        m_pos++;
                    }
                    else type = TokenType::Lesser;
                    break;
                case '{':
                    type = TokenType::BraceOpen;
                    break;
                case '}':
                    type = TokenType::BraceClose;
                    break;
                case '(':
                    type = TokenType::ParenOpen;
                    break;
                case ')':
                    type = TokenType::ParenClose;
                    break;
                case ',':
                    type = TokenType::Comma;
                    break;
                case ':':
                    type = TokenType::Colon;
                    break;
                case ';':
                    type = TokenType::SemiColon;
                    break;
                default:
                    type = TokenType::Invalid;
                    break;
            }
            return { type, string(m_source, m_start, m_pos - m_start) };
        }
        
        // done iterating through the string. Return EndOfInput Token
        return { TokenType::EndOfInput };
    }
    
private:
    
    // read an identifier from the input string and return it as a token
    Token identifier()
    {
        // read while position is within the string and
        // next character is a alpha numeric
        while (m_pos < m_source.length() && isalnum(m_source[m_pos])) m_pos++;
        string lexeme(m_source, m_start, m_pos - m_start);
        
        // is this a keyword?
        auto it = keywordMap.find(lexeme);
        if (it != keywordMap.end()) return { it->second, lexeme };
        
        // this is identifier
        return { TokenType::Identifier, lexeme };
    }
    
    
    // read a number from the input string and return it as a token
    Token number()
    {
        // read while position is within the string and
        // next character is a number
        while (m_pos < m_source.length() && isnumber(m_source[m_pos])) m_pos++;
        
        // Done. Create new token
        return { TokenType::IntegerLiteral, string(m_source, m_start, m_pos - m_start) };
    }
    
    // hold the source code we are lexing
    string m_source;
    
    // current read position
    int m_pos;
    
    // current token start position
    int m_start;
};


//
// Main entry point
int main(int argc, const char * argv[])
{
    // simple app
    Lexer lexer(
        "function fib(int n) : int {\n"
        "    if (n == 0) return 0;\n"
        "    else if (n == 1) return 1;\n"
        "    return fib(n - 1) + fib(n - 2);\n"
        "}\n"
        "function main() {\n"
        "    print(\"fibonacci 10 = \", fib(10));\n"
        "}"
    );
    
    // iterate through the tokens until we reach the EndOfInput
    // Expected output from this:
    // Identifier : rad
    // Assign : =
    // Identifier : pi
    // Divide : /
    // Number : 180
    for (auto t = lexer.next(); t.type != TokenType::EndOfInput; t = lexer.next()) {
        cout << tokenTypeMap[t.type] << " : " << t.value << '\n';
    }
    
    return 0;
}
