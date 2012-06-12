//
// This is a lexer example from the blog series on
// How to build a compiler with LLVM
//
// You can find more on http://lightbasic.com
//
// Author: Albert Varaksin
// Licence: Public Domain.
// Author of this code will not be held liable nor responsible in any shape or
// form, directly or indirectly, for whatever issues, losses or damages using
// this source code might cause you.
//
// This code assumes c++11 compatible compiler.

#include <iostream>
#include <map>
using namespace std;


// Kind enum holds entries for all possible distinct tokens
// in our grammar and special control tokens
enum class Kind {
    Invalid,        // invalid input. Useful for error handling
    Identifier,     // identifier like: foo or bar
    Divide,         // operator /
    Assign,         // operator =
    Number,         // a number: 1, 23, 435
    EndOfInput      // end of the input. Stop lexing further
};

// This is a very simple helper for debugging and textual output of the Tokens
// it maps the Kind enum values above to string values
std::map<Kind, string> kindMap = {
    {Kind::Invalid,     "Invalid"},
    {Kind::Identifier,  "Identifier"},
    {Kind::Divide,      "Divide"},
    {Kind::Assign,      "Assign"},
    {Kind::Number,      "Number"},
    {Kind::EndOfInput,  "End-Of-Input"}
};


// Token structure represents a very simple Token that is returned from
// the lexer. This here holds the token Kind and its textual value
struct Token {
    Kind    kind;
    string  value;
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
        if (len == 0) return { Kind::EndOfInput, "" };
        
        // loop while current position index is smaller than the
        // length of the input source.
        while (m_pos < len) {
            
            // get current character
            auto ch = m_source[m_pos];
            
            // Set the current token start position and advance the position
            // to point to the next character
            m_start = m_pos++;
            
            // skip if current character is a white space or a new line
            if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') continue;
            
            // deal with comments
            if (ch == '/' && m_pos < len && m_source[m_pos] == '/') {
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
            Kind kind;
            switch (ch) {
                case '=':
                    kind = Kind::Assign;
                    break;
                case '/':
                    kind = Kind::Divide;
                    break;
                default:
                    kind = Kind::Invalid;
                    break;
            }
            return { kind, string(m_source, m_start, 1) };
        }
        
        // done iterating through the string. Return EndOfInput Token
        return { Kind::EndOfInput, "" };
    }
    
private:
    
    // read an identifier from the input string and return it as a token
    Token identifier()
    {
        // read while position is within the string and
        // next character is a alpha numeric
        while (m_pos < m_source.length() && isalnum(m_source[m_pos])) m_pos++;
        
        // Done. Create new token
        return { Kind::Identifier, string(m_source, m_start, m_pos - m_start) };
    }
    
    
    // read a number from the input string and return it as a token
    Token number()
    {
        // read while position is within the string and
        // next character is a number
        while (m_pos < m_source.length() && isnumber(m_source[m_pos])) m_pos++;
        
        // Done. Create new token
        return { Kind::Number, string(m_source, m_start, m_pos - m_start) };
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
    // create the lexer object and pass in a very simple expression
    // expression has 2 identifiers (rad, pi), 2 operators (=, /), a number(180)
    // and a comment starting with // and ends with a new line.
    Lexer lexer("rad = // calculate 1 radii\n"
                "pi / 180");
    
    // iterate through the tokens until we reach the EndOfInput
    // Expected output from this:
    // Identifier : rad
    // Assign : =
    // Identifier : pi
    // Divide : /
    // Number : 180
    for (auto t = lexer.next(); t.kind != Kind::EndOfInput; t = lexer.next()) {
        cout << kindMap[t.kind] << " : " << t.value << '\n';
    }
    
    return 0;
}
