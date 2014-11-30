run:
	./compile.sh
	rm -rf out/
	java -jar nl.uva.AssignmentMapreduce.jar ../tweets2009-06-brg1of2.txt out 1

sort: run
	sort -n -k 2 out/*
