#include <ctype.h> // isalnum
#include <math.h> // round
#include <omp.h> // omp pragmas
#include <string.h> // memcpy, memset, strlen
#include <stdio.h> // printf, sprintf
#include <stddef.h> // NULL
#include <stdlib.h> // free, malloc, realloc, strtol

#include "color_bruteforcer.h"

const int OFFSET = 0xff;
bool verbose = false;


bool hex_to_color(const char *hex, Color *out)
{
    if (hex[0] != '#' || strlen(hex) != 7) {
        return false;
    }

    for (size_t i = 1; i < strlen(hex); i++) {
        if (!isalnum(hex[i])) {
            return false;
        }
    }

    char *end;
    int value = strtol(&hex[1], &end, 16);

    // strtol returns 0 on failure, but it is also a valid value for pure black
    if (value == 0 && strcmp(hex, "#000000") != 0) {
        return false;
    }
    
    out->red = ((value >> 16) & OFFSET);
    out->green = ((value >> 8) & OFFSET);
    out->blue = (value & OFFSET);

    return true;
}

bool color_to_hex(const Color *color, char *out)
{
    int value = ((color->red & OFFSET) << 16) +
                ((color->green & OFFSET) << 8) +
                (color->blue & OFFSET);
    sprintf(out, "#%06x", value);

    return true;
}

int parse_single_color(size_t iterations, int *num_samples, Color *color)
{
    char hex[100];
    memset(hex, '\0', 100);
    int display_number = (iterations + 2) / 2; // Offset to start counting at 1
    
    if (iterations % 2 == 0) {
        printf("Enter original color #%d: ", display_number);
    }
    else {
        printf("Enter target color #%d: ", display_number);
    }

    if (fgets(hex, 100, stdin) != NULL) {
        if (hex[0] == '\0' || hex[0] == '\n' || hex[0] == '\r') {
            return PARSE_EMPTY_STRING;
        }

        /*
         * fgets captures the newline sequence from stdin, so if the string seems to be the right
         * length, effectively truncate the string after the hex code with a null byte.
         */
        if ((strlen(hex) == 8 && hex[7] == '\n') || // #123456\n
            (strlen(hex) == 9 && hex[7] == '\r')) { // #123456\r\n
            hex[7] = '\0';
        }

        if (!hex_to_color(hex, color)) {
            return PARSE_ERROR_CONVERSION_FAILURE;
        }

        *num_samples = (iterations + 1) / 2;
        return PARSE_SUCCESS;
    }

    return PARSE_ERROR_READ_FAILURE;
}

Color **parse_colors(int *num_samples)
{
    Color **colors = malloc(sizeof(Color*));
    size_t i = 0;
    int result;
    bool perform_cleanup = false;
    Color *parsed_color = malloc(sizeof(Color));

    while ((result = parse_single_color(i, num_samples, parsed_color)) == PARSE_SUCCESS) {
        colors = realloc(colors, (i + 1) * sizeof(Color*));
        colors[i] = malloc(sizeof(Color));
        memcpy(colors[i], parsed_color, sizeof(Color));
        i++;
    }

    free(parsed_color);

    if (result == PARSE_EMPTY_STRING) {
        if (i % 2 == 1) {
            perform_cleanup = true;
            fprintf(stderr, MISMATCHED_ERROR_MESSAGE);
        }
    }
    else {
        perform_cleanup = true;

        if (result == PARSE_ERROR_CONVERSION_FAILURE) {
            fprintf(stderr, CONVERSION_ERROR_MESSAGE);
        }
        else if (result == PARSE_ERROR_READ_FAILURE) {
            fprintf(stderr, READ_FAILURE_ERROR_MESSAGE);
        }
        else {
            fprintf(stderr, UNKNOWN_ERROR_MESSAGE);
        }
    }

    if (perform_cleanup) {
        for (size_t j = 0; j < i; j++) {
            free(colors[j]);
        }

        free(colors);
        return NULL;
    }
    
    return colors;
}

bool is_in_valid_range(int alpha)
{
    return (1 <= alpha && alpha <= 99);
}

int calculate_channel_overlay(int base, int overlay, int alpha)
{
    return (int)round((base + ((overlay - base) * (alpha / 100.0))));
}

