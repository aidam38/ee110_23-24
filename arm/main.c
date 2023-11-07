#include <stdio.h>

int x = 1;

int main(int argc, char** argv) {
    switch (x) {
        case 0: 
            printf("zero");
            break;
        case 1:
            printf("one");
            break;
        case 2:
            printf("two");
            break;
    };
}