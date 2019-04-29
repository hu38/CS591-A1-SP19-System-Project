#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int errnum;

typedef struct mystruct
{
    int key;
    int value;
} element;

int main (int argc, char** argv)
{
    int a=5;
    int b,i,j;
    element data[100];
    element datain[100];

    for (i=0;i<100;i++)
    {
        data[i].key=i*2;
        data[i].value=i*2+1;
    }
// g++ fopen_test.cpp -o test && ./test
    FILE* fp=fopen("./fopen_test.dat","rw+");
    // printf("%d",fp);    
    // perror("error");
    // printf(strerror(errnum));
    // printf("\n%d\n",errnum);

    // fwrite(&data,sizeof(element),100,fp);

    // fwrite(&a,sizeof(int),1,fp);
    fread(&datain,sizeof(int),100,fp);

    printf("%d,%d\n",datain[10].key,datain[10].value);

    // printf("I just read : %d\n",b);
    
    fclose(fp);


}