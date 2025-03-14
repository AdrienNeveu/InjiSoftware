#include "image.h"
#include <stdlib.h>
#include <stdio.h>

const Pixel RED = {255, 0, 0, 255};
const Pixel GREEN = {0, 255, 0, 255};
const Pixel BLUE = {0, 0, 255, 255};

Image* ULoadImage(char *str)
{
	Image *tmp = malloc(sizeof(Image));
	GError *error = NULL;

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(str, &error);

	if (!pixbuf || error)
	{
		tmp->width = 0;
		tmp->height = 0;
		tmp->has_alpha = FALSE;
		tmp->pixList = NULL;
		printf("%d : %s\n", error->code, error->message);
	}
	else
	{
		tmp->width = gdk_pixbuf_get_width(pixbuf);
		tmp->height = gdk_pixbuf_get_height(pixbuf);
		tmp->has_alpha = gdk_pixbuf_get_has_alpha(pixbuf);
		tmp->bits_per_sample = gdk_pixbuf_get_bits_per_sample(pixbuf);
		tmp->pixList = malloc(sizeof(Pixel*) * tmp->width);
		for (int i = 0 ; i < tmp->width; i ++)
		{
			tmp->pixList[i] = malloc(sizeof(Pixel) *
					tmp->height);
			for (int j = 0; j < tmp->height; j ++)
				tmp->pixList[i][j] = UGetPixel(pixbuf, i, j);
		}
	}
	g_object_unref(pixbuf);
	return tmp;
}

void UFreeImage(Image *img)
{
	for (int i = 0; i < img->width; i ++)
		free(img->pixList[i]);
	free(img->pixList);
	free(img);
}

void UFreeImageGray(ImageGS *img)
{
	for (int i = 0; i < img->width; i ++)
		free(img->intensity[i]);
	free(img->intensity);
	free(img);
}

void UFreeImageBinary(ImageBN *img)
{
	for (int i = 0; i < img->width; i ++)
		free(img->data[i]);
	free(img->data);
	free(img);
}

Pixel UGetPixel(GdkPixbuf *pixbuf, int x, int y)
{
	Pixel pixel;

	guchar *pixelsBuf = gdk_pixbuf_get_pixels(pixbuf);
	gint channel = gdk_pixbuf_get_n_channels(pixbuf);
	gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);

	int idx = y * rowstride + x * channel;
	pixel.r = pixelsBuf[idx];
	pixel.g = pixelsBuf[idx + 1];
	pixel.b = pixelsBuf[idx + 2];
	if (channel == 4)
		pixel.a = pixelsBuf[idx + 3];
	else
		pixel.a = 255;
	return pixel;
}

ImageGS *URgbToGrayscale(Image *rgbImage)
{
	ImageGS *result = malloc(sizeof(ImageGS));
	result->width = rgbImage->width;
	result->height = rgbImage->height;
	result->intensity = malloc(result->width * sizeof(guchar*));

	for(int x = 0; x < result->width; x++)
	{
		result->intensity[x] = malloc(result->height * sizeof(guchar));
		for (int y = 0; y < result->height; y ++)
		{
			result->intensity[x][y] = (
				(0.3 * (double)rgbImage->pixList[x][y].r) +
				(0.59 * (double)rgbImage->pixList[x][y].g) +
				(0.11 * (double)rgbImage->pixList[x][y].b))
					+ 0.5;
		}
	}
	return result;
}

Image *UGrayscaleToRgb(ImageGS *reference)
{
	Image *result = malloc(sizeof(Image));
	result->width = reference->width;
	result->height = reference->height;
	result->has_alpha = 1;
	result->bits_per_sample = 8;
	result->pixList = malloc(result->width * sizeof(Pixel*));

	for (int x = 0; x < result->width; x++)
	{
		result->pixList[x] = malloc(result->height * sizeof(Pixel));
		for(int y = 0; y < result->height; y++)
		{
			result->pixList[x][y].r = reference->intensity[x][y];
			result->pixList[x][y].g = reference->intensity[x][y];
			result->pixList[x][y].b = reference->intensity[x][y];
			result->pixList[x][y].a = 255;
		}
	}
	return result;
}

