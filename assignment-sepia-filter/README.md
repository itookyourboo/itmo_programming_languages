# assignment-8-sepia

In this assignment, we will create a program to perform a sepia filter on an image. A sepia filter makes an
image with vivid colors look like an old, aged photograph. Most graphical editors include a sepia filter.

The filter itself is not hard to code. It recalculates the red, green, and blue components of each pixel based on the old values of red, green, and blue. Mathematically, if we think about a pixel as a threedimensional vector, the transformation is nothing but a multiplication of a vector by matrix.

In this assignment you have to

- Implement in a separate file a routine to apply a filter to a big part of image (except for
the last pixels maybe). It will operate on chunks of multiple pixels at a time using SSE
instructions.

The last few pixels that did not fill the last chunk can be processed one by one using the C code provided
in Listing

- Make sure that both C and assembly versions produce similar results.
- Compile two programs; the first should use a naive C approach and the second one should use SSE instructions.
- Compare the time of execution of C and SSE using a huge image as an input (preferably hundreds of megabytes).
- Repeat the comparison multiple times and calculate the average values for SSE and C.