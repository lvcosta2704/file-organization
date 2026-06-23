#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "RegistrosIO.h"
#include "fornecidas.h"

// iniciarCabecalho
// Inicia o cabecalho com
// os valores zerados e
// retorna ele
Cabecalho iniciarCabecalho() {
  Cabecalho cab;
  cab.status = '0';
  cab.topo = -1;
  cab.proxRRN = 0;
  cab.nroEstacoes = 0;
  cab.nroParesEstacao = 0;

  return cab;
}

// lerCabecalho
// Lê o cabecalho de fileBin usando fread() em
// cada um dos campos e armazena na struct cab
void lerCabecalho(FILE *fileBin, Cabecalho *cab) {
  fseek(fileBin, 0, SEEK_SET); // Garante que lemos o inicio do binario

  // Lê o cabecalho do arquivo e coloca na struct
  fread(&cab->status, sizeof(char), 1, fileBin);
  fread(&cab->topo, sizeof(int), 1, fileBin);
  fread(&cab->proxRRN, sizeof(int), 1, fileBin);
  fread(&cab->nroEstacoes, sizeof(int), 1, fileBin);
  fread(&cab->nroParesEstacao, sizeof(int), 1, fileBin);
}

// escreverCabecalho
// Escreve o cabecalho armazenado em cab
// para o registro de cabecalho de fileBin
// usando fwrite() em cada um dos campos
void escreverCabecalho(FILE *fileBin, Cabecalho cab) {
  fseek(fileBin, 0, SEEK_SET); // Garante que escrevemos no inicio do binario

  // Escreve no arquivo
  fwrite(&cab.status, sizeof(char), 1, fileBin);
  fwrite(&cab.topo, sizeof(int), 1, fileBin);
  fwrite(&cab.proxRRN, sizeof(int), 1, fileBin);
  fwrite(&cab.nroEstacoes, sizeof(int), 1, fileBin);
  fwrite(&cab.nroParesEstacao, sizeof(int), 1, fileBin);
}

// lerRegistro
// recebe: ponteiro para o arquivo binario e ponteiro para o registro
// funcionalidade: lê todos os campos de um registro (exceto o removido) e pula
// o lixo Lê os campos de tamanho fixo
void lerRegistro(FILE *fileBin, Registro *reg) {

  fread(&reg->proximo, sizeof(int), 1, fileBin);
  fread(&reg->codEstacao, sizeof(int), 1, fileBin);
  fread(&reg->codLinha, sizeof(int), 1, fileBin);
  fread(&reg->codProxEstacao, sizeof(int), 1, fileBin);
  fread(&reg->distProxEstacao, sizeof(int), 1, fileBin);
  fread(&reg->codLinhaIntegra, sizeof(int), 1, fileBin);
  fread(&reg->codEstIntegra, sizeof(int), 1, fileBin);

  // Lê os campos de tamanho variável
  fread(&reg->tamNomeEstacao, sizeof(int), 1, fileBin);
  if (reg->tamNomeEstacao > 0) {
    fread(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, fileBin);
    reg->nomeEstacao[reg->tamNomeEstacao] = '\0';
  } else {
    strcpy(reg->nomeEstacao, "");
  }

  fread(&reg->tamNomeLinha, sizeof(int), 1, fileBin);
  if (reg->tamNomeLinha > 0) {
    fread(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, fileBin);
    reg->nomeLinha[reg->tamNomeLinha] = '\0';
  } else {
    strcpy(reg->nomeLinha, "");
  }

  // Pula os bytes de lixo ('$') para alinhar os 80 bytes
  // Obs: são 1 char (removido) + 9 ints (proximo, tamNomes e codigos) = 37
  // bytes fixos
  int bytesLidos = 1 + (9 * 4) + reg->tamNomeEstacao + reg->tamNomeLinha;
  fseek(fileBin, 80 - bytesLidos, SEEK_CUR);
}