ImageBN *UGrayscaleToBinary(ImageGS *ref)
{
	ImageBN *result = malloc(sizeof(ImageBN));
	result->width = ref->width;
	result->height = ref->height;
        guchar threshold = UGetOtsuThreshold(ref);

	result->data = malloc(result->width * sizeof(int*));
	for (int x = 0; x < result->width; x++)
	{
		result->data[x] = malloc(result->height * sizeof(int));
		for (int y = 0; y < result->height; y++)
			result->data[x][y] =
                            ref->intensity[x][y] > threshold ? 1 : 0;
	}

	return result;
}

guchar UGetOtsuThreshold(ImageGS *ref)
{
    int hist [256]; // Pixels Histogram
    int pixNbr = ref->width * ref->height; // Total Pixels Numberi
    guchar threshold = 0;
    int sum = 0;
    int sumB = 0;
    float wbackgrd = 0;
    float wforegrd = 0;
    float varMax = 0;

    // Histogramm Initialization & Filling
    for(int i = 0; i < 256; i++)
        hist[i] = 0;
    for(int y = 0; y < ref->height; y++)
        for(int x = 0; x < ref->width; x++)
        {
            hist[ref->intensity[x][y]] += 1;
        }

    for(int i = 0; i < 256; i++)
        sum += i * hist[i];
    // Apply the otsu algorithm
    for(int i = 0; i < 256; i++)
    {
        wbackgrd += hist[i];
        if(!wbackgrd)
            continue;
        wforegrd = pixNbr - wbackgrd;
        if(!wforegrd)
            break;

        sumB += i * hist[i];
        float meanB = sumB / wbackgrd;
        float meanF = (sum - sumB) / wforegrd;

        float betweenVar = wbackgrd * wforegrd * (meanB - meanF)
		* (meanB - meanF);

        if(betweenVar > varMax)
        {
            varMax = betweenVar;
            threshold = i;
        }

    }

	if (threshold > 127)
		threshold = 127;
    return threshold;
}

/*guchar UGetLocalThreshold(ImageGS *ref, int x, int y,
      int width, int height)
{
    float local_threshold = 0;
    int maxNbr = width * height; // Save the number of elements
    float mean = 0; // Pixel mean value
    float variance = 0; // Use to calculate the Standard deviation
    float standard_deviation = 0; // Used in the Sauvola's formula
    float k = 0.4; // Fixed constant
    float R = 128; // Fixed range of standar deviation

    // Used to compute the mean
    for(int y2 = y; y2  < y + height; y2++)
        for(int x2 = x; x2 < x + width; x2++)
        {
            mean += ref->intensity[x2][y2];
        }
    mean /= maxNbr; // Compute mean pixel value

    // Used to compute the variance
    for(int y2 = y; y2  < y + height; y2++)
        for(int x2 = x; x2 < x + width; x2++)
        {
            variance += (ref->intensity[x][y] - mean) 
                * (ref->intensity[x][y] - mean);
        }

    variance /= maxNbr;
    standard_deviation = sqrt(variance);

    // Finally : compute the threshold
    local_threshold = mean * (1 + k * (standard_deviation / R - 1));

    return (guchar)round(local_threshold);

}*/

ImageBN *URgbToBinary(Image *ref)
{
	ImageGS *tmp = URgbToGrayscale(ref);
	ImageBN *r = UGrayscaleToBinary(tmp);
	UFreeImageGray(tmp);
	return r;
}

Image *UBinaryToRgb(ImageBN *ref)
{
	Image *result = malloc(sizeof(Image));
	result->width = ref->width;
	result->height = ref->height;
	result->has_alpha = 1;
	result->bits_per_sample = 8;

	result->pixList = malloc(result->width * sizeof(Pixel *));
	
	for (int x = 0; x < result->width; x++)
	{
		result->pixList[x] = malloc(result->height * sizeof(Pixel));
		for (int y = 0; y < result->height; y++)
		{
			result->pixList[x][y].r = ref->data[x][y] * 255;
			result->pixList[x][y].g = ref->data[x][y] * 255;
			result->pixList[x][y].b = ref->data[x][y] * 255;
			result->pixList[x][y].a = 255;
		}
	}
	return result;
}

