/*
 * xextool - XEX File Analysis Tool
 * 
 * A simple utility to analyze Xbox 360 executable (XEX) files.
 * This tool parses and displays XEX file headers and structure information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* XEX2 Header structure (big-endian) */
typedef struct {
    uint32_t magic;              /* "XEX2" */
    uint32_t module_flags;
    uint32_t pe_offset;
    uint32_t reserved;
    uint32_t security_offset;
    uint32_t optional_header_count;
} __attribute__((packed)) XEX2_Header;

/* Constants */
#define XEX2_MAGIC 0x58455832           /* "XEX2" in big-endian */
#define MAX_OPTIONAL_HEADERS 100         /* Sanity check limit */
#define DISPLAY_HEADER_LIMIT 20          /* Number of headers to display */

/* Helper function to convert big-endian to host byte order */
uint32_t be32_to_cpu(uint32_t val) {
    return ((val & 0xFF000000) >> 24) |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x000000FF) << 24);
}

/* Display file size in human-readable format */
void print_file_size(off_t size) {
    if (size < 1024) {
        printf("%ld bytes", (long)size);
    } else if (size < 1024 * 1024) {
        printf("%.2f KB", size / 1024.0);
    } else {
        printf("%.2f MB", size / (1024.0 * 1024.0));
    }
}

/* Analyze XEX file */
int analyze_xex_file(const char *filename) {
    FILE *fp;
    XEX2_Header header;
    struct stat st;
    uint32_t magic;
    
    printf("========================================\n");
    printf("XEX File Analysis Tool\n");
    printf("========================================\n\n");
    
    /* Open file */
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: Cannot open file '%s'\n", filename);
        return 1;
    }
    
    /* Get file size */
    if (stat(filename, &st) != 0) {
        fprintf(stderr, "ERROR: Cannot stat file '%s'\n", filename);
        fclose(fp);
        return 1;
    }
    
    printf("File: %s\n", filename);
    printf("Size: ");
    print_file_size(st.st_size);
    printf(" (%ld bytes)\n\n", (long)st.st_size);
    
    /* Read header */
    if (fread(&header, sizeof(header), 1, fp) != 1) {
        fprintf(stderr, "ERROR: Cannot read XEX header\n");
        fclose(fp);
        return 1;
    }
    
    /* Verify magic number */
    magic = be32_to_cpu(header.magic);
    if (magic != XEX2_MAGIC) {
        fprintf(stderr, "ERROR: Invalid XEX file - magic number mismatch\n");
        fprintf(stderr, "Expected: 0x%08X (XEX2)\n", XEX2_MAGIC);
        fprintf(stderr, "Got:      0x%08X\n", magic);
        fclose(fp);
        return 1;
    }
    
    printf("=== XEX2 Header ===\n");
    printf("Magic:                XEX2 (valid)\n");
    printf("Module Flags:         0x%08X\n", be32_to_cpu(header.module_flags));
    printf("PE Offset:            0x%08X\n", be32_to_cpu(header.pe_offset));
    printf("Security Offset:      0x%08X\n", be32_to_cpu(header.security_offset));
    printf("Optional Header Count: %u\n\n", be32_to_cpu(header.optional_header_count));
    
    /* Read and display optional headers */
    uint32_t opt_count = be32_to_cpu(header.optional_header_count);
    if (opt_count > 0 && opt_count < MAX_OPTIONAL_HEADERS) {  /* Sanity check */
        printf("=== Optional Headers ===\n");
        
        for (uint32_t i = 0; i < opt_count && i < DISPLAY_HEADER_LIMIT; i++) {
            uint32_t key, value;
            if (fread(&key, sizeof(key), 1, fp) != 1 ||
                fread(&value, sizeof(value), 1, fp) != 1) {
                break;
            }
            
            key = be32_to_cpu(key);
            value = be32_to_cpu(value);
            
            printf("  [%2u] Key: 0x%08X  Value: 0x%08X", i, key, value);
            
            /* Display known header types */
            switch (key) {
                case 0x000003FF:
                    printf(" (FILE_FORMAT_INFO)");
                    break;
                case 0x00010100:
                    printf(" (ENTRY_POINT)");
                    break;
                case 0x00010201:
                    printf(" (IMAGE_BASE_ADDRESS)");
                    break;
                case 0x000103FF:
                    printf(" (IMPORT_LIBRARIES)");
                    break;
                case 0x000005FF:
                    printf(" (DELTA_PATCH_DESCRIPTOR)");
                    break;
            }
            printf("\n");
        }
        
        if (opt_count > DISPLAY_HEADER_LIMIT) {
            printf("  ... (%u more headers)\n", opt_count - DISPLAY_HEADER_LIMIT);
        }
    }
    
    printf("\n========================================\n");
    printf("Analysis complete!\n");
    printf("========================================\n");
    
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <xex-file>\n", argv[0]);
        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "  %s dolphin.xex\n", argv[0]);
        return 1;
    }
    
    return analyze_xex_file(argv[1]);
}
