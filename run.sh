#!/bin/bash
time_out=1
for file in input/*.clq; do
	printf "$file \n"
	for k in 2 3; do
	
		printf "\nk = $k \n"
	
		tempo_serial=$(./SERIAL $time_out $k $file | tail -n 1)
		printf "\n"
		printf "Threads\tLoad\tStack\t\n"
		for nt in 2 4 8 16; do
			printf "$(echo "$nt \t")"
			tempo_load=$(./LOAD $time_out $nt $k $file | tail -n 1)
			tempo_stack=$(./STACK $time_out $nt $k $file | tail -n 1)
			speedup_load=$(echo "scale = 4; $tempo_serial / ($tempo_load )" | bc -l)
			speedup_stack=$(echo "scale = 4; $tempo_serial / ($tempo_stack )" | bc -l)

			printf "$speedup_load\t$speedup_stack\n"
		done
	done

	printf "\n"
done

