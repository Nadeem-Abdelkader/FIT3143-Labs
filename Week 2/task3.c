#include <stdio.h>
#include <math.h>

int main() {

	int angle_degree;
	double angle_radian, pi, value;
	
	/* Print a header */
	printf("\nCompute a table of the sine function\n\n");
	
	/* obtain pi once for all */
	/* or just use pi = M_PI, where
		M_PI is defined in math.h
	*/
	pi = 4.0 * atan(1.0);
	printf(" Value of PI = %f \n\n", pi);
	
	printf(" angle Sine \n");
	
	angle_degree = 0; /* initial angle value */
	/* scan over angle */
	
	for (angle_degree; angle_degree <= 360; angle_degree = angle_degree + 10) {
		angle_radian = pi * angle_degree / 180.0;
		value = sin(angle_radian);
		printf(" %3d    %f \n ", angle_degree, value);
	}
	return (0);
}
