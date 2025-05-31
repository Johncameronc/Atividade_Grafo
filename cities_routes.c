// Inclusão das bibliotecas padrão necessárias para o programa.
#include <stdio.h>      // Para entrada e saída padrão (printf, scanf)
#include <stdlib.h>     // Para alocação de memória (malloc, free, exit)
#include <string.h>     // Para manipulação de strings (strcpy, strncpy, strcspn)
#include <stdbool.h>    // Para usar o tipo booleano (true, false)
#include <limits.h>     // Para usar constantes como INT_MAX
#include <locale.h>     // Para configuração de localização (idioma, formato de números, etc.)

// Definições de constantes para limites e valores especiais.
#define MAX_CIDADES 50          // Número máximo de cidades que podem ser cadastradas.
#define MAX_NOME_CIDADE 50    // Comprimento máximo para o nome de uma cidade.
#define INFINITO INT_MAX        // Representa uma distância infinita, útil no algoritmo de Dijkstra.

// Estrutura para representar um nó em uma lista de rotas (arestas do grafo).
// Cada nó contém o ID da cidade de destino da rota e o peso (custo) da rota.
typedef struct NoRota {
    int idCidadeDestino;      // ID da cidade para a qual a rota leva.
    int peso;                 // Custo (distância, tempo, etc.) para percorrer a rota.
    struct NoRota* proximo;   // Ponteiro para o próximo nó na lista de rotas.
} NoRota;

// Estrutura para representar uma cidade (vértice do grafo).
// Contém informações sobre a cidade, como ID, nome, uma lista de rotas que partem dela,
// e um indicador se a cidade está ativa no mapa.
typedef struct Cidade {
    int id;                         // Identificador único da cidade.
    char nome[MAX_NOME_CIDADE];     // Nome da cidade.
    NoRota* listaRotas;             // Ponteiro para o início da lista de rotas que partem desta cidade.
    bool ativa;                     // Indica se a cidade está atualmente em uso no mapa.
} Cidade;

// Estrutura para representar o mapa de cidades (o grafo em si).
// Contém um array de todas as cidades possíveis e o número de cidades atualmente ativas.
typedef struct MapaCidades {
    Cidade cidades[MAX_CIDADES];    // Array que armazena os dados de todas as cidades.
    int numCidadesAtivas;           // Contador do número de cidades que foram cadastradas e estão ativas.
} MapaCidades;


// Função para criar um novo nó de rota.
// Aloca memória para um NoRota, inicializa seus campos com o destino e peso fornecidos,
// e define o próximo nó como NULL.
NoRota* criarNoRota(int idDestino, int peso) {
    NoRota* novoNo = (NoRota*)malloc(sizeof(NoRota)); // Aloca memória para o novo nó.
    if (!novoNo) { // Verifica se a alocação de memória falhou.
        perror("Erro ao alocar memória para NoRota"); // Imprime mensagem de erro.
        exit(EXIT_FAILURE); // Termina o programa em caso de falha.
    }
    novoNo->idCidadeDestino = idDestino; // Define o ID da cidade de destino.
    novoNo->peso = peso;                 // Define o peso da rota.
    novoNo->proximo = NULL;              // Inicializa o ponteiro para o próximo como NULL.
    return novoNo;                       // Retorna o ponteiro para o nó criado.
}

// Função para adicionar uma nova rota à lista de rotas de uma cidade de origem.
// Cria um novo nó de rota e o insere no início da lista de adjacências da cidade de origem.
void adicionarRotaNaLista(Cidade* cidadeOrigem, int idDestino, int peso) {
    NoRota* novaRota = criarNoRota(idDestino, peso); // Cria o nó da nova rota.
    novaRota->proximo = cidadeOrigem->listaRotas;   // O próximo da nova rota aponta para o início da lista atual.
    cidadeOrigem->listaRotas = novaRota;            // A nova rota se torna o início da lista.
}

