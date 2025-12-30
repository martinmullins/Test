# xextool - XEX File Analysis Tool

## Overview
xextool is a command-line utility for analyzing Xbox 360 executable (XEX) files.
It parses XEX headers and displays information about the file structure, including
encryption and compression details.

## Features
- Basic XEX header parsing
- Magic number verification
- Module flags display
- Optional header enumeration
- **Encryption type detection** (None, Normal/Encrypted, Unknown)
- **Compression type detection** (None, Basic, Normal, Delta)
- Verbose mode for detailed analysis
- Clear encryption status reporting

## Building
The tool can be built using make:
```bash
cd tools/xextool
make
```

## Usage
Basic usage to analyze a XEX file:
```bash
./xextool <path-to-xex-file>
```

### Command-Line Options
- `-v, --verbose`: Enable verbose output (shows all optional headers)
- `-e, --encryption`: Display detailed encryption information
- `-h, --help`: Show help message

### Examples
Basic analysis:
```bash
./xextool ../../dolphin/dolphin.xex
```

Show encryption details:
```bash
./xextool --encryption ../../dolphin/dolphin.xex
```

Verbose mode with encryption information:
```bash
./xextool --verbose --encryption ../../dolphin/dolphin.xex
```

Using short flags:
```bash
./xextool -v -e ../../dolphin/dolphin.xex
```

## Output Information

### Basic Mode
- File size and path
- XEX2 header validation
- Module flags
- PE offset
- Security offset
- Optional header count

### With --encryption Flag
All basic information plus:
- Encryption type (None, Normal, or Unknown)
- Compression type (None, Basic, Normal, or Delta)
- Clear encryption status summary
- Processing recommendations

### With --verbose Flag
All basic information plus:
- Complete list of optional headers with keys and values
- Known header type identification
- Additional compression analysis

## Encryption Types
- **None (0)**: XEX file is not encrypted, can be processed directly
- **Normal (1)**: XEX file is encrypted, requires decryption before processing
- **Unknown**: Unrecognized encryption type

## Compression Types
- **None (0)**: No compression applied
- **Basic (1)**: Basic compression, may need decompression
- **Normal (2)**: Normal compression, may need decompression
- **Delta (3)**: Delta compression, requires base file

## Exit Codes
- `0`: Success
- `1`: Error (invalid file, missing file, or parsing error)

## Implementation Status
This tool provides production-ready features for:
- XEX header parsing and validation
- Optional header enumeration
- Encryption type detection
- Compression type detection
- Detailed verbose output

## Future Enhancements
Potential improvements for future versions:
- Section analysis
- Import/export table parsing
- Resource extraction
- PE header analysis for basefile
- Support for additional optional header types
