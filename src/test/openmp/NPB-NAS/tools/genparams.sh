BENCHS=(BT CG EP FT IS LU MG SP)

cd ..
if [ $# -gt 2 ]
then	
	if [ $1 -eq 1 ]
	then
		cd $2
		echo Generating $2 CLASS=$3
		./../tools/setparams $2 $3
	else
		for B in ${BENCHS[@]}
		do
			if [ $B  != $2 ]
			then
		       		cd $B
       			 	echo Generating $B CLASS=$3
       				 ./../tools/setparams $B $3
        		 	cd ..
			fi
		done
	fi
else 
	if [ $# -eq 2 ]
	then
		for B in ${BENCHS[@]}
		do
			cd $B
			echo Generating $B CLASS=$1
			./../tools/setparams $B $1
			cd ..
		done
	else
		echo USAGE 1: ./geneprams.sh [CLASS]
		echo -e "	e.g.  ./geneparms.sh S"
		echo -e " 		Generating All benchmarks, CLASS=S"
		echo USAGE 2: ./geneparams.sh 1 [BENCH] [CLASS]
		echo -e " 	e.g.  ./geneparams.sh bt S"
		echo -e	"	Generating BT bech, CLASS=S"
		echo USAGE 1: ./genparams.sh 0 [BENCH] [CLASS]
		echo -e "	e.g ./genparams.sh 0 bt S"
		echo -e " 	Excluding BT, all other CLASS=S"
	fi

fi

