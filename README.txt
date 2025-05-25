Example generated from the source file:
+--------------------------------------------------------------------------------+
|                                                                                |
|                                                                                |
|                                            .                                   |
|                                           . o       o                          |
|                                      .   . . .     +                           |
|                                     + . .   .     o                            |
|                                    o o o o .   +   o                           |
|                                 .   o o + . . + B +   . . .                    |
|                                  o o = . = . o O X   + = + .                   |
|                                   = . + * o * o X B + * o = .                  |
|                                    o o   = * o + B X o . . *                   |
|                                     . . o + = . = * o .   + B                  |
|                                    o o = * + * o = . . . o O *                 |
|                                   o   * = + = o + = + + o = B o                |
|                                    . + .   o o + . + B + o + o                 |
|                                     . =   . . = . o * +   o .   .              |
|                                      . + o . = o   * * + o = . = o             |
|                                         O .   =   . = + + + * O = =            |
|                                        . =     .   . . . o . & + O o           |
|                                         .         .   + +   X B = =            |
|                                                    . + * + B B = = .           |
|                                                     . O o @ + + + . .          |
|                                                      o X % B o o . .           |
|                                                     . o * * =   . .            |
+--------------------------------------------------------------------------------+

g++ hashvis.cc -o hashvis -std=c++17 -O3

A hash visualization algorithm (HVA) is a function h_1 which has, as a minimum the following two properties:

- Image-generation: h_1 maps an input x of arbitrary finite length, to an output image h_1(x) of fixed size.
- Ease of computation: given h and an input x, h_1(x) is easy to compute. 

Two images I_1 and I_2 are near if the two images are perceptually indistinguishable. 

1. Near preimage resistance: for any pre-specified output image y, it is computationally infeasible to find the input x such that h_1(x) is near y.
2. Near 2nd-preimage resistance: given any input x, it's computationally infeasible to find x' such that h_1(x') is near h_1(x)
3. Near collision resistance: it is computationally in-feasible to find any two distinct inputs x, x' which has to to the same output, h_1(x) is near h_1(x').

Since it's difficult to devise and algorithm which can judge automatically whether two images are near since that depends on the person comparing the image. But in general we can find some similarity-metric function s: I x I -> R. Such that if s(I_1, I_2) >= B, then the two images I_1 and I_2 are not near. Finding a good function for s is an active are of resaerch in image retrieval and is not in the scope of this paper.

Regular images = images that contain mostly recognizable shapes. If an image is not regular, i.e. does not contain indentifiable objects or patterns, or is too chaotic (such as white noise), it is difficult for humans to compare or recall it. 

We can test regularity by:

1. If we try to compress an image, and if the image is chaotic, such as white noise, the compression factor will be very small since almost every pixel is random. 

2. Non-regular iamges tend to have wide frequency spectra. Noisy images contain a high percentage of the energy in high frequencies. Hence we can transform an image to the Fourier domain and compute the magnitude spectrum. If the magnitude spectrum does not have too much energy in the high frequencies `sum while f > f_tresh |F(f)| < const` then the image is regular.

Random art is an algorithm such that given a bit string as input, it will generate a funtion F: [-1, 1]^2 -> [-1, 1]^3 which defines an image. The bit-string input is used as a seed for the pseudo-random number generator, and the function is constructed by choosing rules from a grammar depending on the value of the pseudo-random number generator. The function F maps each pixel (x, y) to an RGB value (r,g,b) which is a triple of intensities for the red, green and blue values.