// Função para criar e inicializar o mapa de cidades.
// Aloca memória para a estrutura MapaCidades e inicializa todas as cidades como inativas,
// sem nome e sem rotas.
MapaCidades* criarMapaCidades() {
    MapaCidades* mapa = (MapaCidades*)malloc(sizeof(MapaCidades)); // Aloca memória para o mapa.
    if (!mapa) { // Verifica falha na alocação.
        perror("Erro ao alocar memória para MapaCidades");
        exit(EXIT_FAILURE);
    }
    // Itera por todas as posições possíveis de cidades no mapa.
    for (int i = 0; i < MAX_CIDADES; i++) {
        mapa->cidades[i].id = i;                     // Define o ID da cidade (baseado no índice).
        strcpy(mapa->cidades[i].nome, "");           // Inicializa o nome da cidade como vazio.
        mapa->cidades[i].listaRotas = NULL;          // Inicializa a lista de rotas como nula.
        mapa->cidades[i].ativa = false;              // Define a cidade como inativa.
    }
    mapa->numCidadesAtivas = 0; // Inicializa o contador de cidades ativas.
    printf("Mapa de cidades inicializado.\n");
    return mapa; // Retorna o ponteiro para o mapa criado.
}

// Função para cadastrar uma nova cidade no mapa.
// Procura por um slot de cidade inativo, define suas informações (nome),
// marca como ativa e incrementa o contador de cidades ativas.
// Retorna o ID da cidade cadastrada ou -1 se o limite for atingido.
int cadastrarCidade(MapaCidades* mapa, const char* nome) {
    if (mapa->numCidadesAtivas >= MAX_CIDADES) { // Verifica se o limite de cidades foi atingido.
        printf("Limite máximo de cidades (%d) atingido.\n", MAX_CIDADES);
        return -1; // Retorna -1 indicando falha.
    }
    // Procura por um slot de cidade inativo.
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (!mapa->cidades[i].ativa) { // Se encontrar um slot inativo.
            mapa->cidades[i].ativa = true; // Marca a cidade como ativa.
            // Copia o nome fornecido para a cidade, com cuidado para não exceder o buffer.
            strncpy(mapa->cidades[i].nome, nome, MAX_NOME_CIDADE - 1);
            mapa->cidades[i].nome[MAX_NOME_CIDADE - 1] = '\0'; // Garante terminação nula.
            mapa->cidades[i].listaRotas = NULL; // Inicializa a lista de rotas (cidade nova não tem rotas ainda).
            mapa->numCidadesAtivas++;           // Incrementa o contador de cidades ativas.
            printf("Cidade '%s' (ID: %d) cadastrada.\n", nome, i);
            return i; // Retorna o ID da cidade cadastrada.
        }
    }
    return -1; // Caso algo inesperado ocorra (não deveria, se numCidadesAtivas está correto).
}

// Função para cadastrar uma rota entre duas cidades.
// Verifica se os IDs das cidades de origem e destino são válidos e se as cidades estão ativas.
// Também verifica se o peso da rota é não negativo.
// Se tudo estiver correto, adiciona a rota à lista da cidade de origem.
void cadastrarRota(MapaCidades* mapa, int idOrigem, int idDestino, int peso) {
    // Validação dos IDs das cidades e se estão ativas.
    if (idOrigem < 0 || idOrigem >= MAX_CIDADES || !mapa->cidades[idOrigem].ativa ||
        idDestino < 0 || idDestino >= MAX_CIDADES || !mapa->cidades[idDestino].ativa) {
        printf("Erro: ID de cidade inválido ou cidade não ativa para cadastrar rota.\n");
        return;
    }
    if (peso < 0) { // Validação do peso da rota.
        printf("Erro: O peso da rota não pode ser negativo.\n");
        return;
    }
    // Adiciona a rota na lista de adjacências da cidade de origem.
    adicionarRotaNaLista(&mapa->cidades[idOrigem], idDestino, peso);
    printf("Rota de %s (ID: %d) para %s (ID: %d) com peso %d cadastrada.\n",
           mapa->cidades[idOrigem].nome, idOrigem,
           mapa->cidades[idDestino].nome, idDestino, peso);
}

// Função para visualizar todas as cidades cadastradas e suas respectivas rotas.
// Itera pelas cidades ativas e, para cada uma, imprime seu nome e ID,
// seguido por uma lista de todas as rotas que partem dela, incluindo destino e peso.
void visualizarCidadesERotas(MapaCidades* mapa) {
    printf("\n--- Cidades e Rotas Cadastradas ---\n");
    if (mapa->numCidadesAtivas == 0) { // Verifica se há cidades cadastradas.
        printf("Nenhuma cidade cadastrada.\n");
        return;
    }
    // Itera por todas as cidades.
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) { // Se a cidade estiver ativa.
            printf("Cidade: %s (ID: %d)\n", mapa->cidades[i].nome, i);
            NoRota* rota = mapa->cidades[i].listaRotas; // Obtém a lista de rotas da cidade.
            if (rota == NULL) { // Se não houver rotas.
                printf("  Nenhuma rota saindo desta cidade.\n");
            } else { // Se houver rotas.
                printf("  Rotas saindo de %s:\n", mapa->cidades[i].nome);
                // Itera pela lista de rotas.
                while (rota != NULL) {
                    printf("    -> Para: %s (ID: %d), Peso: %d\n",
                           mapa->cidades[rota->idCidadeDestino].nome, rota->idCidadeDestino, rota->peso);
                    rota = rota->proximo; // Move para a próxima rota na lista.
                }
            }
        }
    }
}

