# XenonRecomp Binary Compatibility Investigation

## Problem Statement
The xenon recomp binary does not like the decrypted binary and crashes with segmentation fault (exit code 139).

## Current Situation

### Binary File Analysis
- **File**: `dolphin/dolphin_decrypted.bin`
- **Size**: 2,686,976 bytes
- **Type**: PE32 executable (XBOX) PowerPC (big-endian), for MS Windows, 8 sections
- **Header**: Valid PE header with MZ magic at 0x00 and PE signature at 0xE0

The file structure is correct - it's a valid PE executable extracted from the XEX file using xex1tool.

### XenonRecomp Workflow Behavior
The workflow successfully:
1. Builds XenonRecomp
2. Verifies the binary file exists and has a valid PE header
3. Runs XenonAnalyse (no jump tables detected - this is normal)
4. Creates configuration file with correct paths

But then crashes at:
- **Step**: "Run XenonRecomp to generate C++ files"
- **Exit Code**: 139 (segmentation fault)
- **Result**: No C++ files generated

## Root Cause Analysis

XenonRecomp is a PowerPC to C++ recompiler that expects specific characteristics in the PE binary:

1. **Big-endian PowerPC executable format**: ✓ (confirmed by `file` output)
2. **Valid PE headers**: ✓ (MZ at start, PE signature present)
3. **Xbox 360 executable sections**: Likely present, but structure may be incompatible

The issue is likely one of the following:

### Hypothesis 1: Binary Format Issue
XenonRecomp may expect a "raw" basefile format (just the executable code) rather than a full PE executable with all headers and sections. However, the workflow explicitly checks for PE headers, so this seems unlikely.

### Hypothesis 2: Section Alignment or Structure
The extracted PE file may have section alignments or structures that XenonRecomp cannot handle. Xbox 360 PE files can have different section layouts than standard PE files.

### Hypothesis 3: Compression/Encryption Remnants  
Even though the file is "decrypted", there may be residual XEX structures or compressed sections that XenonRecomp cannot process.

### Hypothesis 4: XenonRecomp Bug or Limitation
XenonRecomp may have bugs or limitations with certain types of Xbox 360 executables. The tool is experimental and may not handle all binary variations.

## Recommended Solutions

### Solution 1: Use Different Extraction Method
Instead of using `xex1tool -b` to extract the basefile, try using a different extraction method that provides a more "raw" binary format that XenonRecomp expects.

**Action**: Research XenonRecomp documentation and examples to understand the exact input format it requires.

### Solution 2: Pre-process the Binary
Strip unnecessary PE headers/sections and create a minimal PE file that contains only the executable code sections that XenonRecomp needs.

**Implementation**: Create a tool to:
1. Parse the PE headers
2. Extract only the `.text` and essential sections
3. Rebuild a minimal PE file with proper alignments

### Solution 3: Fix XenonRecomp
If the issue is a bug in XenonRecomp itself, we could:
1. Run XenonRecomp with a debugger to get the exact crash location
2. Submit a bug report with the dolphin_decrypted.bin file as a test case
3. Fork and fix XenonRecomp if needed

### Solution 4: Alternative Decompiler
Consider using an alternative PowerPC decompiler that may be more robust:
- Ghidra with PowerPC support
- IDA Pro with PowerPC plugin
- radare2/rizin with PowerPC architecture support

## Next Steps

1. **Get detailed crash information**: Run the workflow with gdb to get a backtrace (the workflow already has this capability)
2. **Review XenonRecomp source code**: Check how it loads and validates PE files
3. **Test with other Xbox 360 binaries**: Determine if this is specific to dolphin.xex or a general issue
4. **Contact XenonRecomp maintainers**: Open an issue on the XenonRecomp GitHub repository with our findings

## References
- XenonRecomp repository: https://github.com/hedge-dev/XenonRecomp
- Workflow logs: See failed workflow run #18
- Binary location: `dolphin/dolphin_decrypted.bin`
