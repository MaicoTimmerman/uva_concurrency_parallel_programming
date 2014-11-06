rm pngs/*.png
for i in {1..500};
do
    echo "$i"
    ./assign1_1.out 1000 $i 8
    gnuplot plot.gnp
    mv plot.png pngs/plot"$i".png
done
