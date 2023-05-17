#!/bin/bash

function F2()
{
    sleep 5
    local  retval='Completed'
    echo "$retval"
}

osascript -e 'tell app "Terminal" to do script "F2"'

getval=$(F2)  
echo $getval



