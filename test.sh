#!/bin/bash

RED='\033[31m'
GREEN='\033[32m'
RESET='\033[0m'

RES_DIR="resources"
SUCCESS_HINT_STRING="....WOODY...."

make
if [ $? -ne 0 ]; then
    echo -e "${RED}[!] Make failed${RESET}"
    exit 1
fi

echo -e "${GREEN}[+] Build OK, starting tests...${RESET}\n"

for bin in "$RES_DIR"/*; do
    [ -f "$bin" ] || continue

    name=$(basename "$bin")

    for mode in s p; do
		rm -rf woody

        echo -e "Testing ${name} with mode -i ${mode} ..."

        ./woody_woodpacker "$bin" -i "$mode" > /dev/null 2>&1

		output=$(timeout 2s ./woody 2>&1)

        if echo "$output" | grep -q "$SUCCESS_HINT_STRING"; then
            echo -e "${GREEN}[PASS] $name (-i $mode)${RESET}"
        else
            echo -e "${RED}[FAIL] $name (-i $mode)${RESET}"
        fi

        echo ""
    done
done
