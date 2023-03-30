#include <stdio.h>
#include <stdlib.h>

int add();

int main()
{
    puts("Hello World");
    add();
    return EXIT_SUCCESS;
}

int add()
{
    printf("%s\n", "Texto de prueba");
    return 0;
}
