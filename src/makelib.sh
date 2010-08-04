#!/bin/sh

# Enter a subdir and make
# using lib.txt and flags in arguments.
#
# obj if=cond
# compile obj.o if cond (conjunction)
# cond -> flag
# cond -> flag "," cond

if ! [ -d "$1" ]
then
    if [ -n "$1" ]
    then echo "not a directory: $1"
    fi
    echo "$0 dir [flag]..."
    exit 1
fi

if [ -z "$MAKE" ]
then MAKE=make
fi
if [ -n "$MAKEFLAGS" ]
then MFLAGS="-$MAKEFLAGS"
fi

OBJS=

# :flag1:flag2:...:flagn:
flags=
for x in $@
do flags="$flags:$x"
done
flags=${flags#:}
flags=":${flags#*:}:"

cd $1 || exit 1

if [ -f lib.txt ]
then
    for line in `sed -e '/^[a-z]/!d;s/ /:/g;s/,/ /g' lib.txt`
    do
        cond=${line#*:if=}
        cond=${cond%%:*}
        if [ "$cond" != "$line" ]
        then
            for x in $cond
            do
                if ! echo $flags | grep ":$cond:" >/dev/null
                then cond=
                fi
            done
        fi
        if [ -n "$cond" ]
        then OBJS="$OBJS ${line%%:*}.o"
        fi
    done
fi

OBJS=${OBJS# } $MAKE $MFLAGS
cd ..
