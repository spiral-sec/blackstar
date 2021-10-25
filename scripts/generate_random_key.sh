#!/bin/env bash

get_random_char() { echo ${1:RANDOM%${#1}:1}; }

{
    UPPERCASE="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    LOWERCASE="abcdefghjiklmnopqrstuvwxyz"
    NUMBERS="0123456789"

    get_random_char "!@#$%^&*?/;:\|+=-_"
    get_random_char $NUMBERS
    get_random_char $LOWERCASE
    get_random_char $UPPERCASE
    for i in $( seq $(( 128 + RANDOM % 8 )) )
    do
        get_random_char "$NUMBERS$UPPERCASE$LOWERCASE"
    done
} | awk '{printf "%s", $1}'
echo ""