// Função auxiliar para o algoritmo de Dijkstra.
// Encontra o vértice (cidade ativa) que ainda não foi incluído na árvore de caminhos mínimos (sptSet)
// e que possui a menor distância conhecida a partir da origem.
// Retorna o ID da cidade com menor distância, ou -1 se não houver mais cidades acessíveis.
int menorDistancia(int dist[], bool sptSet[], int numVerticesAtivosNoGrafo, const Cidade cidades[]) {
    int min = INFINITO, min_index = -1; // Inicializa a distância mínima e o índice.

    // Itera por todos os vértices (cidades).
    for (int v_idx = 0; v_idx < MAX_CIDADES; ++v_idx) {
        // Considera apenas cidades ativas, não incluídas no sptSet e com distância menor que 'min'.
        if (cidades[v_idx].ativa && !sptSet[v_idx] && dist[v_idx] <= min) {
            min = dist[v_idx];      // Atualiza a distância mínima.
            min_index = v_idx;      // Atualiza o índice da cidade com menor distância.
        }
    }
    return min_index; // Retorna o índice da cidade encontrada.
}

// Função recursiva para imprimir o caminho de uma cidade de origem até uma cidade de destino 'j',
// utilizando o array de predecessores 'pred' gerado pelo algoritmo de Dijkstra.
void imprimirCaminho(MapaCidades* mapa, int pred[], int j) {
    if (pred[j] == -1) { // Caso base: chegou na cidade de origem (ou não há caminho).
        printf("%s (ID: %d)", mapa->cidades[j].nome, j);
        return;
    }
    imprimirCaminho(mapa, pred, pred[j]); // Chamada recursiva para o predecessor.
    printf(" -> %s (ID: %d)", mapa->cidades[j].nome, j); // Imprime a cidade atual.
}

