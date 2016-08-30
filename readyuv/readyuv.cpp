// readyuv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdlib.h"
#include <memory.h>
typedef unsigned char uchar;
int clip3(int x, int y, int z)
{
  if (z < x)
  {
    return x;
  }
  if (z > y)
  {
    return y;
  }
  return z;
}
int c1[] = { 1, 2, 1 };
int c2[] = { 0, 4, 4 };
void getf(int*f, int row, int col, uchar* buff444U, int stride)
{
  ::memset(f, 0, sizeof(int)*row*col);
  for (int i = 0; i < row; i++)
  {
    for (int j = 0; j < col; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        f[i*col + j] += c1[k] * ((int)buff444U[i*stride + clip3(0, stride - 1, 2 * j + k - 1)]);
      }
    }
  }
}
void getr(int*r, int row, int col,int*f,int stride)
{
  ::memset(r, 0, sizeof(int)*row*col);
  for (int i = 0; i < row; i++)
  {
    for (int j = 0; j < col; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        r[i*row+ j] += c2[k] * f[clip3(0, stride - 1, 2 * i + k - 1)*col + j];
      }
      r[i*row + j] = (r[i*row + j] + 32) >> 6;
    }
  }
}
void setcolor(int*r, int row, int col, uchar* buff)
{
  for (int i = 0; i < row; i++)
  {
    for (int j = 0; j < col; j++)
    {
      buff[i*col + j] = (uchar)r[i*col + j];
    }
  }
}
int _tmain(int argc, _TCHAR* argv[])
{

  int H = 1080, W = 1920;
  FILE* fp444 = fopen("D:\\Desktop\\学习\\SCC\\sc_desktop_1920x1080_60_8bit_444.yuv","rb");
  FILE* fp420 = fopen("D:\\Desktop\\学习\\SCC\\sc_desktop_1920x1080_60_8bit_downsample_121.yuv", "ab+");
  uchar *buff444Y = new uchar[W * H];
  uchar *buff444U = new uchar[W * H];
  uchar *buff444V = new uchar[W * H];

  uchar *buff420U = new uchar[W * H / 4];
  uchar *buff420V = new uchar[W * H / 4];

  int **f = new int *[H];
  for (int i = 0; i < H;i++)
  {
    f[i] = new int[W];
  }
  int **r = new int *[H/2];
  for (int i = 0; i < H/2; i++)
  {
    r[i] = new int[W/2];
  }

  int shift444;
  int shift420;
  int framesize = W * H * sizeof(uchar);
  int size = sizeof(uchar);
  int TotalFrame = 600;
  for (int frame = 0; frame < TotalFrame;frame++)
  {
    shift444 = frame  * W * H * sizeof(uchar) * 3;
    shift420 = frame * W * H * sizeof(uchar) * 3 / 2;

    fseek(fp444, shift444, 0);
    fseek(fp420, shift420, 0);
    fread(buff444Y, size, W * H, fp444);
    fwrite(buff444Y, size, W * H, fp420);

    fseek(fp444, shift444 + framesize, 0);
    fseek(fp420, shift420 + framesize, 0);
    fread(buff444U, size, W * H, fp444);
    {
      for (int i = 0; i < H; i++)
      {
        for (int j = 0; j < W / 2;j++)
        {
          f[i][j] = 0;
          for (int k = 0; k < 3;k++)
          {
            f[i][j] += c1[k] * buff444U[i*W + clip3(0, W - 1, 2 * j + k - 1)];
          }
        }
      }
      for (int i = 0; i < H / 2; i++)
      {
        for (int j = 0; j < W / 2; j++)
        {
          r[i][j] = 0;
          for (int k = 0; k < 3; k++)
          {
            r[i][j] += c2[k] * f[clip3(0, H - 1, 2 * i + k - 1)][j];
          }
          r[i][j] = (r[i][j] + 16) >> 5;
          buff420U[i * W/2 + j] = (uchar)r[i][j];
        }
      }
    }
    fwrite(buff420U, size, W * H / 4, fp420);


    fseek(fp444, shift444 + framesize * 2, 0);
    fseek(fp420, shift420 + framesize * 5 / 4, 0);
    fread(buff444V, size, W * H, fp444);
    {
      for (int i = 0; i < H; i++)
      {
        for (int j = 0; j < W / 2; j++)
        {
          f[i][j] = 0;
          for (int k = 0; k < 3; k++)
          {
            f[i][j] += c1[k] * buff444V[i*W + clip3(0, W - 1, 2 * j + k - 1)];
          }
        }
      }
      for (int i = 0; i < H / 2; i++)
      {
        for (int j = 0; j < W / 2; j++)
        {
          r[i][j] = 0;
          for (int k = 0; k < 3; k++)
          {
            r[i][j] += c2[k] * f[clip3(0, H - 1, 2 * i + k - 1)][j];
          }
          r[i][j] = (r[i][j] + 16) >> 5;
          buff420V[i * W/2 + j] = (uchar)r[i][j];
        }
      }
    }
    fwrite(buff420V, size, W * H / 4, fp420);
  }






  fclose(fp420);
  fclose(fp444);
}