// escreverRegistro
// Escreve o registro armazenado em reg
// para algum registro de dados de fileBin
// usando fwrite() em cada um dos campos.
// Preenche o campo com lixo no final
void escreverRegistro(FILE *fileBin, Registro reg) {
  fwrite(&reg.removido, sizeof(char), 1, fileBin);
  fwrite(&reg.proximo, sizeof(int), 1, fileBin);
  fwrite(&reg.codEstacao, sizeof(int), 1, fileBin);
  fwrite(&reg.codLinha, sizeof(int), 1, fileBin);
  fwrite(&reg.codProxEstacao, sizeof(int), 1, fileBin);
  fwrite(&reg.distProxEstacao, sizeof(int), 1, fileBin);
  fwrite(&reg.codLinhaIntegra, sizeof(int), 1, fileBin);
  fwrite(&reg.codEstIntegra, sizeof(int), 1, fileBin);

  // Campos Variáveis
  fwrite(&reg.tamNomeEstacao, sizeof(int), 1, fileBin);
  if (reg.tamNomeEstacao > 0) {
    fwrite(reg.nomeEstacao, sizeof(char), reg.tamNomeEstacao, fileBin);
  }

  fwrite(&reg.tamNomeLinha, sizeof(int), 1, fileBin);
  if (reg.tamNomeLinha > 0) {
    fwrite(reg.nomeLinha, sizeof(char), reg.tamNomeLinha, fileBin);
  }

  // Preenchimento com lixo '$'
  int bytesEscritos = 1 + (9 * 4) + reg.tamNomeEstacao +
                      reg.tamNomeLinha; // 1 char, 9 ints, 2 variaveis

  char lixo = '$';
  for (int i = bytesEscritos; i < 80; i++) {
    fwrite(&lixo, sizeof(char), 1, fileBin);
  }
}

// apagarRegistro
// Apaga o registro de RRN especificado no parametro
// conforme a abordagem dinamica de reutilizacao de
// registros logicamente removidos. Coloca o head da lista
// encadeada no campo "topo" do cabecalho
void apagarRegistro(FILE *fileBin, Registro *reg, Cabecalho *cab, int RRN) {
  // Posiciona o fseek no inicio do registro
  fseek(fileBin, TAM_CABECALHO + RRN * TAM_REGISTRO, SEEK_SET);

  reg->removido = '1';      // Marca o registro como logicamente removido
  reg->proximo = cab->topo; // Marca o valor do próximo RRN removido

  // Escreve no arquivo
  fwrite(&(reg->removido), sizeof(char), 1, fileBin);
  fwrite(&(reg->proximo), sizeof(int), 1, fileBin);

  // Atualiza o topo da stack com o valor
  // do RRN do registro removido
  cab->topo = RRN;
  escreverCabecalho(fileBin, *cab);
}

// inputRegistro()
// Cria um registro pedindo ao usuario cada um
// dos seus campos.
Registro inputRegistro() {
  Registro reg;

  reg.removido = '0';
  reg.proximo = -1;

  // Pede ao usuario os campos padrao
  scanf("%d", &reg.codEstacao);
  ScanQuoteString(reg.nomeEstacao);
  scanf("%d", &reg.codLinha);
  ScanQuoteString(reg.nomeLinha);

  // Coloca o tamanho dos nomes em seus
  // respectivos campos
  reg.tamNomeEstacao = strlen(reg.nomeEstacao);
  reg.tamNomeLinha = strlen(reg.nomeLinha);

  // Pede os campos opcionais
  // verificando se o usuario passou
  // "NULO"
  char digitos[64];
  scanf("%s", digitos);
  if (!strcmp(digitos, "NULO")) {
    reg.codProxEstacao = -1;
  } else {
    reg.codProxEstacao = atoi(digitos);
  }

  scanf("%s", digitos);
  if (!strcmp(digitos, "NULO")) {
    reg.distProxEstacao = -1;
  } else {
    reg.distProxEstacao = atoi(digitos);
  }

  scanf("%s", digitos);
  if (!strcmp(digitos, "NULO")) {
    reg.codLinhaIntegra = -1;
  } else {
    reg.codLinhaIntegra = atoi(digitos);
  }

  scanf("%s", digitos);
  if (!strcmp(digitos, "NULO")) {
    reg.codEstIntegra = -1;
  } else {
    reg.codEstIntegra = atoi(digitos);
  }

  return reg;
}

// resetarFiltro
// Preenche um filtro com os
// campos de inicializacao
Busca resetarFiltro() {
  Busca filtro;
  filtro.codEstacao = -2;
  filtro.codLinha = -2;
  filtro.codProxEstacao = -2;
  filtro.distProxEstacao = -2;
  filtro.codLinhaIntegra = -2;
  filtro.codEstIntegra = -2;
  strcpy(filtro.nomeEstacao, "");
  strcpy(filtro.nomeLinha, "");

  return filtro;
}

// inputFiltro
// Pede ao usuario que passe um
// filtro de busca
Busca inputFiltro() {
  // Inicializa o filtro
  Busca filtro = resetarFiltro();

  int m;
  scanf("%d", &m);

  // Preenche 'm' campos do filtro
  for (int j = 0; j < m; j++) {
    // Pede ao usuario o nome do campo
    char nomeCampo[50];
    scanf("%s", nomeCampo);

    // Preenche o campo especificado
    if (strcmp(nomeCampo, "codEstacao") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.codEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "codLinha") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.codLinha = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.codProxEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.distProxEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.codLinhaIntegra = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
      char valor[50];
      scanf("%s", valor);
      filtro.codEstIntegra = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

    } else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
      ScanQuoteString(filtro.nomeEstacao);

    } else if (strcmp(nomeCampo, "nomeLinha") == 0) {
      ScanQuoteString(filtro.nomeLinha);
    }
  }

  return filtro;
}

// LOGICA DE COMPARAÇÃO (AND): Se o filtro for != -2, o registro deve bater
// Testa se o filtro existe e depois faz um AND para ver se o valor BATE
// IMPORTANTE: Só entra no IF se o filtro existir e estiver ERRADO
// Pois se estiver errado basta descartar e partir para o prox reg
int comparaFiltro(Busca filtro, Registro reg) {
  if (filtro.codEstacao != -2 && reg.codEstacao != filtro.codEstacao)
    return 0;
  if (filtro.codLinha != -2 && reg.codLinha != filtro.codLinha)
    return 0;
  if (filtro.codProxEstacao != -2 &&
      reg.codProxEstacao != filtro.codProxEstacao)
    return 0;
  if (filtro.distProxEstacao != -2 &&
      reg.distProxEstacao != filtro.distProxEstacao)
    return 0;
  if (filtro.codLinhaIntegra != -2 &&
      reg.codLinhaIntegra != filtro.codLinhaIntegra)
    return 0;
  if (filtro.codEstIntegra != -2 && reg.codEstIntegra != filtro.codEstIntegra)
    return 0;
  if (strlen(filtro.nomeEstacao) > 0 &&
      strcmp(reg.nomeEstacao, filtro.nomeEstacao) != 0)
    return 0;
  if (strlen(filtro.nomeLinha) > 0 &&
      strcmp(reg.nomeLinha, filtro.nomeLinha) != 0)
    return 0;

  return 1;
}

// inputAtualizacaoes
// Pede ao usuario os campos
// a serem atualizados e retorna
// uma struct contendo eles
Busca inputAtualizacoes() {
  Busca reg = resetarFiltro();

  int p;
  scanf("%d", &p);

  while (p--) {
    char campo[64];
    scanf("%s", campo);

    if (!strcmp(campo, "codEstacao")) {
      scanf("%d", &reg.codEstacao);
    } else if (!strcmp(campo, "codLinha")) {
      scanf("%d", &reg.codLinha);
    } else if (!strcmp(campo, "codProxEstacao")) {
      scanf("%d", &reg.codProxEstacao);
    } else if (!strcmp(campo, "distProxEstacao")) {
      scanf("%d", &reg.distProxEstacao);
    } else if (!strcmp(campo, "codLinhaIntegra")) {
      scanf("%d", &reg.codLinhaIntegra);
    } else if (!strcmp(campo, "codEstIntegra")) {
      scanf("%d", &reg.codEstIntegra);
    } else if (!strcmp(campo, "nomeEstacao")) {
      ScanQuoteString(reg.nomeEstacao);
    } else if (!strcmp(campo, "nomeLinha")) {
      ScanQuoteString(reg.nomeLinha);
    }
  }

  return reg;
}

