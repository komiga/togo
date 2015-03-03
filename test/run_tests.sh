
#!/bin/bash

declare -A exclusions
exclusions["./algorithm/sort.elf"]=true
exclusions["./app/general.elf"]=true
exclusions["./gfx/display.elf"]=true
exclusions["./gfx/renderer_triangle.elf"]=true
exclusions["./gfx/renderer_pipeline.elf"]=true

declare -A arguments
arguments["./utility/args.elf"]="-a --b=1234 --c=\"goats\" cmd -d a1 a2 \"a3\" 1 false true"

runlist=()
while read f
do
	if [[ ! ${exclusions["$f"]} ]]; then
		runlist+=("$f")
	else
		echo "excluded '$f'"
	fi
done < <(find . -maxdepth 2 -type f -iname "*.elf")

runlist_sorted=()
readarray -t runlist_sorted < <(for v in "${runlist[@]}"; do echo "$v"; done | sort)

for f in "${runlist_sorted[@]}"
do
	echo
	echo "running '$f'"
	eval "$f" ${arguments["$f"]}
	exit_code=$?
	if [[ $exit_code != 0 ]]; then
		echo "'$f' failed with exit code $exit_code"
		exit
	fi
done
