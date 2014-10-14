#include "matrixOperation.h"

Pixel** UMultiply(Pixel **matrix, double **convolution, int matrixSize)
{
	Pixel **tmp;

	tmp = malloc(matrixSize * sizeof(Pixel *));

	for (int i = 0; i < matrixSize; i++)
	{
		tmp[i] = malloc(matrixSize * sizeof(Pixel));
	}	

	for (int y = 0; y < matrixSize; y++)
	{
		for (int x = 0; x < matrixSize; x++)
		{
			for(int i = 0; i < x; i++)
			{
				tmp[x][y].r += (matrix[i][y].r *
					(guchar)convolution[x][i]);
				tmp[x][y].g += (matrix[x][y].g *
					(guchar)convolution[y][x]);
				tmp[x][y].b += (matrix[x][y].b *
					(guchar)convolution[y][x]);
			}
		}
	}

	return tmp;
}

Pixel** UExtract(Pixel **matrix, int matrixSize, int extractSize, int pos_x,
	int pos_y)
{
	Pixel **tmp;
	Pixel defaultPix;
	defaultPix.r = 255;
	defaultPix.g = 255;
	defaultPix.b = 255;
	defaultPix.a = 255;

	tmp = malloc(extractSize * sizeof(Pixel *));
	for (int i = 0; i < extractSize; i++)
	{
		tmp[i] = malloc(extractSize * sizeof(Pixel));
	}
	
	
	for (int y = pos_y - (extractSize/2); y < pos_y + (extractSize/2); y++)
	{
		for (int x = pos_x - (extractSize/2); x < pos_x + (extractSize/2); x++)
		{
			if (x >= 0 && x < matrixSize &&
					y >= 0 && y < matrixSize)
				tmp[x][y] = matrix[x][y];
			else
				tmp[x][y] = defaultPix;
		}
	}

	return tmp;
}

Pixel** UConvolution(Pixel **matrix, double **convolution, int size,
	int matrixSize)
{
	Pixel **result;
	Pixel **subMatrix;

	result = malloc(size * sizeof(Pixel *));
	for (int i = 0; i < size; i++)
	{
		result[i] = malloc(size * sizeof(Pixel));
	}
	
	for (int y = 0; y < matrixSize; y++)
	{
		for (int x = 0; x < matrixSize; x++)
		{
			subMatrix = UExtract(matrix, size, matrixSize, x, y);
			subMatrix = UMultiply(subMatrix, convolution,
				matrixSize);

			result[x][y] = subMatrix
				[matrixSize / 2][matrixSize / 2];
		}
	}

	return result;
}

Pixel** URotate(Pixel **matrix, double angle, int width, int height)
{
	double radian = (angle * 3.1415) / 180;
	int newWidth = (int)(cos(radian) * width + sin(radian) * height + 1);
	int newHeight = (int)(-cos(radian) * height - sin(radian) * width + 1);

	Pixel **image;
	image = malloc(newWidth * sizeof(Pixel *));

	for (int i = 0; i < width; i++)
	{
		image[i] = malloc(newHeight * sizeof(Pixel));
	}

	printf("1\n");

	for (int y = 0; y <newHeight; y++)
		for (int x = 0; x < newWidth; x++)
		{	
				image[x][y].r = 255;
				image[x][y].g = 255;
				image[x][y].b = 255;
				image[x][y].a = 255;
		}


	printf("2\n");

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int newX = (int)(cos(radian) * x + sin(radian) * y);
			int newY = (int)(cos(radian) * y - sin(radian) * x);

			if ((newX >= 0 && newX < newWidth) && 
				(newY >= 0 && newY < newHeight))
			{
				image[newX][newY] = matrix[x][y];
			}

		}
	}

	printf("3\n");
	return image;

}
