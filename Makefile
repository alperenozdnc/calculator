compile: calc.c
	@gcc -o calc calc.c -lm

exec: calc 
	@./calc

clean:
	@rm -f ./calc

run: compile exec clean

