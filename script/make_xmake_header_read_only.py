

import os
import stat
from tqdm import tqdm


# Define the directory to process
xmake_dir = r"C:\Users\norm\AppData\Local\.xmake"

# Function to make a file read-only
def make_readonly(file_path):
    try:
        os.chmod(file_path, stat.S_IREAD)
        # print(f"Made {file_path} read-only")
    except Exception as e:
        print(f"Failed to make {file_path} read-only: {e}")

# Walk through the directory and process files
for root, dirs, files in tqdm(os.walk(xmake_dir), desc="Processing files"):
    # Check if the current directory or any parent directory contains "include"
    if "include" in root:
        # Process files in the current directory
        for file in files:
            file_path = os.path.join(root, file)
            make_readonly(file_path)