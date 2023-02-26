 /* This code was written by Alex Juvelekian. I put my heart and soul into this project, enjoy. */

#include "lexer.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

// Below, you can find the definition of the new token object (which the parser returns), 
//the REG and REG nodes (which are used to represent the regular expressions)
//and the lexer object I defined, which implements functions like my GetToken

class my_Token {
public:
    string token_lexeme;
    string token_type;
    int line_no;
};

class REG_node {
public:
    struct REG_node* first_neighbor;
    char first_label;
    struct REG_node* second_neighbor;
    char second_label;
    REG_node() {
        this->first_label = ';';
        this->first_neighbor = NULL;
        this->second_label = ';';
        this->second_neighbor = NULL;
    }
};

class REG {
public:
    struct REG_node* start;
    struct REG_node* end;

    REG() {
        this->start = new REG_node();
        this->end = new REG_node();
    }
};


class my_LexicalAnalyzer {
public:
    void syntax_error() {
        cout << "SYNTAX ERROR\n";
        exit(0);
    };

    my_LexicalAnalyzer(vector<pair<string, REG*>> list, string s) {
        this->list_of_expr = list;
        this->input_text = s;
    };
    // This function takes a vector of strings which contains the ID's of REGs that can generate an epsilon token. 
    // It is called in the main in the case that the vector of epsilone REGs is not empty.
    void epsilon_error(vector<string> name) {
        string names1 = "";
        for (string i : name) {
            names1 += " " + i;
        }
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!!" + names1;
        exit(0);
    };
    // This error is called in case of an error while parsing a regular expression.
    void parsing_error(string token_name) {
        cout << "SYNTAX ERROR IN EXPRESSION OF " + token_name;
        exit(0);
    };
    // In my parser object, I instantiated a vector of strings (which all print in case of a duplicate name error). This function creates a string and
    // pushes it back to this vector in case a duplicate is found. 
    string duplicate_name_error(int line1, int line2, string token_name) {
        string error = "Line " + to_string(line1) + ": " + token_name + " already declared on line " + to_string(line2);
        return error;
    };
    // I created this function to search a vector for a node, as I used it in several later functions.
    bool searchVect(vector<REG_node*> list1, REG_node* node) {
        for (int i = 0; i < list1.size(); i++) {
            if (list1[i] == node) {
                return true;
            }
        }
        return false;
    }

    // Below is my getToken function. Starting from index p (which I store as a static integer in the my_LexicalAnalyzer object), the function returns
    // the next token in the input string (if there is one). It uses match to do so.

    my_Token my_GetToken() {
        while (input_text[p] == ' ' && p < input_text.size()) {
            p++;
        }
        if (p == input_text.size()) {
            exit(0);
        }
        int max = 0;
        bool error = true;
        pair<string, REG*> a;
        for (pair<string, REG*> reg : list_of_expr) {
            int new_p = match(reg.second, input_text, p);
            if (new_p > max) {
                if (new_p != p) {
                    error = false;
                }
                max = new_p;
                a = reg;
            }
            if (new_p == -1 && max == 0) {
                cout << "ERROR";
                exit(0);
            }
        }
        if (input_text[p] == ' ') {
            error = false;
        }
        if (input_text[p] == '"') {
            exit(0);
        }
        if (error) {
            cout << "ERROR";
            exit(0);
        }
        my_Token* tok = new my_Token();
        tok->token_lexeme = input_text.substr(p, max - p);
        tok->token_type = a.first;
        p = max;
        my_Token tok1 = *tok;
        return tok1;
    };

    int p_getter() {
        return p;
    }

    string input_text;

    vector<pair<string, REG*>> list_of_expr;

    vector<my_Token> list_of_tokens;
    int p = 0;

