/*
 * xextool - XEX File Analysis Tool
 * 
 * A simple utility to analyze Xbox 360 executable (XEX) files.
 * This tool parses and displays XEX file headers and structure information,
 * including encryption and compression details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>

/* XEX2 Header structure (big-endian) */
typedef struct {
    uint32_t magic;              /* "XEX2" */
    uint32_t module_flags;
    uint32_t pe_offset;
    uint32_t reserved;
    uint32_t security_offset;
    uint32_t optional_header_count;
} __attribute__((packed)) XEX2_Header;

/* FILE_FORMAT_INFO structure */
typedef struct {
    uint32_t info_size;
    uint16_t encryption_type;
    uint16_t compression_type;
} __attribute__((packed)) FileFormatInfo;

/* Constants */
#define XEX2_MAGIC 0x58455832           /* "XEX2" in big-endian */
#define MAX_OPTIONAL_HEADERS 100         /* Sanity check limit */
#define DISPLAY_HEADER_LIMIT 20          /* Number of headers to display */

/* Optional header keys */
#define XEX_HEADER_FILE_FORMAT_INFO     0x000003FF
#define XEX_HEADER_ENTRY_POINT          0x00010100
#define XEX_HEADER_IMAGE_BASE_ADDRESS   0x00010201
#define XEX_HEADER_IMPORT_LIBRARIES     0x000103FF
#define XEX_HEADER_DELTA_PATCH          0x000005FF

/* Encryption types */
#define XEX_ENCRYPTION_NONE   0
#define XEX_ENCRYPTION_NORMAL 1

/* Compression types */
#define XEX_COMPRESSION_NONE   0
#define XEX_COMPRESSION_BASIC  1
#define XEX_COMPRESSION_NORMAL 2
#define XEX_COMPRESSION_DELTA  3

/* Global flags for verbose and encryption display */
static int verbose_mode = 0;
static int show_encryption = 0;

/* Helper function to convert big-endian to host byte order (32-bit) */
uint32_t be32_to_cpu(uint32_t val) {
    return ((val & 0xFF000000) >> 24) |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x000000FF) << 24);
}

/* Helper function to convert big-endian to host byte order (16-bit) */
uint16_t be16_to_cpu(uint16_t val) {
    return ((val & 0xFF00) >> 8) |
           ((val & 0x00FF) << 8);
}

/* Get encryption type name */
const char* get_encryption_name(uint16_t type) {
    switch (type) {
        case XEX_ENCRYPTION_NONE:   return "None";
        case XEX_ENCRYPTION_NORMAL: return "Normal (Encrypted)";
        default:                    return "Unknown";
    }
}

