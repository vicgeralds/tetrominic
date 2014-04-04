#!/bin/sh

echo 'CCFLAGS = $(CFLAGS) $(CPPFLAGS)'

if [ -f lib.txt ]
then
    echo 'CONFIG_PREFIX = ..'
    echo

    name=`sed '1!d;s/^\[//;s/\]$//' lib.txt`
    echo "$name.a: "'$(OBJS)'
    echo -e "\t\$(AR) rs $name.a "'$(OBJS)'
    echo

    for line in `sed -e '/^[a-z]/!d;s/ /:/g;s/,/ /g' lib.txt`
    do
        name=${line%%:*}
        hfiles=`grep '#include "' $name.c | sed 's/#include //;s/"//g'`
        command='$(CC) $(CCFLAGS)'

        if echo $hfiles | grep -q '\<config/'
        then
            hfiles=`echo $hfiles | sed 's:\<config/:$(CONFIG_PREFIX)/&:g'`
            command="$command -I"'$(CONFIG_PREFIX)'
        fi

        for x in terminal textgfx
        do
            if echo $hfiles | grep -q "\<$x[.]h\>"
            then
                if [ ! -f $x.h ] && [ -f ../$x/$x.h ]
                then
                    hfiles=`echo $hfiles | sed "s:\<$x[.]h\>:../$x/&:"`
                    command="$command -I../$x"
                fi
            fi
        done

        cond=${line#*:if=}
        cond=${cond%%:*}
        if [ "$cond" != "$line" ]
        then
            for x in $cond
            do
                case $x in
                xlib) command="$command "'$(XLIB_INC)'
                esac
            done
        fi

        echo $name.o: $name.c $hfiles
        echo -e "\t$command -c $name.c"
    done
fi

echo
echo 'clean:'
echo -e "\trm -f *.o"
echo -e "\trm -f *.a"
echo
echo '.PHONY: clean'
