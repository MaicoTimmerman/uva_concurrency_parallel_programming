echo "run 100000 10000"
echo "1 threads"
prun -np 1 ./assign1_1.out 100000 10000 1
echo "2 threads"
prun -np 1 ./assign1_1.out 100000 10000 2
echo "4 threads"
prun -np 1 ./assign1_1.out 100000 10000 4
echo "8 threads"
prun -np 1 ./assign1_1.out 100000 10000 8

echo "run 1000000 10000"
echo "1 threads"
prun -np 1 ./assign1_1.out 1000000 10000 1
echo "2 threads"
prun -np 1 ./assign1_1.out 1000000 10000 2
echo "4 threads"
prun -np 1 ./assign1_1.out 1000000 10000 4
echo "8 threads"
prun -np 1 ./assign1_1.out 1000000 10000 8

echo "run 10000000 10000"
echo "1 threads"
prun -np 1 ./assign1_1.out 10000000 10000 1
echo "2 threads"
prun -np 1 ./assign1_1.out 10000000 10000 2
echo "4 threads"
prun -np 1 ./assign1_1.out 10000000 10000 4
echo "8 threads"
prun -np 1 ./assign1_1.out 10000000 10000 8
