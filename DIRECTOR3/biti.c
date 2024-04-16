#include <stdio.h>
#include <stdint.h>


/*
pb8

int main(void)
{
  uint8_t n;
  
  scanf("%hhd", &n);

  showBits(n);

  n = n | (1<<0);
  n = n | (1<<2);
  n = n | (1<<3);
  n = n & ~(1<<1);  
  n = n & ~(1<<5);
  n = n & ~(1<<6);
  n = n ^ (1<<7);
  n = n ^ (1<<4);

  printf("%hhd\n", n); 

  showBits(n);
  
  return 0;
}
*/



/*
pb 9

int main(void)
{

  char x;
  int a, b;
  scanf("%d%d", &a, &b);
  showBits(a);
  showBits(b);
  
  x = x | a;
  x = x << 4;
  x = x | b;

  printf("%d\n", x&15);
  
  x = x >> 4;
  
  printf("%d\n", x);

  
  return 0;
}
*/



/*
//pb 10 ????????

int main(void)
{
  int n;
  uint32_t mask;
  mask = 1 << 31;
  // printf("%ld", sizeof(int));
  
  scanf("%d", &n);

  if(n & mask)
    {
      printf("1\n");
    }
  else
    {
      printf("0\n");
    }

  return 0;
}
*/



/*
pb 11

int main(void)
{

  int x, s=0;
  scanf("%d", &x);

  s = s + (x&1);
  
  for(int i=0; i<3; i++)
    {
      x = x >> 1;
      s += (x&1);
    }
      
  printf("%d\n", s); 
  
  return 0;
}
*/



/*
pb 13

int main(void)
{
  int ct = 0;
  unsigned int a;
  scanf("%u", &a);

  for(int i = 0; i <= sizeof(a)*8-1; i++)
    {
      if( ((a>>i) & 1) == 0)
	ct;
      else
	ct++;
    }

  printf("%d\n", ct);
  return 0;
}
*/



/*
pb 15

int main(void)
{

  int ct = 1;
  unsigned a;
  scanf("%u", &a);

  int ok = a&1;
  
  for(int i=1; i<sizeof(a)*8; i++)
    {
      if(((a>>i) & 1) != ok)
	{
	  ok = (a>>i)&1;
	  ct++;
	}
    }
    
  printf("%d\n", ct);
  return 0;
}
*/



/*
//pb 16

unsigned int myAdd(unsigned int a, unsigned int b)
{
    unsigned int carry = a & b;
    unsigned int result = a ^ b;
    while(carry != 0)
    {
        unsigned int shiftedcarry = carry << 1;
        carry = result & shiftedcarry;
        result ^= shiftedcarry;
    }
    return result;
}

int main(void)
{
  unsigned a, b;
  scanf("%u%u", &a, &b);
  
  printf("%d\n", myAdd(a, b));
  
  return 0;
}
*/



/*
// pb 18

void showBits(unsigned a){
    int i;
    for(i=sizeof(a)*8-1;i>=0;i--)
        printf("%d",(a>>i)&1);
    printf("\n");
}

void octet(long long int x, long int n, int p)
{
    long long int msk = 0xFFFFFFFF;
    long long int aux = n << ((4-p)*8);
    long long int aux1;
    showBits(aux);
    
    switch (p)
    {
        case 1:
            aux1 = 0x00FFFFFF;
            break;
        case 2:
            aux1 = 0xFF00FFFF;
            break;
        case 3:
            aux1 = 0xFFFF00FF;
            break;
        case 4:
            aux1 = 0xFFFFFF00;
            break;
    }
    
    x = x & aux1;
    x = x | aux;
    
    showBits(x);
}

int main(void)
{
    octet(911947355, 239, 3);

    return 0;
}
*/



/*
pb 20

int fct(uint8_t n1, uint8_t n2)
{
   uint16_t a = 0;
   a = a | n2;
   a = a << 8;
   a = a | n1;
   return a;
}

int main(void)
{
  uint16_t a = 0;
  uint8_t n1, n2;

   scanf("%hhd%hhd", &n1, &n2);
   a =  fct(n1, n2);
   showbits(a);

   return 0;
}
*/



/*
pb 21

long int biti(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)
{
  int v[10] = {n3, n2, n1, n0};
  uint64_t x = 0;
 
  for(int i=0; i<4; i++)
    {
      x = x << 8;
      x = x | v[i];
    }
  return x;
}

int main()
{

  uint8_t n0,n1,n2,n3;

  scanf("%hhd%hhd%hhd%hhd", &n0, &n1, &n2, &n3);

  printf("%ld\n", biti(n0,n1,n2,n3));
  
  return 0;
}
*/