    // Below is match_one_char, which is used by match to find the nodes that can be reached by consuming the character of the input string 
    // at index p. Therefore, it is called over all characters of a series of characters until an empty space or EOF is found. 
    vector<REG_node*> Match_One_Char(vector<REG_node*> v, char c) {
        vector<REG_node*> new_vect;
        for (REG_node* node : v) {
            if (node->first_label == c && searchVect(new_vect, node->first_neighbor) == false) {
                new_vect.push_back(node->first_neighbor);
            }
            if (node->second_label == c && searchVect(new_vect, node->second_neighbor) == false) {
                new_vect.push_back(node->second_neighbor);
            }
        }

        bool changed = true;
        while (changed) {
            changed = false;
            vector<REG_node*> new_new;
            for (REG_node* node : new_vect) {
                new_new.push_back(node);
                if (node->first_label == '_' && searchVect(new_new, node->first_neighbor) == false && searchVect(new_vect, node->first_neighbor) == false) {
                    new_new.push_back(node->first_neighbor);
                }
                if (node->second_label == '_' && searchVect(new_new, node->second_neighbor) == false && searchVect(new_vect, node->second_neighbor) == false) {
                    new_new.push_back(node->second_neighbor);
                }
            }
            if (new_new.size() != new_vect.size()) {
                new_vect = new_new;
                changed = true;
                new_new.clear();
            }

        }
        return new_vect;
    };

    // This function is called in the main over all REGs to check if a given REG generates the epsilon token. 
    //It is called over all REGs before getting tokens.
    bool eps_error_checker(REG* r) {
        vector<REG_node*> start_nodes;
        start_nodes.push_back(r->start);
        bool changed = true;
        vector<REG_node*> newnew;
        while (changed) {
            changed = false;
            for (REG_node* j : start_nodes) {
                newnew.push_back(j);
                if (j->first_label == '_' && searchVect(newnew, j->first_neighbor) == false && searchVect(start_nodes, j->first_neighbor) == false) {
                    if (j->first_neighbor == r->end) {
                        return true;
                    }
                    newnew.push_back(j->first_neighbor);
                }
                if (j->second_label == '_' && searchVect(newnew, j->second_neighbor) == false && searchVect(start_nodes, j->second_neighbor) == false) {
                    if (j->second_neighbor == r->end) {
                        return true;
                    }
                    newnew.push_back(j->second_neighbor);
                }

            }
            if (newnew.size() > start_nodes.size()) {
                changed = true;
                start_nodes = newnew;
                newnew.clear();
            }
        }
        return false;
    };

    //Below is the match function. given a starting point alt_p (which in practice is the static variable p), it returns an int "save"
    //which is the furthest index for which the string matches to a regular expression.

    int match(REG* r, string s, int alt_p) {
        vector<REG_node*> start_nodes;
        start_nodes.push_back(r->start);
        bool changed = true;
        vector<REG_node*> newnew;
        while (changed) {
            changed = false;
            for (REG_node* j : start_nodes) {
                newnew.push_back(j);
                if (j->first_label == '_' && searchVect(newnew, j->first_neighbor) == false && searchVect(start_nodes, j->first_neighbor) == false) {
                    if (j->first_neighbor == r->end) {
                        cout << "REG has eps error";
                        exit(0);
                    }
                    newnew.push_back(j->first_neighbor);
                }
                if (j->second_label == '_' && searchVect(newnew, j->second_neighbor) == false && searchVect(start_nodes, j->second_neighbor) == false) {
                    if (j->second_neighbor == r->end) {
                        cout << "REG has eps error";
                        exit(0);
                    }
                    newnew.push_back(j->second_neighbor);
                }

            }
            if (newnew.size() > start_nodes.size()) {
                changed = true;
                start_nodes = newnew;
                newnew.clear();
            }
        }
        int save = alt_p;
        bool check = false;
        if (s[alt_p] == ' ' || s[alt_p] == '"') {
            check = true;
        }
        while (start_nodes.size() != 0 && s[alt_p] != ' ' && s[alt_p] != '"' && alt_p < s.size()) {
            start_nodes = Match_One_Char(start_nodes, s[alt_p]);
            if (searchVect(start_nodes, r->end)) {
                check = true;
                save = alt_p + 1;
            }
            if (start_nodes.size() > 0) {
                alt_p++;
            }
        }
        if (s[alt_p] == ' ' || s[alt_p] == '"') {
            check = true;
        }
        return save;

    };
};


// Below is the parser object and its defined functions. I created a make_star (takes one REG), make_or (takes two REGs), 
// and concatenate (takes two REGs) function which each do what their name says. 

