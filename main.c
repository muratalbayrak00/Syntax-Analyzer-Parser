#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000

bool ERROR = false;

// Token struct definition
typedef struct {
    char *type;
    char *value;
    int line;
    int col;
} Token;

bool isKeyword(char *word);
bool isIdentifier(char *word);
bool isNumber(char *word);
bool isBool(char *word);
bool isString(char *word);
bool isChar(char *word);

// Create new token
Token *new_token(char *type, char *value, int line, int col) {
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->line = line;
    token->col = col;
    return token;
}

// Print token stream
void print_tokens(Token **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        if (strcmp(tokens[i]->type, "LEXICAL_ERROR") == 0) {
            printf("LEXICAL ERROR [%d:%d]: Invalid token '%s'", tokens[i]->line,
                   tokens[i]->col, tokens[i]->value);
        } else {
            printf("%s %d:%d\n", tokens[i]->type, tokens[i]->line, tokens[i]->col);
        }
        i++;
    }
}

// write the tokens to output.txt
void write_tokens(Token **tokens) {
    FILE *fp;
    fp = fopen("outputLexical.txt", "w");
    int i = 0;
    while (tokens[i] != NULL) {
        if (strcmp(tokens[i]->type, "LEXICAL_ERROR") == 0) {
            fprintf(fp, "LEXICAL ERROR [%d:%d]: Invalid token '%s'", tokens[i]->line,
                    tokens[i]->col, tokens[i]->value);
        } else {
            fprintf(fp, "%s %d:%d %s\n", tokens[i]->type, tokens[i]->line,
                    tokens[i]->col, tokens[i]->value);
        }
        i++;
    }
    fclose(fp);
}

