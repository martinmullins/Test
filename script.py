"""
Simple demonstration script using pyautogui.
This script is used by the GitHub Actions workflow.
"""

import sys

# Try to import pyautogui with error handling for headless environments
try:
    import pyautogui
    print("✓ pyautogui imported successfully")
    
    # Get screen size if display is available
    try:
        screen_size = pyautogui.size()
        print(f"✓ Screen size detected: {screen_size[0]}x{screen_size[1]}")
    except Exception as e:
        print(f"⚠ Warning: Could not detect screen (headless environment): {e}")
        print("This is expected behavior in CI/CD environments")
        
except ImportError as e:
    print(f"✗ Error: pyautogui is not installed: {e}")
    sys.exit(1)
except Exception as e:
    print(f"⚠ Warning: pyautogui import issue (possibly headless): {e}")
    print("This is expected behavior in CI/CD environments")

print("\n✓ Script executed successfully")
print("Note: Full GUI automation is not possible in headless CI environments")
sys.exit(0)
