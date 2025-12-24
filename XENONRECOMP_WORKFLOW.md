# XenonRecomp Workflow Documentation

## Overview

This workflow uses [XenonRecomp](https://github.com/hedge-dev/XenonRecomp) to convert Xbox 360 executables into C++ code.

## Workflow Steps

1. **Check out repository** - Clones the Test repository
2. **Set up Clang 18** - Installs Clang 18 compiler (required by XenonRecomp)
3. **Install CMake** - Installs CMake build system
4. **Clone XenonRecomp** - Clones the XenonRecomp repository
5. **Build XenonRecomp** - Builds XenonRecomp using CMake
6. **Verify XEX file** - Checks that the input XEX file exists
7. **Create output directory** - Creates directory for generated C++ files
8. **Run XenonAnalyse** - Detects jump tables in the XEX file
9. **Extract function addresses** - Extracts runtime function addresses from .map file
10. **Create configuration** - Generates XenonRecomp configuration file
11. **Validate configuration** - Validates that all required addresses are present
12. **Run XenonRecomp** - Converts XEX to C++ code
13. **List generated files** - Shows the generated C++ files
14. **Commit and push** - Commits generated files back to repository

## Configuration

### Required Runtime Function Addresses

XenonRecomp requires addresses for Xbox 360 runtime functions. These are automatically extracted from the `.map` file if available:

- `__restgprlr_14` - Restore general purpose registers
- `__savegprlr_14` - Save general purpose registers
- `__restfpr_14` - Restore floating point registers
- `__savefpr_14` - Save floating point registers
- `__restvmx_14` - Restore VMX registers
- `__savevmx_14` - Save VMX registers
- `__restvmx_64` - Restore VMX registers (64-bit)
- `__savevmx_64` - Save VMX registers (64-bit)

### Input Parameters

- **xex_file_path** (optional): Path to the XEX file (default: `dolphin/dolphin.xex`)
- **output_directory** (optional): Directory for generated C++ files (default: `generated_cpp`)

## Known Issues and Limitations

### Segmentation Faults

XenonRecomp may crash with a segmentation fault (exit code 139) on certain XEX files. This can occur for several reasons:

1. **Unsupported XEX Format**: XenonRecomp was designed primarily for specific games like Sonic Unleashed. Some XEX files may use features or formats that are not fully supported.

2. **Incomplete Analysis**: If XenonAnalyse cannot properly analyze the XEX structure, XenonRecomp may crash when trying to process it.

3. **Missing or Incorrect Addresses**: While the workflow attempts to extract all required function addresses from the .map file, some XEX files may not have all required symbols.

### Dolphin Sample

The included `dolphin/dolphin.xex` sample is from the Xbox 360 SDK and may not be fully compatible with XenonRecomp. This is a known limitation of the tool.

## Troubleshooting

### "XenonRecomp crashed with a segmentation fault"

If you see this error:

1. **Check the .map file**: Ensure `dolphin/dolphin.map` exists and contains the required function symbols
2. **Verify addresses**: Check the "Extract function addresses" step to ensure valid addresses were found
3. **Try a different XEX**: Consider using a different Xbox 360 executable that is known to work with XenonRecomp

### "XenonAnalyse exited with code 139"

XenonAnalyse may also segfault on unsupported XEX files. The workflow handles this by creating an empty jump tables file and continuing, but XenonRecomp may still fail.

### Timeout Errors

If XenonRecomp runs for more than 5 minutes, the workflow will timeout. This may indicate:
- The XEX file is very large
- XenonRecomp encountered an infinite loop
- The analysis is taking longer than expected

## Success Criteria

The workflow completes successfully when:
1. XenonRecomp runs without crashing
2. C++ files are generated in the output directory
3. Generated files are committed back to the repository

## Resources

- [XenonRecomp GitHub Repository](https://github.com/hedge-dev/XenonRecomp)
- [XenonRecomp README](https://github.com/hedge-dev/XenonRecomp/blob/main/README.md)
- [Example Configuration (Sonic Unleashed)](https://github.com/hedge-dev/UnleashedRecomp/blob/main/UnleashedRecompLib/config/SWA.toml)
