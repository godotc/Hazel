# make some directories readonly in python

import os
import stat

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


def make_readonly(paths):
    for path in paths:
        for root, dirs, files in os.walk(path):
            for file in files:
                file_path = os.path.join(root, file)
                os.chmod(file_path, stat.S_IREAD)
                print(f"make {file_path} readonly")
        os.chmod(path, stat.S_IREAD)
        print(f"make {path} readonly")


make_readonly(
    [
        SCRIPT_DIR + "/../src/pkgs/glad",
        SCRIPT_DIR + "/../src/pkgs/imgui-docking",
        SCRIPT_DIR + "/../src/pkgs/imguizmo",
    ]
)
