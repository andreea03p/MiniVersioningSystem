#include <stdio.h>
#include <ctype.h>

rtertert
/*
// pb1

int main (void)
{

  char c, d;
  int ct = 0;
  d = getchar();

  while( (c = getchar()) != EOF)
    {
      if( (c == '\n' || c == '\t' || c == ' ') && isalpha(d) )
	ct++;
      d = c;
    }

  printf("%d \n", ct);
  
  return 0;
}
*/


/*
// pb 2

int main(void)
{
  char c, d;
  d = getchar();

  while( (c = getchar()) != EOF )
    {
      if( d == '/')
	{
	  if( c == '/' )
	    {
	      while( c != '\n' )
		{
		  c = getchar();
		  d = c;
		}
	      c = getchar();
	      d = c;
	      printf("\n");
	    }
	  else if( c == '*' )
	    {
	      while( d != '*' && c != '/' )
		{
		  c = getchar();
		  d = c;
		}
	      c = getchar();
	      d = c;
	      c = getchar();
	      d = c;
	    }
	}
      printf("%c", d);
      d = c;
    }
  return 0;
}
*/


/*
// pb 3

int v[100];

int main()
{
  int s = 0;
  char c;

  while((c=getchar()) != EOF)
    {
      if(isalpha(c))
	v[c]++;
    }

  for(int i='a', j='A'; i<='z', j<='Z'; i++, j++)
    s = s + v[i] + v[j];
  printf("%d \n", s);
  
  for(int i = 'a'; i <= 'z'; i++)
    {
      if(v[i])
	{
	  float l1 = v[i]*100/s;
	  printf("%c %.2f %% \n", i, l1);
	}
    }

    for(int i = 'A'; i <= 'Z'; i++)
    {
      if(v[i])
	{
	  float l2 = v[i]*100/s;
	  printf("%c %.2f %% \n", i, l2);
	}
    }
  
  return 0;
}
*/



/*
// pb 4

int main()
{
  char c;

  while ( (c=getchar()) != EOF )
    {
      if( c >= 'a' && c <= 'z' )
	c = c - 32;
      else if ( c >= 'A' && c <= 'Z' )
	c = c + 32;

      putchar(c);
    }
  
  return 0;
}
*/


/*
// pb 5

int main()
{
  int car = 0, lines = 0, cuv = 0;
  char c, d;
  
  d = getchar();
  if( d>=0 && d<=127)
    car++;
  
  while( (c=getchar()) != EOF)
    {
      if( c>=0 && c<=127)
	car++;
      if( c == '\n' )
	lines++;
      if((c == ' ' || c == '\n' || c == '\t') && isalpha(d))
	  cuv++;

      d = c;
    }

  printf("%7d %d %d \n", car, lines, cuv); 
  
  return 0;
}
*/


/*
// pb 6

int main()
{

  char c, d;
  int ct = 0;
  
  d = getchar();

  while( (c = getchar()) != EOF)
    {
      if((isalpha(d) || isdigit(d) || d == '.' || d == '_') && (c == ' ' || c == '\n' || c == '\t'))
	ct++;
      d = c;
    }

  printf("%d \n", ct);
  
  return 0;
}
*/


/*
// pb 7

int main()
{
  char c, d;
  int cuv = 0, mx = 0, t = 0;
  
  d = getchar();
  if(d >= 'a' && d <= 'z')
     d = d - 32;
  printf("%c", d);
  while( (c = getchar()) != EOF)
    {
      if( (d==' ' || d=='\n' || d=='\t') && isalpha(c))
	{
	  if(c>='a' && c<='z')
	    c -= 32;
	}
      if( (c == ' ' || c == '\n' || c == '\t') && isalpha(d) )
	{
	  cuv++;
	  t++;
	}
      if(c == '\n')
	{
	  if(cuv>mx)
	    {
	      mx = cuv;
	      cuv = 0;
	    }
	}
      d = c;
      printf("%c", c);
    }

  printf("%d\n%d\n", t, mx); 
  
  return 0;
}
*/

