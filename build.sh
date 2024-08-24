clang -g -O0 main.c platform/*.c -o gen
rm *.html
./gen