/* Get compression type name */
const char* get_compression_name(uint16_t type) {
    switch (type) {
        case XEX_COMPRESSION_NONE:   return "None";
        case XEX_COMPRESSION_BASIC:  return "Basic";
        case XEX_COMPRESSION_NORMAL: return "Normal";
        case XEX_COMPRESSION_DELTA:  return "Delta";
        default:                     return "Unknown";
    }
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
    uint32_t file_format_info_offset = 0;
    int has_encryption_info = 0;
    
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
    printf("Optional Header Count: %u\n", be32_to_cpu(header.optional_header_count));
    
    if (verbose_mode) {
        printf("\nVerbose mode: ON\n");
    }
    printf("\n");
    
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
            
            /* Check if this is FILE_FORMAT_INFO header */
            if (key == XEX_HEADER_FILE_FORMAT_INFO) {
                file_format_info_offset = value;
                has_encryption_info = 1;
            }
            
            if (verbose_mode || show_encryption) {
                printf("  [%2u] Key: 0x%08X  Value: 0x%08X", i, key, value);
                
                /* Display known header types */
                switch (key) {
                    case XEX_HEADER_FILE_FORMAT_INFO:
                        printf(" (FILE_FORMAT_INFO)");
                        break;
                    case XEX_HEADER_ENTRY_POINT:
                        printf(" (ENTRY_POINT)");
                        break;
                    case XEX_HEADER_IMAGE_BASE_ADDRESS:
                        printf(" (IMAGE_BASE_ADDRESS)");
                        break;
                    case XEX_HEADER_IMPORT_LIBRARIES:
                        printf(" (IMPORT_LIBRARIES)");
                        break;
                    case XEX_HEADER_DELTA_PATCH:
                        printf(" (DELTA_PATCH_DESCRIPTOR)");
                        break;
                }
                printf("\n");
            }
        }
        
        if (verbose_mode && opt_count > DISPLAY_HEADER_LIMIT) {
            printf("  ... (%u more headers)\n", opt_count - DISPLAY_HEADER_LIMIT);
        }
        
        if (verbose_mode || show_encryption) {
            printf("\n");
        }
    }
    
    /* Parse and display FILE_FORMAT_INFO if encryption details requested */
    if (has_encryption_info && (show_encryption || verbose_mode)) {
        printf("=== FILE_FORMAT_INFO (Encryption & Compression Details) ===\n");
        
        /* Seek to FILE_FORMAT_INFO offset */
        if (fseek(fp, file_format_info_offset, SEEK_SET) != 0) {
            fprintf(stderr, "ERROR: Cannot seek to FILE_FORMAT_INFO at offset 0x%08X\n", 
                    file_format_info_offset);
        } else {
            FileFormatInfo ffi;
            if (fread(&ffi, sizeof(ffi), 1, fp) == 1) {
                uint32_t info_size = be32_to_cpu(ffi.info_size);
                uint16_t encryption = be16_to_cpu(ffi.encryption_type);
                uint16_t compression = be16_to_cpu(ffi.compression_type);
                
                printf("Info Size:         %u bytes\n", info_size);
                printf("Encryption Type:   %u (%s)\n", encryption, get_encryption_name(encryption));
                printf("Compression Type:  %u (%s)\n", compression, get_compression_name(compression));
                
                /* Display encryption status clearly */
                if (show_encryption) {
                    printf("\n");
                    printf("*** ENCRYPTION STATUS ***\n");
                    if (encryption == XEX_ENCRYPTION_NONE) {
                        printf("This XEX file is NOT encrypted\n");
                    } else if (encryption == XEX_ENCRYPTION_NORMAL) {
                        printf("This XEX file IS ENCRYPTED (Normal encryption)\n");
                        printf("Decryption required before further processing\n");
                    } else {
                        printf("This XEX file has UNKNOWN encryption type (%u)\n", encryption);
                    }
                    printf("*************************\n");
                }
                
                /* Verbose mode: Additional compression details */
                if (verbose_mode) {
                    printf("\nCompression Analysis:\n");
                    if (compression == XEX_COMPRESSION_NONE) {
                        printf("  - File is not compressed\n");
                    } else if (compression == XEX_COMPRESSION_DELTA) {
                        printf("  - WARNING: Delta compression requires base file\n");
                        printf("  - This compression type may not be supported by all tools\n");
                    } else {
                        printf("  - Decompression may be required before processing\n");
                    }
                }
            } else {
                fprintf(stderr, "ERROR: Cannot read FILE_FORMAT_INFO structure\n");
            }
        }
        printf("\n");
    } else if (show_encryption && !has_encryption_info) {
        printf("=== ENCRYPTION STATUS ===\n");
        printf("WARNING: FILE_FORMAT_INFO header not found\n");
        printf("Cannot determine encryption status\n");
        printf("This may be an unusual or corrupted XEX file\n\n");
    }
    
    printf("========================================\n");
    printf("Analysis complete!\n");
    printf("========================================\n");
    
    fclose(fp);
    return 0;
}

/* Display help message */
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [OPTIONS] <xex-file>\n", prog_name);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "  -v, --verbose      Enable verbose output (shows all optional headers)\n");
    fprintf(stderr, "  -e, --encryption   Display detailed encryption information\n");
    fprintf(stderr, "  -h, --help         Show this help message\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s dolphin.xex\n", prog_name);
    fprintf(stderr, "  %s --verbose dolphin.xex\n", prog_name);
    fprintf(stderr, "  %s --encryption dolphin.xex\n", prog_name);
    fprintf(stderr, "  %s -v -e dolphin.xex\n", prog_name);
}

int main(int argc, char *argv[]) {
    int opt;
    const char *xex_file = NULL;
    
    /* Define long options */
    static struct option long_options[] = {
        {"verbose",    no_argument, 0, 'v'},
        {"encryption", no_argument, 0, 'e'},
        {"help",       no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    /* Parse command-line options */
    while ((opt = getopt_long(argc, argv, "veh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v':
                verbose_mode = 1;
                break;
            case 'e':
                show_encryption = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    /* Check if XEX file is provided */
    if (optind >= argc) {
        fprintf(stderr, "ERROR: No XEX file specified\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    xex_file = argv[optind];
    
    return analyze_xex_file(xex_file);
}
