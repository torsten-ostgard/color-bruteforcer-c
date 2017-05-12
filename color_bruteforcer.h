#ifndef COLOR_BRUTEFORCER_H
#define COLOR_BRUTEFORCER_H

#define PARSE_EMPTY_STRING (0)
#define PARSE_SUCCESS (1)
#define PARSE_ERROR_CONVERSION_FAILURE (2)
#define PARSE_ERROR_READ_FAILURE (3)

#define MISMATCHED_ERROR_MESSAGE ("The number of base and target colors do not match.")
#define CONVERSION_ERROR_MESSAGE ("The most recently entered hex code could not be converted.")
#define READ_FAILURE_ERROR_MESSAGE ("An error occurred while reading input.")
#define UNKNOWN_ERROR_MESSAGE ("An unspecified error occurred.")

#include <stdbool.h>

typedef struct Color
{
    int red;
    int green;
    int blue;
} Color;

typedef struct Result
{
    Color *color;
    int alpha;
    bool is_exact;
} Result;

extern bool verbose;


bool hex_to_color(const char *hex, Color *out);
bool color_to_hex(const Color *color, char *out);
int parse_single_color(size_t iterations, int *num_samples, Color *color);
Color **parse_colors(int *num_samples);
bool is_in_valid_range(int alpha);
int calculate_channel_overlay(int base, int overlay, int alpha);
void calculate_color_overlay(const Color *base, const Color *overlay, int alpha, Color *result);
bool is_channel_within_error(int target, int guess, int error);
bool is_color_within_error(const Color *target, const Color *guess, int error);
bool color_matches(const Color *target, const Color *guess);
Result *find_match(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    const Color *overlay,
    int alpha,
    Color *guess,
    int range,
    bool exact_only
);
Result **search_alpha(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    int alpha,
    int range,
    bool exact_only
);
Result **color_bruteforce(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    int alpha_min,
    int alpha_max,
    int range,
    bool exact_only
);

#endif // COLOR_BRUTEFORCER_H
