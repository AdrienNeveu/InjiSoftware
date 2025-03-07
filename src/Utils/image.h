#ifndef __IMAGE__
#define __IMAGE__
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>

#define M_PI 3.14159265358979323846

/*
	Image Utils
*/

// Structure representing a Pixel
typedef struct gdk_pixel
{
	guchar r, g, b, a;
} Pixel;

// Base structure of the image to be manipulated
typedef struct gdk_image
{
	int width, height, bits_per_sample;
	gboolean has_alpha;
	Pixel **pixList;
} Image;

// Base structure of a GrayScale image
typedef struct
{
	int width, height;
	guchar** intensity;
} ImageGS;

// Base structure of a Binary Image
typedef struct
{
	int width, height;
	int** data;
}ImageBN;

// Creates an Image structure from the path str
Image* ULoadImage(char* str);

// Free the Pixels buffer of a RGB Image
void UFreeImage(Image *img);

// Free the Pixels buffer of a Gray Scale Image
void UFreeImageGray(ImageGS *img);

// Free the Pixels buffer of a Binary Image
void UFreeImageBinary(ImageBN *img);

// Get Pixel at Coordinate
Pixel UGetPixel(GdkPixbuf *pixbuf, int x, int y);

// Translate RGB Image to GrayScale Image
ImageGS *URgbToGrayscale(Image *rgbImage);

// Get Local Threshold using Sauvola Method
    // Local Threshold computed inside the region (x, y, size) = Square Region
guchar UGetLocalThreshold(ImageGS *ref, int x, int y,
        int width, int height);

// Split Img into Tile - Compute Tile's Threshold - Binarize the image
ImageBN *UGrayscaleToBinary(ImageGS *ref);

// Translate GrayScale Image to RGB Image
Image *UGrayscaleToRgb(ImageGS *reference);

guchar UGetOtsuThreshold(ImageGS *ref);

// RGBImage to Binary Image
ImageBN *URgbToBinary(Image *ref);

// Binary to Rgb Image
Image *UBinaryToRgb(ImageBN *ref);

Image *ImageCopy(Image *img);

ImageBN *NegativeBinaryImage(ImageBN *img);

// Get packed pixels data from Pixels structure
guchar* UGetPixelDataFromPixelsStruct(Pixel **pixlist, int width,
	int height, int channel);

// Create pixbuf from Pixel struct
GdkPixbuf *UGetPixbufFromImage(Image *img, guchar **tmpPixels);

//Dilate the white pixels
ImageBN *DilatationOnBinary(ImageBN *img, int coef);

// Rotate an image matrix left/right
void URotateImage(Image *img);

extern const Pixel RED;
extern const Pixel GREEN;
extern const Pixel BLUE;

#endif
