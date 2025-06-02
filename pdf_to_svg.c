
#include <stdio.h>
#include <stdlib.h>
#include <mupdf/fitz.h>

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s input.pdf page_number output.svg\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_pdf = argv[1];
    int page_number = atoi(argv[2]) - 1; // Pages are zero-indexed
    const char *output_svg = argv[3];

    fz_context *ctx = NULL;
    fz_document *doc = NULL;
    fz_page *page = NULL;
    fz_rect bounds;
    fz_output *out = NULL;
    fz_device *dev = NULL;

    // Initialize MuPDF context
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) {
        fprintf(stderr, "Cannot create MuPDF context\n");
        return EXIT_FAILURE;
    }

    // Register document handlers (e.g., PDF, XPS, etc.)
    fz_register_document_handlers(ctx);

    // Open the PDF document
    doc = fz_open_document(ctx, input_pdf);
    if (!doc) {
        fprintf(stderr, "Cannot open document: %s\n", input_pdf);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    // Load the specified page
    page = fz_load_page(ctx, doc, page_number);
    if (!page) {
        fprintf(stderr, "Cannot load page %d\n", page_number + 1);
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    // Determine page bounds
    fz_bound_page(ctx, page, &bounds);

    // Create output stream for SVG file
    out = fz_new_output_with_path(ctx, output_svg, 0);
    if (!out) {
        fprintf(stderr, "Cannot create output file: %s\n", output_svg);
        fz_drop_page(ctx, page);
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    // Create SVG device
    dev = fz_new_svg_device(ctx, out, bounds.x1 - bounds.x0, bounds.y1 - bounds.y0, FZ_SVG_TEXT_AS_PATH, 0);
    if (!dev) {
        fprintf(stderr, "Cannot create SVG device\n");
        fz_drop_output(ctx, out);
        fz_drop_page(ctx, page);
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    // Render page to SVG device
    fz_run_page(ctx, page, dev, &fz_identity, NULL);

    // Clean up resources
    fz_drop_device(ctx, dev);
    fz_drop_output(ctx, out);
    fz_drop_page(ctx, page);
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);

    printf("Successfully converted page %d of %s to %s\n", page_number + 1, input_pdf, output_svg);
    return EXIT_SUCCESS;
}