bool isBracket(char c) {
    return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

void lex(char word[], int row, int col, Token **tokens, int *tokenIndex) {

    if (isKeyword(word)) {
        if (!strcmp(word, "define")) {
            tokens[*tokenIndex] = new_token("DEFINE", word, row, col - strlen(word));
            (*tokenIndex)++;
        }
        if (!strcmp(word, "let")) {
            tokens[*tokenIndex] = new_token("LET", word, row, col - strlen(word));
            (*tokenIndex)++;
        }
        if (!strcmp(word, "cond")) {
            tokens[*tokenIndex] = new_token("COND", word, row, col - strlen(word));
            (*tokenIndex)++;
        }
        if (!strcmp(word, "if")) {
            tokens[*tokenIndex] = new_token("IF", word, row, col - strlen(word));
            (*tokenIndex)++;
        }
        if (!strcmp(word, "begin")) {
            tokens[*tokenIndex] = new_token("BEGIN", word, row, col - strlen(word));
            (*tokenIndex)++;
        }
    }

    else if (isBool(word)) {
        tokens[*tokenIndex] = new_token("BOOLEAN", word, row, col - strlen(word));
        (*tokenIndex)++;
    }

    else if (isNumber(word)) {
        tokens[*tokenIndex] = new_token("NUMBER", word, row, col - strlen(word));
        (*tokenIndex)++;
    }

    else if (isIdentifier(word)) {
        tokens[*tokenIndex] =
                new_token("IDENTIFIER", word, row, col - strlen(word));
        (*tokenIndex)++;
    }

    else if (isChar(word)) {
        tokens[*tokenIndex] = new_token("CHAR", word, row, col - strlen(word));
        (*tokenIndex)++;
    }

    else if (isString(word)) {
        tokens[*tokenIndex] = new_token("STRING", word, row, col - strlen(word));
        (*tokenIndex)++;
    }

    else {
        tokens[*tokenIndex] =
                new_token("LEXICAL_ERROR", word, row, col - strlen(word));
        ERROR = true;
    }
}

bool isKeyword(char *word) {
    return (!strcmp(word, "define") || !strcmp(word, "let") ||
            !strcmp(word, "cond") || !strcmp(word, "if") ||
            !strcmp(word, "begin"));
}

bool isString(char *word) {
    if (word[0] == '"' && word[strlen(word) - 1] == '"') {
        return true;
    } else {
        return false;
    }
}

bool isChar(char *word) {
    if (word[0] == '\'' && word[strlen(word) - 1] == '\'') {
        if (strlen(word) == 3) {
            return true;
        } else if (strlen(word) == 4) {
            if (word[1] == '\\' && word[2] == '\'') {
                return true;
            } else if (word[1] == '\\' && word[2] == '\\') {
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool isBool(char *word) {
    if ((!strcmp(word, "true")) || (!strcmp(word, "false"))) {
        return true;
    } else {
        return false;
    }
}

bool isIdentifier(char *word) {

    // If first character is + - . and word length bigger than 1 then it is not an
    // identifier
    if ((word[0] == '+' || word[0] == '-' || word[0] == '.') &&
        strlen(word) > 1) {
        return false;
    } else if ((word[0] == '+' || word[0] == '-' || word[0] == '.') &&
               strlen(word) == 1) {
        return true;
    }

    // First character should be ! * / < = > ? or alphabetic
    if (!(word[0] == '!' || word[0] == '*' || word[0] == '/' || word[0] == '<' ||
          word[0] == '=' || word[0] == '>' || word[0] == '?' ||
          isalpha(word[0]))) {
        return false;
    }

    // Rest of characters can be empty or letter or digir or + - .
    int i;
    for (i = 1; i < strlen(word); i++) {
        if (!(word[i] == '+' || word[i] == '-' || word[i] == '.' ||
              isalpha(word[i]) || isdigit(word[i]))) {
            return false;
        }
    }

    return true;
}

bool isDecimalSignedInteger(char *word) {
    // First character should be + or - or numeric
    if (!(word[0] == '+' || word[0] == '-' || isdigit(word[0]))) {
        return false;
    }

    // If first character is + or - then second character should be numeric
    if (word[0] == '+' || word[0] == '-') {
        if (!isdigit(word[1])) {
            return false;
        }
    }

    // Rest of the characters should be numeric
    int i;
    for (i = 1; i < strlen(word); i++) {
        if (!isdigit(word[i])) {
            return false;
        }
    }

    return true;
}

bool isHexadecimalUnsignedInteger(char *word) {
    // First two character should be 0x and rest should be hex numeric
    if (strlen(word) > 2) {
        if (word[0] == '0' && word[1] == 'x') {
            int i;
            for (i = 2; i < strlen(word); i++) {
                if (!(word[i] == '0' || word[i] == '1' || word[i] == '2' ||
                      word[i] == '3' || word[i] == '4' || word[i] == '5' ||
                      word[i] == '6' || word[i] == '7' || word[i] == '8' ||
                      word[i] == '9' || word[i] == 'a' || word[i] == 'b' ||
                      word[i] == 'c' || word[i] == 'd' || word[i] == 'e' ||
                      word[i] == 'f' || word[i] == 'A' || word[i] == 'B' ||
                      word[i] == 'C' || word[i] == 'D' || word[i] == 'E' ||
                      word[i] == 'F')) {
                    return false;
                }
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool isBinaryUnsignedInteger(char *word) {
    // First two character should be 0b and rest should be binary numeric
    if (strlen(word) > 2) {
        if (word[0] == '0' && word[1] == 'b') {
            int i;
            for (i = 2; i < strlen(word); i++) {
                if (!(word[i] == '0' || word[i] == '1')) {
                    return false;
                }
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool is_floatOpt1(char *word) {
    int len = strlen(word);
    int i = 0;

    // check if the first character is a valid starting character for a float
    if (!(word[0] == '-' || word[0] == '+' || isdigit(word[0]) ||
          word[0] == '.')) {
        return false;
    }

    // if the first character is a sign, move to the next character
    if (word[0] == '-' || word[0] == '+') {
        i++;
    }

    // check if the integer part of the float is valid
    while (isdigit(word[i])) {
        i++;
    }

    // check if the decimal point is present
    if (word[i] != '.') {
        return false;
    }

    i++;

    // check if the fractional part of the float is valid
    while (i < len && isdigit(word[i])) {
        i++;
    }

    // check if the exponent part of the float is valid
    if (i < len && (word[i] == 'e' || word[i] == 'E')) {
        i++;

        // check if the exponent sign is present
        if (word[i] == '-' || word[i] == '+') {
            i++;
        }

        // check if the exponent part is valid
        while (i < len && isdigit(word[i])) {
            i++;
        }
    }

    // check if there are no remaining characters after the float
    if (!(i <= len)) {
        return false;
    }

    return true;
}

bool is_floatOption2(char *word) {
    int len = strlen(word);
    int i = 0;
    // check if the first character is a valid starting character for a float
    if (!(word[0] == '-' || word[0] == '+' || isdigit(word[0]))) {
        return false;
    }
    // if the first character is a sign, move to the next character
    if (word[0] == '-' || word[0] == '+') {
        i++;
    }
    if (!isdigit(word[i])) {
        return false;
    }
    // check if the fractional part of the float is valid
    while (i < len && isdigit(word[i])) {
        i++;
    }
    while (i < len && isdigit(word[i])) {
        i++;
    }

    // check if the exponent part of the float is valid
    if (i < len && (word[i] == 'e' || word[i] == 'E')) {
        i++;

        // check if the exponent sign is present
        if (word[i] == '-' || word[i] == '+') {
            i++;
        }

        // check if the exponent part is valid
        while (i < len && isdigit(word[i])) {
            i++;
        }
    }
    // check if there are no remaining characters after the float
    if (!(i < len)) {
        return false;
    }

    return true;
}

bool isNumber(char *word) {

    // First character should be - + next characters should be numeric or . and
    // next characters should be numeric
    if (is_floatOpt1(word) || is_floatOption2(word) ||
        isBinaryUnsignedInteger(word) || isHexadecimalUnsignedInteger(word) ||
        isDecimalSignedInteger(word)) {
        return true;
    } else {
        return false;
    }
}

//==================Tree==================

struct Node {
    char *type;
    char *value;
    struct Node *child;
    struct Node *sibling;
} typedef Node;

void printParseTree(Node* node, int indent, FILE* file) {
    // Print the current node
    if (!strcmp(node->type, "ERROR")) {
        printf("%s", node->value);
        if (file != NULL) {
            fprintf(file, "%s", node->value);
        }
        exit(0);
    } else {
        for (int i = 0; i < indent; i++) {
            printf(" ");
            if (file != NULL) {
                fprintf(file, " ");
            }
        }
        printf("%s", node->type);
        if (file != NULL) {
            fprintf(file, "%s", node->type);
        }

        if (node->value != NULL) {
            printf(" (%s)", node->value);
            if (file != NULL) {
                fprintf(file, " (%s)", node->value);
            }
        }
        printf("\n");
        if (file != NULL) {
            fprintf(file, "\n");
        }

        // Print the child nodes recursively
        Node* child = node->child;
        while (child != NULL) {
            printParseTree(child, indent + 1, file);
            child = child->sibling;
        }
    }
}

Node *createNode(char *type, char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    node->value = value;

    node->child = NULL;
    node->sibling = NULL;
    return node;
}

void addChild(Node *parent, Node *child) {
    if (parent->child == NULL) {
        parent->child = child;
    } else {
        Node *temp = parent->child;
        while (temp->sibling != NULL) {
            temp = temp->sibling;
        }
        temp->sibling = child;
    }
}

void addSibling(Node *node, Node *sibling) {
    if (node->sibling == NULL) {
        node->sibling = sibling;
    } else {
        Node *temp = node->sibling;
        while (temp->sibling != NULL) {
            temp = temp->sibling;
        }
        temp->sibling = sibling;
    }
}



//==================Tree==================
// Function definitions
Node *program(Token **tokens, int *tokenIndex);
Node *topLevelForm(Token **tokens, int *tokenIndex);
Node *secondLevelForm(Token **tokens, int *tokenIndex);
Node *definition(Token **tokens, int *tokenIndex);
Node *definitionRight(Token **tokens, int *tokenIndex);
Node *argList(Token **tokens, int *tokenIndex);
Node *statements(Token **tokens, int *tokenIndex);
Node *expressions(Token **tokens, int *tokenIndex);
Node *expression(Token **tokens, int *tokenIndex);
Node *expr(Token **tokens, int *tokenIndex);
Node *funCall(Token **tokens, int *tokenIndex);
Node *letExpression(Token **tokens, int *tokenIndex);
Node *letExpr(Token **tokens, int *tokenIndex);
Node *varDefs(Token **tokens, int *tokenIndex);
Node *varDef(Token **tokens, int *tokenIndex);
Node *condExpression(Token **tokens, int *tokenIndex);
Node *condBranches(Token **tokens, int *tokenIndex);
Node *condBranch(Token **tokens, int *tokenIndex);
Node *ifExpression(Token **tokens, int *tokenIndex);
Node *endExpression(Token **tokens, int *tokenIndex);
Node *beginExpression(Token **tokens, int *tokenIndex);



int main() {
    char filename[100];
    printf("Please enter the filename: \n");
    scanf("%s", filename);
    //strcpy(filename, "input2.txt");

    FILE *fp = fopen(filename, "r");
    FILE *outputFileName = fopen("output.txt", "w");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }
    if (outputFileName == NULL) {

        perror("Failed Output: ");
        return 1;
    }

    Token **tokens = (Token **)malloc(sizeof(Token *) * MAX_LEN);
    char character;
    char next;

    int row = 1;
    int col = 1;
    int tokenIndex = 0;

    bool isString = false;
    bool isChar = false;

    // char word
    char word[100];
    int wordIndex = 0;

    while ((character = fgetc(fp)) != EOF && ERROR == false) {

        // if character is single quote
        if (character == '\'' && isString == false) {
            if (isChar == false) {
                isChar = true;
            }
                // If isChar == true
            else {
                if (word[wordIndex - 1] != '\\') {
                    isChar = false;
                }
            }
        }

        if (character == '"') {
            if (isString == false) {
                isString = true;
            }
                // If isString == true
            else {
                if (word[wordIndex - 1] != '\\') {
                    isString = false;
                }
            }
        }
        if (isChar == true) {
            word[wordIndex] = character;
            wordIndex++;
            col++;
        }

        else if (isString == true) {
            word[wordIndex] = character;
            wordIndex++;
            col++;
        }

        else {
            // If character is not a space
            if (isspace(character) == false) {

                // If character is bracket than
                if (isBracket(character)) {
                    // If wordIndex > 0 then we have a word before the bracket and we need
                    // to lex it
                    if (wordIndex > 0) {

                        char *temp = (char *)malloc(sizeof(char) * wordIndex);
                        strcpy(temp, word);
                        lex(temp, row, col, tokens, &tokenIndex);
                        // reset the word and wordIndex
                        memset(word, 0, sizeof(word));
                        wordIndex = 0;
                    }

                    // If character is a bracket than we need to lex it
                    if (character == '(') {
                        tokens[tokenIndex] = new_token("LEFTPAR", "(", row, col);
                        tokenIndex++;
                    }
                    if (character == ')') {
                        tokens[tokenIndex] = new_token("RIGHTPAR", ")", row, col);
                        tokenIndex++;
                    }
                    if (character == '[') {
                        tokens[tokenIndex] = new_token("LEFTSQUAREB", "[", row, col);
                        tokenIndex++;
                    }
                    if (character == ']') {
                        tokens[tokenIndex] = new_token("RIGHTSQUAREB", "]", row, col);
                        tokenIndex++;
                    }
                    if (character == '{') {
                        tokens[tokenIndex] = new_token("LEFTCURLYB", "{", row, col);
                        tokenIndex++;
                    }
                    if (character == '}') {
                        tokens[tokenIndex] = new_token("RIGHTCURLYB", "}", row, col);
                        tokenIndex++;
                    }
                    // reset the word and wordIndex
                    memset(word, 0, sizeof(word));
                    wordIndex = 0;
                }
                    //~ means comment
                else if (character == '~') {
                    while (character != '\n') {
                        character = fgetc(fp);
                    }
                    row++;
                    col = 1;
                }

                    // If character is not a bracket than we need to add it to the word
                else {
                    word[wordIndex] = character;
                    wordIndex++;
                }
                col++;

            }

                // If character is a space
            else if (isspace(character)) {
                // If wordIndex > 0 then we have a word before the space and we need to
                // lex it
                if (wordIndex > 0) {
                    char *temp = (char *)malloc(sizeof(char) * wordIndex);
                    strcpy(temp, word);
                    lex(temp, row, col, tokens, &tokenIndex);
                }

                // Reset word and wordIndex
                memset(word, 0, sizeof(word));
                wordIndex = 0;

                // If character is a new line than we need to increment row and reset
                // col
                if (character == '\n') {
                    row++;
                    col = 1;
                }
                    // If character is a tab than we need to increment col by 4
                else if (character == '\t') {
                    col += 4;
                }
                    // If character is a space than we need to increment col by 1
                else if (character == ' ') {
                    col++;
                }
            }
        }
    }

    //print_tokens(tokens);
    //write_tokens(tokens);
    //==============================================================================================================
    //============================================SYNTAX
    // ANALYSIS===================================================
    //==============================================================================================================



    tokenIndex = 0;
    while (tokens[tokenIndex] != NULL) {
        if (strcmp(tokens[tokenIndex]->type, "LEXICAL_ERROR") == 0) {
            printf("LEXICAL ERROR [%d:%d]: Invalid token '%s'", tokens[tokenIndex]->line, tokens[tokenIndex]->col, tokens[tokenIndex]->value);
            fprintf(outputFileName, "LEXICAL ERROR [%d:%d]: Invalid token '%s'", tokens[tokenIndex]->line, tokens[tokenIndex]->col, tokens[tokenIndex]->value);
            return 1;
        }
        tokenIndex++;
    }
    tokenIndex = 0;
    Node* root = program(tokens, &tokenIndex);
    printParseTree(root, 0, outputFileName);



    return 0;
}
//<Program> -> ε | <TopLevelForm> <Program>
//<TopLevelForm> -> ( <SecondLevelForm> )
//<SecondLevelForm> -> <Definition> | ( <FunCall> )
//<Definition> -> DEFINE <DefinitionRight>
//<DefinitionRight> -> IDENTIFIER <Expression> | ( IDENTIFIER <ArgList> )
//<Statements> <ArgList> -> ε | IDENTIFIER <ArgList> <Statements> ->
//<Expression> | <Definition> <Statements>

//<Expressions> -> ε | <Expression> <Expressions>
//<Expression> -> IDENTIFIER | NUMBER | CHAR | BOOLEAN | STRING | ( <Expr> )
//<Expr> -> <LetExpression> | <CondExpression> | <IfExpression> |
//<BeginExpression> | <FunCall> <FunCall> -> IDENTIFIER <Expressions>
//<LetExpression> -> LET <LetExpr>
//<LetExpr> -> ( <VarDefs> ) <Statements> | IDENTIFIER ( <VarDefs> )
//<Statements> <VarDefs> -> ( IDENTIFIER <Expression> ) <VarDef>

//<VarDef> -> ε | <VarDefs>
//<CondExpression> -> COND <CondBranches>
//<CondBranches> -> ( <Expression> <Statements> ) <CondBranch>
//<CondBranch> -> ε | ( <Expression> <Statements> )
//<IfExpression> -> IF <Expression> <Expression> <EndExpression>
//<EndExpression> -> ε | <Expression>
//<BeginExpression> -> BEGIN <Statements>

// burak ↓
void addError(Node *node, Token token, const char *expected) {
    char *errorInfo = (char *)malloc(sizeof(char) * 100);
    sprintf(errorInfo, "SYNTAX ERROR [%d,%d]: '%s' is expected", token.line,
            token.col, expected);
    addChild(node, createNode("ERROR", errorInfo));
}

Node *program(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Program>", NULL);

    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        Node* topLevelFormNode = topLevelForm(tokens, tokenIndex);
        addChild(node, topLevelFormNode);
        Node* programNode = program(tokens, tokenIndex);
        addChild(node, programNode);
        return node;
    }
    else {
        addChild(node, createNode("__", NULL));
        return node;
    }
}

Node *topLevelForm(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<TopLevelForm>", NULL);

    // token == LEFTPAR
    if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        Node *secondLevelFormNode = secondLevelForm(tokens, tokenIndex);
        addChild(node, secondLevelFormNode);

        // token == RIGHTPAR
        if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
            addChild(node,
                     createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
        }

            // token != RIGHTPAR
        else {
            addError(node, *tokens[*tokenIndex], ")");
        }
    }

        // token != LEFTPAR
    else {
        addError(node, *tokens[*tokenIndex], "(");
    }

    return node;
}

Node *secondLevelForm(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<SecondLevelForm>", NULL);

    // token == DEFINE
    if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "DEFINE")) {
        Node *definitionNode = definition(tokens, tokenIndex);
        addChild(node, definitionNode);
    }

        // token == LEFTPAR
    else if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        Node *funCallNode = funCall(tokens, tokenIndex);
        addChild(node, funCallNode);

        // token == RIGHTPAR
        if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
            addChild(node,
                     createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
        }
            // token != RIGHTPAR
        else {
            addError(node, *tokens[*tokenIndex], ")");
        }
    }
        // UNEXPECTED TOKEN
    else {
        addError(node, *tokens[*tokenIndex], "DEFINE or (");
    }

    return node;
}

Node *definition(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Definition>", NULL);

    // token == DEFINE
    if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "DEFINE")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        Node *definitionRightNode = definitionRight(tokens, tokenIndex);
        addChild(node, definitionRightNode);
    }
        // token != DEFINE
    else {
        addError(node, *tokens[*tokenIndex], "DEFINE");
    }

    return node;
}

Node *definitionRight(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<DefinitionRight>", NULL);

    // token == IDENTIFIER
    if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        Node *expressionNode = expression(tokens, tokenIndex);
        addChild(node, expressionNode);
    }
        // token == LEFTPAR
    else if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        // token == IDENTIFIER
        if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type,
                                                  "IDENTIFIER")) {
            addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;

            Node *argListNode = argList(tokens, tokenIndex);
            addChild(node, argListNode);

            // token == RIGHTPAR
            if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
                addChild(node, createNode(tokens[*tokenIndex]->type,
                                          tokens[*tokenIndex]->value));
                (*tokenIndex)++;

                Node *statementsNode = statements(tokens, tokenIndex);
                addChild(node, statementsNode);
            }
                // token != RIGHTPAR
            else {
                addError(node, *tokens[*tokenIndex], ")");
            }
        }
            // token != IDENTIFIER
        else {
            addError(node, *tokens[*tokenIndex], "IDENTIFIER");
        }
    }
        // UNEXPECTED TOKEN
    else {
        addError(node, *tokens[*tokenIndex], "IDENTIFIER (");
    }

    return node;
}

