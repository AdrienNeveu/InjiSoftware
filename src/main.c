#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "Interface/interface.h"
#include "PreTreatment/split.h"
#include "Utils/image.h"

int main(int argc, char *argv[])
{
	Image *img = NULL;
	if(argc > 1)
		img = ULoadImage(argv[1]);

	if(img)
	{
		int count = 0;
		Box *b = GetBreadthBoxArray(GetBoxFromSplit(*img), &count);
		printf("%d\n", count);
		for (int i = 0; i < count; i ++)
		{
			DrawNotInSubBoxes(*img, b[i], BLUE);
			getchar();
		}
	}

	Gtk_Initialize(argc, argv, img);
	gtk_main();	

	return 0;
}
