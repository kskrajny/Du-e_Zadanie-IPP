#!/bin/bash

if [[ $# < 2 ]]
then
exit 1
fi
if ! [ -f "$1" ]
then
	exit 1
fi
for i in $@
do
	if ! [[ $1 = $i ]]
	then
		if ! [[ "$i" =~ ^[0-9]+$ ]]
		then
			exit 1
		fi
		if [[ (("$i" > 999)) || (("$i" < 1)) ]]
		then
			exit 1
		fi
		while read x
		do
			if [[ ${x%%;*} = $i ]]
			then
				s=0
				x=${x#*;}
				while [[ $x = *';'* ]]
				do
					x=${x#*;}
					s=$(($s + ${x%%;*}))
					x=${x#*;}
					x=${x#*;}
				done
				echo "$i;$s"
			fi
		done < $1
	fi
done

	
