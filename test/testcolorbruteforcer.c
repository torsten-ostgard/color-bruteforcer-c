#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <sys/types.h>
#endif
#include <sys/stat.h>

#include <CUnit/CUnit.h>

#include "color_bruteforcer.h"

#define STDERR_FILE ("stderr_redirect")

#ifdef _WIN32
#define NULL_OUTPUT ("nul")
#define _stat stat
#else
#define NULL_OUTPUT ("/dev/null")
#endif

// Standard colors used throughout the tests:
Color STANDARD_RED = {237, 28, 36};    // #ed1c24 Photoshop "Pure Red"
Color STANDARD_GREEN = {60, 184, 120}; // #3cb878 Photoshop "Light Green"
Color STANDARD_BLUE = {0, 84, 166};    // #0054a6 Photoshop "Pure Blue"
Color WHITE = {255, 255, 255};         // #ffffff 100% brightness
Color LIGHT_GRAY = {191, 191, 191};    // #bfbfbf 75% brightness
Color MEDIUM_GRAY = {128, 128, 128};   // #808080 50% brightness
Color DARK_GRAY = {64, 64, 64};        // #404040 25% brightness
Color BLACK = {0, 0, 0};               // #000000  0% brightness
Color PINK = {250, 187, 189};          // #fabbbd
Color LIGHT_RED = {205, 142, 145};     // #cd8e91
Color MID_RED = {161, 98, 100};        // #a16264
Color DARK_RED = {116, 53, 56};        // #743538
Color MAROON = {71, 8, 11};            // #47080b
const int DEFAULT_ERROR_RANGE = 1;
FILE *saved_in;
FILE *saved_out;
FILE *saved_err;


void set_up()
{
    saved_in = stdin;
    saved_out = stdout;
    saved_err = stderr;
}

void tear_down()
{
    stdin = saved_in;
    stdout = saved_out;
    stderr = saved_err;
}

void test_hex_to_color()
{
    char hex[] = "#ed1c24";
    Color *result = malloc(sizeof(Color));

    bool success = hex_to_color(hex, result);
    CU_ASSERT_TRUE(success);
    CU_ASSERT_EQUAL(STANDARD_RED.red, result->red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, result->green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, result->blue);

    free(result);
}

void test_hex_to_color_black()
{
    char hex[] = "#000000";
    Color *result = malloc(sizeof(Color));
    result->red = STANDARD_RED.red;
    result->green = STANDARD_RED.green;
    result->blue = STANDARD_RED.blue;

    bool success = hex_to_color(hex, result);
    CU_ASSERT_TRUE(success);
    // The input color should have been changed
    CU_ASSERT_EQUAL(0, result->red);
    CU_ASSERT_EQUAL(0, result->green);
    CU_ASSERT_EQUAL(0, result->blue);

    free(result);
}

void test_hex_to_color_short_string()
{
    char hex[] = "ed1c24";
    Color *result = malloc(sizeof(Color));
    result->red = STANDARD_RED.red;
    result->green = STANDARD_RED.green;
    result->blue = STANDARD_RED.blue;

    bool success = hex_to_color(hex, result);
    CU_ASSERT_FALSE(success);
    // The input color should be unchanged
    CU_ASSERT_EQUAL(STANDARD_RED.red, result->red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, result->green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, result->blue);

    free(result);
}

void test_hex_to_color_long_string()
{
    char hex[] = "#ed1c24567";
    Color *result = malloc(sizeof(Color));
    result->red = STANDARD_RED.red;
    result->green = STANDARD_RED.green;
    result->blue = STANDARD_RED.blue;

    bool success = hex_to_color(hex, result);
    CU_ASSERT_FALSE(success);
    // The input color should be unchanged
    CU_ASSERT_EQUAL(STANDARD_RED.red, result->red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, result->green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, result->blue);

    free(result);
}