Image *ImageCopy(Image *img)
{
	Image *r = malloc(sizeof(Image));
	r->width = img->width;
	r->height = img->height;
	r->bits_per_sample = img->bits_per_sample;
	r->has_alpha = img->has_alpha;
	r->pixList = malloc(sizeof(Pixel *) * r->width);
	for (int i = 0; i < r->width; i ++)
	{
		r->pixList[i] = malloc(sizeof(Pixel) * r->height);
		for (int j = 0; j < r->height; j ++)
			r->pixList[i][j] = img->pixList[i][j];
	}
	return r;
}

guchar* UGetPixelDataFromPixelsStruct(Pixel **pixList, int width, int height,
	int channel)
{
	guchar *tmp = malloc(sizeof(guchar) * width * height  * channel);
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			tmp[x*channel + y*width*channel] = pixList[x][y].r;
			tmp[x*channel + y*width*channel + 1] = pixList[x][y].g;
			tmp[x*channel + y*width*channel + 2] = pixList[x][y].b;
			
			if (channel == 4)
				tmp[x*channel + y*width*channel + 3] =
					pixList[x][y].a;
		}
	return tmp;
}

GdkPixbuf *UGetPixbufFromImage(Image *img, guchar **tmpPixels)
{
	*tmpPixels = UGetPixelDataFromPixelsStruct(img->pixList,
			img->width, img->height, img->has_alpha ? 4 : 3);
	GdkPixbuf *r = gdk_pixbuf_new_from_data(*tmpPixels,
		GDK_COLORSPACE_RGB,
		img->has_alpha,
		img->bits_per_sample,
		img->width, img->height,
		(img->has_alpha ? 4 : 3) * img->width,
		NULL, NULL);
	return r;
}

void URotateImage(Image *img)
{
	Pixel **tmp = malloc(sizeof(Pixel*) * img->height);

	for(int i = 0; i < img->height; i ++)
	{
		tmp[i] = malloc(sizeof(Pixel) * img->width);
		for (int j = 0; j < img->width; j ++)
			tmp[i][j] = img->pixList[img->width - j - 1][i];

	}

	img->pixList = tmp;
	
	int c = img->width;
	img->width = img->height;
	img->height = c;
}

ImageBN *NegativeBinaryImage(ImageBN *img)
{
	ImageBN *r = malloc(sizeof(Image));
	r->width = img->width;
	r->height = img->height;
	r->data = malloc(sizeof(int*) * img->width);
	for (int i = 0; i < img->width; i ++)
	{
		r->data[i] = malloc(sizeof(int) * img->height);
		for (int j = 0; j < img->height; j ++)
			r->data[i][j] = img->data[i][j] ? 0 : 1;
	}
	return r;
}

ImageBN *DilatationOnBinary(ImageBN *img, int coef)
{
	ImageBN *r = malloc(sizeof(ImageBN));
	r->width = img->width;
	r->height = img->height;
	r->data = malloc(sizeof(int*) * r->width);
	for (int i = 0; i < r->width; i ++)
	{
		r->data[i] = malloc(sizeof(int) * r->height);
		for (int j = 0; j < r->height; j ++)
			r->data[i][j] = 0;
		int tmp = coef + 1;
		for (int j = 0; j < r->height; j++)
		{
			if (img->data[i][j])
				tmp = 0;
			if (tmp <= coef)
				r->data[i][j] = 1;
			tmp++;
		}
		tmp = coef + 1;
		for (int j = r->height - 1; j >= 0; j--)
		{
			if (img->data[i][j])
				tmp = 0;
			if (tmp <= coef)
				r->data[i][j] = 1;
			tmp++;
		}
	}
	for (int i = 0; i < r->height; i ++)
	{
		int tmp = coef + 1;
		for (int j = 0; j < r->width; j++)
		{
			if (img->data[j][i])
				tmp = 0;
			if (tmp <= coef)
				r->data[j][i] = 1;
			tmp++;
		}
		tmp = coef + 1;
		for (int j = r->width - 1; j >= 0; j--)
		{
			if (img->data[j][i])
				tmp = 0;
			if (tmp <= coef)
				r->data[j][i] = 1;
			tmp++;
		}
	}
	return r;
}
