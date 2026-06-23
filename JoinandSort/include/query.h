#ifndef QUERY_H
#define QUERY_H

void criarArvoreB(char *binName, char *btreeName);
void busca(char *binName, char *btreeName, int N);
void remocao(char *binName, char *btreeName, int N);
void insercao(char *binName, char *btreeName, int N);
void nestedLoopJoin(char *binName, char *nomeCampo1, char *binName2, char *nomeCampo2);
void singleLoopJoin(char *binName1, char *nomeCampo1, char *binName2, char *nomeCampo2, char *btreeName);
void orderBy(char *binEntrada, char *campoOrd, char *binSaida);

#endif