void test_color_to_hex()
{
    Color color = STANDARD_RED;
    char *result = calloc(8, sizeof(char));

    color_to_hex(&color, result);
    CU_ASSERT_STRING_EQUAL("#ed1c24", result);

    free(result);
}

void test_hex_to_color_to_hex()
{
    char hex[] = "#7f8081";
    Color *result = malloc(sizeof(Color));
    char *final_result = calloc(8, sizeof(char));

    hex_to_color(hex, result);
    color_to_hex(result, final_result);
    CU_ASSERT_STRING_EQUAL(hex, final_result);

    free(result);
    free(final_result);
}

void test_color_to_hex_to_color()
{
    Color color = STANDARD_RED;
    char *result = calloc(8, sizeof(char));
    Color *final_result = malloc(sizeof(Color));

    color_to_hex(&color, result);
    hex_to_color(result, final_result);
    CU_ASSERT_EQUAL(color.red, final_result->red);
    CU_ASSERT_EQUAL(color.green, final_result->green);
    CU_ASSERT_EQUAL(color.blue, final_result->blue);

    free(result);
    free(final_result);
}

void test_parse_single_color()
{
    FILE *test_file = fopen("test/input_files/test_parse_single_color", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    size_t i = 1;
    int num_samples = 0;
    Color *color = malloc(sizeof(Color));
    
    if (color == NULL) { printf("OH NO!"); }

    int result = parse_single_color(i, &num_samples, color);
    CU_ASSERT_EQUAL(PARSE_SUCCESS, result);
    CU_ASSERT_EQUAL(1, num_samples);
    CU_ASSERT_EQUAL(STANDARD_RED.red, color->red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, color->green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, color->blue);

    free(color);
    fclose(test_file);
    fclose(null_file);
}

void test_parse_single_color_windows()
{
    FILE *test_file = fopen("test/input_files/test_parse_single_color_windows", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    size_t i = 1;
    int num_samples = 0;
    Color *color = malloc(sizeof(Color));

    int result = parse_single_color(i, &num_samples, color);
    CU_ASSERT_EQUAL(PARSE_SUCCESS, result);
    CU_ASSERT_EQUAL(1, num_samples);
    CU_ASSERT_EQUAL(STANDARD_RED.red, color->red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, color->green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, color->blue);

    free(color);
    fclose(test_file);
    fclose(null_file);
}

void test_parse_single_color_empty_string()
{
    FILE *test_file = fopen("test/input_files/test_parse_single_color_empty_string", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    size_t i = 1;
    int num_samples = 0;
    Color color = STANDARD_RED;

    int result = parse_single_color(i, &num_samples, &color);
    CU_ASSERT_EQUAL(PARSE_EMPTY_STRING, result);
    // The input values should be unchanged
    CU_ASSERT_EQUAL(0, num_samples);
    CU_ASSERT_EQUAL(STANDARD_RED.red, color.red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, color.green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, color.blue);

    fclose(test_file);
    fclose(null_file);
}

void test_parse_single_color_invalid_color()
{
    FILE *test_file = fopen("test/input_files/test_parse_single_color_invalid_color", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    size_t i = 1;
    int num_samples = 0;
    Color color = STANDARD_RED;

    int result = parse_single_color(i, &num_samples, &color);
    CU_ASSERT_EQUAL(PARSE_ERROR_CONVERSION_FAILURE, result);
    // The input values should be unchanged
    CU_ASSERT_EQUAL(0, num_samples);
    CU_ASSERT_EQUAL(STANDARD_RED.red, color.red);
    CU_ASSERT_EQUAL(STANDARD_RED.green, color.green);
    CU_ASSERT_EQUAL(STANDARD_RED.blue, color.blue);

    fclose(test_file);
    fclose(null_file);
}

void test_parse_colors()
{
    FILE *test_file = fopen("test/input_files/test_parse_colors", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 0;

    Color **all_colors = parse_colors(&num_samples);
    CU_ASSERT_EQUAL(5, num_samples);
    CU_ASSERT_PTR_NOT_NULL_FATAL(all_colors);
    CU_ASSERT_TRUE(color_matches(all_colors[0], original_colors[0]));
    CU_ASSERT_TRUE(color_matches(all_colors[1], target_colors[0]));
    CU_ASSERT_TRUE(color_matches(all_colors[2], original_colors[1]));
    CU_ASSERT_TRUE(color_matches(all_colors[3], target_colors[1]));
    CU_ASSERT_TRUE(color_matches(all_colors[4], original_colors[2]));
    CU_ASSERT_TRUE(color_matches(all_colors[5], target_colors[2]));
    CU_ASSERT_TRUE(color_matches(all_colors[6], original_colors[3]));
    CU_ASSERT_TRUE(color_matches(all_colors[7], target_colors[3]));
    CU_ASSERT_TRUE(color_matches(all_colors[8], original_colors[4]));
    CU_ASSERT_TRUE(color_matches(all_colors[9], target_colors[4]));

    for (size_t i = 0; i < num_samples * 2; i++) {
        free(all_colors[i]);
    }

    free(all_colors);
    fclose(test_file);
    fclose(null_file);
}

void test_parse_colors_mismatched()
{
    FILE *test_file = fopen("test/input_files/test_parse_colors_mismatched", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    FILE *err_file = fopen(STDERR_FILE, "w");
    stderr = err_file;
    int num_samples = 0;

    Color **all_colors = parse_colors(&num_samples);
    fflush(stderr);
    CU_ASSERT_EQUAL(4, num_samples);
    CU_ASSERT_PTR_NULL(all_colors);

    char buf[100];
    fclose(err_file);
    err_file = fopen(STDERR_FILE, "r");
    fgets(buf, 100, err_file);
    CU_ASSERT_STRING_EQUAL(MISMATCHED_ERROR_MESSAGE, buf);

    fclose(test_file);
    fclose(null_file);
    fclose(err_file);
    remove(STDERR_FILE);
}

void test_parse_colors_invalid_color()
{
    FILE *test_file = fopen("test/input_files/test_parse_colors_invalid_color", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    FILE *err_file = fopen(STDERR_FILE, "w");
    stderr = err_file;
    int num_samples = 0;

    Color **all_colors = parse_colors(&num_samples);
    fflush(stderr);
    CU_ASSERT_EQUAL(0, num_samples);
    CU_ASSERT_PTR_NULL(all_colors);

    char buf[100];
    fclose(err_file);
    err_file = fopen(STDERR_FILE, "r");
    fgets(buf, 100, err_file);
    CU_ASSERT_STRING_EQUAL(CONVERSION_ERROR_MESSAGE, buf);

    fclose(test_file);
    fclose(null_file);
    fclose(err_file);
    remove(STDERR_FILE);
}

void test_parse_colors_read_failure()
{
    FILE *test_file = fopen("test/input_files/test_parse_colors_read_failure", "r");
    stdin = test_file;
    FILE *null_file = fopen(NULL_OUTPUT, "w");
    stdout = null_file;
    FILE *err_file = fopen(STDERR_FILE, "w");
    stderr = err_file;
    int num_samples = 0;

    Color **all_colors = parse_colors(&num_samples);
    fflush(stderr);
    CU_ASSERT_EQUAL(1, num_samples);
    CU_ASSERT_PTR_NULL(all_colors);

    char buf[100];
    fclose(err_file);
    err_file = fopen(STDERR_FILE, "r");
    fgets(buf, 100, err_file);
    CU_ASSERT_STRING_EQUAL(READ_FAILURE_ERROR_MESSAGE, buf);

    fclose(test_file);
    fclose(null_file);
    fclose(err_file);
    remove(STDERR_FILE);
}


void is_in_valid_range_valid()
{
    for (int alpha = 1; alpha <= 99; alpha++) {
        CU_ASSERT_TRUE(is_in_valid_range(alpha));
    }
}

void is_in_valid_range_invalid_low()
{
    int alpha = 0;
    CU_ASSERT_FALSE(is_in_valid_range(alpha));
}

void is_in_valid_range_invalid_high()
{
    int alpha = 100;
    CU_ASSERT_FALSE(is_in_valid_range(alpha));
}

void test_calculate_channel_overlay_0_opacity()
{
    int base = MEDIUM_GRAY.red;
    int overlay = STANDARD_RED.red;
    int alpha = 0;
    int result = calculate_channel_overlay(base, overlay, alpha);
    CU_ASSERT_EQUAL(base, result);
}

void test_calculate_channel_overlay_50_opacity()
{
    int base = MEDIUM_GRAY.red;
    int overlay = STANDARD_RED.red;
    int alpha = 50;
    int correct_result = 183;
    int result = calculate_channel_overlay(base, overlay, alpha);
    CU_ASSERT_EQUAL(correct_result, result);
}

void test_calculate_channel_overlay_100_opacity()
{
    int base = MEDIUM_GRAY.red;
    int overlay = STANDARD_RED.red;
    int alpha = 100;
    int result = calculate_channel_overlay(base, overlay, alpha);
    CU_ASSERT_EQUAL(overlay, result);
}

void test_calculate_color_overlay_gradient()
{
    int num_samples = 5;
    int num_overlay_colors = 3;
    int alpha = 30;
    Color bases[] ={
        WHITE,
        LIGHT_GRAY,
        MEDIUM_GRAY,
        DARK_GRAY,
        BLACK,
    };
    Color red_results[] = {
        {250, 187, 189},
        {205, 142, 145},
        {161, 98, 100},
        {116, 53, 56},
        {71, 8, 11},
    };
    Color green_results[] = {
        {197, 234, 215},
        {152, 189, 170},
        {108, 145, 126},
        {63, 100, 81},
        {18, 55, 36},
    };
    Color blue_results[] = {
        {179, 204, 228},
        {134, 159, 184},
        {90, 115, 139},
        {45, 70, 95},
        {0, 25, 50},
    };
    Color *correct_results[] = {
        red_results,
        green_results,
        blue_results,
    };
    Color overlays[] = {
        STANDARD_RED,
        STANDARD_GREEN,
        STANDARD_BLUE,
    };
    Color *result = malloc(sizeof(Color));

    for (size_t i = 0; i < num_overlay_colors; i++) {
        for (size_t j = 0; j < num_samples; j++) {
            calculate_color_overlay(&bases[j], &overlays[i], alpha, result);
            // Do not use color_matches(), as this output is  slightly more descriptive
            CU_ASSERT_EQUAL(correct_results[i][j].red, result->red);
            CU_ASSERT_EQUAL(correct_results[i][j].green, result->green);
            CU_ASSERT_EQUAL(correct_results[i][j].blue, result->blue);
        }
    }

    free(result);
}

void test_calculate_color_overlay_0_opacity()
{
    int alpha = 0;
    Color base = MEDIUM_GRAY;
    Color overlay = WHITE;
    Color *result = malloc(sizeof(Color));

    calculate_color_overlay(&base, &overlay, alpha, result);
    CU_ASSERT_TRUE(color_matches(&base, result));

    free(result);
}

void test_calculate_color_overlay_100_opacity()
{
    int alpha = 100;
    Color base = MEDIUM_GRAY;
    Color overlay = WHITE;
    Color *result = malloc(sizeof(Color));

    calculate_color_overlay(&base, &overlay, alpha, result);
    CU_ASSERT_TRUE(color_matches(&overlay, result));

    free(result);
}

void test_is_channel_within_error_exact()
{
    int target = 128;
    int guess = 128;
    bool retval = is_channel_within_error(target, guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_TRUE(retval);
}

void test_is_channel_within_error_low_yes()
{
    int target = 128;
    int guess = 127;
    bool retval = is_channel_within_error(target, guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_TRUE(retval);
}

void test_is_channel_within_error_high_yes()
{
    int target = 128;
    int guess = 129;
    bool retval = is_channel_within_error(target, guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_TRUE(retval);
}

void test_is_channel_within_error_low_no()
{
    int target = 128;
    int guess = 126;
    bool retval = is_channel_within_error(target, guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_FALSE(retval);
}

void test_is_channel_within_error_high_no()
{
    int target = 128;
    int guess = 130;
    bool retval = is_channel_within_error(target, guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_FALSE(retval);
}

void test_is_color_within_error_exact()
{
    Color target = MEDIUM_GRAY;
    Color guess = MEDIUM_GRAY;
    bool retval = is_color_within_error(&target, &guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_TRUE(retval);
}

void test_is_color_within_error_all_within()
{
    Color target = MEDIUM_GRAY;
    Color guess = {127, 127, 129};
    bool retval = is_color_within_error(&target, &guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_TRUE(retval);
}

void test_is_color_within_error_two_within()
{
    Color target = MEDIUM_GRAY;
    Color guess = {127, 127, 130};
    bool retval = is_color_within_error(&target, &guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_FALSE(retval);
}

void test_is_color_within_error_none_within()
{
    Color target = MEDIUM_GRAY;
    Color guess = {126, 126, 130};
    bool retval = is_color_within_error(&target, &guess, DEFAULT_ERROR_RANGE);
    CU_ASSERT_FALSE(retval);
}

void test_color_matches_match()
{
    Color target = MEDIUM_GRAY;
    Color guess = MEDIUM_GRAY;
    bool retval = color_matches(&target, &guess);
    CU_ASSERT_TRUE(retval);
}

void test_color_matches_no_match()
{
    Color target = MEDIUM_GRAY;
    Color guess = {128, 128, 127};
    bool retval = color_matches(&target, &guess);
    CU_ASSERT_FALSE(retval);
}

void test_find_match_exact()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 30;
    Color overlay = STANDARD_RED;
    Color *guess = malloc(sizeof(Color));
    bool exact_only = false;

    Result *result = find_match(original_colors, target_colors, num_samples, &overlay, alpha,
        guess, DEFAULT_ERROR_RANGE, exact_only);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_TRUE(result->is_exact);
    /*
     * find_match() should be copying the overlay into the result, so ensure that the two buffers
     * are not pointing to the same place.
     */
    CU_ASSERT_PTR_NOT_EQUAL(result->color, &overlay);
    CU_ASSERT_EQUAL(result->color->red, overlay.red);
    CU_ASSERT_EQUAL(result->color->green, overlay.green);
    CU_ASSERT_EQUAL(result->color->blue, overlay.blue);

    free(guess);
    free(result->color);
    free(result);
}

void test_find_match_within_error()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 30;
    Color overlay = {STANDARD_RED.red - 3, STANDARD_RED.green, STANDARD_RED.blue};
    Color *guess = malloc(sizeof(Color));
    bool exact_only = false;

    Result *result = find_match(original_colors, target_colors, num_samples, &overlay, alpha,
        guess, DEFAULT_ERROR_RANGE, exact_only);
    CU_ASSERT_PTR_NOT_NULL(result);
    /*
     * find_match() should be copying the overlay into the result, so ensure that the two buffers
     * are not pointing to the same place.
     */
    CU_ASSERT_PTR_NOT_EQUAL(result->color, &overlay);
    CU_ASSERT_FALSE(result->is_exact);
    CU_ASSERT_EQUAL(result->alpha, 30);
    CU_ASSERT_EQUAL(result->color->red, overlay.red);
    CU_ASSERT_EQUAL(result->color->green, overlay.green);
    CU_ASSERT_EQUAL(result->color->blue, overlay.blue);

    free(guess);
    free(result->color);
    free(result);
}

void test_find_match_within_error_but_exact_only()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 30;
    Color overlay = {STANDARD_RED.red - 3, STANDARD_RED.green, STANDARD_RED.blue};
    Color *guess = malloc(sizeof(Color));
    bool exact_only = true;

    Result *result = find_match(original_colors, target_colors, num_samples, &overlay, alpha,
        guess, DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NULL(result);

    free(guess);
}


void test_find_match_no_match()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 30;
    Color overlay = WHITE;
    Color *guess = malloc(sizeof(Color));
    bool exact_only = false;

    Result *result = find_match(original_colors, target_colors, num_samples, &overlay, alpha,
        guess, DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NULL(result);

    free(guess);
}

void test_search_alpha()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 30;
    /*
     * For a simple test case with lots of similar results, limiting results to exact matches helps
     * keep the number of matches reasonable.
     */
    bool exact_only = true;
    Color expected_results[] = {
        {237, 27, 36},
        {237, 28, 36},
        {238, 27, 36},
        {238, 28, 36},
    };

    Result **results = search_alpha(original_colors, target_colors, num_samples, alpha, 
        DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NOT_NULL_FATAL(results);
    for (size_t i = 0; i < 4; i++) {
        CU_ASSERT_TRUE(color_matches(results[i]->color, &expected_results[i]));
        CU_ASSERT_TRUE(results[i]->is_exact);
        CU_ASSERT_EQUAL(results[i]->alpha, 30);
    }
    CU_ASSERT_PTR_NULL(results[4]);

    Result *result;

    for (size_t i = 0; (result = results[i]) != NULL; i++) {
        free(result->color);
        free(result);
    }

    free(results);
}

void test_search_alpha_no_results()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha = 29;
    bool exact_only = true;

    Result **results = search_alpha(original_colors, target_colors, num_samples, alpha,
        DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NOT_NULL_FATAL(results);
    CU_ASSERT_PTR_NULL(results[0]);

    free(results);
}

void test_color_bruteforce()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha_min = 29;
    int alpha_max = 31;
    /*
     * For a simple test case with lots of similar results, limiting results to exact matches helps
     * keep the number of matches reasonable.
     */
    bool exact_only = true;
    Color expected_results[] = {
        {237, 27, 36},
        {237, 28, 36},
        {238, 27, 36},
        {238, 28, 36},
    };

    Result **results = color_bruteforce(original_colors, target_colors, num_samples, alpha_min,
        alpha_max, DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NOT_NULL_FATAL(results);
    for (size_t i = 0; i < 4; i++) {
        CU_ASSERT_TRUE(color_matches(results[i]->color, &expected_results[i]));
        CU_ASSERT_TRUE(results[i]->is_exact);
        CU_ASSERT_EQUAL(results[i]->alpha, 30);
    }
    CU_ASSERT_PTR_NULL(results[4]);

    Result *result;

    for (size_t i = 0; (result = results[i]) != NULL; i++) {
        free(result->color);
        free(result);
    }

    free(results);
}

void test_color_bruteforce_no_results()
{
    const Color *original_colors[] = {
        &WHITE,
        &LIGHT_GRAY,
        &MEDIUM_GRAY,
        &DARK_GRAY,
        &BLACK,
    };
    const Color *target_colors[] = {
        &PINK,
        &LIGHT_RED,
        &MID_RED,
        &DARK_RED,
        &MAROON,
    };
    int num_samples = 5;
    int alpha_min = 29;
    int alpha_max = 29;
    /*
     * For a simple test case with lots of similar results, limiting results to exact matches helps
     * keep the number of matches reasonable.
     */
    bool exact_only = true;

    Result **results = color_bruteforce(original_colors, target_colors, num_samples, alpha_min,
        alpha_max, DEFAULT_ERROR_RANGE, exact_only);

    CU_ASSERT_PTR_NOT_NULL_FATAL(results);
    CU_ASSERT_PTR_NULL(results[0]);

    free(results);
}

CU_TestInfo test_array_conversion[] = {
    {"test_hex_to_color", test_hex_to_color},
    {"test_hex_to_color_black", test_hex_to_color_black},
    {"test_hex_to_color_short_string", test_hex_to_color_short_string},
    {"test_hex_to_color_long_string", test_hex_to_color_long_string},
    {"test_color_to_hex", test_color_to_hex},
    {"test_hex_to_color_to_hex", test_hex_to_color_to_hex},
    {"test_color_to_hex_to_color", test_color_to_hex_to_color},
    {"test_parse_single_color", test_parse_single_color},
    {"test_parse_single_color_windows", test_parse_single_color_windows},
    {"test_parse_single_color_empty_string", test_parse_single_color_empty_string},
    {"test_parse_single_color_invalid_color", test_parse_single_color_invalid_color},
    {"test_parse_colors", test_parse_colors},
    {"test_parse_colors_mismatched", test_parse_colors_mismatched},
    {"test_parse_colors_invalid_color", test_parse_colors_invalid_color},
    {"test_parse_colors_read_failure", test_parse_colors_read_failure},
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_validation[] = {
    {"is_in_valid_range_valid", is_in_valid_range_valid},
    {"is_in_valid_range_invalid_low", is_in_valid_range_invalid_low},
    {"is_in_valid_range_invalid_high", is_in_valid_range_invalid_high},
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_overlay[] = {
    {"test_calculate_channel_overlay_0_opacity", test_calculate_channel_overlay_0_opacity},
    {"test_calculate_channel_overlay_50_opacity", test_calculate_channel_overlay_50_opacity},
    {"test_calculate_channel_overlay_100_opacity", test_calculate_channel_overlay_100_opacity},
    {"test_calculate_color_overlay_gradient", test_calculate_color_overlay_gradient},
    {"test_calculate_color_overlay_0_opacity", test_calculate_color_overlay_0_opacity},
    {"test_calculate_color_overlay_100_opacity", test_calculate_color_overlay_100_opacity},
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_matching[] = {
    {"test_is_channel_within_error_exact", test_is_channel_within_error_exact},
    {"test_is_channel_within_error_low_yes", test_is_channel_within_error_low_yes},
    {"test_is_channel_within_error_high_yes", test_is_channel_within_error_high_yes},
    {"test_is_channel_within_error_low_no", test_is_channel_within_error_low_no},
    {"test_is_channel_within_error_high_no", test_is_channel_within_error_high_no},
    {"test_is_color_within_error_exact", test_is_color_within_error_exact},
    {"test_is_color_within_error_all_within", test_is_color_within_error_all_within},
    {"test_is_color_within_error_two_within", test_is_color_within_error_two_within},
    {"test_is_color_within_error_none_within", test_is_color_within_error_none_within},
    {"test_color_matches_match", test_color_matches_match},
    {"test_color_matches_no_match", test_color_matches_no_match},
    {"test_find_match_exact", test_find_match_exact},
    {"test_find_match_within_error", test_find_match_within_error},
    {"test_find_match_within_error_but_exact_only", test_find_match_within_error_but_exact_only},
    {"test_find_match_no_match", test_find_match_no_match},
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_functional[] = {
    {"test_search_alpha", test_search_alpha},
    {"test_search_alpha_no_results", test_search_alpha_no_results},
    {"test_color_bruteforce", test_color_bruteforce},
    {"test_color_bruteforce_no_results", test_color_bruteforce_no_results},
    CU_TEST_INFO_NULL,
};

CU_SuiteInfo suites[] = {
    {"conversion", NULL, NULL, set_up, tear_down, test_array_conversion},
    {"validation", NULL, NULL, NULL, NULL, test_array_validation},
    {"overlay", NULL, NULL, NULL, NULL, test_array_overlay},
    {"matching", NULL, NULL, NULL, NULL, test_array_matching},
    {"functional", NULL, NULL, NULL, NULL, test_array_functional},
    CU_SUITE_INFO_NULL,
};

void add_tests_colorbruterforcer()
{
    CU_register_suites(suites);
}
