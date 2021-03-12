#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
void gerador_de_numeros (int a[ ],int tamanho) //função para gerar valores aleatórios no vetor
{
 int i;
 struct timeval time;
 gettimeofday(&time, (struct timezone *) 0);
 srand((int) time.tv_sec);
 for (i=0;i<tamanho;++i)
 a[i] = rand();
}
int maximo(int a[ ],int tamanho) //função para achar valor maximo
{
 int i;
 int max; /* Current max */
 max = a[0];
 for (i=1;i<tamanho;++i)
 if (a[i] > max) max = a[i];
 return max;
}
int main (int argc, char *argv[])
{
 int tamanhoVetor; //Tamanho do vetor principal
 int tamanhoSub; //Tamanho do subvetor
 int nProcessos; //Numero total de processos
 int atualMax; //Maximo do subvetor
 int localMax; //Maximo do processo
 int principalMax; //Maximo do vetor principal
 int atualRank;
 int rank;
 int sequencia;
 int quociente; //tamanhoVetor/nProcessos
 int LSub; // tamanhoVetor % nProcessos
 int inicioSub;
 int buscaVetor[1000];
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD,&atualRank);
 if (atualRank == 0) {
 sequencia = atoi(argv[1]);
 tamanhoVetor = atoi(argv[2]);
 MPI_Comm_size(MPI_COMM_WORLD, &nProcessos);
 quociente = tamanhoVetor / nProcessos;
 LSub = tamanhoVetor % nProcessos; //Numero de processos que precisam de um elemento adicional
 gerador_de_numeros(buscaVetor,tamanhoVetor);
 if (sequencia)
 printf("A busca sequencial retornou %d\n",
 maximo(buscaVetor,tamanhoVetor));
 for (rank=1; rank < LSub; ++rank){
 tamanhoSub = quociente+1;
 inicioSub = rank*quociente+rank;
 MPI_Send(&tamanhoSub,1,MPI_INT,rank,0,MPI_COMM_WORLD);
 MPI_Send(&(buscaVetor[inicioSub]),tamanhoSub, MPI_INT,
 rank, 0, MPI_COMM_WORLD);
 }
 for (rank=LSub; rank < nProcessos; ++rank){
 tamanhoSub = quociente;
 inicioSub = rank*quociente+LSub;
 MPI_Send(&tamanhoSub,1,MPI_INT,rank,0,MPI_COMM_WORLD);
 MPI_Send(&(buscaVetor[rank*quociente+LSub]),quociente,
 MPI_INT, rank, 0, MPI_COMM_WORLD);
}
 if (LSub==0)
 tamanhoSub=quociente;
 else tamanhoSub=quociente+1;
 principalMax = maximo(buscaVetor,quociente+1);
 for (rank=1;rank<nProcessos;++rank){
 MPI_Recv(&localMax,1,MPI_INT,MPI_ANY_SOURCE,0,
 MPI_COMM_WORLD, &status);
 if (localMax > principalMax)
 global_max = localMax;
 }
 printf("A busca paralela retornou %d\n", global_max);
 }
 else {
 MPI_Recv(&tamanhoSub,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
 MPI_Recv(buscaVetor,tamanhoSub,MPI_INT,0,0,MPI_COMM_WORLD,
&status);
 atualMax = maximo(buscaVetor,tamanhoSub);
 MPI_Send(&atualMax,1,MPI_INT,0,0,MPI_COMM_WORLD);
 }
 MPI_Finalize();
return 0;
}
