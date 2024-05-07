# mango

# this is a builder tool to help simplify all of the flags in the building process of a execution using any language!!!

## Examples

./mango gcc @f src .c -o Main @b @r Main
./mango python main.py @b @s 

## Here are rules that apply for using this tool
1. everything saves in order from left to right
2. everything that isn't a flag will be saved as it is

./mango -scmd     -ecmd run -run main 
