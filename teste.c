#include <stdio.h>
#include <stdlib.h>

int main() {
    int **a, **b;
    a = (int**) malloc(sizeof(int*) * 4);
    for( int i = 0; i < 4; ++i) {
        a[i] = (int*) malloc(sizeof(int));
    }
    b = a;
    for(int i = 0; i < 4; ++i) {
        a[i][0] = 0;
    }
    a[0][0] = 10;
    for(int i = 0; i < 4; ++i) {
        printf("%d, ", b[i][0]);
    }
    printf("\n");
    return 0;
}