class Parser {
public:

    string the_error = " ";
    vector<string> dups;
    pair<string, int> the_dup = { " ",-1 };
    vector<pair<string, int>> ID_list;
    vector<pair<string, REG*>> list_of_expr;
    string input_text;
    LexicalAnalyzer lexer = LexicalAnalyzer();
    vector<string> token_names;
    Token expect(TokenType expected_type) {
        Token t = lexer.GetToken();
        if (t.token_type != expected_type) {
            syntax_error();
        }
        return t;
    };
    void parse_input() {
        parse_tokens_section();
        Token t = expect(INPUT_TEXT);
        input_text = t.lexeme.substr(1, t.lexeme.size() - 1);
        if (lexer.peek(1).token_type == END_OF_FILE) {
            expect(END_OF_FILE);
        }
        else {
            syntax_error();
        }

    };
    REG* make_star(REG* a) {
        REG* result = new REG();
        result->start->first_label = '_';
        result->start->first_neighbor = a->start;
        result->start->second_label = '_';
        result->start->second_neighbor = result->end;

        a->end->first_label = '_';
        a->end->first_neighbor = a->start;
        a->end->second_label = '_';
        a->end->second_neighbor = result->end;

        return result;
    }

    REG* make_OR(REG* a, REG* b) {
        REG* result = new REG();
        result->start->first_label = '_';
        result->start->first_neighbor = a->start;
        result->start->second_label = '_';
        result->start->second_neighbor = b->start;

        a->end->first_label = '_';
        a->end->first_neighbor = result->end;
        b->end->first_label = '_';
        b->end->first_neighbor = result->end;

        return result;
    }

    REG* concatenate(REG* a, REG* b) {
        REG* result = new REG();
        result->start->first_label = '_';
        result->start->first_neighbor = a->start;
        a->end->first_label = '_';
        a->end->first_neighbor = b->start;
        b->end->first_label = '_';
        b->end->first_neighbor = result->end;

        return result;
    }



    void parse_tokens_section() {
        parse_token_list();
        Token t = lexer.peek(1);
        if (t.token_type == HASH) {
            expect(HASH);
        }
        else {
            syntax_error();
        }


    };


    void parse_token_list() {
        parse_token();
        Token t = lexer.peek(1);
        if (t.token_type == COMMA) {
            expect(COMMA);
            parse_token_list();
        }
        else if (t.token_type == HASH) {
            return;
        }
        else {
            syntax_error();
        }

    };


    void parse_token() {
        Token t = lexer.peek(1);
        if (t.token_type == ID) {
            expect(ID);
        }
        else {
            syntax_error();
        }

        token_names.push_back(t.lexeme);
        bool check = false;
        for (pair<string, int> i : ID_list) {
            if (i.first == t.lexeme) {
                dups.push_back(duplicate_name_error(t.line_no, i.second, t.lexeme));
                check = true;
            }
        }
        pair<string, int> a;
        a.first = t.lexeme;
        a.second = t.line_no;
        if (check == false) {
            ID_list.push_back(a);
        }

        REG* y = parse_expr();
        if (y->start->first_label == '+' && y->end->second_label == '+') {
            parsing_error(t.lexeme);
        }
        pair<string, REG*> b;
        b.first = t.lexeme;
        b.second = y;
        list_of_expr.push_back(b);
    };

