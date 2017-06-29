#!/bin/bash
time_out=3600
for file in input/*.clq; do
	printf "$file \n"
	for k in 2 3; do
	
		printf "\nk = $k \n"

		saida_serial=$(./SERIAL $time_out $k $file)
		tempo_serial=$(echo "$saida_serial" | tail -n 1)
		echo "$saida_serial" > $file-serial.out

		printf "\n"
		printf "Threads\tLoad\tStack\t\n"
		for nt in 2 4 8 16; do
			printf "$(echo "$nt \t")"
			saida_load=$(./LOAD $time_out $nt $k $file)
			tempo_load=$(echo "$saida_load" | tail -n 1)

			echo "$saida_load" > $file-load.out

			saida_stack=$(./STACK $time_out $nt $k $file)
			tempo_stack=$(echo "$saida_stack" | tail -n 1)
			echo "$saida_stack" > $file-stack.out

			speedup_load=$(echo "scale = 4; $tempo_serial / ($tempo_load )" | bc -l)
			speedup_stack=$(echo "scale = 4; $tempo_serial / ($tempo_stack )" | bc -l)

			printf "$speedup_load\t$speedup_stack\n"
		done
	done

	printf "\n"
done

