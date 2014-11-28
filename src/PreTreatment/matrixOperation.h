#ifndef MATRIXOPERATION_H
#define MATRIXOPERATION_H

#include "../Utils/image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct c_vector2{

	int x,y;

}Vector2;

// Simple clamp function applied on all channels of a given Pixel
void ClampPixel(Pixel *pix, int min, int max);

int ClampDouble(double a, int min, int max);

// Modify the position of a point according to a angle of rotation.
Vector2 ApplyVectorRot(Vector2 origin, double radian);

// Get min and max values from several vectors
void ExtremumVectorValues(Vector2 *tab, int arraySize, 
	Vector2 *min, Vector2 *max);

// Apply A Convolution Matrix On Every Pixel
Image *UConvolution(Image *ref, double **convolution, int matrixSize);

// Rotate A Given Matrix
Image *URotate(Image *ref, double angle);

#endif