void calculate_color_overlay(const Color *base, const Color *overlay, int alpha, Color *result)
{
    result->red = calculate_channel_overlay(base->red, overlay->red, alpha);
    result->green = calculate_channel_overlay(base->green, overlay->green, alpha);
    result->blue = calculate_channel_overlay(base->blue, overlay->blue, alpha);
}

bool is_channel_within_error(int target, int guess, int range)
{
    return ((target - range) <= guess) && (guess <= (target + range));
}

bool is_color_within_error(const Color *target, const Color *guess, int range)
{
    return (is_channel_within_error(target->red, guess->red, range) &&
            is_channel_within_error(target->green, guess->green, range) &&
            is_channel_within_error(target->blue, guess->blue, range));
}

bool color_matches(const Color *target, const Color *guess)
{
    return ((target->red == guess->red) &&
            (target->green == guess->green) &&
            (target->blue == guess->blue));
}

/*
 * Without inlining, the overhead of the function call severely impacts performace, sometimes more
 * than doubling runtime.
 */
inline Result *find_match(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    const Color *overlay,
    int alpha,
    Color *guess,
    int range,
    bool exact_only
)
{
    bool all_within_error = true;
    bool all_exact = true;

    for (size_t i = 0; i < num_samples; i++) {
        const Color *original = original_colors[i];
        const Color *target = target_colors[i];
        calculate_color_overlay(original, overlay, alpha, guess);

        if (!exact_only && !(is_color_within_error(target, guess, range))) {
            all_within_error = false;
            all_exact = false;
            break;
        }

        if (!(color_matches(target, guess))) {
            all_exact = false;

            if (exact_only) {
                all_within_error = false;
                break;
            }
        }
    }

    if (all_within_error) {
        Result *result = malloc(sizeof(Result));
        result->color = malloc(sizeof(Color));
        memcpy(result->color, overlay, sizeof(Color));
        result->alpha = alpha;
        result->is_exact = all_exact;
        return result;
    }

    return NULL;
}

Result **search_alpha(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    int alpha,
    int range,
    bool exact_only
)
{
    // Allocate memory in this scope to prevent num_samples * 256^3 calls to malloc and free
    Color *overlay = malloc(sizeof(Color));
    Color *guess = malloc(sizeof(Color));
    // Results is a null-terminated array, since we do not care exactly how many results we get back
    Result **results = malloc(sizeof(Result*));
    results[0] = NULL;
    size_t num_results = 0;

    for (int red = 0; red < 256; red++) {
        for (int green = 0; green < 256; green++) {
            for (int blue = 0; blue < 256; blue++) {
                overlay->red = red;
                overlay->green = green;
                overlay->blue = blue;

                Result *result = find_match(original_colors, target_colors, num_samples, overlay,
                    alpha, guess, range, exact_only);

                if (result != NULL) {
                    results[num_results] = result;
                    num_results++;
                    results = realloc(results, (num_results + 1) * sizeof(Result*));
                    results[num_results] = NULL;
                }
            }
        }
    }

    free(guess);
    free(overlay);

    return results;
}

Result **color_bruteforce(
    const Color *original_colors[],
    const Color *target_colors[],
    int num_samples,
    int alpha_min,
    int alpha_max,
    int range,
    bool exact_only
)
{
    int num_result_buckets = alpha_max - alpha_min + 1;
    Result **all_results[num_result_buckets];

    #pragma omp parallel for shared(all_results)
    for (int alpha = alpha_min; alpha <= alpha_max; alpha++) {
        if (verbose) {
            printf("Thread rank %d working on opacity %d\n",
                omp_get_thread_num(), alpha);
        }

        Result **results = search_alpha(original_colors, target_colors, num_samples, alpha,
            range, exact_only);
        all_results[alpha - alpha_min] = results;
    }

    #pragma omp barrier
    if (verbose) {
        printf("Search done.\n");
    }

    Result *result;
    Result **flat_results = malloc(sizeof(Result*));
    flat_results[0] = NULL;
    size_t num_results = 0;

    for (size_t i = 0; i < num_result_buckets; i++) {
        Result **results = all_results[i];

        for (size_t j = 0; (result = results[j]) != NULL; j++) {
            flat_results[num_results] = result;
            num_results++;
            flat_results = realloc(flat_results, (num_results + 1) * sizeof(Result*));
            flat_results[num_results] = NULL;
        }

        free(results);
    }

    return flat_results;
}
