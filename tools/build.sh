#!/bin/bash

FILE_EXT=("*.c")

get_files() {
    local files=()
    for ext in $FILE_EXT; do
        local result=$(find "$1" -type f -name $ext)
        files+="$result"
    done
    echo "$files"
}

get_resolved_path() {
    local path=$(readlink $1)
    echo "$path"
}

include_linked() {
    local array=()
    for path in "$@"; do
        local result="-I./$path "
        local resolved_path=$(get_resolved_path $path)
        local src=$(get_files "$resolved_path" $FILE_EXT)
        result+=$src
        array+="$result"
    done
    local result="${array[*]}"
    echo "$result"
}

CC="clang"
NAME="main"
#INCLUDE=$(include_linked "../src/ctools")
SRC=$(get_files "../src" $FILE_EXT)
C_FLAGS=""

COMPILE="$CC $C_FLAGS $INCLUDE $SRC -o $NAME"
$COMPILE
EXECUTE="./$NAME"
time $EXECUTE $@
