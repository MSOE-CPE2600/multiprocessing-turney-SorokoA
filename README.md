## System Programming Lab 11 Multiprocessing

Edited by: Aiden Soroko
# Brief overview of changes made:
```
-Added a -n call to the initial command line check to allow a user to
specify any number of processors their computer might have.

-Made the image generation process split until the number of processors
specified was filled, waited for a child process to finish, then began
another until all 50 images we completed.

-Scaled each image using this function xscale = xscale/((imageNum * 2)/2);
which allowed each following image produce an even closer view of the fractal.
```
# Graph of time taken for given number of processors:
<img width="712" height="424" alt="image" src="https://github.com/user-attachments/assets/22d2ddd5-e642-4b76-8219-9c1225ac2e13" />

# Results:
```
This graph shows the results of running the program with the same settings each time, just a different number of processors each
time ranging from 1 - 20 processors. It shows that adding more processors usually increased the speed up until a certain
point where the difference in time won't be guarenteed to be faster than lesser number of processors.
```
