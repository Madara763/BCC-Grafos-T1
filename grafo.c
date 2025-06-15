/*
  Implementacao de funcoes para manipulacao de grafos. 
  Criado por: Mardoqueu Nunes e Davi Lazzarin
  Data: 14/06/25
*/

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "grafo.h"

#define MAX_VERTICES 1024

//------------------------------------------------------------------------------
// Funcoes internas (declaracoes aqui devido as flags de compilacao usadas no makefile do trabalho)

// busca auxiliar para n_componentes
void dfs_n_comp(unsigned int v, int *visitado, grafo *g);

// Compara strings, usado em vertices corte
int cmp_nome(const void *a, const void *b);

// Funcao auxiliar para obter nome do vertice pelo id
char *nome_vertice_por_id(grafo *g, unsigned int id);

// bsuca auxilair para vertices de corte
void dfs_vertices_corte(grafo *g, int v, int pai, int *visitado, int *pre_ordem, int *low_point, int *eh_corte, int *timer);

// busca em profundidade para arestas de corte
void dfs_arestas_corte(grafo *g, int v, int pai, int *visitado, int *pre_ordem, int *low_point, int *timer, char **arestas, unsigned int *count);
 
//------------------------------------------------------------------------------
// lê um grafo de f e o devolve
grafo *le_grafo(FILE *f) {
  char linha[MAX_LINHA];
  grafo *g = malloc(sizeof(grafo));
  g->vertices = NULL;
  g->arestas = NULL;
  g->na = 0;
  g->nv = 0;

  // Nome do grafo
  while (fgets(linha, MAX_LINHA, f)) {
    if (strncmp(linha, "//", 2) == 0 || linha[0] == '\n')
      continue;
    linha[strcspn(linha, "\r\n")] = 0;
    g->nome = strdup(linha);
    break;
  }

  // Restante
  while (fgets(linha, MAX_LINHA, f)) {
    if (strncmp(linha, "//", 2) == 0 || linha[0] == '\n')
      continue;

    linha[strcspn(linha, "\r\n")] = 0;

    char *sep = strstr(linha, "--");
    if (sep) {
      char nome1[256], nome2[256];
      int peso = 0;
      if (sscanf(linha, " %255s -- %255s %d", nome1, nome2, &peso) >= 2) {
        Vertice *v1 = busca_ou_adiciona_vertice(g, nome1);
        Vertice *v2 = busca_ou_adiciona_vertice(g, nome2);
        adiciona_aresta(g, v1, v2, peso);
      }
    } else {
      busca_ou_adiciona_vertice(g, linha);
    }
  }

  //Cria um matriz de adjacencia nv x nv
  //Aloca vetor de pronteiros
  g->matriz_adj = malloc(sizeof(int *) * g->nv+1);
  if(!g->matriz_adj){perror("Erro ao alocar matriz de adjacencia\n"); exit(-1);}

  //Aloca vetor de inteiros
  for(unsigned int i=0; i<g->nv; i++){
    g->matriz_adj[i] = calloc(g->nv+1, sizeof(int));
    if(!g->matriz_adj[i]){perror("Erro ao alocar matriz de adjacencia\n"); exit(-1);}
  }
  
  //Preenche a matriz (cria ela espelhada)
  for (Aresta *a = g->arestas; a != NULL; a = a->prox){
    //Quando uma aresta nao tem peso, insere o valor 1 na matriz para indicar que existe uma aresta ali
    if(!a->peso){ 
      g->matriz_adj[a->v1->id][a->v2->id] = 1;
      g->matriz_adj[a->v2->id][a->v1->id] = 1;  
    }
    else{
      g->matriz_adj[a->v1->id][a->v2->id] = a->peso;
      g->matriz_adj[a->v2->id][a->v1->id] = a->peso;
    }
  }
  return g;
}

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo(grafo *g){
    
  //free nome do grafo
  free(g->nome);
  
  //Free vertices e nomes
  Vertice *aux, *v = g->vertices;
  while(v){
    aux = v->prox;
    free(v->nome);
    free(v);
    v = aux;
  }
  
  //Free arestas
  Aresta *bkp, *a=g->arestas;
  while(a){
    bkp = a->prox;
    free(a);
    a = bkp;
  }

  //Free vetor de inteiros
  for(unsigned int i=0; i<g->nv; i++)
    free(g->matriz_adj[i]);

  //Free Matriz adj
  free(g->matriz_adj);

  //Free grafo
  free(g);
  return 1;
}

//------------------------------------------------------------------------------
// devolve o nome de g
char *nome(grafo *g){
  return g->nome;
}

//------------------------------------------------------------------------------
// devolve o número de vértices em g

unsigned int n_vertices(grafo *g){
  if(g)
    return g->nv;
  return 0;
}

//------------------------------------------------------------------------------
// devolve 1 se g é bipartido e 0 caso contrário
unsigned int bipartido(grafo *g) {

    // Fila de vértices para BFS
    Vertice *fila[MAX_VERTICES];
    int frente = 0, tras = 0;   //tras onde insere - frente de onde removo

    // Zera a cor de todos os vértices
    // for (Vertice *v = g->vertices; v != NULL; v = v->prox) {
    //     v->cor = 0;
    // }

    // Percorre todos os vértices (para cobrir grafos desconexos)
    for (Vertice *v = g->vertices; v != NULL; v = v->prox) {
        if (v->cor == 0) {
            // Inicia a coloração a partir desse vértice
            v->cor = 1;
            fila[tras++] = v;

            // BFS
            while (frente < tras) {
                Vertice *atual = fila[frente++];

                // Percorre todas as arestas para achar vizinhos do vértice atual
                for (Aresta *a = g->arestas; a != NULL; a = a->prox) {
                    Vertice *vizinho = NULL;

                    if (a->v1 == atual) {
                        vizinho = a->v2;
                    } else if (a->v2 == atual) {
                        vizinho = a->v1;
                    }

                    if (vizinho != NULL) {
                        if (vizinho->cor == 0) {
                            // Pinta o vizinho com cor oposta
                            vizinho->cor = (atual->cor == 1) ? 2 : 1;
                            fila[tras++] = vizinho;
                        } else if (vizinho->cor == atual->cor) {
                            // Conflito de cores => não bipartido
                            return 0;
                        }
                    }
                }
            }
        }
    }

    return 1;
}

//------------------------------------------------------------------------------
// devolve o número de arestas em g

unsigned int n_arestas(grafo *g){
  if(g)
    return g->na;
  return 0;
}

//------------------------------------------------------------------------------
// devolve o número de componentes em g
unsigned int n_componentes(grafo *g){
  if (!g) return 0;

  // Vetor auxiliar, indica se o vertice 
  int *visitado = calloc(g->nv, sizeof(int));
  if (!visitado){ perror("Erro ao alocar vetor de visitados"); exit(1); }

  //Percorre os vertices que ainda nao foram consumidos na dfs
  unsigned int componentes = 0;
  for (unsigned int v = 0; v < g->nv; v++) {
    if (!visitado[v]){ //Cada nova arvore de busca indica um novo componente
      dfs_n_comp(v, visitado, g);
      componentes++;  
    }
  }

  free(visitado);
  return componentes;
}

//------------------------------------------------------------------------------
// devolve uma "string" com os diâmetros dos componentes de g separados por brancos
// em ordem não decrescente


char *diametros(grafo *g) {
  int *visitado = calloc(g->nv, sizeof(int));
  int *dias = malloc(g->nv * sizeof(int));
  int nd = 0;
  for (int v = 0; v < (int)g->nv; v++) {
      if (!visitado[v]) {
          int *componente = malloc(g->nv * sizeof(int));
          int tam = 0;
          dfs_matriz(g, v, visitado, componente, &tam);
          int d = calcular_diametro(g, componente, tam);
          dias[nd++] = d;
      }
  }
  // Ordenar os diâmetros (insertion sort simples)
  for (int i = 0; i < nd - 1; i++) {
      for (int j = i + 1; j < nd; j++) {
          if (dias[i] > dias[j]) {
              int tmp = dias[i];
              dias[i] = dias[j];
              dias[j] = tmp;
          }
      }
  }
  // Criar string de saída
  char *saida = malloc((size_t)(nd * 12));  // espaço suficiente para todos os ints + espaços
  saida[0] = '\0';
  for (int i = 0; i < nd; i++) {
      char buf[12];
      sprintf(buf, "%d", dias[i]);
      strcat(saida, buf);
      if (i < nd - 1) strcat(saida, " ");
  }
  free(visitado);
  free(dias);
  return saida;
}

//------------------------------------------------------------------------------
// devolve uma "string" com os nomes dos vértices de corte de g em
// ordem alfabética, separados por brancos
char *vertices_corte(grafo *g) {
  if (!g) return NULL;

  unsigned int n = g->nv;                       //quant vertices
  int *pre_ordem = calloc(n, sizeof(int));      //tempo q entra no vertice
  int *low_point = calloc(n, sizeof(int));      //lowpoint do vertice na arborescencia
  int *visitado = calloc(n, sizeof(int));       //bitmap se o vertice ja foi consumido na busca
  int *eh_corte = calloc(n, sizeof(int));       //indica se o vertice eh de corte
  int timer = 0;                                //tempo atual na exec 

  // Inicia a dfs
  for (int i = 0; (unsigned int)i < n; i++)
    if (!visitado[i]) dfs_vertices_corte(g, i, -1, visitado, pre_ordem, low_point, eh_corte, &timer);

  // Coleta nomes
  unsigned int count = 0;
  char **nomes = malloc(n * sizeof(char *));
  for(unsigned int i = 0; i < n; i++)
    if(eh_corte[i]) nomes[count++] = nome_vertice_por_id(g, i); //Se eh vertice de corte pega o nome do vertice

  //Ordena lexicograficamente (usa o strcmp) 
  qsort(nomes, count, sizeof(char *), cmp_nome);

  //Tamanho da saida
  size_t tam = 1;
  for (unsigned int i = 0; i < count; i++) tam += strlen(nomes[i]) + 1;

  char *resultado = malloc(tam);
  resultado[0] = '\0';
  //Aloca e monta a string de retorno
  for (unsigned int i = 0; i < count; i++) {
    strcat(resultado, nomes[i]);
    if (i != count - 1) strcat(resultado, " ");
  }

  //Libera a memoria
  free(pre_ordem);
  free(low_point); 
  free(visitado); 
  free(eh_corte); 
  free(nomes);
  return resultado;
}

//------------------------------------------------------------------------------
// devolve uma "string" com as arestas de corte de g em ordem alfabética, separadas por brancos
// cada aresta é o par de nomes de seus vértices em ordem alfabética, separadas por brancos
//
// por exemplo, se as arestas de corte são {z, a}, {x, b} e {y, c}, a resposta será a string
// "a z b x c y"

char *arestas_corte(grafo *g) {
  if (!g) return NULL;

  unsigned int n = g->nv;                         //quant vertices
  int *pre_ordem = calloc(n, sizeof(int));        //pre-ordem do vertice na arvore
  int *low_point = calloc(n, sizeof(int));        //lowpoint do vertice na arborescencia
  int *visitado = calloc(n, sizeof(int));         //bitmap se o vertice ja foi consumido na busca
  char **arestas = malloc(sizeof(char *) * g->na);
  unsigned int count = 0;
  int timer = 0;                                  //tempo atual na exec 

  for (unsigned int i = 0; i < n; i++)
    if (!visitado[i]) dfs_arestas_corte(g,(int) i, -1, visitado, pre_ordem, low_point, &timer, arestas, &count);

  qsort(arestas, count, sizeof(char *), cmp_nome);

  size_t tam = 1;
  for (unsigned int i = 0; i < count; i++)
    tam += strlen(arestas[i]) + 1;

  char *resultado = malloc(tam);
  resultado[0] = '\0';
  for (unsigned int i = 0; i < count; i++) {
    strcat(resultado, arestas[i]);
    if (i != count - 1) strcat(resultado, " ");
    free(arestas[i]);
  }

  //Libera memoria
  free(pre_ordem); 
  free(low_point); 
  free(visitado); 
  free(arestas);
  return resultado;
}