/*
pb  22

long long int biti(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4, uint8_t n5, uint8_t n6, uint8_t n7)
{
  int v[10] = {n7, n6, n5, n4, n3, n2, n1, n0};
  uint64_t x = 0;
 
  for(int i=0; i<8; i++)
    {
      printf("%ld\n ", x);
      x = x << 8;
      x = x | v[i];
    }
  return x;
}

int main()
{

  uint8_t n0,n1,n2,n3,n4,n5,n6,n7;

  scanf("%hhd%hhd%hhd%hhd%hhd%hhd%hhd%hhd", &n0, &n1, &n2, &n3, &n4, &n5, &n6, &n7);

  printf("%lld\n", biti(n0,n1,n2,n3,n4,n5,n6,n7));
  
  return 0;
}
*/






/*
pc sub b pb 1
void showBits(unsigned a)
{
    int i;
    for(i = 7; i >= 0; i--)
        printf("%d",(a>>i)&1);
    printf("\n");
}

int main(void)
{
  uint8_t a, b, n;
  scanf("%hhd%hhd", &a, &b);
  showBits(a);
  showBits(b);
  
  for(int i=0; i<8; i++)
    {
      if( ((a & (1<<i)) != 0) && ((b & (1<<i)) != 0) )
	n = n & ~(1 << i);
      else
	n = n | (1 << i);
    }

  printf("%d\n", n);
  showBits(n);
  
  return 0;
}



pb 2
int sortare(int *v, int n)
{
  int ok, aux;
  ok = 0;

  for(int i=0; i<n-1; i++)
    for(int j=i; j<n; j++)
      {
	if(v[i]>v[j])
	  {
	    aux = v[i];
	    v[i] = v[j];
	    v[j] = aux;
	    ok = 1;
	  }
      }
  
  return ok;
}

int fv[100]={0};

int main(void)
{
  int a[100], b[100], c[100];
  int n, m;

  scanf("%d%d", &n, &m);

  for(int i=0; i<n; i++)
    {
      scanf("%d", &a[i]);
      fv[a[i]]=1;
    }

   for(int i=0; i<m; i++)
    scanf("%d", &b[i]);

   if(sortare(a,n)==0)
     printf("vectorul a e sortat \n");
   else
     printf("vectorul a nu a fost sortat \n");

   if(sortare(b,m)==0)
     printf("vectorul b e sortat \n");
   else
     printf("vectorul b nu a fost sortat \n");

   for(int i=0; i<n; i++)
    printf("%d ", a[i]);
   printf("\n");

   for(int i=0; i<m; i++)
    printf("%d ", b[i]);
   printf("\n");

   int k=0;
   for(int i=m-1; i>=0; i--)
     {
       if(fv[b[i]]!=0)
	 c[k++] = b[i];
     }

   for(int j=0; j<k; j++)
     printf("%d ", c[j]);
   printf("\n");

   
   return 0;
}
*/


// pb 14

void showBits(unsigned  int n){
  int i;
  for(i = sizeof(n) * 8 -1; i >= 0; i--)
    printf("%u",(n>>i) & 1);
  printf("\n");
}

unsigned int reverseNibbleOrder(unsigned int n)
{
  unsigned int reversed = 0;
  
  for (int i = 0; i < sizeof(n) * 2; i++)
    {
      reversed = (reversed << 4) | (n & 0xF);
      n >>= 4;
    }
  
  return reversed;
}

unsigned int inversarea_biti_din_nibble(unsigned int n){
  unsigned int m = 0;
  int i;
  for(i = 7; i >=0; i--)
    {
      unsigned int nibble = ((n>>(i*4)) & 0xF);
      unsigned int nibble_aux = (nibble<<3) & (1<<3);
      nibble_aux = nibble_aux |((nibble<<1) & (1<<2));
      nibble_aux = nibble_aux | ((nibble>>1) & (1<<1));
      nibble_aux = nibble_aux | ((nibble>>3) & (1));
      m = m | (nibble_aux<<(i*4));    
  }
  
  return m;
}

int main(void)
{
  unsigned int a;
  scanf("%d", &a);

  unsigned int r = reverseNibbleOrder(a);
  unsigned int rev = inversarea_biti_din_nibble(a);

  showBits(a);
  showBits(r);
  showBits(rev);
   
  return 0;
}
