#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_LEN 256

int check(int puzzle[][9], int row, int col, int num)
{
    int rowStart = (row/3) * 3;
    int colStart = (col/3) * 3;
    int i;

    for(i = 0; i < 9; i++)
    {
        if (puzzle[row][i] == num) 
            return 0;
        if (puzzle[i][col] == num) 
            return 0;
        if (puzzle[rowStart + (i%3)][colStart + (i/3)] == num) 
            return 0;
    }
    return 1;
}

int solver(int puzzle[][9], int row, int col)
{
    int i;
    if(row < 9 && col < 9)
    {
        if(puzzle[row][col] != 0)
        {
            if((col+1)<9) 
                return solver(puzzle, row, col+1);
            else if((row+1)<9) 
                return solver(puzzle, row+1, 0);
            else 
                return 1;
        }
        else
        {
            for(i=0; i<9; ++i)
            {
                if(check(puzzle, row, col, i+1))
                {
                    puzzle[row][col] = i + 1;
                    if(solver(puzzle, row, col)) 
                        return 1;
                    else 
                        puzzle[row][col] = 0;
                }
            }
        }
        return 0;
    }
    else return 1;
}

int main(int argc, char *argv[])
{
    int i, j;
    char f[BUFFER_LEN] = {0};
    int puzzle[9][9] = {};
    if (argc == 2)
        strcpy(f, argv[1]);

    FILE* rf = fopen(f, "r");
    //fscanf(rf,"%d", &k);
    while (!feof(rf))
    {
        for (int a = 0 ; a < 9 ; a++)
        {
            for (int b = 0; b < 9; b++)
                fscanf(rf, "%d", &puzzle[a][b]);
        }
    }
    fclose(rf);

    FILE* wf = fopen("solution.txt", "w");
    if(solver(puzzle, 0, 0))
    {
        for(i=0; i<9; ++i)
        {
            for(j=0; j<9; ++j) 
                fprintf(wf,"%d ", puzzle[i][j]);
            fprintf(wf,"\n");
        }
    }
    else {
        fprintf(wf,"Invalid");
    }

    fclose(wf);

    return 0;
}