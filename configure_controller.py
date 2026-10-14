#!/usr/bin/env python3
import argparse
import json
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--controller", choices=["flight", "hand"], required=True)
parser.add_argument("--input", type=Path, required=True)
parser.add_argument("--output", type=Path)
args = parser.parse_args()

with args.input.open() as file:
    config = json.load(file)

value = {"flight": 0, "hand": 1}[args.controller]
config.setdefault("config", {})["AIRBIT_CONTROLLER"] = value

destination = args.output or args.input
with destination.open("w") as file:
    json.dump(config, file, indent=4)
    file.write("\n")

print()
print(f"\033[95m    SELECTED CONTROLLER: {args.controller} (AIRBIT_CONTROLLER={value})\033[0m")
print()