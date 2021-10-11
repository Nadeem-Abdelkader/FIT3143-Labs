#include <stdio.h>
#include <string.h>

int main()
{
	char line[100], *sub_text;
		/* initialize string */
		
	strcpy(line,"hello, I am a string;");
	
	printf("Line: %s\n", line);
		/* add to end of string */
	
	strcat(line," what are you?");
	
	printf("Line: %s\n", line);
		/* find length of string */
		/* strlen brings back */
		/* length as type size_t */
		
	printf("Length of line: %d\n", (int)strlen(line));
	
		/* find occurence of substrings */
	if ((sub_text = strchr(line, 'W'))!= NULL)
		printf("String starting with \"W\" ->%s\n", sub_text);
	if ((sub_text = strchr(line, 'w'))!= NULL)
		printf("String starting with \"w\" ->%s\n", sub_text);
	if ((sub_text = strchr(sub_text, 'u'))!= NULL)
		printf("String starting with \"u\" ->%s\n", sub_text);
		
	return(0);
}
