## System Programming Lab 11 Multiprocessing
```
Added a -n call to the initial command line check to allow a user to specify any number of processors their computer might have.
Made the image generation process split until the number of processors specified was filled, waited for a child process to finish, then began another until all 50 images we completed
Scaled each image using this function xscale = xscale/((imageNum * 2)/2); which allowed each following image produce an even closer view of the fractal.
```
# Graph of time taken for given number of processors: