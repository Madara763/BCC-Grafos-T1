# Funções de Manipulação de Grafos
---

## 1. Estruturas Utilizadas

O grafo é representado por três estruturas principais:

- **Vertice**: representa um nó, com nome, cor, id e ponteiro para o próximo.
- **Aresta**: conecta dois vértices com peso.
- **grafo**: estrutura principal que armazena o nome do grafo, listas de vértices e arestas, e matriz de adjacência de pesos.
- Algumas funções são resolvidas utilizando lista de adjacência e outras utilizam a matriz de adjacência.

---

## 2. Funções Principais

### `bipartido(grafo *g)`
A função utiliza a Busca em Largura (BFS) para tentar colorir o grafo usando duas cores.  
Se em algum momento dois vértices vizinhos recebem a mesma cor, então o grafo não é bipartido.

**Execução:**
- O vértice inicial é colorido com a cor 1.
- Enquanto houver vértices na fila:
  - Remove o vértice da frente.
  - Percorre todas as arestas do grafo.
  - Verifica se o vértice atual está conectado a alguma aresta.
- Para cada vizinho encontrado:
  - Se ainda não tiver sido colorido, colore com a cor oposta e insere na fila.
  - Se já estiver colorido com a mesma cor do vértice atual, retorna 0 → o grafo não é bipartido.
- Se conseguir colorir todos os vértices dos componentes sem conflitos, retorna 1 → o grafo é bipartido.

---

### `n_vertices(grafo *g)`
Retorna o número de vértices. Calculado na hora de leitura do grafo.

---

### `n_arestas(grafo *g)`
Retorna o número de arestas. Calculado na hora de leitura do grafo.

---

### `n_componentes(grafo *g)`
A função utiliza uma Busca em Profundidade (DFS) repetida para contar quantas vezes é necessário "começar uma nova exploração" — o que corresponde exatamente ao número de componentes conexos.

**Execução:**
- Para cada vértice v do grafo:
  - Se ainda não foi visitado (`visitado[v] == 0`), isso significa que está em um componente ainda não explorado.
  - A função `dfs_n_comp(v, visitado, g)` é chamada para visitar todo esse componente.
  - O contador de componentes é incrementado.
- Retorna o total de componentes encontrados.

---

### `diametros(grafo *g)`
A função percorre o grafo para identificar os componentes conexos, e para cada componente, calcula o seu diâmetro. Ao final, ordena esses valores e monta uma string de saída com todos eles.

**Execução:**
- Para cada vértice v, se ainda não foi visitado:
  - Cria um vetor `componente` para armazenar os vértices do componente atual.
  - Realiza uma DFS com `dfs_matriz()` a partir de v, preenchendo `componente` e seu tamanho `tam`.
  - Chama `calcular_diametro()` para obter o diâmetro desse conjunto de vértices.
  - Armazena o valor retornado em `dias[nd++]`.

O vetor `dias` é ordenado em ordem crescente usando um sort simples com dois loops (O(n²)), suficiente já que o número de componentes geralmente é pequeno.  
Aloca uma string `saida` com espaço suficiente para conter todos os números e espaços.  
Concatena cada número do vetor `dias` na string final, separando-os com espaços.

---

### `vertices_corte(grafo *g)`
A função identifica os vértices de corte do grafo. Utiliza uma versão modificada de DFS (recursiva) para encontrar esses vértices.

**Execução:**
- Para cada vértice v ainda não visitado:
  - Inicia uma DFS para v, registrando tempos de descoberta (`pre_ordem`) e `low_point`, que indicam o quão "cedo" uma DFS pode retornar na árvore gerada.
  - Um vértice v é identificado como vértice de corte se `low_point[v] ≥ pre_ordem[v]` para algum filho (exceto raiz) ou se for raiz e tiver mais de um filho.
- Todos os vértices identificados como vértices de corte são armazenados em um vetor, ordenados alfabeticamente e transformados na string de saída final.

---

### `arestas_corte(grafo *g)`
A função identifica as arestas de corte do grafo. Uma aresta de corte é aquela cuja remoção aumenta o número de componentes conexos. Também utiliza o algoritmo de busca em profundidade (DFS).

**Execução:**
- Para cada vértice v ainda não visitado:
  - Inicia uma DFS a partir de v, calculando `pre_ordem` e `low_point`.
  - Uma aresta `(v, u)` é considerada aresta de corte se `low_point[u] > pre_ordem[v]`, indicando que não há caminho alternativo para retornar a um vértice ancestral sem passar por essa aresta.
  - As arestas são armazenadas como pares de nomes de vértices em ordem alfabética.
  - As arestas identificadas são ordenadas alfabeticamente e montadas na string final.

---

## 3. Funções Auxiliares

- **`cria_vertice(const char *nome)`**  
  Cria e inicializa um vértice.

- **`busca_ou_adiciona_vertice(grafo *g, const char *nome)`**  
  Busca um vértice pelo nome ou o adiciona ao grafo.

- **`adiciona_aresta(grafo *g, Vertice *v1, Vertice *v2, int peso)`**  
  Cria uma aresta entre dois vértices com peso.

- **`dijkstra(grafo *g, int origem, int *dist)`**  
  Calcula menores distâncias a partir de um vértice usando Dijkstra.

- **`dfs_matriz(grafo *g, int v, int *visitado, int *componente, int *tam)`**  
  Realiza DFS para descobrir um componente conexo.

- **`calcular_diametro(grafo *g, int *componente, int tam)`**  
  Calcula o diâmetro de um componente por Dijkstra múltiplas vezes.

- **`imprime_grafo(grafo *g)`**  
  Exibe a estrutura do grafo para depuração.
