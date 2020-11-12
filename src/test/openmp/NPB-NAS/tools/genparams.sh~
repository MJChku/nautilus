BENCHS=(BT CG EP FT IS LU MG SP)

if [ $# -gt 2 ]
then	
	if [ $1 -eq 1 ]
	then
		cd $2
		echo Generating $2 CLASS=$3
		./../setparams $2 $3
	else
		for B in ${BENCHS[@]}
		do
			if [ $B  != $2 ]
			then
		       		cd $B
       			 	echo Generating $B CLASS=$3
       				 ./../setparams $B $3
        		 	cd ..
			fi
		done
	fi
else
	for B in ${BENCHS[@]}
	do
		cd $B
		echo Generating $B CLASS=$1
		./../setparams $B $1
		cd ..
	done
fi

