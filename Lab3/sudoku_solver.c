#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_LEN 256

int puzzle[9][9] = {};

int check(int row, int col, int num)
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

int solver(int row, int col)
{
    if(row < 9 && col < 9)
    {
        if(puzzle[row][col] != 0)
        {
            if((col+1)<9) 
                return solver(row, col+1);
            else if((row+1)<9) 
                return solver(row+1, 0);
            else 
                return 1;
        }
        else
        {
            for(int i = 0; i < 9; i++)
            {
                if(check(row, col, i + 1))
                {
                    puzzle[row][col] = i + 1;
                    if(solver(row, col)) 
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

int main()
{

    FILE* rf = fopen("puzzle.txt", "r");
    //fscanf(rf,"%d", &k);
    while (!feof(rf))
    {
        for (int i = 0 ; i < 9 ; i++)
        {
            for (int j = 0; j < 9; j++)
                fscanf(rf, "%d", &puzzle[i][j]);
        }
    }
    fclose(rf);

    FILE* wf = fopen("solution.txt", "w");
    if(solver(0, 0) == 1)
    {
        for(int i = 0; i < 9; i++)
        {
            for(int j = 0; j < 9; j++) 
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