#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h> // Para INT_MAX
#include <locale.h> // Para setlocale()

// Define o número máximo de cidades
#define MAX_CIDADES 50
// Define o tamanho máximo para nomes de cidades
#define MAX_NOME_CIDADE 50
#define INFINITO INT_MAX

// Estrutura para um nó da lista de adjacência (representa uma rota)
typedef struct NoRota {
    int idCidadeDestino;    // ID da cidade de destino da rota
    int peso;               // Peso/custo da rota
    struct NoRota* proximo; // Ponteiro para a próxima rota na lista
} NoRota;

// Estrutura para uma cidade
typedef struct Cidade {
    int id;                     // ID único da cidade
    char nome[MAX_NOME_CIDADE]; // Nome da cidade
    NoRota* listaRotas;         // Cabeça da lista de adjacência (rotas saindo desta cidade)
    bool ativa;                 // Indica se a cidade existe/está ativa
} Cidade;

// Estrutura para o grafo do mapa de cidades
typedef struct MapaCidades {
    Cidade cidades[MAX_CIDADES]; // Array de cidades
    int numCidadesAtivas;        // Número de cidades atualmente no mapa
} MapaCidades;

// --- Funções Auxiliares para Lista de Adjacência ---

// Cria um novo nó de rota
NoRota* criarNoRota(int idDestino, int peso) {
    NoRota* novoNo = (NoRota*)malloc(sizeof(NoRota));
    if (!novoNo) {
        perror("Erro ao alocar memória para NoRota");
        exit(EXIT_FAILURE);
    }
    novoNo->idCidadeDestino = idDestino;
    novoNo->peso = peso;
    novoNo->proximo = NULL;
    return novoNo;
}

// Adiciona uma rota (aresta ponderada) à lista de adjacência de uma cidade de origem
void adicionarRotaNaLista(Cidade* cidadeOrigem, int idDestino, int peso) {
    // Verifica se a rota já existe (opcional, pode permitir múltiplas rotas ou atualizar)
    // Por simplicidade, apenas adiciona.
    NoRota* novaRota = criarNoRota(idDestino, peso);
    novaRota->proximo = cidadeOrigem->listaRotas;
    cidadeOrigem->listaRotas = novaRota;
}

// --- Funções Principais do Mapa de Cidades ---

// Inicializa o mapa de cidades
MapaCidades* criarMapaCidades() {
    MapaCidades* mapa = (MapaCidades*)malloc(sizeof(MapaCidades));
    if (!mapa) {
        perror("Erro ao alocar memória para MapaCidades");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CIDADES; i++) {
        mapa->cidades[i].id = i; // ID sequencial
        strcpy(mapa->cidades[i].nome, "");
        mapa->cidades[i].listaRotas = NULL;
        mapa->cidades[i].ativa = false;
    }
    mapa->numCidadesAtivas = 0;
    printf("Mapa de cidades inicializado.\n");
    return mapa;
}

// Cadastra uma nova cidade no mapa
// Retorna o ID da cidade ou -1 se não puder adicionar
int cadastrarCidade(MapaCidades* mapa, const char* nome) {
    if (mapa->numCidadesAtivas >= MAX_CIDADES) {
        printf("Limite máximo de cidades (%d) atingido.\n", MAX_CIDADES);
        return -1;
    }
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (!mapa->cidades[i].ativa) {
            mapa->cidades[i].ativa = true;
            strncpy(mapa->cidades[i].nome, nome, MAX_NOME_CIDADE - 1);
            mapa->cidades[i].nome[MAX_NOME_CIDADE - 1] = '\0';
            mapa->cidades[i].listaRotas = NULL;
            mapa->numCidadesAtivas++;
            printf("Cidade '%s' (ID: %d) cadastrada.\n", nome, i);
            return i;
        }
    }
    return -1;
}

// Cadastra uma rota entre duas cidades com um peso
void cadastrarRota(MapaCidades* mapa, int idOrigem, int idDestino, int peso) {
    if (idOrigem < 0 || idOrigem >= MAX_CIDADES || !mapa->cidades[idOrigem].ativa ||
        idDestino < 0 || idDestino >= MAX_CIDADES || !mapa->cidades[idDestino].ativa) {
        printf("Erro: ID de cidade inválido ou cidade não ativa para cadastrar rota.\n");
        return;
    }
    if (peso < 0) {
        printf("Erro: O peso da rota não pode ser negativo.\n");
        return;
    }
    adicionarRotaNaLista(&mapa->cidades[idOrigem], idDestino, peso);
    printf("Rota de %s (ID: %d) para %s (ID: %d) com peso %d cadastrada.\n",
           mapa->cidades[idOrigem].nome, idOrigem,
           mapa->cidades[idDestino].nome, idDestino, peso);
    // Se as rotas forem bidirecionais com o mesmo peso, adicione a volta:
    // adicionarRotaNaLista(&mapa->cidades[idDestino], idOrigem, peso);
}

