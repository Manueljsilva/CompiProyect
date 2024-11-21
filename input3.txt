#include<stdio.h>

int main(){
    int x;
    x = 1;
    
    for (int i = 0; i < 10; i++){//declara i como variable local
        x = x + i;
    }
    printf("%d\n", x);

    return 0;
}
