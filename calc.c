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

void get_expression(char *expression);
int get_precedence(char c);
bool is_operator(char c);
bool is_parenthesis(char c);
bool validate_expression(char expression[]);
void tokenize_expression(char expression[], char tokens[][BUFFER_SIZE],
                         int *token_count);

int main() {
    char expression[EXPRESSION_SIZE] = {0};

    get_expression(expression);

    const bool IS_EXP_VALID = validate_expression(expression);

    if (!IS_EXP_VALID)
        return 1;

    char tokens[EXPRESSION_SIZE][BUFFER_SIZE] = {0};
    int token_count = 0;

    tokenize_expression(expression, tokens, &token_count);

    for (int i = 0; i < token_count; i++) {
        printf("%s, ", tokens[i]);
    }

    return 0;
}

void get_expression(char *expression) {
    printf("enter expression: ");
    fgets(expression, EXPRESSION_SIZE, stdin);

    if (strlen(expression) == 1) {
        printf("no input given\n");
        exit(1);
    }

    expression[strlen(expression) - 1] = '\0';
}

// 2 -> ()
// 1 -> */
// 0 -> +-
// -1 -> 0123456789
int get_precedence(char c) {
    if (c == LEFT || c == RIGHT) {
        return 2;
    } else if (c == TIMES || c == DIVIDE) {
        return 1;
    } else if (c == PLUS || c == MINUS) {
        return 0;
    } else {
        return -1;
    }
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
            char allowed_c = allowed_characters[j];

            if (allowed_c == c) {
                char_is_allowed = true;

                if (c == space)
                    continue;

                int l = strlen(exp_no_spaces);

                exp_no_spaces[l] = c;
                exp_no_spaces[l + 1] = '\0';
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
    int token_idx = 0;
    int buffer_idx = 0;
    int number_add_count = 0;

    for (int i = 0; i < strlen(expression); i++) {
        char c = expression[i];
        Precedence precedence = get_precedence(c);

        if (precedence == PARENTHESIS) {
            strcpy(tokens[token_idx++], &c);

            continue;
        }

        if (precedence == NUMBER) {
            buffer[buffer_idx++] = c;

            if (i == strlen(expression) - 1 ||
                get_precedence(expression[i + 1]) != NUMBER) {
                if (buffer_idx > 0) {
                    buffer[buffer_idx] = '\0';
                    strcpy(tokens[token_idx++], buffer);
                    buffer_idx = 0;
                    buffer[0] = '\0';
                }
            }

            continue;
        }

        // add buffer to tokens
        if (buffer_idx > 0) {
            strcpy(tokens[token_idx], buffer);
            strcpy(buffer, "");

            token_idx++;
            buffer_idx = 0;
        }

        if (precedence == ADDITION || precedence == MULTIPLICATION) {
            strcpy(tokens[token_idx], &c);
            strcpy(buffer, "");

            token_idx++;
            buffer_idx = 0;
        }
    }

    *token_count = token_idx;
}