// Visualiza todas as cidades cadastradas e suas rotas
void visualizarCidadesERotas(MapaCidades* mapa) {
    printf("\n--- Cidades e Rotas Cadastradas ---\n");
    if (mapa->numCidadesAtivas == 0) {
        printf("Nenhuma cidade cadastrada.\n");
        return;
    }
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) {
            printf("Cidade: %s (ID: %d)\n", mapa->cidades[i].nome, i);
            NoRota* rota = mapa->cidades[i].listaRotas;
            if (rota == NULL) {
                printf("  Nenhuma rota saindo desta cidade.\n");
            } else {
                printf("  Rotas saindo de %s:\n", mapa->cidades[i].nome);
                while (rota != NULL) {
                    printf("    -> Para: %s (ID: %d), Peso: %d\n",
                           mapa->cidades[rota->idCidadeDestino].nome, rota->idCidadeDestino, rota->peso);
                    rota = rota->proximo;
                }
            }
        }
    }
}

// --- Algoritmo de Dijkstra ---

// Encontra o vértice com a menor distância, do conjunto de vértices ainda não incluídos na árvore de caminhos mínimos
int menorDistancia(int dist[], bool sptSet[], int numVerticesAtivosNoGrafo, const Cidade cidades[]) {
    int min = INFINITO, min_index = -1;

    for (int v_idx = 0; v_idx < MAX_CIDADES; ++v_idx) {
        if (cidades[v_idx].ativa && !sptSet[v_idx] && dist[v_idx] <= min) {
            min = dist[v_idx];
            min_index = v_idx;
        }
    }
    return min_index;
}

// Imprime o caminho do源 (src) até o destino (j) usando o array de predecessores
void imprimirCaminho(MapaCidades* mapa, int pred[], int j) {
    if (pred[j] == -1) { // Chegou na origem ou não há caminho
        printf("%s (ID: %d)", mapa->cidades[j].nome, j);
        return;
    }
    imprimirCaminho(mapa, pred, pred[j]);
    printf(" -> %s (ID: %d)", mapa->cidades[j].nome, j);
}


// Função principal do algoritmo de Dijkstra
void dijkstra(MapaCidades* mapa, int idOrigem, int idDestino) {
    if (idOrigem < 0 || idOrigem >= MAX_CIDADES || !mapa->cidades[idOrigem].ativa ||
        idDestino < 0 || idDestino >= MAX_CIDADES || !mapa->cidades[idDestino].ativa) {
        printf("Erro: ID de cidade de origem ou destino inválido para Dijkstra.\n");
        return;
    }

    int dist[MAX_CIDADES];    // Array para guardar a menor distância da origem para i
    bool sptSet[MAX_CIDADES]; // sptSet[i] será true se o vértice i estiver incluído na árvore de caminhos mínimos
    int pred[MAX_CIDADES];    // Array para armazenar o predecessor de cada cidade no caminho mínimo

    // Inicializa todas as distâncias como INFINITO e sptSet[] como false
    for (int i = 0; i < MAX_CIDADES; i++) {
        dist[i] = INFINITO;
        sptSet[i] = false;
        pred[i] = -1; // -1 indica que não há predecessor (ou é a origem)
    }

    // Distância da origem para ela mesma é sempre 0
    dist[idOrigem] = 0;

    printf("\nCalculando menor caminho de %s (ID: %d) para %s (ID: %d) usando Dijkstra...\n",
           mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino);

    // Encontra o caminho mínimo para todos os vértices
    // (poderia otimizar para parar quando o destino for incluído no sptSet)
    for (int count = 0; count < mapa->numCidadesAtivas -1; count++) {
        // Pega o vértice com a menor distância do conjunto de vértices ainda não processados
        int u = menorDistancia(dist, sptSet, mapa->numCidadesAtivas, mapa->cidades);

        // Se u for -1, significa que os vértices restantes não são alcançáveis
        if (u == -1) break;
        
        // Marca o vértice pego como processado
        sptSet[u] = true;

        // Se o vértice processado é o destino, podemos parar (otimização)
        if (u == idDestino && dist[u] != INFINITO) {
             //printf("Destino alcançado e incluído no SPT. Parando busca.\n");
             break;
        }


        // Atualiza o valor da distância dos vértices adjacentes ao vértice pego.
        NoRota* rota = mapa->cidades[u].listaRotas;
        while (rota != NULL) {
            int v = rota->idCidadeDestino;
            if (mapa->cidades[v].ativa && !sptSet[v] && dist[u] != INFINITO &&
                dist[u] + rota->peso < dist[v]) {
                dist[v] = dist[u] + rota->peso;
                pred[v] = u; // Define u como predecessor de v
            }
            rota = rota->proximo;
        }
    }

    // Imprime o resultado
    if (dist[idDestino] == INFINITO) {
        printf("Não há caminho de %s (ID: %d) para %s (ID: %d).\n",
               mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino);
    } else {
        printf("Menor custo de %s (ID: %d) para %s (ID: %d) é: %d\n",
               mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino, dist[idDestino]);
        printf("Caminho: ");
        imprimirCaminho(mapa, pred, idDestino);
        printf("\n");
    }
}


