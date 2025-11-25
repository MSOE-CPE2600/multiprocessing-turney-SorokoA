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

## Lab 12 Multithreading changes

# Changes made:
```
-Added -t call to specify number of threads for the process to run (default 1)

-Made a struct so the threads could edit the same img pointer, while recieving
differing information on where to start editing the image so ne memory was overridden

-Changes the compute_image() method, changing it's arguments to accept a struct for the
pthread_join() method call
```
# Table of time for a given number of processors and threads:
<img width="643" height="175" alt="image" src="https://github.com/user-attachments/assets/338fd0dd-24d8-4b8f-ae7d-25c0119cdf20" />

# Graph of time taken for a different number of threads with a set number of processors ranging from 1-20:
<img width="752" height="452" alt="image" src="https://github.com/user-attachments/assets/d2e9141a-812a-4799-b0b0-108c3d53a498" />

# Results
```
-Multiprocessing seemed to be the most impactful on the time of the program because
it uses the unused processors of the computer while multithreading is on one processor
just splitting the work within that processor, but it can't go as fast as multiprocessing
because it all comes down the the fact that each processor has one ALU and can only go so fast

-The optimal spot for this program seems to be around 20 processors with anywhere from 2-5 threads,
where in my testing, 20 processors and 5 threads actually achieved a time of 25 seconds once.
```

