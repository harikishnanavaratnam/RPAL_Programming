#include <iostream>
#include <stack>
#include <iterator>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"
#include "environment.h"

using namespace std;

stack<tree *> st; // Stack for syntax tree

// Array of operators
char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

// Array of binary operators
string binary_operators[] = {"+", "-", "*", "/", "**", "gr", "ge", "<", "<=", ">", ">=", "ls", "le", "eq", "ne", "&", "or", "><"};

// Array of keywords
string keys[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

class parser
{
public:
    token nextToken;     // Next token to be read
    char readnew[10000]; // Stores program for parsing
    int index;           // Index of character
    int sizeOfFile;      // Size of file
    int astFlag;         // Flag to check if AST or ST is to be printed

    parser(char read_array[], int i, int size, int af)
    {
        copy(read_array, read_array + 10000, readnew);
        index = i;
        sizeOfFile = size;
        astFlag = af;
    }

    // Checks if the given string is a keyword
    bool isReservedKey(string str)
    {
        // int size = keys.size();
        int i = 0;
        for (i = 0; i < 20; i++)
        {
            if (str == keys[i])
                return true;
        }
        return false;
    }

    // Checks if the given character is an operator
    bool isOperator(char ch)
    {
        for (int i = 0; i < 25; i++)
        {
            if (ch == operators[i])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given character is an alphabet letter
    bool isAlpha(char ch)
    {
        if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        {
            return true;
        }
        return false;
    }

    // Checks if the given character is a digit
    bool isDigit(char ch)
    {
        if (ch >= 48 && ch <= 57)
        {
            return true;
        }
        return false;
    }

    // Checks if the given string is a binary operator
    bool isBinaryOperator(string op)
    {
        for (int i = 0; i < 18; i++)
        {
            if (op == binary_operators[i])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given string is a number
    bool isNumber(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    // Read the next token
    void read(string val, string type)
    {
        if (val != nextToken.getVal() || type != nextToken.getType()) // Check if the next token is the expected token
        {
            cout << "Parse error: Expected " << "\"" << val << "\"" << ", but " << "\"" << nextToken.getVal() << "\"" << " was there" << endl;
            exit(0);
        }

        if (type == "ID" || type == "INT" || type == "STR") // If the token is an identifier, integer or string
            buildTree(val, type, 0);

        nextToken = getToken(readnew); // Get the next token

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);
    }

    // Build tree for the given string, type and number of children
    void buildTree(string val, string type, int child)
    {
        if (child == 0) // Leaf node
        {
            tree *temp = createNode(val, type);
            st.push(temp);
        }
        else if (child > 0) // Non-leaf node
        {
            stack<tree *> temp;
            int no_of_pops = child;
            while (!st.empty() && no_of_pops > 0)
            {
                temp.push(st.top());
                st.pop();
                no_of_pops--;
            }
            tree *tempLeft = temp.top();
            temp.pop();
            child--;
            if (!temp.empty() && child > 0)
            {
                tree *rightNode = temp.top();
                tempLeft->right = rightNode;
                temp.pop();
                child--;
                while (!temp.empty() && child > 0)
                {
                    tree *addRight = temp.top();
                    temp.pop();
                    rightNode->right = addRight;
                    rightNode = rightNode->right;
                    child--;
                }
            }
            tree *toPush = createNode(val, type);
            toPush->left = tempLeft;
            st.push(toPush);
        }
    }

    // Get the next token
    token getToken(char read[])
    {
        token t;
        int i = index;         // Index of character
        string id = "";        // Identifier
        string num = "";       // Number
        string isop = "";      // Operator
        string isString = "";  // String
        string isPun = "";     // Punctuation
        string isComment = ""; // Comment
        string isSpace = "";   // Space

        // Check if end of file is reached
        if (read[i] == '\0' || i == sizeOfFile)
        {
            t.setType("EOF");
            t.setVal("EOF");
            return t;
        }

        // Read the next token
        while (i < sizeOfFile || i < 10000 || read[i] != '\0')
        {
            // Check if character is a digit
            if (isDigit(read[i]))
            {
                while (isDigit(read[i]))
                {
                    num = num + read[i];
                    i++;
                }
                index = i;
                t.setVal(num);
                t.setType("INT");
                return t;
            }
            // Check if character is an alphabet letter
            else if (isAlpha(read[i]))
            {
                while (isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_')
                {
                    id = id + read[i];
                    i++;
                }

                if (isReservedKey(id))
                {
                    index = i;
                    t.setVal(id);
                    t.setType("KEYWORD");
                    return t;
                }
                else
                {
                    index = i;
                    t.setVal(id);
                    t.setType("ID");
                    return t;
                }
            }
            // Check if character is a comment
            else if (read[i] == '/' && read[i + 1] == '/')
            {
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || read[i] == '\t' || isAlpha(read[i]) || isDigit(read[i]) || isOperator(read[i]))
                {

                    if (read[i] == '\n')
                    {
                        i++;
                        break;
                    }
                    else
                    {
                        isComment = isComment + read[i];
                        i++;
                    }
                }

                index = i;
                t.setVal(isComment);
                t.setType("DELETE");
                return t;
            }
            // Check if character is an operator
            else if (isOperator(read[i]))
            {
                while (isOperator(read[i]))
                {
                    isop = isop + read[i];
                    i++;
                }

                index = i;
                t.setVal(isop);
                t.setType("OPERATOR");
                return t;
            }

            else if (read[i] == '\'')
            {
                isString = isString + read[i];
                i++;
                while (read[i] == '\'' || read[i] == '\\' || read[i] == '(' || read[i] == ')' || read[i] == ';' || read[i] == ',' || read[i] == ' ' || isAlpha(read[i]) || isDigit(read[i]) || read[i] == '_' || isOperator(read[i]))
                {
                    if (read[i] == '\'')
                    {
                        isString = isString + read[i];
                        i++;
                        break;
                    }
                    else if (read[i] == '\\')
                    {
                        isString = isString + read[i];
                        i++;
                        if (read[i] == 't' || read[i] == 'n' || read[i] == '\\' || read[i] == '"')
                        {
                            isString = isString + read[i];
                            i++;
                        }
                        else
                        {
                            i++;
                        }
                    }
                    else
                    {
                        isString = isString + read[i];
                        i++;
                    }
                }
                index = i;
                t.setVal(isString);
                t.setType("STR");
                return t;
            }
            else if (read[i] == ')' || read[i] == '(' || read[i] == ';' || read[i] == ',')
            {
                isPun = isPun + read[i];
                i++;

                index = i;
                t.setVal(isPun);
                t.setType("PUNCTION");
                return t;
            }
            else if (isspace(read[i]))
            {
                while (isspace(read[i]))
                {
                    isSpace = isSpace + read[i];
                    i++;
                }
                index = i;
                t.setVal(isSpace);
                t.setType("DELETE");
                return t;
            }
            else
            {
                string temp = "";
                temp = temp + read[i];
                t.setVal(temp);
                t.setType("UNKNOWN");
                i++;
                index = i;
                return t;
            }
        }

        return t;
    }

    // Start parsing
    void parse()
    {
        nextToken = getToken(readnew);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        procedure_E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = getToken(readnew);

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the abstact syntax tree if the flag is set
            if (astFlag == 1)
                t->print_tree(0);

            // Make the ST
            makeST(t);

            // Print the standardized tree if the flag is set
            if (astFlag == 2)
                t->print_tree(0);

            // Create
            tree *controlStructureArray[200][200];
            // createControlStructures(t, controlStructureArray);

            int size = 0;
            int inner = 0;
            while (controlStructureArray[size][0] != NULL)
                size++;

            vector<vector<tree *> > setOfControlStruct;
            for (int i = 0; i < size; i++)
            {
                vector<tree *> temp;
                for (int j = 0; j < 200; j++)
                {
                    if (controlStructureArray[i][j] != NULL)
                        temp.push_back(controlStructureArray[i][j]);
                }
                setOfControlStruct.push_back(temp);
            }

            // cse_machine(setOfControlStruct);
        }
    }

   // Helper function for makeStandardTree
    void makeST(tree *t)
    {
        makeStandardTree(t);
    }

    // Makes the tree standard
    tree *makeStandardTree(tree *t)
    {
        if (t == NULL)
            return NULL;
        makeStandardTree(t->left);
        makeStandardTree(t->right);

        if (t->getVal() == "let")
        {
            if (t->left->getVal() == "=")
            {
                t->setVal("gamma");
                t->setType("KEYWORD");
                tree *P = createNode(t->left->right);
                tree *X = createNode(t->left->left);
                tree *E = createNode(t->left->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E; 
                tree *lambda = t->left;
                lambda->left = X;
                lambda->left->right = P;
            }
        }
        else if (t->getVal() == "and" && t->left->getVal() == "=")
        {
            tree *equal = t->left;
            t->setVal("=");
            t->setType("KEYWORD");
            t->left = createNode(",", "PUNCTION");
            tree *comma = t->left;
            comma->left = createNode(equal->left);
            t->left->right = createNode("tau", "KEYWORD");
            tree *tau = t->left->right;

            tau->left = createNode(equal->left->right);
            tau = tau->left;
            comma = comma->left;
            equal = equal->right;

            while (equal != NULL)
            {
                comma->right = createNode(equal->left);
                comma = comma->right;
                tau->right = createNode(equal->left->right);
                tau = tau->right;

                equal = equal->right;
            }
        }
        else if (t->getVal() == "where")
        {
            t->setVal("gamma");
            t->setType("KEYWORD");
            if (t->left->right->getVal() == "=")
            {
                tree *P = createNode(t->left);
                tree *X = createNode(t->left->right->left);
                tree *E = createNode(t->left->right->left->right);
                t->left = createNode("lambda", "KEYWORD");
                t->left->right = E;
                t->left->left = X;
                t->left->left->right = P;
            }
        }
        else if (t->getVal() == "within")
        {
            if (t->left->getVal() == "=" && t->left->right->getVal() == "=")
            {
                tree *X1 = createNode(t->left->left);
                tree *E1 = createNode(t->left->left->right);
                tree *X2 = createNode(t->left->right->left);
                tree *E2 = createNode(t->left->right->left->right);
                t->setVal("=");
                t->setType("KEYWORD");
                t->left = X2;
                t->left->right = createNode("gamma", "KEYWORD");
                tree *temp = t->left->right;
                temp->left = createNode("lambda", "KEYWORD");
                temp->left->right = E1;
                temp = temp->left;
                temp->left = X1;
                temp->left->right = E2;
            }
        }
        else if (t->getVal() == "rec" && t->left->getVal() == "=")
        {
            tree *X = createNode(t->left->left);
            tree *E = createNode(t->left->left->right);

            t->setVal("=");
            t->setType("KEYWORD");
            t->left = X;
            t->left->right = createNode("gamma", "KEYWORD");
            t->left->right->left = createNode("YSTAR", "KEYWORD");
            tree *ystar = t->left->right->left;

            ystar->right = createNode("lambda", "KEYWORD");

            ystar->right->left = createNode(X);
            ystar->right->left->right = createNode(E);
        }
        else if (t->getVal() == "function_form")
        {
            tree *P = createNode(t->left);
            tree *V = t->left->right;

            t->setVal("=");
            t->setType("KEYWORD");
            t->left = P;

            tree *temp = t;
            while (V->right->right != NULL)
            {
                temp->left->right = createNode("lambda", "KEYWORD");
                temp = temp->left->right;
                temp->left = createNode(V);
                V = V->right;
            }

            temp->left->right = createNode("lambda", "KEYWORD");
            temp = temp->left->right;

            temp->left = createNode(V);
            temp->left->right = V->right;
        }
        else if (t->getVal() == "lambda")
        {
            if (t->left != NULL)
            {
                tree *V = t->left;
                tree *temp = t;
                if (V->right != NULL && V->right->right != NULL)
                {
                    while (V->right->right != NULL)
                    {
                        temp->left->right = createNode("lambda", "KEYWORD");
                        temp = temp->left->right;
                        temp->left = createNode(V);
                        V = V->right;
                    }

                    temp->left->right = createNode("lambda", "KEYWORD");
                    temp = temp->left->right;
                    temp->left = createNode(V);
                    temp->left->right = V->right;
                }
            }
        }
        else if (t->getVal() == "@")
        {
            tree *E1 = createNode(t->left);
            tree *N = createNode(t->left->right);
            tree *E2 = createNode(t->left->right->right);
            t->setVal("gamma");
            t->setType("KEYWORD");
            t->left = createNode("gamma", "KEYWORD");
            t->left->right = E2;
            t->left->left = N;
            t->left->left->right = E1;
        }

        return NULL;
    }

    /* -------------------------------- Helper Functions -------------------------------- */

    // Arranges nodes in tauNode into a stack
    void arrangeTuple(tree *tauNode, stack<tree *> &res)
    {
        if (tauNode == NULL)
            return;
        if (tauNode->getVal() == "lamdaTuple")
            return;
        if (tauNode->getVal() != "tau" && tauNode->getVal() != "nil")
        {
            res.push(tauNode);
        }
        arrangeTuple(tauNode->left, res);
        arrangeTuple(tauNode->right, res);
    }

    // Adds spaces to the string
    string addSpaces(string temp)
    {
        for (int i = 1; i < temp.length(); i++)
        {
            if (temp[i - 1] == 92 && (temp[i] == 'n'))
            {
                temp.replace(i - 1, 2, "\\\n");
            }
            if (temp[i - 1] == 92 && (temp[i] == 't'))
            {
                temp.replace(i - 1, 2, "\\\t");
            }
        }

        for (int i = 0; i < temp.length(); i++)
        {
            if (temp[i] == 92 || temp[i] == 39)
            {
                temp.replace(i, 1, "");
            }
        }
        return temp;
    }
 
    /* -------------------------------- Grammar Rules -------------------------------- */

    // E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
    void procedure_E()
    {
        // E -> ’let’ D ’in’ E
        if (nextToken.getVal() == "let")
        {
            read("let", "KEYWORD");
            procedure_D();

            read("in", "KEYWORD"); // read in
            procedure_E();

            buildTree("let", "KEYWORD", 2);
        }
        // E -> ’fn’ Vb+ ’.’ E
        else if (nextToken.getVal() == "fn")
        {
            int n = 0;
            read("fn", "KEYWORD");
            do
            {
                procedure_Vb();
                n++;
            } while (nextToken.getType() == "ID" || nextToken.getVal() == "(");
            read(".", "OPERATOR");
            procedure_E();

            buildTree("lambda", "KEYWORD", n + 1);
        }
        // E -> Ew
        else
        {
            procedure_Ew();
        }
    }

    // Ew ->  T ’where’ Dr | T
    void procedure_Ew()
    {
        procedure_T();

        if (nextToken.getVal() == "where") // If next token is 'where', then rule is, Ew ->  T ’where’ Dr | T
        {
            read("where", "KEYWORD");
            procedure_Dr();
            buildTree("where", "KEYWORD", 2);
        }
    }

    // T -> Ta ( ’,’ Ta )+ | Ta
    void procedure_T()
    {
        procedure_Ta();

        int n = 1;
        while (nextToken.getVal() == ",") // Iterate and read ","; Rule is T -> Ta ( ’,’ Ta )+
        {
            n++;
            read(",", "PUNCTION");
            procedure_Ta();
        }
        if (n > 1)
        {
            buildTree("tau", "KEYWORD", n);
        }
    }

    // Ta -> Ta ’aug’ Tc | Tc
    void procedure_Ta()
    {
        procedure_Tc();

        while (nextToken.getVal() == "aug") // If next token is 'aug', then rule is, Ta -> Ta ’aug’ Tc
        {
            read("aug", "KEYWORD");
            procedure_Tc();
            buildTree("aug", "KEYWORD", 2);
        }
    }

    // Tc -> B ’->’ Tc ’|’ Tc | B
    void procedure_Tc()
    {
        procedure_B();

        if (nextToken.getVal() == "->") // If next token is '->', then rule is, Tc -> B ’->’ Tc ’|’ Tc
        {
            read("->", "OPERATOR");
            procedure_Tc();
            read("|", "OPERATOR");
            procedure_Tc();
            buildTree("->", "KEYWORD", 3);
        }
    }

    // B -> B ’or’ Bt | Bt
    void procedure_B()
    {
        procedure_Bt();
        while (nextToken.getVal() == "or") // If next token is 'or', then rule is, B -> B ’or’ Bt
        {
            read("or", "KEYWORD");
            procedure_Bt();
            buildTree("or", "KEYWORD", 2);
        }
    }

    // Bt -> Bt ’&’ Bs | Bs
    void procedure_Bt()
    {
        procedure_Bs();
        while (nextToken.getVal() == "&") // If next token is '&', then rule is, Bt -> Bt ’&’ Bs
        {
            read("&", "OPERATOR");
            procedure_Bs();
            buildTree("&", "KEYWORD", 2);
        }
    }

    // Bs -> ’not’ Bp | Bp
    void procedure_Bs()
    {
        if (nextToken.getVal() == "not") // If next token is 'not', then rule is, Bs -> ’not’ Bp
        {
            read("not", "KEYWORD");
            procedure_Bp();
            buildTree("not", "KEYWORD", 1);
        }
        else // Else the rule is, Bs -> Bp
        {
            procedure_Bp();
        }
    }

    /*
    Bp  -> A (’gr’ | ’>’ )
        -> A (’ge’ | ’>=’)
        -> A (’ls’ | ’<’ )
        -> A (’le’ | ’<=’)
        -> A ’eq’ A
        -> A ’ne’ A
        -> A ;
    */
    void procedure_Bp()
    {
        procedure_A();
        string temp = nextToken.getVal();
        string temp2 = nextToken.getType();

        if (nextToken.getVal() == "gr" || nextToken.getVal() == ">") // Bp -> A (’gr’ | ’>’ )
        {
            read(temp, temp2);
            procedure_A();
            buildTree("gr", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ge" || nextToken.getVal() == ">=") // Bp -> A (’ge’ | ’>=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ge", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ls" || nextToken.getVal() == "<") // Bp -> A (’ls’ | ’<’ )
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ls", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "le" || nextToken.getVal() == "<=") // Bp -> A (’le’ | ’<=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("le", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "eq") // Bp -> A ’eq’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("eq", "KEYWORD", 2);
        }
        else if (nextToken.getVal() == "ne") // Bp -> A ’ne’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ne", "KEYWORD", 2);
        }
    }

    /*
    A   -> A ’+’ At
        -> A ’-’ At
        -> ’+’ At
        -> ’-’ At
        -> At ;
    */
    void procedure_A()
    {
        if (nextToken.getVal() == "+") // A -> ’+’ At
        {
            read("+", "OPERATOR");
            procedure_At();
        }
        else if (nextToken.getVal() == "-") // A -> ’-’ At
        {
            read("-", "OPERATOR");
            procedure_At();
            buildTree("neg", "KEYWORD", 1);
        }
        else
        {
            procedure_At();
        }

        // A   -> A ’+’ At | A ’-’ At
        while (nextToken.getVal() == "+" || nextToken.getVal() == "-")
        {
            string temp = nextToken.getVal();
            read(temp, "OPERATOR");
            procedure_At();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    /*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
    */
    void procedure_At()
    {
        procedure_Af(); // At -> Af

        while (nextToken.getVal() == "*" || nextToken.getVal() == "/") // At -> At ’*’ Af | At ’/’  Af
        {
            string temp = nextToken.getVal();
            read(temp, "OPERATOR");
            procedure_Af();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    /*
    Af -> Ap ’**’ Af
       -> Ap ;
    */
    void procedure_Af()
    {
        procedure_Ap();

        if (nextToken.getVal() == "**") // Af -> Ap ’**’ Af
        {
            read("**", "OPERATOR");
            procedure_Af();
            buildTree("**", "KEYWORD", 2);
        }
    }

    /*
    Ap -> Ap ’@’ ’<IDENTIFIERENTIFIER>’ R
       -> R ;
    */
    void procedure_Ap()
    {
        procedure_R();
        while (nextToken.getVal() == "@")
        {
            read("@", "OPERATOR"); // read a type ID
            if (nextToken.getType() != "ID")
            {
                cout << "Exception: UNEXPECTED_TOKEN";
            }
            else
            {
                read(nextToken.getVal(), "ID");
                procedure_R();
                buildTree("@", "KEYWORD", 3);
            }
        }
    }

    /*
    R -> R Rn
      -> Rn ;
    */
    void procedure_R()
    {
        procedure_Rn();
        while (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR" || nextToken.getVal() == "true" || nextToken.getVal() == "false" || nextToken.getVal() == "nil" || nextToken.getVal() == "(" || nextToken.getVal() == "dummy")
        {
            procedure_Rn();
            buildTree("gamma", "KEYWORD", 2);
        }
    }

    /*
    Rn -> ’<ID>’
        -> ’<INT>’
        -> ’<STR>’
        -> ’true’
        -> ’false’
        -> ’nil’
        -> ’(’ E ’)’
        -> ’dummy’
    */
    void procedure_Rn()
    {
        if (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR") // Rn -> ’<ID>’ | ’<INT>’ | ’<STR>’
        {
            read(nextToken.getVal(), nextToken.getType());
        }
        else if (nextToken.getVal() == "true") // Rn -> ’true’
        {
            read("true", "KEYWORD");
            buildTree("true", "BOOL", 0);
        }
        else if (nextToken.getVal() == "false") // Rn -> ’false’
        {
            read("false", "KEYWORD");
            buildTree("false", "BOOL", 0);
        }
        else if (nextToken.getVal() == "nil") // Rn -> ’nil’
        {
            read("nil", "KEYWORD");
            buildTree("nil", "NIL", 0);
        }
        else if (nextToken.getVal() == "(") // Rn -> ’(’ E ’)’
        {
            read("(", "PUNCTION");
            procedure_E();
            read(")", "PUNCTION");
        }
        else if (nextToken.getVal() == "dummy") // Rn -> ’dummy’
        {
            read("dummy", "KEYWORD");
            buildTree("dummy", "DUMMY", 0);
        }
    }

    /*
    D -> Da ’within’ D => ’within’
      -> Da ;
    */
    void procedure_D()
    {
        procedure_Da();
        if (nextToken.getVal() == "within")
        {
            read("within", "KEYWORD");
            procedure_Da();
            buildTree("within", "KEYWORD", 2);
        }
    }

    /*
    Da -> Dr ( ’and’ Dr )+
       -> Dr ;
    */
    void procedure_Da()
    {
        procedure_Dr();

        int n = 1;
        while (nextToken.getVal() == "and")
        {
            n++;
            read("and", "KEYWORD");
            procedure_Dr();
        }
        if (n > 1)
        {
            buildTree("and", "KEYWORD", n);
        }
    }

    /*
    Dr -> ’rec’ Db
        -> Db ;
    */
    void procedure_Dr()
    {
        if (nextToken.getVal() == "rec")
        {
            read("rec", "KEYWORD");
            procedure_Db();
            buildTree("rec", "KEYWORD", 1);
        }
        else
        {
            procedure_Db();
        }
    }

    /*
   Db -> Vl ’=’ E => ’=’
      -> ’<ID>’ Vb+ ’=’ E => ’fcn_form’
      -> ’(’ D ’)’ ;
    */
    void procedure_Db()
    {
        if (nextToken.getVal() == "(")
        {
            read("(", "PUNCTION");
            procedure_D();
            read(")", "PUNCTION");
        }
        else if (nextToken.getType() == "ID")
        {
            read(nextToken.getVal(), "ID");
            int n = 1;
            if (nextToken.getVal() == "=" || nextToken.getVal() == ",")
            {
                while (nextToken.getVal() == ",")
                {
                    read(",", "PUNCTION");
                    read(nextToken.getVal(), "ID");
                    n++;
                }
                if (n > 1)
                {
                    buildTree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR");
                procedure_E();
                buildTree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    procedure_Vb();
                    n++;
                } while (nextToken.getType() == "ID" || nextToken.getVal() == "(");
                read("=", "OPERATOR");
                procedure_E();
                buildTree("function_form", "KEYWORD", n + 1);
            }
        }
    }

    /*
    Vb -> ’<ID>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’
    */
    void procedure_Vb()
    {
        if (nextToken.getType() == "ID")
        {
            read(nextToken.getVal(), "ID");
        }
        else if (nextToken.getVal() == "(")
        {
            read("(", "PUNCTION");
            if (nextToken.getVal() == ")")
            {
                read(")", "PUNCTION");
                buildTree("()", "KEYWORD", 0);
            }
            else
            {
                procedure_Vl();
                read(")", "PUNCTION");
            }
        }
    }

    /*
    Vl -> ’<ID>’ list ’,’
    */
    void procedure_Vl()
    {
        int n = 1;
        read(nextToken.getVal(), "ID");

        while (nextToken.getVal() == ",")
        {
            read(",", "PUNCTION");
            read(nextToken.getVal(), "ID");
            n++;
        }
        if (n > 1)
        {
            buildTree(",", "KEYWORD", n);
        }
    }
};