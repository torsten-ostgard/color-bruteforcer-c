This repository has been deprecated and exists only as a historical curiosity. A much improved version of the same tool written in Rust is available [here](https://github.com/torsten-ostgard/color-bruteforcer).

# color_bruteforcer - Find an unknown, semitransparent overlay color


## Introduction
color_bruteforcer is a program that given a set of input colors C<sub>i</sub> and output colors C<sub>o</sub>, attempts to find the unknown color C<sub>u</sub> at opacity &alpha; that, when overlayed on all elements of C<sub>i</sub>, produces the corresponding colors of C<sub>o</sub>. This is done by performing a bruteforce search on the entire RGB color space and alpha values from 1% to 99% opacity.


## Example
| Base                | Target                | After overlaying result color |
| ------------------- | --------------------- | ----------------------------- |
| ![](./img/base.jpg) | ![](./img/target.jpg) | ![](./img/result.jpg)         |

The color samples were taken from the single pixel acting as the "bullseye" as seen in the above images; the values can be found in the examples directory. Upon running the following command:

    $ ./color_bruteforcer --range=3 < example/example_colors

The program produced a number of results, from which I chose #ffd6a6 at 78% opacity. To produce the result image, I copied the shape of the text bubble from the base image and applied a Color Overlay layer effect to it using the aforementioned color and opacity.


## Requirements
- gcc with OpenMP support
- CUnit (only for testing)


## Usage
    Usage: color_bruteforcer [-hev] [--alpha-min=<n>] [--alpha-max=<n>] [-r <n>]
    Find an unknown, semi-transparent overlay color.

      -h, --help                display this help and exit
      --alpha-min=<n>           the lowest opacity value to check; defaults to 1
      --alpha-max=<n>           the highest opacity value to check; defaults to 99
      -r, --range=<n>           the error range of +/-n that will let a channel be considered an inexact match; defaults to 1
      -e, --exact               only display exact matches
      -v, --verbose             show verbose output
