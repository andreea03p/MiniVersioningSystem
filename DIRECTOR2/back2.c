#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int x, y;
} puncte;

int valid(int px, int py, int limita_x, int limita_y, int ma[][4], int energie_curent, int viz[][4]) 
{
  if (viz[px][py] == 1) return 0;
  if (px < 0 || px > limita_x || py < 0 || py > limita_y) return 0;
  if (ma[px][py] == -1) return 0;
  if (energie_curent == 0) return 0;
  return 1;
}

int solutie(int k, puncte v[], int limita_x, int limita_y) 
{
  if (v[k - 1].x == 0 || v[k - 1].y == 0 || v[k - 1].x == limita_x || v[k - 1].y == limita_y) 
  {
    return 1;
  }
  return 0;
}

void bck(int k, int ma[][4], puncte v[], int energie, int deplasare_linie[], int deplasare_coloana[], int limita_x, int limita_y, int viz[][4]) 
{
  if (solutie(k, v, limita_x, limita_y)) 
  {
    printf("coordonatele drumului sunt: ");
    for (int i = 0; i < k; i++)
    {
        printf("%d - %d, ", v[i].x, v[i].y);
    }
    printf("\n");
    return;
  } 
  else
  {
    for (int i = 0; i <= 3; i++) 
    {
      int px = v[k - 1].x + deplasare_linie[i];
      int py = v[k - 1].y + deplasare_coloana[i];
      if (valid(px, py, limita_x, limita_y, ma, energie, viz)) 
      {
        v[k].x = px;
        v[k].y = py;
        viz[px][py] = 1;
        bck(k + 1, ma, v, energie - 1 + ma[px][py], deplasare_linie, deplasare_coloana, limita_x, limita_y, viz);
        viz[px][py] = 0;
      }
    }
  }
}

int main(void) 
{
  puncte v[20];
  int ma[4][4] = {{0, -1, 0, 0}, {-1, 3, 0, 1}, {-1, 0, -1, 2}, {0, 0, -1, -1}}, viz[4][4]; // viz e pt vizitate, ca sa nu se mearga pe acelasi drum
  int deplasare_linie[] = {-1, 1, 0, 0}, deplasare_coloana[] = {0, 0, -1, 1};

  v[0].x = 2;
  v[0].y = 1;

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
        viz[i][j] = 0;
    }
  }
  viz[2][1] = 1;

  for (int i = 0; i < 4; i++) 
  {
    for (int j = 0; j < 4; j++)
    {
        printf("%d ", ma[i][j]);
    }
    printf("\n");
  }

  bck(1, ma, v, 3, deplasare_linie, deplasare_coloana, 3, 3, viz);

  return 0;
}