Node *argList(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<ArgList>", NULL);

    // token == IDENTIFIER
    if (tokens[*tokenIndex] != NULL &&!strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;

        Node *argListNode = argList(tokens, tokenIndex);
        addChild(node, argListNode);
    }
    else {
        addChild(node, createNode("__", NULL));
        return node;
    }

    return node;
}

Node *statements(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Statements>", NULL);

    // token == <expression>
    if (!strcmp(tokens[*tokenIndex]->type, "IDENTIFIER") ||
        !strcmp(tokens[*tokenIndex]->type, "NUMBER") ||
        !strcmp(tokens[*tokenIndex]->type, "CHAR") ||
        !strcmp(tokens[*tokenIndex]->type, "BOOLEAN") ||
        !strcmp(tokens[*tokenIndex]->type, "STRING") ||
        !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        if(tokens[*tokenIndex] != NULL ){
            Node *expressionNode = expression(tokens, tokenIndex);
            addChild(node, expressionNode);
        }
    }
        // token == DEFINE
    else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "DEFINE")) {

        Node *definitionNode = definition(tokens, tokenIndex);
        addChild(node, definitionNode);

        Node *statementsNode = statements(tokens, tokenIndex);
        addChild(node, statementsNode);
    }
        // UNEXPECTED TOKEN
    else {
        addError(node, *tokens[*tokenIndex], "IDENTIFIER NUMBER CHAR BOOLEAN STRING (");
    }

    return node;
}
// burak ↑
//  kadir
Node *expressions(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Expressions>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
        addChild(node, createNode("__", NULL));
        return node;
    }
    Node *expressionNode = expression(tokens, tokenIndex);
    addChild(node, expressionNode);

    Node *expressionsNode = expressions(tokens, tokenIndex);
    addChild(node, expressionsNode);

    return node;
}
Node *expression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Expression>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "NUMBER")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "CHAR")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "BOOLEAN")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "STRING")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *exprNode = expr(tokens, tokenIndex);
        addChild(node, exprNode);

        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
            addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
        } else {
            addError(node, *tokens[*tokenIndex], ")");
        }
    } else {
        addError(node, *tokens[*tokenIndex], "IDENTIFIER, NUMBER, CHAR, BOOLEAN, STRING or (");
    }
    return node;
}
Node *expr(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<Expr>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LET")) {
        Node *letExpressionNode = letExpression(tokens, tokenIndex);
        addChild(node, letExpressionNode);
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "COND")) {
        Node *condExpressionNode = condExpression(tokens, tokenIndex);
        addChild(node, condExpressionNode);
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IF")) {
        Node *ifExpressionNode = ifExpression(tokens, tokenIndex);
        addChild(node, ifExpressionNode);
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "BEGIN")) {
        Node *beginExpressionNode = beginExpression(tokens, tokenIndex);
        addChild(node, beginExpressionNode);
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        Node *funCallNode = funCall(tokens, tokenIndex);
        addChild(node, funCallNode);
    } else {
        addError(node, *tokens[*tokenIndex], "LET COND IF BEGIN IDENTIFIER");
    }
    return node;
}
Node *funCall(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<FunCall>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *expressionsNode = expressions(tokens, tokenIndex);
        addChild(node, expressionsNode);

    } else {
        addError(node, *tokens[*tokenIndex], "IDENTIFIER");
    }
    return node;
}
Node *letExpression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<LetExpression>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LET")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *letExprNode = letExpr(tokens, tokenIndex);
        addChild(node, letExprNode);
    } else {
        addError(node, *tokens[*tokenIndex], "LET");
    }
    return node;
}
Node *letExpr(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<LetExpr>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *varDefsNode = varDefs(tokens, tokenIndex);
        addChild(node, varDefsNode);

        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
            addChild(node,
                     createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
            Node *statementsNode = statements(tokens, tokenIndex);
            addChild(node, statementsNode);

        } else {
            addError(node, *tokens[*tokenIndex], ")");
        }
    } else if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
            addChild(node,
                     createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
            Node *varDefsNode =
                    varDefs(tokens, tokenIndex); // ayni fonkta iki tane vardefsnode var!!
            addChild(node, varDefsNode);
            if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
                addChild(node, createNode(tokens[*tokenIndex]->type,
                                          tokens[*tokenIndex]->value));
                (*tokenIndex)++;
                Node *statementsNode =
                        statements(tokens, tokenIndex); // iki tane var !!
                addChild(node, statementsNode);

            } else {
                addError(node, *tokens[*tokenIndex], ")");
            }
        } else {
            addError(node, *tokens[*tokenIndex], "(");
        }
    } else {
        addError(node, *tokens[*tokenIndex], "IDENTIFIER (");
    }
    return node;
}
Node *varDefs(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<VarDefs>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IDENTIFIER")) {
            addChild(node, createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
            Node *expressionNode = expression(tokens, tokenIndex);
            addChild(node, expressionNode);

            if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
                addChild(node, createNode(tokens[*tokenIndex]->type,tokens[*tokenIndex]->value));
                (*tokenIndex)++;
                Node *varDefNode = varDef(tokens, tokenIndex);
                addChild(node, varDefNode);

            } else {
                addError(node, *tokens[*tokenIndex], ")");

            }

        } else {
            addError(node, *tokens[*tokenIndex], "IDENTIFIER");

        }
    } else {
        addError(node, *tokens[*tokenIndex], "(");

    }
    return node;
}
Node *varDef(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<VarDef>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
        addChild(node, createNode("__", NULL));
        return node;
    }
    Node *varDefsNode = varDefs(tokens, tokenIndex);
    addChild(node, varDefsNode);

    return node;
}
// here
Node *condExpression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<CondExpression>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "COND")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++; // !!!
        Node *condBranchesNode = condBranches(tokens, tokenIndex);
        addChild(node, condBranchesNode);
        return node;
    }
    addError(node, *tokens[*tokenIndex], "COND");

    return node;

}
Node *condBranches(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<CondBranches>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *expressionNode = expression(tokens, tokenIndex);
        addChild(node, expressionNode);
        Node *statementsNode = statements(tokens, tokenIndex);
        addChild(node, statementsNode);

        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
            addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;
            Node *condBranchNode = condBranch(tokens, tokenIndex);
            addChild(node, condBranchNode);
            return node;


        } else {
            addError(node, *tokens[*tokenIndex], ")");

        }
    } else {
        addError(node, *tokens[*tokenIndex], "(");

    }

    return node;
}


