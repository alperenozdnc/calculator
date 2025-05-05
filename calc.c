#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef enum { PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/' } Operations;
typedef enum { LEFT = '(', RIGHT = ')' } Parentheses;

struct AllowedCharacters {
    char parentheses[2];
    char operations[4];
    char space;
    int digits[10];
};

void get_expression(char *expression);
bool validate_expression(char expression[]);

int main() {
    char expression[100] = {0};

    get_expression(expression);

    const Parentheses left_paren = LEFT;
    const Parentheses right_paren = RIGHT;
    const Operations plus = PLUS;
    const Operations minus = MINUS;
    const Operations times = TIMES;
    const Operations divide = DIVIDE;

    struct AllowedCharacters allowed_characters = {
        .parentheses = {left_paren, right_paren},
        .operations = {plus, minus, times, divide},
        .space = ' ',
        .digits = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0}};

    const bool IS_EXP_VALID = validate_expression(expression);

    if (!IS_EXP_VALID) {
        return 1;
    }

    return 0;
}

void get_expression(char *expression) {
    printf("enter expression: ");
    fgets(expression, sizeof(expression), stdin);
    expression[strlen(expression) - 1] = '\0';
}

bool validate_expression(char expression[]) { return false; }
