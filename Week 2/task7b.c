#include <stdio.h>

 int main()
{
	 FILE *fp;
	 int i;
	 
	 fp = fopen("foo.dat", "w"); /* open foo.dat for writing 
	*/
	
	 fprintf(fp, "\nSample Code\n\n"); /* write some info */
	 for (i = 1; i <= 10 ; i++)
	fprintf(fp, "i = %d\n", i);
	
	 fclose(fp); /* close the file */
	return(0);
}