Node *condBranch(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<CondBranch>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "LEFTPAR")){
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *expressionNode = expression(tokens, tokenIndex);
        addChild(node, expressionNode);
        Node *statementsNode = statements(tokens, tokenIndex);
        addChild(node, statementsNode);
        if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")){
            addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
            (*tokenIndex)++;

        }
    }
    else {
        addChild(node,createNode("__", NULL));
        return node;
    }
    return node;
}
Node *ifExpression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<IfExpression>", NULL);

    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "IF")) {
        addChild(node,
                 createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *expressionNode1 = expression(tokens, tokenIndex);
        addChild(node, expressionNode1);
        Node *expressionNode2 = expression(tokens, tokenIndex);
        addChild(node, expressionNode2);
        Node *endExpressionNode = endExpression(tokens, tokenIndex);
        addChild(node, endExpressionNode);

    } else {
        addError(node, *tokens[*tokenIndex], "IF");

    }

    return node;
}
Node *endExpression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<EndExpression>", NULL);

    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "RIGHTPAR")) {
        addChild(node,createNode("__", NULL));
        return node;
    }
    Node *expressionNode = expression(tokens, tokenIndex);
    addChild(node, expressionNode);

    return node;
}
Node *beginExpression(Token **tokens, int *tokenIndex) {
    Node *node = createNode("<BeginExpression>", NULL);
    if (tokens[*tokenIndex] != NULL && !strcmp(tokens[*tokenIndex]->type, "BEGIN")) {
        addChild(node,createNode(tokens[*tokenIndex]->type, tokens[*tokenIndex]->value));
        (*tokenIndex)++;
        Node *statementsNode = statements(tokens, tokenIndex);
        addChild(node, statementsNode);
        return node;
    }
    addError(node, *tokens[*tokenIndex], "BEGIN");

    return node;
}
