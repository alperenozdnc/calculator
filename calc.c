#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPRESSION_SIZE 100
#define BUFFER_SIZE 50

typedef enum { PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/' } Operations;
typedef enum { LEFT = '(', RIGHT = ')' } Parentheses;
typedef enum { NUMBER = -1, ADDITION, MULTIPLICATION, PARENTHESIS } Precedence;

typedef enum { TOKEN_NUMBER, TOKEN_OPERATOR, TOKEN_GROUP } TokenType;

typedef struct Expression Expression;

typedef struct Token {
    TokenType type;
    union {
        int number;
        char operator;
        Expression *group;
    };
} Token;

struct Expression {
    Token tokens[EXPRESSION_SIZE];
    int token_count;
};

void read_expression(char *expression);
int get_precedence(char c);
bool is_operator(char c);
bool is_parenthesis(char c);
bool validate_expression(char expression[]);
void tokenize_expression(char expression[], char tokens[][BUFFER_SIZE],
                         int *token_count);
Expression *build_expression(char tokens[][BUFFER_SIZE], int token_count);
int evaluate(Expression *expr);

int main() {
    char expression[EXPRESSION_SIZE] = {0};
    read_expression(expression);

    if (!validate_expression(expression))
        return 1;

    char tokens[EXPRESSION_SIZE][BUFFER_SIZE] = {0};
    int tokens_len = 0;

    tokenize_expression(expression, tokens, &tokens_len);

    Expression *root = build_expression(tokens, tokens_len);

    int result = evaluate(root);

    printf("= %d\n", result);

    return 0;
}

void read_expression(char *expression) {
    printf("enter expression: ");
    fgets(expression, EXPRESSION_SIZE, stdin);

    if (strlen(expression) == 1) {
        printf("no input given\n");
        exit(1);
    }

    expression[strlen(expression) - 1] = '\0';
}

int get_precedence(char c) {
    if (c == LEFT || c == RIGHT)
        return 2;
    else if (c == TIMES || c == DIVIDE)
        return 1;
    else if (c == PLUS || c == MINUS)
        return 0;
    else
        return -1;
}

bool is_operator(char c) { return get_precedence(c) > -1; }
bool is_parenthesis(char c) { return c == LEFT || c == RIGHT; }

bool validate_expression(char *expression) {
    const char space = ' ';
    char allowed_characters[] = {LEFT,  RIGHT, PLUS, MINUS, TIMES, DIVIDE,
                                 space, '1',   '2',  '3',   '4',   '5',
                                 '6',   '7',   '8',  '9',   '0'};

    bool result = true;
    int operation_count = 0;
    int left_paren_count = 0;
    int right_paren_count = 0;
    char exp_no_spaces[EXPRESSION_SIZE] = {0};

    for (int i = 0; i < strlen(expression); i++) {
        char c = expression[i];
        bool char_is_allowed = false;

        for (int j = 0; j < strlen(allowed_characters); j++) {
            if (allowed_characters[j] == c) {
                char_is_allowed = true;
                if (c == space)
                    break;
                int l = strlen(exp_no_spaces);
                exp_no_spaces[l] = c;
                exp_no_spaces[l + 1] = '\0';
                break;
            }
        }

        if (!char_is_allowed) {
            printf("'%c' is not an allowed character\n", expression[i]);
            return false;
        }
    }

    for (int i = 0; i < strlen(exp_no_spaces); i++) {
        char c = exp_no_spaces[i];
        char next_c = exp_no_spaces[i + 1];

        if (is_operator(c)) {
            if (i == 0 || i == strlen(exp_no_spaces) - 1) {
                if (!is_parenthesis(c)) {
                    printf("expression can't start or end with an operator\n");
                    return false;
                }
            }

            if (is_operator(next_c)) {
                if (!is_parenthesis(c) && !is_parenthesis(next_c)) {
                    printf("%c can't be next to %c\n", c, exp_no_spaces[i + 1]);
                    return false;
                }
            }

            operation_count++;
        }

        if (c == LEFT) {
            if (i == strlen(exp_no_spaces) - 1) {
                printf("can't end with (\n");
                return false;
            }

            if (!isdigit(exp_no_spaces[i + 1])) {
                printf("expected number after (\n");
                return false;
            }

            left_paren_count++;
        } else if (c == RIGHT) {
            if (!isdigit(exp_no_spaces[i - 1])) {
                printf("expected number before )\n");
                return false;
            }

            right_paren_count++;
        }
    }

    if (left_paren_count != right_paren_count) {
        printf("a parenthesis wasn't closed\n");
        return false;
    }

    if (operation_count == 0) {
        printf("there has to be at least one operation\n");
        return false;
    }

    strcpy(expression, exp_no_spaces);
    return result;
}

void tokenize_expression(char expression[], char tokens[][BUFFER_SIZE],
                         int *token_count) {
    char buffer[BUFFER_SIZE] = {0};
    int token_idx = 0, buffer_idx = 0;

    for (int i = 0; i < strlen(expression); i++) {
        char c = expression[i];
        Precedence precedence = get_precedence(c);

        if (precedence == PARENTHESIS) {
            tokens[token_idx][0] = c;
            tokens[token_idx][1] = '\0';
            token_idx++;
            continue;
        }

        if (precedence == NUMBER) {
            buffer[buffer_idx++] = c;
            if (i == strlen(expression) - 1 ||
                get_precedence(expression[i + 1]) != NUMBER) {
                buffer[buffer_idx] = '\0';
                strcpy(tokens[token_idx++], buffer);
                buffer_idx = 0;
            }
            continue;
        }

        if (buffer_idx > 0) {
            buffer[buffer_idx] = '\0';
            strcpy(tokens[token_idx++], buffer);
            buffer_idx = 0;
        }

        if (precedence == ADDITION || precedence == MULTIPLICATION) {
            tokens[token_idx][0] = c;
            tokens[token_idx][1] = '\0';
            token_idx++;
        }
    }

    *token_count = token_idx;
}

Expression *parse_group(char tokens[][BUFFER_SIZE], int *pos, int token_count) {
    Expression *expr = malloc(sizeof(Expression));
    expr->token_count = 0;

    while (*pos < token_count) {
        char *token = tokens[*pos];
        (*pos)++;

        if (strcmp(token, "(") == 0) {
            Expression *sub = parse_group(tokens, pos, token_count);
            expr->tokens[expr->token_count].type = TOKEN_GROUP;
            expr->tokens[expr->token_count].group = sub;
            expr->token_count++;
        } else if (strcmp(token, ")") == 0) {
            return expr;
        } else if (strspn(token, "0123456789") == strlen(token)) {
            expr->tokens[expr->token_count].type = TOKEN_NUMBER;
            expr->tokens[expr->token_count].number = atoi(token);
            expr->token_count++;
        } else {
            expr->tokens[expr->token_count].type = TOKEN_OPERATOR;
            expr->tokens[expr->token_count].operator= token[0];
            expr->token_count++;
        }
    }

    return expr;
}

Expression *build_expression(char tokens[][BUFFER_SIZE], int token_count) {
    int pos = 0;
    return parse_group(tokens, &pos, token_count);
}

int evaluate(Expression *expr) {
    for (int i = 0; i < expr->token_count; i++) {
        if (expr->tokens[i].type == TOKEN_OPERATOR &&
            (expr->tokens[i].operator== '*' || expr->tokens[i].operator==
             '/')) {
            Token *left = &expr->tokens[i - 1];
            Token *right = &expr->tokens[i + 1];

            int left_val = (left->type == TOKEN_GROUP) ? evaluate(left->group)
                                                       : left->number;
            int right_val = (right->type == TOKEN_GROUP)
                                ? evaluate(right->group)
                                : right->number;

            int result = expr->tokens[i].operator== '*'
                             ? (left_val * right_val)
                             : (left_val / right_val);

            left->type = TOKEN_NUMBER;
            left->number = result;

            for (int j = i; j + 2 < expr->token_count; j++)
                expr->tokens[j] = expr->tokens[j + 2];

            expr->token_count -= 2;
            i--;
        }
    }

    int result = 0;
    bool first = true;
    char current_op = '+';

    for (int i = 0; i < expr->token_count; i++) {
        Token t = expr->tokens[i];

        if (t.type == TOKEN_OPERATOR) {
            current_op = t.operator;
        } else {
            int val = t.type == TOKEN_GROUP ? evaluate(t.group) : t.number;

            if (first) {
                result = val;
                first = false;
            } else if (current_op == '+') {
                result += val;
            } else if (current_op == '-') {
                result -= val;
            }
        }
    }

    return result;
}