// Implementação do algoritmo de Dijkstra para encontrar o menor caminho entre duas cidades.
// Recebe o mapa, ID da cidade de origem e ID da cidade de destino.
// Calcula e imprime o menor custo e o caminho correspondente.
void dijkstra(MapaCidades* mapa, int idOrigem, int idDestino) {
    // Validação dos IDs de origem e destino.
    if (idOrigem < 0 || idOrigem >= MAX_CIDADES || !mapa->cidades[idOrigem].ativa ||
        idDestino < 0 || idDestino >= MAX_CIDADES || !mapa->cidades[idDestino].ativa) {
        printf("Erro: ID de cidade de origem ou destino inválido para Dijkstra.\n");
        return;
    }

    int dist[MAX_CIDADES];     // Array para armazenar as menores distâncias da origem até cada cidade.
    bool sptSet[MAX_CIDADES];  // Array booleano: true se a cidade i está incluída na árvore de caminhos mínimos.
    int pred[MAX_CIDADES];     // Array para armazenar o predecessor de cada cidade no caminho mínimo.

    // Inicialização dos arrays.
    for (int i = 0; i < MAX_CIDADES; i++) {
        dist[i] = INFINITO;     // Distância inicial de todas as cidades é infinita.
        sptSet[i] = false;      // Nenhuma cidade está no sptSet inicialmente.
        pred[i] = -1;           // Predecessor inicial é -1 (sem predecessor).
    }

    dist[idOrigem] = 0; // A distância da cidade de origem para ela mesma é 0.

    printf("\nCalculando menor caminho de %s (ID: %d) para %s (ID: %d) usando Dijkstra...\n",
           mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino);

    // Loop principal do Dijkstra: executa numCidadesAtivas - 1 vezes.
    // (No pior caso, precisa processar todas as cidades ativas).
    for (int count = 0; count < mapa->numCidadesAtivas -1; count++) {
        // Escolhe a cidade com a menor distância conhecida que ainda não foi processada.
        int u = menorDistancia(dist, sptSet, mapa->numCidadesAtivas, mapa->cidades);

        if (u == -1) break; // Se não houver mais cidades alcançáveis, interrompe.
        sptSet[u] = true;   // Marca a cidade 'u' como processada (incluída no sptSet).

        // Otimização: se a cidade 'u' escolhida é o destino e sua distância é finita,
        // o menor caminho para o destino já foi encontrado.
        if (u == idDestino && dist[u] != INFINITO) {
             break;
        }

        // Atualiza as distâncias das cidades adjacentes à cidade 'u'.
        NoRota* rota = mapa->cidades[u].listaRotas; // Obtém as rotas que partem de 'u'.
        while (rota != NULL) {
            int v = rota->idCidadeDestino; // ID da cidade vizinha.
            // Se a cidade 'v' é ativa, não está no sptSet, 'u' é alcançável,
            // e o caminho através de 'u' é mais curto para 'v'.
            if (mapa->cidades[v].ativa && !sptSet[v] && dist[u] != INFINITO &&
                dist[u] + rota->peso < dist[v]) {
                dist[v] = dist[u] + rota->peso; // Atualiza a distância para 'v'.
                pred[v] = u;                    // Define 'u' como predecessor de 'v'.
            }
            rota = rota->proximo; // Próxima rota de 'u'.
        }
    }

    // Impressão do resultado.
    if (dist[idDestino] == INFINITO) { // Se a distância até o destino permaneceu infinita.
        printf("Não há caminho de %s (ID: %d) para %s (ID: %d).\n",
               mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino);
    } else { // Se um caminho foi encontrado.
        printf("Menor custo de %s (ID: %d) para %s (ID: %d) é: %d\n",
               mapa->cidades[idOrigem].nome, idOrigem, mapa->cidades[idDestino].nome, idDestino, dist[idDestino]);
        printf("Caminho: ");
        imprimirCaminho(mapa, pred, idDestino); // Imprime o caminho.
        printf("\n");
    }
}

// Função para liberar toda a memória alocada dinamicamente para o mapa de cidades.
// Itera por cada cidade ativa, liberando a memória de cada nó em sua lista de rotas.
// Por fim, libera a memória da própria estrutura MapaCidades.
void liberarMapaCidades(MapaCidades* mapa) {
    if (mapa == NULL) return; // Se o mapa for nulo, não há nada a fazer.
    // Itera por todas as cidades.
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) { // Se a cidade estiver ativa.
            NoRota* atual = mapa->cidades[i].listaRotas; // Ponteiro para o início da lista de rotas.
            // Itera pela lista de rotas, liberando cada nó.
            while (atual != NULL) {
                NoRota* temp = atual;       // Guarda o nó atual.
                atual = atual->proximo;     // Avança para o próximo nó.
                free(temp);                 // Libera o nó guardado.
            }
            mapa->cidades[i].listaRotas = NULL; // Define a lista de rotas como nula após liberar.
        }
    }
    free(mapa); // Libera a estrutura do mapa.
    printf("Memória do mapa de cidades liberada.\n");
}

// Função para exibir o menu de opções para o usuário.
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

// Função para listar todas as cidades que estão atualmente ativas no mapa.
// Itera pelo array de cidades e imprime o ID e nome de cada cidade ativa.
void listarCidadesAtivas(MapaCidades* mapa) {
    printf("\n--- Cidades Ativas no Mapa ---\n");
    int count = 0; // Contador de cidades ativas encontradas.
    for (int i = 0; i < MAX_CIDADES; i++) {
        if (mapa->cidades[i].ativa) { // Se a cidade está ativa.
            printf("ID: %d, Nome: %s\n", mapa->cidades[i].id, mapa->cidades[i].nome);
            count++;
        }
    }
    if (count == 0) { // Se nenhuma cidade ativa foi encontrada.
        printf("Nenhuma cidade ativa no mapa.\n");
    }
}

