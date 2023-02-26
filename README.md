# Parser-Project
This project was done for a Theory of Programming Languages class.

The aim of it is to parse an input string based on a given set of regular expressions. The program is given a text file as input, which consists of the regular expressions separated by commas, and input text, with the two components separated by a hash symbol.

The lexical analyzer, which scans the characters of the input and matches them to token types, was written and provided to me by my professor, Rida Bazzi. 

# PARSING THE REGULAR EXPRESSIONS #

To explain how the program works, a bit of context is due. In the parser.cpp file, I defined two structs: a REG and a REG_node. The REG is a graph which represents the regular expressions, and consists of REG_nodes connected by "links" (pointers to each other). Each REG_node represents a stage in the regular expression, and each REG represents a whole regular expression. For example, if I were to define a regular expression which tokenizes the character "a" or "b"

t1, (a)|(b)

This regular expression's first node would have two links, one with label "a" and the other with label "b", each pointing to different nodes. Hence, based on the scanned character, the corresponding node is reached. This character scanning and matching is done by the function match_one_char, which I will elaborate on later.

The parser is defined in a way to be able to recognize the OR symbol (which is a "|"), the concatenation symbol (which is a "."), and the Kleene star symbol (which is a star). For context, the Kleene star placed after an expression means that the token will recognize zero or more instances of the expression. 

# PARSING THE INPUT TEXT #

After having parsed the first part of the input and having created and stored the regular expressions, the program moves to parsing the input text. This is done by instanatiating a static integer p, which serves to keep track of the input text's index. This is done by the my_LexicalAnalyzer object, which calls the my_GetToken function repetitively until the string is not matchable to any expression or the string is completely consumed. Starting from p, the match function (which is called by my_GetToken) returns the furthest index in the input string that can be matched to a regular expression. The match function, in turn, calls match_one_char over every character in the string and checks for a reachable node from its consumption. 

Errors are also accounted for, such as 
- A regular expression generating epsilon as a token, which in this project is represented by the underscore symbol.
- Parsing errors in the regular expressions, such as a left parenthesis not matched with a corresponding right one, or an illegal character/symbol used. 
- Syntax errors in the input text, which is when a piece of the input text cannot be matched to a regular expression. 
- Duplicate regular expression names, which is when two separated regular expressions are given the same ID. 

Hope you enjoy this projet. 
