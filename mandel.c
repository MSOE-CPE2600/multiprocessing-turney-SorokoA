/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
//  Edited by: Aiden Soroko
//  Date: 11/18/2025
//  Assignment: Lab 11 Multiprocessing
///
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "jpegrw.h"

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void *compute_image(void *arg);
static void show_help();

struct image_input{
	imgRawImage* image;
	double xcenterminus;
	double xcentermax;
	double ycenterminus;
	double ycentermax;
	int maximum;
	int count;
	int maxcount;
};

int main( int argc, char *argv[] )
{
	char c;
	// These are the default configuration values used
	// if no command line arguments are given.
	char *outfile = "mandel";
	double xcenter = -0.497656;
	double ycenter = 0.518554;
	long double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int    nprocs = 1;
	int    usedprocs = 0;
	int    imageNum = 0;
	int	   numthreads = 1;

	// For each command line argument given,
	// override the appropriate configuration value. 

	while((c = getopt(argc,argv,"n:t:x:y:s:W:H:m:h"))!=-1) {
		switch(c) 
		{
			case 'n':
				nprocs = atoi(optarg);
				if (nprocs > 20){
					nprocs = 20;
				}
				break;
			case 't':
				numthreads = atoi(optarg);
				if (numthreads > 20){
					numthreads = 20;
				}
				break;
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}
	for (int i = 0; i < 50; i++){
		if(usedprocs == nprocs || usedprocs > nprocs){
			wait(NULL);
			usedprocs--;
		}
		imageNum++;
		usedprocs++;
		int pid = fork();
		if(pid == 0){

			char newname[100];
			sprintf(newname, "%s%d.jpg", outfile, imageNum);
			outfile = newname;
			
			xscale = xscale - xscale/imageNum*i;

			// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
			yscale = xscale / image_width * image_height;

			// Display the configuration of the image.
			printf("mandel: x=%lf y=%lf xscale=%Lf yscale=%1f max=%d outfile=%s\n",xcenter,ycenter,xscale,yscale,max,outfile);

			// Create a raw image of the appropriate size.
			imgRawImage* img = initRawImage(image_width,image_height);
			
			// Fill it with a black
			setImageCOLOR(img,0);

			// Compute the Mandelbrot image

			//create array of threads and input_image structs to allow for easy creation with custom ammount of threads
			pthread_t *threads = malloc(numthreads * sizeof(pthread_t));
			struct image_input *images = malloc(numthreads * sizeof(struct image_input));
			for(int j = 0; j < numthreads; j++){
				images[j].image = img;
				images[j].xcenterminus = xcenter-xscale/2;
				images[j].xcentermax = xcenter+xscale/2;
				images[j].ycenterminus = ycenter-yscale/2;
				images[j].ycentermax = ycenter+yscale/2;
				images[j].maximum = max;
				images[j].count = j + 1;
				images[j].maxcount = numthreads;
				pthread_create(&threads[j], NULL,&compute_image, &images[j]);
			}
			for(int j = 0; j < numthreads; j++){
				pthread_join(threads[j],NULL);	
			}

			// Save the image in the stated file.
			storeJpegImageFile(img,outfile);

			// free the mallocs
			freeRawImage(img);
			free(images);
			free(threads);
			exit(0);
		}
	}
	wait(NULL);
	return 0;
}




/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}


/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void *compute_image(void *arg)
{
	
	struct image_input *wrapped = arg;
	int i,j;
	int count = wrapped->count;
	int maxcount = wrapped->maxcount;
	int width = wrapped->image->width;
	int height = wrapped->image->height;
	double rowstart = (height * (double)((double)(count - 1) / (double)maxcount));
	double rowend = (double)height*((double)count / (double)maxcount);


	// For every pixel in the image...
	for(j=rowstart;j<rowend;j++) {
		for(i=0;i<width;i++) {
			// Determine the point in x,y space for that pixel.
			double x = wrapped->xcenterminus + i*(wrapped->xcentermax-wrapped->xcenterminus)/width;
			double y = wrapped->ycenterminus + j*(wrapped->ycentermax-wrapped->ycenterminus)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,wrapped->maximum);

			// Set the pixel in the bitmap.
			setPixelCOLOR(wrapped->image,i,j,iteration_to_color(iters,wrapped->maximum));
		}
	}
	return NULL;
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-n          Set the number of processors to utilize. (default=1)\n");
	printf("-t          Set the number of threads to use for each image (default=1)\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
