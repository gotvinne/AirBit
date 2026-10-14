#!/usr/bin/env python3
import argparse
import json
import sys
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--controller", choices=["flight", "hand"], required=True)
parser.add_argument("--codalpath", type=Path, required=True)
args = parser.parse_args()

previous_value = None
with args.codalpath.open() as file:
    config = json.load(file)
previous_value = config.get("config", {}).get("AIRBIT_CONTROLLER")

value = {"flight": 0, "hand": 1}[args.controller]
config.setdefault("config", {})["AIRBIT_CONTROLLER"] = value

with args.codalpath.open("w") as file:
    json.dump(config, file, indent=4)
    file.write("\n")

print(file=sys.stderr)
print(
    f"\033[95m    SELECTED CONTROLLER: {args.controller} "
    f"(AIRBIT_CONTROLLER={value})\033[0m",
    file=sys.stderr,
)
print(file=sys.stderr)

print("true" if previous_value != value else "false")