#!/bin/bash

declare -i seed current_x current_y color_code

handle_sigint() {
    clear
    exit 1
}

initialize_application() {
    echo -ne "\e[?25l"
    
    trap 'echo -ne "\e[?25h"' EXIT
    trap handle_sigint SIGINT

    local interval=${1:-1}
    if ! [[ "$interval" =~ ^[0-9]+$ ]] || (( interval < 1 )); then
        echo "Error: Interval must be a positive integer."
        exit 1
    fi

    seed=$(date +%s)
}


initialize_rng() {
    local a=16807
    local m=2147483647
    
    seed=$(( (a * seed) % m ))
}


generate_new_position() {
    local term_lines=$(tput lines)
    local term_cols=$(tput cols)
    
    max_x=$(( term_lines - 1 ))
    (( max_x < 0 )) && max_x=0
    
    max_y=$(( term_cols - 8 ))
    (( max_y < 0 )) && max_y=0

    initialize_rng
    current_x=$(( seed % (max_x + 1) ))

    initialize_rng
    current_y=$(( seed % (max_y + 1) ))
}


generate_new_color() {
    initialize_rng
    color_code=$(( 31 + (seed % 7) ))
}


validate_positions() {
    local term_lines=$(tput lines)
    local term_cols=$(tput cols)

    (( current_x > term_lines - 1 )) && current_x=$(( term_lines - 1 ))
    (( current_x < 0 )) && current_x=0

    (( current_y > term_cols - 8 )) && current_y=$(( term_cols - 8 ))
    (( current_y < 0 )) && current_y=0
}


print_clock() {
    clear
    tput cup "$current_x" "$current_y"
    echo -ne "\e[${color_code}m$(date +%T)\e[0m"
}


main_loop() {
    local interval=$1
    ((interval == 0)) && interval=1
    
    local counter=0

    while true; do
        if (( counter == 0 )); then
            generate_new_position
            generate_new_color
            validate_positions
        fi

        print_clock
        sleep 1

        counter=$(( (counter + 1) % interval ))
    done
}

initialize_application "$@"
main_loop "$1"