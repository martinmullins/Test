# xextool - XEX File Analysis Tool

## Overview
xextool is a command-line utility for analyzing Xbox 360 executable (XEX) files.
It parses XEX headers and displays information about the file structure.

## Building
The tool can be built using make:
```bash
cd tools/xextool
make
```

## Usage
To analyze a XEX file:
```bash
./xextool <path-to-xex-file>
```

Example:
```bash
./xextool ../../dolphin/dolphin.xex
```

## Implementation Status
This is a placeholder implementation. The current version provides:
- Basic XEX header parsing
- Magic number verification
- Module flags display
- Header size information
- Security offset information

## Future Enhancements
To make this tool production-ready, consider implementing:
- Detailed optional header parsing
- Section analysis
- Import/export table parsing
- Resource extraction
- PE header analysis for basefile
- Encryption type detection
