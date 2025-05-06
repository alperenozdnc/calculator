#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/' } Operations;
typedef enum { LEFT = '(', RIGHT = ')' } Parentheses;

void get_expression(char *expression);
bool is_operator(char c);
bool validate_expression(char expression[]);

int main() {
    char expression[100] = {0};

    get_expression(expression);

    const bool IS_EXP_VALID = validate_expression(expression);

    if (!IS_EXP_VALID) {
        return 1;
    }

    return 0;
}

void get_expression(char *expression) {
    printf("enter expression: ");
    fgets(expression, 100, stdin);

    if (strlen(expression) == 1) {
        printf("no input given\n");
        exit(1);
    }

    expression[strlen(expression) - 1] = '\0';
}

bool is_operator(char c) {
    return c == PLUS || c == MINUS || c == TIMES || c == DIVIDE;
}

bool validate_expression(char expression[]) {
    const char space = ' ';

    char allowed_characters[] = {LEFT,  RIGHT, PLUS, MINUS, TIMES, DIVIDE,
                                 space, '1',   '2',  '3',   '4',   '5',
                                 '6',   '7',   '8',  '9',   '0'};

    bool result = true;

    int operation_count = 0;
    int left_paren_count = 0;
    int right_paren_count = 0;

    char exp_no_spaces[100] = {0};

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

        if (is_operator(c)) {
            if (i == 0 || i == strlen(exp_no_spaces) - 1) {
                printf("expression can't start or end with an operator\n");
                return false;
            }

            if (is_operator(exp_no_spaces[i + 1])) {
                printf("%c can't be next to %c\n", c, exp_no_spaces[i + 1]);
                return false;
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

    return result;
}
