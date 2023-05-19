#!/bin/bash
# Rohan Kallur
# I pledge my honor that I have abided by the Stevens Honor System.
# :)

readonly junk=~/.junk

usage() {
    cat << usage
Usage: $(basename "$0") [-hlp] [list of files]
    -h: Display help.
    -l: List junked files.
    -p: Purge all files.
    [list of files] with no other arguments to junk those files.
usage
}

num_args=0
h_arg=0
l_arg=0
p_arg=0

while getopts ":hlp" option; do
    case ${option} in
    h)
        ((num_args=num_args+1))
        ((h_arg=h_arg+1))
        ;;
    l)
        ((num_args=num_args+1))
        ((l_arg=l_arg+1))
        ;;
    p)
        ((num_args=num_args+1))
        ((p_arg=p_arg+1))
        ;;
    *)
        echo "Error: Unknown option '-$OPTARG'." >&2
        usage
        exit 1
        ;;
    esac
done

if [ $num_args -gt 1 ]; then
    if [ $h_arg -gt 0 ] && [ $l_arg -gt 0 ]; then
        echo "Error: Too many options enabled." >&2
        usage
        exit 1
    elif [ $h_arg -gt 0 ] && [ $p_arg -gt 0 ]; then
        echo "Error: Too many options enabled." >&2
        usage
        exit 1
    elif [ $l_arg -gt 0 ] && [ $p_arg -gt 0 ]; then
        echo "Error: Too many options enabled." >&2
        usage
        exit 1
    fi
fi

files=()
i=0
for file in $@; do 
    files[$i]="$file"
    ((i=i+1))
done

if [ $num_args -eq 0 ] && [ $i -eq 0 ]; then
    usage
    exit 0
fi

if [ $i -gt 1 ]; then
    if [ $h_arg -eq 1 ] || [ $l_arg -eq 1 ] || [ $p_arg -eq 1 ]; then
        echo "Error: Too many options enabled." >&2
        usage
        exit 1
    fi
fi

if [ ! -d $junk ]; then
    mkdir $junk
fi

if [ $h_arg -ne 0 ]; then
    usage
    exit 0

elif [ $l_arg -ne 0 ]; then
    ls -lAF $junk
    exit 0

elif [ $p_arg -ne 0 ]; then
    rm -r $junk
    mkdir $junk
    exit 0

else
    for file in ${files[@]}; do
        if [ -d $file ] || [ -f $file ]; then
            mv $file $junk
        else
            echo "Warning: '$file' not found."
        fi
    done
fi