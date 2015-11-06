#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int puzzle[9][9] = {};

int checker(int row, int col, int num)
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
        if (puzzle[rowStart + (i % 3)][colStart + (i/3)] == num) 
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
            if((col + 1) < 9) 
                return solver(row, col + 1);
            else if((row + 1) < 9) 
                return solver(row + 1, 0);
            else 
                return 1;
        }
        else
        {
            for(int i = 0; i < 9; i++)
            {
                if(checker(row, col, i + 1))
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

typedef struct 
{
    int row;
    int col;
} parameters;

void * rowValidator(void * arg) {
    parameters * data = (parameters *) arg;
    for (int i = data->row; i < 9; i++) {
        int row[10] = {0};
        for (int j = data->col; j < 9; j++) {
            int value = puzzle[i][j];
            if (row[value] != 0) {
                return (void *) 0;
            }
            else{
                row[value] = 1;
            }
        }
    }
    return (void *) 1;
}

void * colValidator(void * arg) {
    parameters * data = (parameters *) arg;
    for (int i = data->col; i < 9; i++) {
        int col[10] = {0};
        for (int j = data->row; j < 9; j++) {
            int value = puzzle[j][i];
            if (col[value] != 0) {
                return (void *) 0;
            }
            else{
                col[value] = 1;
            }
        }
    }
    return (void *) 1;
}

void * subGridValidator(void * arg) {
    parameters * data = (parameters *) arg;
    int subGrid[10] = {0};
    for (int i = data->row; i < data->row + 3; i++) {
        for (int j = data->col; j < data->col + 3; j++) {
            int value = puzzle[i][j];
            if (subGrid[value] != 0) {
                return (void *) 0;
            }
            else{
                subGrid[value] = 1;
            }
        }
    }
    return (void *) 1;
}

int main() 
{
    // solve the puzzle
    FILE* f = fopen("puzzle.txt", "r");
    while (!feof(f))
    {
        for (int i = 0 ; i < 9 ; i++)
        {
            for (int j = 0; j < 9; j++)
                fscanf(f, "%d", &puzzle[i][j]);
        }
    }
    fclose(f);

    f = fopen("solution.txt", "w");
    if(solver(0, 0) == 1)
    {
        for(int i = 0; i < 9; i++)
        {
            for(int j = 0; j < 9; j++) 
                fprintf(f,"%d ", puzzle[i][j]);
            fprintf(f,"\n");
        }
    }
    else {
        fprintf(f,"Invalid");
    }

    fclose(f);

    // Valid the puzzle

    f = fopen("solution.txt", "r");
    while (!feof(f))
    {
        for (int i = 0 ; i < 9 ; i++)
        {
            for (int j = 0; j < 9; j++)
                fscanf(f, "%d", &puzzle[i][j]);
        }
    }

    fclose(f);

    
    parameters * rowColParam = (parameters *) malloc(sizeof(parameters));
    rowColParam->row = 0;
    rowColParam->col = 0;

    parameters *subGridParam[9];
    
    subGridParam[0] = (parameters *) malloc(sizeof(parameters));
    subGridParam[0]->row = 0;
    subGridParam[0]->col = 0;
    
    subGridParam[1] = (parameters *) malloc(sizeof(parameters));
    subGridParam[1]->row = 0;
    subGridParam[1]->col = 3;
    
    subGridParam[2] = (parameters *) malloc(sizeof(parameters));
    subGridParam[2]->row = 0;
    subGridParam[2]->col = 6;
    
    subGridParam[3] = (parameters *) malloc(sizeof(parameters));
    subGridParam[3]->row = 3;
    subGridParam[3]->col = 0;

    subGridParam[4] = (parameters *) malloc(sizeof(parameters));
    subGridParam[4]->row = 3;
    subGridParam[4]->col = 3;
    
    subGridParam[5] = (parameters *) malloc(sizeof(parameters));
    subGridParam[5]->row = 3;
    subGridParam[5]->col = 6;
    
    subGridParam[6] = (parameters *) malloc(sizeof(parameters));
    subGridParam[6]->row = 6;
    subGridParam[6]->col = 0;

    subGridParam[7] = (parameters *) malloc(sizeof(parameters));
    subGridParam[7]->row = 6;
    subGridParam[7]->col = 3;
    
    subGridParam[8] = (parameters *) malloc(sizeof(parameters));
    subGridParam[8]->row = 6;
    subGridParam[8]->col = 6;

    pthread_t rowsThread, colsThread, subGridThread[9];
    
    void * status[11];
    
    pthread_create(&rowsThread, NULL, rowValidator, (void *) rowColParam);
    pthread_create(&colsThread, NULL, colValidator, (void *) rowColParam);

    for (int i = 0 ; i < 9; i++) {
        pthread_create(&subGridThread[i], NULL, subGridValidator, (void *) subGridParam[i]);  
    }

    pthread_join(rowsThread, &status[0]);
    pthread_join(colsThread, &status[1]);

    for (int i = 0; i < 9; i++) {
        pthread_join(subGridThread[i], &status[i+2]);
    }

    if ((int) status[0] == 1 && (int) status[1] == 1 && (int) status[2] == 1 && (int) status[3] == 1 &&
        (int) status[4] == 1 && (int) status[5] == 1 && (int) status[6] == 1 && (int) status[7] == 1 &&
        (int) status[8] == 1 && (int) status[9] == 1 && (int) status[10] == 1 ) {
        printf("Valid!\n");
    }
    else {
        printf("Not Valid!\n");
    }
    
}