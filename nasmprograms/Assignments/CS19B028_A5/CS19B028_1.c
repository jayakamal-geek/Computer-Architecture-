#include<stdio.h>   //general header for all general functions of C
#include<stdlib.h>  //header file for randomisation of the inputs
#include<time.h>    //header file for all time related functions
#include<sys/time.h>    //headerfile for gettimeofday()
#define MatrixSize 1024 //defining the matrix Size as a global constant

/*-------------------------------------------------------------------------------------------------
*    Author   : Maddula Jaya Kamal(cs19b028)
*    Code     : C code to implement multiplication of two square matrices(A&B row major)
*    Question : CS2610 Assignment-5
-------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------
*    Function Name : main
*    Args          : Nil
*    Return Type   : int(0)
*    Application   : Entry point to the Program
-------------------------------------------------------------------------------------------------*/
int main()  //entry point of the code
{
    struct timeval begin, end;
    gettimeofday(&begin, NULL); //time stamp at the start of the program

    int **A, **B, **C;    //A and B being Matrix operands and C being the Destination of the result
    srand(time(0)); //
	A = (int**)malloc(MatrixSize*sizeof(int*));   //allocating heap memory
	B = (int**)malloc(MatrixSize*sizeof(int*));   //allocating heap memory
	C = (int**)malloc(MatrixSize*sizeof(int*));   //allocating heap memory
    
    for(int i=0; i<MatrixSize; i++)
    {
		A[i] = (int*)malloc(MatrixSize*sizeof(int));  //allocating heap memory
		B[i] = (int*)malloc(MatrixSize*sizeof(int));  //allocating heap memory
        C[i] = (int*)malloc(MatrixSize*sizeof(int));  //allocating heap memory

        for(int j=0; j<MatrixSize; j++)
        {
            A[i][j] = rand()%11;    //genarating a random number x using rand and taking mod 11 to create a number from 0 to 10.
            B[i][j] = rand()%11;    //genarating a random number x using rand and taking mod 11 to create a number from 0 to 10.
        }
    }
    
    for(int i=0; i<MatrixSize; i++) //for loop to control traversal using Matrix Multiplication
    {
        for(int j=0; j<MatrixSize; j++) //for loop to control traversal using Matrix Multiplication
        {
            C[i][j] = A[i][0]*B[0][j];
            for(int k=1; k<MatrixSize; k++) //for loop to control traversal using Matrix Multiplication
                C[i][j] += A[i][k]*B[k][j]; //carrying out multiplication row elements of A with corresponding column Elements of B 
        }
    }

    gettimeofday(&end, NULL);   //obtaining the time stamp at the end of the program
    double timeTaken = (end.tv_sec - begin.tv_sec)*1e6; //calculating the time taken to run the code(tv_sec is seconds)
    timeTaken = (timeTaken + (end.tv_usec - begin.tv_usec))*1e-6; //calculating the time taken to run the code(tv_usec is micro-seconds)
    printf("%lf \n", timeTaken);   //printing the time taken to run the program

    free(A);    //freeing the allotted heap memory
    free(B);    //freeing the allotted heap memory
    free(C);    //freeing the allotted heap memory

    return 0;   //returning 0 on succesful execution of the code
}