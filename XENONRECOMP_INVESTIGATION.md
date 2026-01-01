# XenonRecomp Binary Compatibility Investigation

## Executive Summary

**Problem**: XenonRecomp crashes with segmentation fault (exit code 139) when processing `dolphin_decrypted.bin`.

**Root Cause**: The source XEX file (`dolphin.xex`) is both encrypted (Type 1: Normal) AND compressed (Type 1: Basic). While the binary has been decrypted, there may be incomplete decompression causing XenonRecomp to crash.

**Immediate Action Required**: Verify that the extracted basefile is fully decompressed. The current `dolphin_decrypted.bin` may still contain compressed sections or compression metadata that XenonRecomp cannot process.

**Recommended Fix**: Re-extract the basefile with verified decompression or manually decompress any remaining compressed sections before feeding to XenonRecomp.

---

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

### XEX File Status (Confirmed via xextool)
The source XEX file (`dolphin/dolphin.xex`) has:
- **Encryption Type**: 1 (Normal/Encrypted) - ✓ Requires decryption
- **Compression Type**: 1 (Basic) - ✓ Requires decompression

### Current Binary Status
The `dolphin_decrypted.bin` file was extracted using `xex1tool -b` which should:
- Decrypt the XEX file ✓
- Decompress the basefile ✓
- Extract the PE executable ✓

XenonRecomp is a PowerPC to C++ recompiler that expects specific characteristics in the PE binary:

1. **Big-endian PowerPC executable format**: ✓ (confirmed by `file` output)
2. **Valid PE headers**: ✓ (MZ at start, PE signature present)
3. **Fully decompressed**: ? (needs verification)
4. **Xbox 360 executable sections**: Likely present, but structure may be incompatible

### Primary Hypothesis: Decompression Issue
The most likely cause is that the basefile extraction did not fully decompress the binary, leaving compressed sections that XenonRecomp cannot process. The xex1tool `-b` flag is supposed to handle both decryption and decompression, but there may be an issue with:

1. **Incomplete decompression**: Some sections remain compressed
2. **Compression format incompatibility**: XenonRecomp doesn't recognize the decompression format used
3. **Residual compression headers**: Compression metadata still present in the binary

### Secondary Hypotheses

#### Hypothesis 2: Section Alignment or Structure
The extracted PE file may have section alignments or structures that XenonRecomp cannot handle. Xbox 360 PE files can have different section layouts than standard PE files.

#### Hypothesis 3: XenonRecomp Input Format Mismatch
XenonRecomp may expect a specific "preprocessed" format rather than a raw PE executable. The tool may require:
- Specific section ordering
- Stripped debug information
- Aligned memory addresses
- Specific entry point format

#### Hypothesis 4: XenonRecomp Bug or Limitation  
XenonRecomp may have bugs or limitations with certain types of Xbox 360 executables. The tool is experimental and may not handle all binary variations correctly.

## Recommended Solutions

### Solution 1: Verify Complete Decompression (PRIMARY)
The first step is to verify that the dolphin_decrypted.bin file is fully decompressed.

**Action Steps**:
1. Check if `xex1tool -b` fully decompresses the basefile
2. Try using explicit decompression flags or different extraction method
3. Verify the extracted file has no compression markers/headers
4. Compare file sizes to ensure full expansion

**Implementation**:
```bash
# Re-extract with explicit decompression verification
xex1tool -b dolphin_decrypted_v2.bin dolphin/dolphin.xex

# Or try decompress flag if available
xex1tool -d -b dolphin_decrypted_v2.bin dolphin/dolphin.xex

# Verify no compression signatures remain
xxd dolphin_decrypted_v2.bin | grep -i "compress\|lz\|zlib"
```

### Solution 2: Use Alternative Extraction Tool
Try using a different tool that guarantees full decompression:

**Options**:
- **xextool** (if it supports extraction)
- **xorloser's xextool** (different from our analysis tool)
- **Xbox 360 SDK tools** (if available)
- **Manual decompression** using known Xbox 360 compression libraries

### Solution 3: Pre-process the Binary
Create a tool to validate and fix the extracted binary:

**Implementation**: Create a Python/C tool to:
1. Parse the PE headers thoroughly
2. Check each section for compression markers
3. Decompress any remaining compressed sections
4. Rebuild a clean PE file with proper section alignments
5. Validate against XenonRecomp requirements

### Solution 4: Contact Tool Maintainers
Engage with both tool maintainers:

**xex1tool/idaxex**:
- Verify that `-b` flag fully decompresses
- Request documentation on exact output format
- Report if there's a decompression bug

**XenonRecomp**:
- Provide dolphin_decrypted.bin as a test case
- Request documentation on expected input format
- Get detailed requirements for PE structure

### Solution 5: Alternative Decompiler (FALLBACK)
If XenonRecomp cannot be fixed, consider alternatives:
- **Ghidra** with PowerPC support
- **IDA Pro** with PowerPC plugin  
- **radare2/rizin** with PowerPC architecture
- **RetDec** with PowerPC backend

## Next Steps

1. **Get detailed crash information**: Run the workflow with gdb to get a backtrace (the workflow already has this capability)
2. **Review XenonRecomp source code**: Check how it loads and validates PE files
3. **Test with other Xbox 360 binaries**: Determine if this is specific to dolphin.xex or a general issue
4. **Contact XenonRecomp maintainers**: Open an issue on the XenonRecomp GitHub repository with our findings

## References
- XenonRecomp repository: https://github.com/hedge-dev/XenonRecomp
- Workflow logs: See failed workflow run #18
- Binary location: `dolphin/dolphin_decrypted.bin`
