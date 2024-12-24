import os
import sys
import json


VSCODE_TASK_JSON_FILE = ".vscode/tasks.json"
VSCODE_LAUNCH_JSON_FILE = ".vscode/launch.json"

script_dir = os.path.dirname(os.path.abspath(__file__))


def get_lldb_launch_config(target_name, target_dir, target_base_name):
    """
    "name":"[nut] LLDB RELEASE",
    "args":[],
    "windows":{
      "program":"${workspaceFolder}/build\\windows\\x64/release/hazel-editor-nut"
    },
    "preLaunchTask":"build nut",
    "request":"launch",
    "cwd":"${workspaceFolder}",
    "type":"lldb"
    """

    bDebug = target_dir.find("debug")
    return {
        "name": f"[{target_name}] LLDB {bDebug and 'DEBUG' or 'RELEASE'}",
        "args": [],
        "windows": {
            "program": "${workspaceFolder}"
        }
        }


def update_launch_profile(target_name, target_dir, target_base_name):
    json_data = json.load(open(VSCODE_LAUNCH_JSON_FILE))

    json_data["configurations"] = json_data["configurations"] or []

    -- Utils.DumpPrint(generates)

    def add_or_replace_configurations(configurations: list, new_configuration: dict):
        configurations = configurations or {}
        for config in configurations:
            if config["name"] == new_configuration["name"]:
                config = new_configuration
                print("replace launch config: " + new_configuration["name"])
            else: 
                print("add launch config: " + new_configuration["name"])



    lldb =  gen_lldb_configurations(target_base_name, target_dir)
    add_or_replace_configurations(json_data["configurations"], lldb)

    gdb = gen_gdb_configurations(configs)
    add_or_replace_configurations(json_data["configurations"], gdb)



def update_task_profile(target_name):
    json_data = json.load(open(VSCODE_TASK_JSON_FILE))

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

    json_data["tasks"] = json_data["tasks"] or []

    for t in json_data["tasks"]:
        if t["label"] == task["label"]:
            t = task
            print(f"replace task: {task['label']}")
            break
    else:
        json_data["tasks"].append(task)
        print(f"add task: {task['label']}")


    json.dump(json_data, open(VSCODE_TASK_JSON_FILE, "w"), indent=2)
  

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: vscode.py <target_name> <target_dir> <target_base_name>")
        sys.exit(1)

    for arg in sys.argv:
        print(arg)

    target_name = sys.argv[0]
    target_dir = sys.argv[1]
    target_base_name = sys.argv[2]

    update_task_profile(target_name)