// Função principal do programa.
// Configura a localização, cria o mapa, cadastra algumas cidades e rotas iniciais (para teste/exemplo),
// e entra em um loop para exibir o menu e processar as escolhas do usuário.
// Libera a memória alocada antes de terminar.
int main() {
    // Tenta configurar a localização para Português do Brasil (UTF-8 ou outros fallbacks).
    // Isso é importante para a correta exibição de caracteres acentuados no console.
    if (setlocale(LC_ALL, "pt_BR.UTF-8") == NULL) {
        if (setlocale(LC_ALL, "pt_BR") == NULL) {
            if (setlocale(LC_ALL, "Portuguese_Brazil.1252") == NULL) {
                fprintf(stderr, "Erro ao definir a localização para o Brasil! Usando padrão do sistema.\n");
                setlocale(LC_ALL, ""); // Tenta usar a localização padrão do sistema.
            }
        }
    }

    MapaCidades* mapa = criarMapaCidades(); // Cria e inicializa o mapa.
    int opcao;                              // Variável para armazenar a escolha do usuário no menu.
    char nome[MAX_NOME_CIDADE];             // Buffer para ler o nome da cidade.
    int idOrigem, idDestino, peso;          // Variáveis para ler IDs de cidades e peso de rotas.

    // Cadastro inicial de algumas cidades e rotas para demonstração.
    int cA = cadastrarCidade(mapa, "A");
    int cB = cadastrarCidade(mapa, "B");
    int cC = cadastrarCidade(mapa, "C");
    int cD = cadastrarCidade(mapa, "D");
    int cE = cadastrarCidade(mapa, "E");

    // Cadastra rotas, verificando se as cidades foram cadastradas com sucesso (ID != -1).
    if(cA !=-1 && cB !=-1) cadastrarRota(mapa, cA, cB, 4);
    if(cA !=-1 && cC !=-1) cadastrarRota(mapa, cA, cC, 2);
    if(cB !=-1 && cC !=-1) cadastrarRota(mapa, cB, cC, 5);
    if(cB !=-1 && cD !=-1) cadastrarRota(mapa, cB, cD, 10);
    if(cC !=-1 && cD !=-1) cadastrarRota(mapa, cC, cD, 3);
    if(cC !=-1 && cE !=-1) cadastrarRota(mapa, cC, cE, 7);
    if(cD !=-1 && cE !=-1) cadastrarRota(mapa, cD, cE, 4);
    if(cB !=-1 && cA !=-1) cadastrarRota(mapa, cB, cA, 6); // Rota de volta (exemplo de grafo direcionado)


    // Loop principal do menu. Continua até o usuário escolher a opção 0 (Sair).
    do {
        exibirMenuCidades(); // Mostra as opções.
        // Lê a opção do usuário, com tratamento básico de erro para entrada não numérica.
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, insira um número.\n");
            while (getchar() != '\n'); // Limpa o buffer de entrada.
            opcao = -1;                // Define uma opção inválida para continuar no loop.
            continue;                  // Volta para o início do loop.
        }
        getchar(); // Consome o caractere newline deixado pelo scanf.

        // Estrutura switch para tratar a opção escolhida pelo usuário.
        switch (opcao) {
            case 1: // Cadastrar Cidade
                printf("Digite o nome da nova cidade: ");
                fgets(nome, MAX_NOME_CIDADE, stdin); // Lê o nome da cidade.
                nome[strcspn(nome, "\n")] = 0;      // Remove o newline do final da string, se houver.
                cadastrarCidade(mapa, nome);
                break;
            case 2: // Cadastrar Rota
                printf("Digite o ID da cidade de origem: ");
                if (scanf("%d", &idOrigem) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID da cidade de destino: ");
                if (scanf("%d", &idDestino) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o peso/custo da rota: ");
                if (scanf("%d", &peso) != 1) { printf("Peso inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                cadastrarRota(mapa, idOrigem, idDestino, peso);
                break;
            case 3: // Visualizar Cidades e Rotas
                visualizarCidadesERotas(mapa);
                break;
            case 4: // Calcular Menor Caminho (Dijkstra)
                printf("Digite o ID da cidade de origem para Dijkstra: ");
                if (scanf("%d", &idOrigem) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID da cidade de destino para Dijkstra: ");
                if (scanf("%d", &idDestino) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                dijkstra(mapa, idOrigem, idDestino);
                break;
            case 5: // Listar todas as cidades ativas
                listarCidadesAtivas(mapa);
                break;
            case 0: // Sair
                printf("Saindo do Sistema de Rotas...\n");
                break;
            default: // Opção inválida
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0); // Continua o loop enquanto a opção não for 0.

    liberarMapaCidades(mapa); // Libera toda a memória alocada antes de encerrar.
    return 0; // Retorna 0 indicando sucesso na execução do programa.
}