//------------------------------------------------------------------------------
//funções auxiliares

// Compara strings, usado em vertices corte
int cmp_nome(const void *a, const void *b) {
  const char * const *na = a;
  const char * const *nb = b;
  return strcmp(*na, *nb);
}


// Função auxiliar para obter nome do vértice pelo id
char *nome_vertice_por_id(grafo *g, unsigned int id) {
  Vertice *v = g->vertices;
  while (v) {
    if (v->id == id)
      return v->nome;
    v = v->prox;
  }
  return NULL;
}

// Cria um vértice
Vertice *cria_vertice(const char *nome){
    Vertice *v = malloc(sizeof(Vertice));
    v->nome = strdup(nome);
    v->prox = NULL;
    v->cor = 0;
    return v;
}

// busca ou adiciona um vértice no grafo
Vertice *busca_ou_adiciona_vertice(grafo *g, const char *nome) {
  Vertice *v = g->vertices;
  while (v) {
    if (strcmp(v->nome, nome) == 0) //Se ja existe um vert com esse nome, retorna ele
      return v;
    v = v->prox;
  }
  Vertice *novo = cria_vertice(nome); //Se n existe, cria um novo
  if(!novo){perror("Erro de alocacao ao criar vertice.\n"); exit(-1);}
  novo->id = g->nv;
  novo->prox = g->vertices;
  g->vertices = novo;
  g->nv ++;
  return novo;
}

// cria e adiciona uma aresta
void adiciona_aresta(grafo *g, Vertice *v1, Vertice *v2, int peso) {
  Aresta *a = malloc(sizeof(Aresta));
  if(!a){perror("Erro de alocacao ao criar aresta.\n"); exit(-1);}
  a->v1 = v1;
  a->v2 = v2;
  a->peso = peso;
  a->prox = g->arestas;
  g->arestas = a;
  g->na ++;
}

void imprime_grafo(grafo *g) {
  printf("Grafo: %s\n", g->nome);
  printf("Vértices:\n");
  for (Vertice *v = g->vertices; v != NULL; v = v->prox)
    printf("  %s\n", v->nome);

  printf("Arestas:\n");
  for (Aresta *a = g->arestas; a != NULL; a = a->prox)
    printf("  %s -- %s (%d)\n", a->v1->nome, a->v2->nome, a->peso);

  //imprime matriz de adjacencia
  printf("Matriz de adj:\n\n");
  
  printf("   ");
  for(unsigned int i=0; i<g->nv; i++)
    printf("%d ", i);
  
  printf("\n   ");
  for(unsigned int i=0; i<g->nv; i++)
    printf("--");
  
  for(unsigned int i=0; i<g->nv; i++){
    printf("\n%d: ", i);
    for(unsigned int j=0; j<g->nv; j++)
      printf("%d ", g->matriz_adj[i][j]);
    
  }
  printf("\n");
}

//------------------------------------------------------------------------------
// DFS's auxiliares

// busca auxiliar para n_componentes
void dfs_n_comp(unsigned int v, int *visitado, grafo *g) {
  visitado[v] = 1;
  for (unsigned int i = 0; i < g->nv; i++){
    if (g->matriz_adj[v][i] != 0 && !visitado[i])
      dfs_n_comp(i, visitado, g);
  }
}

