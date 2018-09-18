#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 10
#define MAGIC_NUMBER 20180907

// Some kind of atempt at a hashing function...
int elementValue(int x, int y)
{
  return ((x*x*x + y*y + MAGIC_NUMBER) % (4*SIZE*SIZE));
}

int main()
{
  // Instantiate stuff
  int matrix[SIZE][SIZE];
  FILE *mFile;

  // File name
  const char * matrixFilename = "matrix.csv";
  
  // Max number of characters needed to represent the matrix elements.
  int longestNumber = 3;  
    
  // Populate the matrix with values.
  // Important to keep track of which index is for rows and which is for
  // columns. As implemented: i = y, j = x.
  puts("Creating matrix..");
  for(int i = 0; i < SIZE; i++)
    for(int j = 0; j < SIZE; j++)
      {
	matrix[j][i] = elementValue(j,i);
	//printf("(%i,%i): %i\n", j, i, matrix[i][j]);
      }
  
  // Write the matrix to file.
  // This could be done in the loop above but this way is more explicit.
  puts("Writing to file...");
  mFile = fopen(matrixFilename, "w+");
  for(int i = 0; i < SIZE; i++)
  {
    for(int j = 0; j < SIZE; j++)
      {
	// Write element to file
	fprintf(mFile, "%i", matrix[j][i]);
	
	// Add commas for all but last column
	if(j != SIZE-1)
	  fputs(",", mFile);
      }

    // Add newline char for all but last row
    if(i != SIZE-1)
      fputs("\n", mFile);
  }


  // Compare matrix in memory to matrix on file
  puts("Comparing matrices...");
  rewind(mFile);
  int c;
  int n = 0, x = 0, y = 0;
  char valc[4];
  int read_matrix[SIZE][SIZE];
  
  while((c = getc(mFile)) != EOF)
  {
    //putchar((char)c);
    if((char)c == ',')
    {
      x++; valc[n] = '\0'; n = 0;
      read_matrix[x][y] = (int)strtol(valc, NULL, 10);
      //puts(valc);
    }
    else if((char)c == '\n')
    {
      y++; x = 0; valc[n] = '\0'; n = 0;
      read_matrix[x][y] = (int)strtol(valc, &valc[3], 10);
    }
    else
    {
      valc[n++] = (char)c;
    }
    
  }

  
  for(int i = 0; i < SIZE; i++)
  {
    for(int j = 0; j < SIZE; j++)
    {
      // Write element to file
      printf("%i:%i\n", matrix[j][i], read_matrix[j][i]);
    }
  }
  
  // Clean up
  fclose(mFile);
  
  puts("All done!");

  return 0;
}
