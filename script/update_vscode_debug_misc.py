import os
import sys
import json


VSCODE_TASK_JSON_FILE = ".vscode/tasks.json"
VSCODE_LAUNCH_JSON_FILE = ".vscode/launch.json"
INDENT = 4

script_dir = os.path.dirname(os.path.abspath(__file__))


def gen_launch_configs(
    target_name, target_dir, target_base_name, toolchain_name, bDebug
):
    config = {}
    if toolchain_name == "msvc":
        """msvc
        {
        "type": "cppvsdbg",
        "name": "[nut] MSVC DEBUG",
        "program": "${workspaceRoot}/build/windows/x64/debug/hazel-editor-nut.exe",
        "logging": {
            "moduleLoad": true,
            "threadExit": true,
            "processExit": true,
            "programOutput": true,
             "engineLogging": false,
            "exceptions": true,
        },
        "cwd": "${workspaceRoot}",
        "console": "integratedTerminal",
        "visualizerFile": "${workspaceFolder}/my.natvis",
        "request": "launch",
        "preLaunchTask": "build nut",
        },
        """
        config = {
            "type": "cppvsdbg",
            "name": f"[{target_name}] MSVC {bDebug and 'DEBUG' or 'RELEASE'}",
            "request": "launch",
            "program": (
                f"${{workspaceRoot}}/{target_dir}/{bDebug and 'debug' or 'release'}/{target_base_name}"
            ).replace("\\", "/"),
            "logging": {
                "moduleLoad": True,
                "threadExit": True,
                "processExit": True,
                "programOutput": True,
                "engineLogging": True,
                "exceptions": True,
            },
            "cwd": "${workspaceRoot}",
            "console": "integratedTerminal",
            "visualizerFile": "${workspaceFolder}/my.natvis",
            "preLaunchTask": f"build {target_name}",
        }
        pass
    elif toolchain_name == "gcc":
        """gcc
        {
        "type": "gdb",
        "name": "[nut] GDB DEBUG",
        "arguments": ""
        "internalConsoleOptions": "openOnSessionStart",
        "windows": {
            "target": "${workspaceFolder}/build\\windows\\x64/debug/hazel-editor-nut"
        },
        "request": "launch",
        "cwd": "${workspaceFolder}",
        "valuesFormatting": "prettyPrinters",
        "preLaunchTask": "build nut",
        },
        """
        config = {
            "type": "gdb",
            "name": f"[{target_name}] GDB {bDebug and 'DEBUG' or 'RELEASE'}",
            "internalConsoleOptions": "openOnSessionStart",
            "arguments": "",
            "windows": {
                "target": (
                    f"${{workspaceFolder}}/{target_dir}/{bDebug and 'debug' or 'release'}/{target_base_name}"
                ).replace("\\", "/")
            },
            "request": "launch",
            "cwd": "${workspaceFolder}",
            "valuesFormatting": "prettyPrinters",
            "preLaunchTask": f"build {target_name}",
        }
        pass
    elif toolchain_name == "clang":
        """lldb
        {
        "name":"[nut] LLDB RELEASE",
        "args":[],
        "windows":{
        "program":"${workspaceFolder}/build\\windows\\x64/release/hazel-editor-nut"
        },
        "request":"launch",
        "cwd":"${workspaceFolder}",
        "type":"lldb"
        "preLaunchTask":"build nut",
        }
        """
        config = {
            "type": "lldb",
            "name": f"[{target_name}] LLDB {bDebug and 'DEBUG' or 'RELEASE'}",
            "args": [],
            "windows": {
                "program": (
                    "${workspaceFolder}"
                    + f"/{target_dir}/{bDebug and 'debug' or 'release'}/{target_base_name}"
                ).replace("\\", "/"),
            },
            "cwd": "${workspaceFolder}",
            "preLaunchTask": f"build {target_name}",
        }

        pass
    else:
        print("unknown toolchain name: " + toolchain_name)
        exit(1)

    return config


def update_launch_profile(target_name, target_dir, target_base_name, toolchain_name):
    json_data = None
    try:
        json_data = json.load(open(VSCODE_LAUNCH_JSON_FILE))
    except Exception as e:
        print(e)
        json_data = {}

    if not json_data.get("version"):
        json_data["version"] = "2.0.0"

    if not json_data.get("configurations"):
        json_data["configurations"] = []

    def add_or_replace_configurations(configurations: list, new_configuration: dict):
        bReplaced = False
        for i in range(len(configurations)):
            config = configurations[i]
            if config["name"] == new_configuration["name"]:
                configurations[i] = new_configuration
                print("replace launch config: " + new_configuration["name"])
                bReplaced = True
        if not bReplaced:
            print("add launch config: " + new_configuration["name"])
            configurations.append(new_configuration)

    configs = [
        gen_launch_configs(
            target_name, target_dir, target_base_name, toolchain_name, True
        ),
        gen_launch_configs(
            target_name, target_dir, target_base_name, toolchain_name, False
        ),
    ]

    for config in configs:
        # print(config)
        add_or_replace_configurations(json_data["configurations"], config)

    print(json_data)
    json.dump(json_data, open(VSCODE_LAUNCH_JSON_FILE, "w"), indent=INDENT)


def update_task_profile(target_name):
    json_data = {}
    try:
        json_data = json.load(open(VSCODE_TASK_JSON_FILE, "r"))
    except Exception as e:
        print(e)

    if not json_data.get("version"):
        json_data["version"] = "2.0.0"

    """
     "tasks":[{
      "label":"build nut",
      "type":"shell",
      "presentation":{
        "panel":"shared",
        "showReuseMessage":true,
        "echo":true,
        "clear":false,
        "focus":false,
        "reveal":"always"
      },
      "command":"xmake",
      "args":["build","nut"]
    }]
    """
    task = {
        "label": f"build {target_name}",
        "type": "shell",
        "presentation": {
            "panel": "shared",
            "showReuseMessage": True,
            "echo": True,
            "clear": False,
            "focus": False,
            "reveal": "always",
        },
        "command": "xmake",
        "args": ["build", target_name],
    }

    json_data["tasks"] = json_data.get("tasks", [])

    for i in range(len(json_data["tasks"])):
        t = json_data["tasks"][i]
        if t["label"] == task["label"]:
            json_data["tasks"][i] = task
            print(f"replace task: {task['label']}")
            break
    else:
        json_data["tasks"].append(task)
        print(f"add task: {task['label']}")

    print(json_data)
    json.dump(json_data, open(VSCODE_TASK_JSON_FILE, "w"), indent=INDENT)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: vscode.py <target_name> <target_dir> <target_base_name>")
        sys.exit(1)

    for i in range(0, len(sys.argv)):
        arg = sys.argv[i]
        print(f"args[{i}]= {arg}")

    # script = sys.argv[0]
    toolchain_name = sys.argv[1]
    print("toolchain: " + toolchain_name)
    target_name = sys.argv[2]
    target_base_name = sys.argv[3]
    target_dir = sys.argv[4]

    update_task_profile(target_name)
    update_launch_profile(target_name, target_dir, target_base_name, toolchain_name)