// bsuca auxilair para vertices de corte
void dfs_vertices_corte(grafo *g, int v, int pai, int *visitado, int *pre_ordem, int *low_point, int *eh_corte, int *timer) {
  visitado[v] = 1;
  pre_ordem[v] = low_point[v] = (*timer)++;
  int filhos = 0;

  for (int u = 0; (unsigned int)u < g->nv; u++) {
    if (g->matriz_adj[v][u] == 0) continue;
    if ((int)u == pai) continue;

    if (visitado[u]) {
      if (pre_ordem[u] < low_point[v]) low_point[v] = pre_ordem[u];
    }
    else {
      dfs_vertices_corte(g, u, v, visitado, pre_ordem, low_point, eh_corte, timer);
      if (low_point[u] < low_point[v]) low_point[v] = low_point[u];
      if (low_point[u] >= pre_ordem[v] && pai != -1) eh_corte[v] = 1;
      filhos++;
    }
  }

  if (pai == -1 && filhos > 1) eh_corte[v] = 1;
}

// busca em profundidade para arestas de corte
void dfs_arestas_corte(grafo *g, int v, int pai, int *visitado, int *pre_ordem, int *low_point, int *timer, char **arestas, unsigned int *count){
  visitado[v] = 1;    
  pre_ordem[v] = low_point[v] = (*timer)++; //Atualiza pre_ordem e lowpoint atual

  for (int u = 0; (unsigned int)u < g->nv; u++) { 
    if (g->matriz_adj[v][u] == 0) continue; // se nao tem aresta pula
    if ((int)u == pai) continue; //aresta com o pai ja foi processada

    if (visitado[u]) { //se tem uma aresta com um vertice q ja foi processado, atualiza o lowpoint
      if (pre_ordem[u] < low_point[v]) low_point[v] = pre_ordem[u];
    } 
    else { //Se a aresta eh com um vertice novo, consome esse novo vertice

      dfs_arestas_corte(g, (int) u, v, visitado, pre_ordem, low_point, timer, arestas, count);
      if (low_point[u] > pre_ordem[v]) {
        // Encontrou aresta de corte v -- u
        char *nome1 = nome_vertice_por_id(g, (unsigned int)v);
        char *nome2 = nome_vertice_por_id(g, (unsigned int)u);
        if (strcmp(nome1, nome2) > 0) {
          char *tmp = nome1;
          nome1 = nome2;
          nome2 = tmp;
        }
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "%s %s", nome1, nome2);
        arestas[(*count)++] = strdup(buffer);
      }
      if (low_point[u] < low_point[v]) low_point[v] = low_point[u];
    }//else
  }//for
}



// Função auxiliar: Dijkstra para calcular distâncias de um vértice origem
void dijkstra(grafo *g, int origem, int *dist) {
    unsigned int nv = g->nv;
    unsigned int *usado = calloc(nv, sizeof(int));
    for (unsigned int i = 0; i < nv; i++) dist[i] = INF;
    dist[origem] = 0;

    for (unsigned int count = 0; count < nv; count++) {
        int u = -1;
        for ( int i = 0; i < (int)nv; i++) {
            if (!usado[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }

        if (dist[u] == INF) break;  // componente desconectado
        usado[u] = 1;

        for (unsigned int v = 0; v < nv; v++) {
            if (g->matriz_adj[u][v] > 0 && !usado[v]) {
                if (dist[u] + g->matriz_adj[u][v] < dist[v]) {
                    dist[v] = dist[u] + g->matriz_adj[u][v];
                }
            }
        }
    }

    free(usado);
}

// DFS para encontrar vértices de um componente conexo
void dfs_matriz(grafo *g, int v, int *visitado, int *componente, int *tam) {
    visitado[v] = 1;
    componente[(*tam)++] = v;
    for (int u = 0; u < (int)g->nv; u++) {
        if (g->matriz_adj[v][u] > 0 && !visitado[u]) {
            dfs_matriz(g, u, visitado, componente, tam);
        }
    }
}

// Calcula diâmetro de um componente (conjunto de vértices)
int calcular_diametro(grafo *g, int *componente, int tam) {
    int max_diam = 0;
    int *dist = malloc(sizeof(int) * g->nv);

    for (int i = 0; i < tam; i++) {
        int origem = componente[i];
        dijkstra(g, origem, dist);
        for (int j = 0; j < tam; j++) {
            int d = dist[componente[j]];
            if (d != INF && d > max_diam) {
                max_diam = d;
            }
        }
    }

    free(dist);
    return max_diam;
}



