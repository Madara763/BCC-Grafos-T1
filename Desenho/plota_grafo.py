import sys
import networkx as nx
import matplotlib.pyplot as plt

def ler_arquivo_grafo(caminho_arquivo):
    with open(caminho_arquivo, 'r') as f:
        linhas = f.readlines()

    nome_grafo = None
    arestas = []
    vertices_isolados = set()

    lendo_nome = True

    for linha in linhas:
        linha = linha.strip()

        if not linha or linha.startswith("//"):
            continue

        if lendo_nome:
            nome_grafo = linha
            lendo_nome = False
            continue

        if "--" in linha:
            partes = linha.split()
            origem = partes[0]
            destino = partes[2]
            peso = int(partes[3])
            arestas.append((origem, destino, peso))
        else:
            vertices_isolados.add(linha)

    return nome_grafo, arestas, vertices_isolados

def main():
    if len(sys.argv) != 2:
        print("Uso: python3 grafo_plot.py <arquivo_entrada>")
        sys.exit(1)

    caminho_arquivo = sys.argv[1]
    nome_grafo, arestas, vertices_isolados = ler_arquivo_grafo(caminho_arquivo)

    G = nx.Graph()

    # Adiciona as arestas com peso
    for origem, destino, peso in arestas:
        G.add_edge(origem, destino, weight=peso)

    # Adiciona vértices isolados
    for vertice in vertices_isolados:
        if vertice not in G:
            G.add_node(vertice)

    pos = nx.spring_layout(G)  # Layout automático
    pesos = nx.get_edge_attributes(G, 'weight')

    plt.figure(figsize=(8, 6))
    nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=800, edge_color='gray', font_weight='bold')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=pesos)

    plt.title(nome_grafo or "Grafo")
    plt.axis('off')
    plt.show()

if __name__ == '__main__':
    main()
