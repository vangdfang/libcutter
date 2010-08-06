for i in *.c *.h *.cpp *.hpp; do
    bcpp -s -f 2 -i 4 -bnl -fi $i -fo bcpp.tmp
    mv bcpp.tmp $i
done
