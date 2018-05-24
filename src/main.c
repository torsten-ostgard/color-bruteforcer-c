#include <stdlib.h>

#include "argtable3.h"

#include "color_bruteforcer.h"

struct arg_lit *help_arg;
struct arg_int *alpha_min_arg;
struct arg_int *alpha_max_arg;
struct arg_int *range_arg;
struct arg_lit *exact_arg;
struct arg_lit *verbose_arg;
struct arg_end *end;
/*
 * In my testing, +/-1 in each channel is the sweet spot, offering plenty of relevant, potential
 * colors, without returning a bunch of irrelevant results that are noticeably incorrect.
 */
const int DEFAULT_ERROR_RANGE = 1;


int main(int argc, char *argv[])
{
    void *argtable[] = {
        help_arg = arg_litn("h", "help", 0, 1, "display this help and exit"),
        alpha_min_arg = arg_intn(NULL, "alpha-min", "<n>", 0, 1,
            "the lowest opacity value to check; defaults to 1"),
        alpha_max_arg = arg_intn(NULL, "alpha-max", "<n>", 0, 1,
            "the highest opacity value to check; defaults to 99"),
        range_arg = arg_intn("r", "range", "<n>", 0, 1,
            "the error range of +/-n that will let a channel be considered an inexact match; "
            "defaults to 1"),
        exact_arg = arg_litn("e", "exact", 0, 1, "only display exact matches"),
        verbose_arg = arg_litn("v", "verbose", 0, 1, "show verbose output"),
        end = arg_end(10)
    };

    char program_name[] = "color_bruteforcer";
    int num_errors = arg_parse(argc, argv, argtable);

    // Special case: '--help' takes precedence over error reporting
    if (help_arg->count > 0) {
        printf("Usage: %s", program_name);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Find an unknown, semitransparent overlay color.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return EXIT_SUCCESS;
    }

    // If the parser returned any errors, display them and exit
    if (num_errors > 0) {
        // Display the error details contained in the arg_end struct
        arg_print_errors(stdout, end, program_name);
        printf("Try '%s --help' for more information.\n", program_name);
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return EXIT_FAILURE;
    }

    int alpha_min = alpha_min_arg->count == 1 ? alpha_min_arg->ival[0] : 1;
    int alpha_max = alpha_max_arg->count == 1 ? alpha_max_arg->ival[0] : 99;
    int range = range_arg->count == 1 ? range_arg->ival[0] : DEFAULT_ERROR_RANGE;
    bool exact_only = exact_arg->count;
    verbose = verbose_arg->count;

    if (!is_in_valid_range(alpha_min) || !is_in_valid_range(alpha_max)) {
        fprintf(stderr, "The alphas to search must be between 1 and 99.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return EXIT_FAILURE;
    }
    else if (alpha_min > alpha_max) {
        fprintf(stderr, "alpha-min must be less than or equal to alpha-max.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return EXIT_FAILURE;
    }

    int num_samples = 0;
    Color **all_colors = parse_colors(&num_samples);

    if (all_colors == NULL || num_samples < 1) {
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return EXIT_FAILURE;
    }

    const Color **original_colors = malloc(num_samples * sizeof(Color*));
    const Color **target_colors = malloc(num_samples * sizeof(Color*));

    for (size_t i = 0; i < num_samples; i++) {
        original_colors[i] = all_colors[i * 2];
        target_colors[i] = all_colors[(i * 2) + 1];
    }

    Result **flat_results = color_bruteforce(original_colors, target_colors, num_samples,
        alpha_min, alpha_max, range, exact_only);
    Result *result;

    // Print results and clean up allocated memory
    for (size_t i = 0; (result = flat_results[i]) != NULL; i++) {
        if (result->is_exact) {
            printf("EXACT MATCH FOUND: ");
        }

        char hex[] = "#______";
        color_to_hex(result->color, hex);
        printf("%s at %d%% opacity\n", hex, result->alpha);

        free(result->color);
        free(result);
    }

    free(flat_results);

    for (size_t i = 0; i < num_samples * 2; i++) {
        free(all_colors[i]);
    }

    free(all_colors);
    free(original_colors);
    free(target_colors);

    // Deallocate each non-null entry in argtable[]
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return EXIT_SUCCESS;
}
