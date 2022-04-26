#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>



int mov_random(int *array, int size, int rank , int pasos){
    int cont = 0;
    int posicion = rand() %size;
    int mov;
    for ( int i=0 ; i<pasos; i++){
        mov = rand() %2;
        if (mov == 0 && (posicion + 1)< size){
            posicion +=1;
            printf("Perro %d se movio a la derecha\n",rank);
            if (array[posicion] == 1){
                array[posicion]= 0;
                cont +=1;
                printf("Perro %d encontro una oveja\n",rank);
            }
        }
        else if (mov == 1 && (posicion - 1)>0){
            posicion -=1;
            printf("Perro %d se movio a la Izquierda\n",rank);
            if (array[posicion] == 1){
                array[posicion]= 0;
                cont +=1;
                printf("Perro %d encontro una oveja\n",rank);
            }
        }
    }
    return cont;   
    }

int mat(int *array, int size){
    int a=0;
    for(int i=0; i < size; ++i){
        a +=array[i];
    }
    return a;
}
void matriz (int N , int *parcela){
    for(int i = 0 ; i<(N*N) ; i++){
        if((i+1) % (N) == 0 ){
            printf("%d \n",parcela[i]);
            }
        else{
            printf("%d ",parcela[i]);
        }
    }
}

int main(int argc, char const *argv[]){
    srand(time(NULL));
    if(argc < 3){
        printf("Ejecutar como ./prog N P\n");
    }
    int process_size, process_rank, N = atoi(argv[1]), P = atoi(argv[2]), root = 0 , i, j;
    
    int total = 0;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &process_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    int tamao_parcela_p = N*N / process_size;
    int *parcela = NULL;
    if(process_rank == root){
        parcela = (int*)malloc((N*N)*sizeof(int));
        printf("Parcela: \n");
        for(i = 0; i<N*N ; i++){
            parcela[i] = rand() % 2;
            if(parcela[i] == 1){
                total++;
            }
        }
        matriz(N,parcela);
        printf("\n");

    }
    int *sub_parcela = (int*)malloc((tamao_parcela_p)*sizeof(int));
    MPI_Scatter(parcela, tamao_parcela_p, MPI_INT, sub_parcela, tamao_parcela_p, MPI_INT, root, MPI_COMM_WORLD);
    int cantidad_ovejas = mov_random(sub_parcela,tamao_parcela_p,process_rank,P);
    int *resultado_parcial = NULL;
    if(process_rank == root){
        resultado_parcial = (int*)malloc(process_size*sizeof(int));
    }    
    MPI_Gather(&cantidad_ovejas, 1, MPI_INT, resultado_parcial, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (process_rank == root){
        int resultado = mat(resultado_parcial, process_size);
        printf("El Resultado es %d \n", resultado);
        if(resultado == total){
            printf("EXITO TOTAL");
        }
        else if(resultado < total && resultado > 0){
            printf("EXITO PARCIAL");
        }
        else if(resultado=0){
            printf("FRACASO");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

}