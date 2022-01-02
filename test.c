#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *tab;
}foo_t;

void changeStruct(foo_t *foo) {
    foo[0].tab[0] = 42;
}

void print(int *tab, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", tab[i]);
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    foo_t foots[4];
    for (int i = 0; i < 4; i++) {
        int *tab = (int *)malloc(4 * sizeof(int));
        if (tab == NULL) { perror("memory allocation failed"); exit(EXIT_FAILURE); }
        foots[i].tab = tab;
    }

    printf("foots initial : \n");
    print(foots[0].tab, 4);

    changeStruct(foots);

    printf("foots then : \n");
    print(foots[0].tab, 4);

    return 0;
}