// Libera a memória alocada para o mapa de cidades
void liberarMapaCidades(MapaCidades* mapa) {
    if (mapa == NULL) return;
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) {
            NoRota* atual = mapa->cidades[i].listaRotas;
            while (atual != NULL) {
                NoRota* temp = atual;
                atual = atual->proximo;
                free(temp);
            }
            mapa->cidades[i].listaRotas = NULL;
        }
    }
    free(mapa);
    printf("Memória do mapa de cidades liberada.\n");
}

// --- Interface de Linha de Comando (Menu) ---
void exibirMenuCidades() {
    printf("\n--- Sistema de Rotas Otimizadas entre Cidades ---\n");
    printf("1. Cadastrar Cidade\n");
    printf("2. Cadastrar Rota\n");
    printf("3. Visualizar Cidades e Rotas\n");
    printf("4. Calcular Menor Caminho (Dijkstra)\n");
    printf("5. Listar todas as cidades ativas\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

void listarCidadesAtivas(MapaCidades* mapa) {
    printf("\n--- Cidades Ativas no Mapa ---\n");
    int count = 0;
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) {
            printf("ID: %d, Nome: %s\n", mapa->cidades[i].id, mapa->cidades[i].nome);
            count++;
        }
    }
    if (count == 0) {
        printf("Nenhuma cidade ativa no mapa.\n");
    }
}

int main() { // Renomeado para main_routes
    if (setlocale(LC_ALL, "pt_BR.UTF-8") == NULL) {
        if (setlocale(LC_ALL, "pt_BR") == NULL) {
            if (setlocale(LC_ALL, "Portuguese_Brazil.1252") == NULL) {
                fprintf(stderr, "Erro ao definir a localização para o Brasil!\n");
                // Opcional: define para a localização padrão do sistema se a do Brasil falhar
                setlocale(LC_ALL, "");
            }
        }
    }

    MapaCidades* mapa = criarMapaCidades();
    int opcao;
    char nome[MAX_NOME_CIDADE];
    int idOrigem, idDestino, peso;

    // Adicionando algumas cidades e rotas para teste rápido
    int cA = cadastrarCidade(mapa, "A"); // ID 0
    int cB = cadastrarCidade(mapa, "B"); // ID 1
    int cC = cadastrarCidade(mapa, "C"); // ID 2
    int cD = cadastrarCidade(mapa, "D"); // ID 3
    int cE = cadastrarCidade(mapa, "E"); // ID 4

    if(cA !=-1 && cB !=-1) cadastrarRota(mapa, cA, cB, 4); // A->B (4)
    if(cA !=-1 && cC !=-1) cadastrarRota(mapa, cA, cC, 2); // A->C (2)
    if(cB !=-1 && cC !=-1) cadastrarRota(mapa, cB, cC, 5); // B->C (5)
    if(cB !=-1 && cD !=-1) cadastrarRota(mapa, cB, cD, 10);// B->D (10)
    if(cC !=-1 && cD !=-1) cadastrarRota(mapa, cC, cD, 3); // C->D (3)
    if(cC !=-1 && cE !=-1) cadastrarRota(mapa, cC, cE, 7); // C->E (7)
    if(cD !=-1 && cE !=-1) cadastrarRota(mapa, cD, cE, 4); // D->E (4)
    // Rota de volta para teste
    if(cB !=-1 && cA !=-1) cadastrarRota(mapa, cB, cA, 6); // B->A (6)


    do {
        exibirMenuCidades();
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, insira um número.\n");
            while (getchar() != '\n'); // Limpa o buffer de entrada
            opcao = -1;
            continue;
        }
        getchar(); // Consome o newline

        switch (opcao) {
            case 1:
                printf("Digite o nome da nova cidade: ");
                fgets(nome, MAX_NOME_CIDADE, stdin);
                nome[strcspn(nome, "\n")] = 0; // Remove newline
                cadastrarCidade(mapa, nome);
                break;
            case 2:
                printf("Digite o ID da cidade de origem: ");
                if (scanf("%d", &idOrigem) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID da cidade de destino: ");
                if (scanf("%d", &idDestino) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o peso/custo da rota: ");
                if (scanf("%d", &peso) != 1) { printf("Peso inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome newline
                cadastrarRota(mapa, idOrigem, idDestino, peso);
                break;
            case 3:
                visualizarCidadesERotas(mapa);
                break;
            case 4:
                printf("Digite o ID da cidade de origem para Dijkstra: ");
                if (scanf("%d", &idOrigem) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID da cidade de destino para Dijkstra: ");
                if (scanf("%d", &idDestino) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome newline
                dijkstra(mapa, idOrigem, idDestino);
                break;
            case 5:
                listarCidadesAtivas(mapa);
                break;
            case 0:
                printf("Saindo do Sistema de Rotas...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarMapaCidades(mapa);
    return 0;
}