// atualizar
// Atualiza um registro com base nos campos nao-nulos
// de uma struct que contem os campos a serem atualizados
void atualizar(Registro *reg, Busca atualizacoes) {
  if (atualizacoes.codEstacao != -2) {
    reg->codEstacao = atualizacoes.codEstacao;
  }
  if (atualizacoes.codLinha != -2) {
    reg->codLinha = atualizacoes.codLinha;
  }
  if (atualizacoes.codProxEstacao != -2) {
    reg->codProxEstacao = atualizacoes.codProxEstacao;
  }
  if (atualizacoes.distProxEstacao != -2) {
    reg->distProxEstacao = atualizacoes.distProxEstacao;
  }
  if (atualizacoes.codLinhaIntegra != -2) {
    reg->codLinhaIntegra = atualizacoes.codLinhaIntegra;
  }
  if (atualizacoes.codEstIntegra != -2) {
    reg->codEstIntegra = atualizacoes.codEstIntegra;
  }
  if (strcmp(atualizacoes.nomeEstacao, "")) {
    strcpy(reg->nomeEstacao, atualizacoes.nomeEstacao);
    reg->tamNomeEstacao = strlen(reg->nomeEstacao);
  }
  if (strcmp(atualizacoes.nomeLinha, "")) {
    strcpy(reg->nomeLinha, atualizacoes.nomeLinha);
    reg->tamNomeLinha = strlen(reg->nomeLinha);
  }
}

// contarEstacoesEPares
// Conta a quantidade de estacoes e de pares de estacao
// e coloca no Cabecalho
void contarEstacoesEPares(FILE *fileBin, Cabecalho *cab) {
  if (fileBin == NULL)
    return;

  int RRN = 0;

  // Reseta a contagem de estacoes e
  // de pares do cabecalho
  cab->nroEstacoes = 0;
  cab->nroParesEstacao = 0;

  char nomesVistos[MAX_REGISTROS][256];
  ParEstacao paresVistos[MAX_REGISTROS];

  // Posiciona o fseek pro primeiro registro
  fseek(fileBin, TAM_CABECALHO, SEEK_SET);

  Registro reg;

  while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1) {
    // Se estiver removido pula para o proximo
    if (reg.removido == '1') {
      fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
      RRN++; // Atualiza o valor do RRN
      continue;
    }

    lerRegistro(fileBin, &reg);

    // --- LOGICA DE CONTAGEM ---
    // Checa se a estação já foi vista pelo NOME 
    int estacaoRepetida = 0;
    for (int i = 0; i < cab->nroEstacoes; i++) {
      if (strcmp(nomesVistos[i], reg.nomeEstacao) == 0) {
        estacaoRepetida = 1;
        break;
      }
    }
    if (!estacaoRepetida) {
      strcpy(nomesVistos[cab->nroEstacoes], reg.nomeEstacao);
      cab->nroEstacoes++;
    }

    // Checa se o par já foi visto
    if (reg.codProxEstacao != -1) {
      int parRepetido = 0;
      for (int i = 0; i < cab->nroParesEstacao; i++) {
        if (paresVistos[i].origem == reg.codEstacao &&
            paresVistos[i].destino == reg.codProxEstacao) {
          parRepetido = 1;
          break;
        }
      }
      if (!parRepetido) {
        paresVistos[cab->nroParesEstacao].origem = reg.codEstacao;
        paresVistos[cab->nroParesEstacao].destino = reg.codProxEstacao;
        cab->nroParesEstacao++;
      }
    }

    RRN++;
    fseek(fileBin, TAM_CABECALHO + RRN * TAM_REGISTRO, SEEK_SET);
  }
}
