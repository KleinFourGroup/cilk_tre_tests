
OPTS=(0 1 2 3)
WORK=(1 2 5 10)

for i in ${OPTS[*]}; do
    echo g++ -fcilkplus -std=c++11 merge.cc -o mergeO${i} -O${i}
    g++ -fcilkplus -std=c++11 merge.cc -o mergeO${i} -O${i}
    for n in ${WORK[*]}; do
	echo CILK_NWORKERS=${n} ./mergeO${i}
	CILK_NWORKERS=${n} ./mergeO${i}
    done
done