    struct REG* parse_expr() {
        REG* graph = new REG();
        Token t = lexer.peek(1);
        if (t.token_type == CHAR) {
            Token x = expect(CHAR);
            graph->start->first_label = x.lexeme[0];
            graph->start->first_neighbor = graph->end;
            return graph;
        }
        else if (t.token_type == UNDERSCORE) {
            Token x = expect(UNDERSCORE);
            graph->start->first_label = '_';
            graph->start->first_neighbor = graph->end;
            return graph;
        }
        else if (t.token_type == LPAREN) {
            expect(LPAREN);
            graph = parse_expr();
            if (graph->start->first_label == '+' && graph->end->second_label == '+') {
                parsing_error(token_names[token_names.size() - 1]);
            }
            Token t = lexer.peek(1);
            if (t.token_type == RPAREN) {
                expect(RPAREN);
            }
            else {
                parsing_error(token_names[token_names.size() - 1]);
            }
            Token t1 = lexer.peek(1);
            if (t1.token_type == STAR) {
                expect(STAR);
                return make_star(graph);
            }
            else if (t1.token_type == OR) {
                expect(OR);
                Token t3 = lexer.peek(1);
                if (t3.token_type == LPAREN) {
                    expect(LPAREN);
                }
                else {
                    parsing_error(token_names[token_names.size() - 1]);
                }

                REG* graph2 = parse_expr();
                if (graph2->start->first_label == '+' && graph2->end->second_label == '+') {
                    parsing_error(token_names[token_names.size() - 1]);
                }
                Token t2 = lexer.peek(1);
                if (t2.token_type == RPAREN) {
                    expect(RPAREN);
                }
                else {
                    parsing_error(token_names[token_names.size() - 1]);
                }
                return make_OR(graph, graph2);
            }
            else if (t1.token_type == DOT) {
                expect(DOT);
                Token t3 = lexer.peek(1);
                if (t3.token_type == LPAREN) {
                    expect(LPAREN);
                }
                else {
                    parsing_error(token_names[token_names.size() - 1]);
                }

                REG* graph2 = parse_expr();
                if (graph2->start->first_label == '+' && graph2->end->second_label == '+') {
                    parsing_error(token_names[token_names.size() - 1]);
                }
                Token t2 = lexer.peek(1);
                if (t2.token_type == RPAREN) {
                    expect(RPAREN);
                }
                else {
                    parsing_error(token_names[token_names.size() - 1]);
                }
                return concatenate(graph, graph2);
            }
            else
                parsing_error(token_names[token_names.size() - 1]);
            // In the case where a right parenthesis is not followed by a star, OR sign, or dot (concatenation), the REG is incorrect and the 
            // function returns a parsing error. 

            // Furthermore, whenever an ID is consumed, its lexeme is pushed back to the vector "token_names", therefore whenever an REG is wrong, its 
            // ID will be the last element pushed to the vector. 
        }
        else {
            parsing_error(token_names[token_names.size() - 1]);
            REG_node* start1 = new REG_node();
            REG_node* end1 = new REG_node();
            start1->first_label = '+';
            end1->second_label = '+';
            graph->start = start1;
            graph->end = end1;
        }
        return graph;
    };

    void syntax_error() {
        cout << "SNYTAX ERORR\n";
        exit(0);
    };

    void epsilon_error(vector<string> name) {
        string names1 = "";
        for (string i : name) {
            names1 += " " + i;
        }
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!!" + names1;
        exit(0);
    };

    void parsing_error(string token_name) {
        cout << "SYNTAX ERROR IN EXPRESSION OF " + token_name;
        exit(0);
    };

    string duplicate_name_error(int line1, int line2, string token_name) {
        string error = "Line " + to_string(line1) + ": " + token_name + " already declared on line " + to_string(line2);
        return error;
    };
};
int main()
{
    Parser parser;
    parser.parse_input();
    bool check = true;
    if (parser.dups.size() > 0) {
        for (string i : parser.dups) {
            cout << i;
            cout << "\n";
        }
        exit(0);
    }
    vector<string> eps_error;
    my_LexicalAnalyzer my_lexer = my_LexicalAnalyzer(parser.list_of_expr, parser.input_text);
    for (pair<string, REG*> i : my_lexer.list_of_expr) {
        if (my_lexer.eps_error_checker(i.second) == true) {
            eps_error.push_back(i.first);
        }
    }

    if (eps_error.size() > 0) {
        parser.epsilon_error(eps_error);
    }

    // After having checked for errors, the main function keeps on calling get token until p reaches the end of the input.
    my_Token tok = my_lexer.my_GetToken();
    while (my_lexer.p < my_lexer.input_text.size()) {
        cout << tok.token_type + ", " + "\"" + tok.token_lexeme + "\" " + "\n";
        my_lexer.list_of_tokens.push_back(tok);
        tok = my_lexer.my_GetToken();

    }

    return 0;

};



