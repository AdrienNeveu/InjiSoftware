#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "Interface/interface.h"
#include "Utils/image.h"
#include "Serialization/serialization.h"

int main(int argc, char *argv[])
{
	//SWrite(SRead("test.inji"), "testRead.inji");
	Image *img = NULL;
	srand(time(NULL));
	if(argc > 1)
		img = ULoadImage(argv[1]);

	Gtk_Initialize(argc, argv, img);
	gtk_main();
	return 0;
}
