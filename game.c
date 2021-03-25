#include <unistd.h>
#include<stdio.h>

void regra(int matrix[144], int res[144], int i, int j)
{
    int n_vizinhos = 0;
    for (int i2 = i-1; i2 < i+2; i2++)
    {
        for (int j2 = j-1; j2 < j+2; j2++)
        {
            if (i2 == i && j2 == j)
            {
                continue;
            }
            else if (matrix[i2*12+j2] == 1)
            {
                n_vizinhos++;
            }
        }
    }
    if (matrix[i*12+j] == 0)
    {
        if (n_vizinhos == 3)
        {
            res[i*12+j] = 1;
        }
        else
        {
            res[i*12+j] = 0;
        }
    }
    else
    {
        if (n_vizinhos == 2 || n_vizinhos == 3)
        {
            res[i*12+j] = 1;
        }
        else
        {
            res[i*12+j] = 0;
        }
    }
}

int main()
{
    int mat[144];
    int mat2[144];

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            mat[i*12+j] = 0;
            mat2[i*12+j] = 0;
        }
    }
    mat[1*12+3] = 1;
    mat[2*12+1] = 1;
    mat[2*12+3] = 1;
    mat[3*12+2] = 1;
    mat[3*12+3] = 1;
    int* p1 = mat;
    int* p2 = mat2;
    while(1)
    {
        for (int i = 1; i < 11; i++)
        {
            for (int j = 1; j < 11; j++)
            {
                regra(p1,p2,i,j);
                printf("%d", p2[i*12+j]);
            }
            printf("\n");
        }
        printf("\n");
        int* temp = p1;
        p1 = p2;
        p2 = temp;
        sleep(1);
    }
    return 0;
}
