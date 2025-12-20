"""
Simple demonstration script using pyautogui.
This script is used by the GitHub Actions workflow.
"""

import sys
import io

# Enforce UTF-8 encoding for stdout to handle Unicode characters
# This prevents UnicodeEncodeError on systems with non-UTF-8 default encoding
if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
    if hasattr(sys.stdout, 'buffer'):
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# Try to import pyautogui with error handling for headless environments
try:
    import pyautogui
    print("[OK] pyautogui imported successfully")
    
    # Get screen size if display is available
    try:
        screen_size = pyautogui.size()
        print(f"[OK] Screen size detected: {screen_size[0]}x{screen_size[1]}")
    except Exception as e:
        print(f"[Warning] Could not detect screen (headless environment): {e}")
        print("This is expected behavior in CI/CD environments")
        
except ImportError as e:
    print(f"[ERROR] pyautogui is not installed: {e}")
    sys.exit(1)
except Exception as e:
    print(f"[Warning] pyautogui import issue (possibly headless): {e}")
    print("This is expected behavior in CI/CD environments")

print("\n[OK] Script executed successfully")
print("Note: Full GUI automation is not possible in headless CI environments")
sys.exit(0)
