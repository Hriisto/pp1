all:
    mpicc sum.c -o prog

run:
    mpirun -np ${1} --hostfile myhostfile ./prog ${2} ${3}