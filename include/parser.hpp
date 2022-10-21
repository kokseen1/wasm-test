#include <sstream>
#include <string>
#include <map>

enum class Token
{
    Eof = -1,
    Number = 0,

    // Descending order of precedence
    Lp = '(',
    Rp = ')',
    Id = '#',
    Incr = '++',
    Decr = '--',

    Mul = '*',
    Div = '/',

    Plus = '+',
    Minus = '-',

    Shl = '<<',
    Shr = '>>',

    Lt = '<',
    Gt = '>',

    Eq = '==',
    Neq = '!=',

    Band = '&',

    Bor = '|',

    And = '&&',

    Or = '||',

    Assign = '=',
};

class Lexer;

class Parser
{
public:
    double parse(std::string &);

    void set_lexer_buffer(std::string &);

private:
    std::string last_var_name; // TODO: Might have better alternative
    Lexer *p_lexer = NULL;
    std::map<std::string, double> symbol_table{};

    double inequality_expr();
    double equality_expr();
    double assign_expr();
    double add_expr();
    double mul_expr();
    double unary_expr();
    double primary();
};

class Lexer
{
    friend void Parser::set_lexer_buffer(std::string &);

private:
    Token current_token;
    std::string buffer;
    std::istringstream iss;

    Token get_next_token();

public:
    Lexer(const std::string);
    Token get_current_token() { return current_token; };
    std::string get_curr_buffer() { return buffer; };
    void advance();
};
