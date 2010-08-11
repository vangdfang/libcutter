for i in `find . -name '*.c' -or -name '*.h' -or -name '*.cpp' -or -name '*.hpp'`; do
    bcpp -s -f 2 -i 4 -bnl -fi $i -fo bcpp.tmp
    mv bcpp.tmp $i
done
