import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


sources = SCRIPT_DIR + "/../../src/hazel/"


class NProperty:
    def __init__(self, name):
        self.name = name


class NClass:
    def __init__(self, name):
        self.name = name


for root, dirs, files in os.walk(sources):
    for file in files:
        if not file.endswith([".h", ".hpp"]):
            continue
        file_path = os.path.join(root, file)
        print(file_path)

        with open(file_path, "r") as f:
            content = f.read()
