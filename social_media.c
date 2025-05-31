#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h> // Para setlocale()

// Define o número máximo de usuários na rede social
#define MAX_USUARIOS 100
// Define o tamanho máximo para nomes de usuários
#define MAX_NOME_USUARIO 50

// Estrutura para um nó da lista de adjacência (representa um amigo)
typedef struct NoAmigo {
    int idUsuario;              // ID do usuário amigo
    struct NoAmigo* proximo;    // Ponteiro para o próximo amigo na lista
} NoAmigo;

// Estrutura para um usuário
typedef struct Usuario {
    int id;                     // ID único do usuário
    char nome[MAX_NOME_USUARIO]; // Nome do usuário
    NoAmigo* listaAmigos;       // Cabeça da lista de adjacência (amigos)
    bool ativo;                 // Indica se o usuário existe/está ativo
} Usuario;

// Estrutura para o grafo da rede social
typedef struct RedeSocial {
    Usuario usuarios[MAX_USUARIOS]; // Array de usuários
    int numUsuariosAtivos;          // Número de usuários atualmente na rede
} RedeSocial;

// --- Funções Auxiliares para Lista de Adjacência ---

// Cria um novo nó de amigo
NoAmigo* criarNoAmigo(int idUsuario) {
    NoAmigo* novoNo = (NoAmigo*)malloc(sizeof(NoAmigo));
    if (!novoNo) {
        perror("Erro ao alocar memória para NoAmigo");
        exit(EXIT_FAILURE);
    }
    novoNo->idUsuario = idUsuario;
    novoNo->proximo = NULL;
    return novoNo;
}

// Adiciona uma amizade (aresta) à lista de adjacência de um usuário
void adicionarAmizadeNaLista(Usuario* usuario, int idAmigo) {
    // Verifica se a amizade já existe
    NoAmigo* atual = usuario->listaAmigos;
    while (atual != NULL) {
        if (atual->idUsuario == idAmigo) {
            //printf("Amizade entre %s e usuário %d já existe.\n", usuario->nome, idAmigo);
            return; // Amizade já existe
        }
        atual = atual->proximo;
    }

    // Adiciona no início da lista
    NoAmigo* novoAmigo = criarNoAmigo(idAmigo);
    novoAmigo->proximo = usuario->listaAmigos;
    usuario->listaAmigos = novoAmigo;
}

// --- Funções Principais da Rede Social ---

// Inicializa a rede social
RedeSocial* criarRedeSocial() {
    RedeSocial* rede = (RedeSocial*)malloc(sizeof(RedeSocial));
    if (!rede) {
        perror("Erro ao alocar memória para RedeSocial");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_USUARIOS; i++) {
        rede->usuarios[i].id = i; // ID sequencial
        strcpy(rede->usuarios[i].nome, "");
        rede->usuarios[i].listaAmigos = NULL;
        rede->usuarios[i].ativo = false; // Nenhum usuário ativo inicialmente
    }
    rede->numUsuariosAtivos = 0;
    printf("Rede social inicializada.\n");
    return rede;
}

// Adiciona um novo usuário à rede
// Retorna o ID do usuário ou -1 se não puder adicionar
int adicionarUsuario(RedeSocial* rede, const char* nome) {
    if (rede->numUsuariosAtivos >= MAX_USUARIOS) {
        printf("Limite máximo de usuários (%d) atingido. Não é possível adicionar mais.\n", MAX_USUARIOS);
        return -1;
    }
    // Encontra o primeiro ID disponível (usuário inativo)
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (!rede->usuarios[i].ativo) {
            rede->usuarios[i].ativo = true;
            strncpy(rede->usuarios[i].nome, nome, MAX_NOME_USUARIO - 1);
            rede->usuarios[i].nome[MAX_NOME_USUARIO - 1] = '\0'; // Garante terminação nula
            rede->usuarios[i].listaAmigos = NULL; // Nova lista de amigos vazia
            rede->numUsuariosAtivos++;
            printf("Usuário '%s' (ID: %d) adicionado com sucesso.\n", nome, i);
            return i; // Retorna o ID do usuário adicionado
        }
    }
    return -1; // Não deveria acontecer se numUsuariosAtivos < MAX_USUARIOS
}


// Cria uma conexão de amizade entre dois usuários (bidirecional)
void criarConexao(RedeSocial* rede, int idUsuario1, int idUsuario2) {
    if (idUsuario1 < 0 || idUsuario1 >= MAX_USUARIOS || !rede->usuarios[idUsuario1].ativo ||
        idUsuario2 < 0 || idUsuario2 >= MAX_USUARIOS || !rede->usuarios[idUsuario2].ativo) {
        printf("Erro: ID de usuário inválido ou usuário não ativo.\n");
        return;
    }
    if (idUsuario1 == idUsuario2) {
        printf("Erro: Um usuário não pode ser amigo de si mesmo.\n");
        return;
    }

    adicionarAmizadeNaLista(&rede->usuarios[idUsuario1], idUsuario2);
    adicionarAmizadeNaLista(&rede->usuarios[idUsuario2], idUsuario1); // Conexão bidirecional
    printf("Conexão criada entre %s (ID: %d) e %s (ID: %d).\n",
           rede->usuarios[idUsuario1].nome, idUsuario1,
           rede->usuarios[idUsuario2].nome, idUsuario2);
}

// Visualiza a rede de amizades de um usuário específico
void visualizarRedeAmizades(RedeSocial* rede, int idUsuario) {
    if (idUsuario < 0 || idUsuario >= MAX_USUARIOS || !rede->usuarios[idUsuario].ativo) {
        printf("Erro: ID de usuário inválido ou usuário não ativo.\n");
        return;
    }

    printf("Amigos de %s (ID: %d):\n", rede->usuarios[idUsuario].nome, idUsuario);
    NoAmigo* amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    if (amigoAtual == NULL) {
        printf("  Nenhum amigo encontrado.\n");
        return;
    }
    while (amigoAtual != NULL) {
        printf("  - %s (ID: %d)\n", rede->usuarios[amigoAtual->idUsuario].nome, amigoAtual->idUsuario);
        amigoAtual = amigoAtual->proximo;
    }
}

// --- Algoritmos de Busca ---

// Busca em Largura (BFS) a partir de um usuário
// Usado para encontrar conexões de primeiro grau e sugerir amigos
void BFS(RedeSocial* rede, int idUsuarioInicio) {
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inicial inválido para BFS.\n");
        return;
    }

    printf("\n--- BFS a partir de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);

    bool visitado[MAX_USUARIOS] = {false};
    int fila[MAX_USUARIOS];
    int inicioFila = 0, fimFila = 0;
    int distancia[MAX_USUARIOS]; // Distância do usuário inicial

    // Inicializa distâncias
    for(int i=0; i<MAX_USUARIOS; i++) distancia[i] = -1; // -1 significa não alcançado

    // Adiciona o usuário inicial à fila
    fila[fimFila++] = idUsuarioInicio;
    visitado[idUsuarioInicio] = true;
    distancia[idUsuarioInicio] = 0;

    printf("Usuários alcançáveis e suas distâncias (níveis):\n");
    printf("  %s (ID: %d) - Nível 0 (você mesmo)\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);

    while (inicioFila < fimFila) {
        int idUsuarioAtual = fila[inicioFila++]; // Desenfileira

        NoAmigo* amigo = rede->usuarios[idUsuarioAtual].listaAmigos;
        while (amigo != NULL) {
            int idVizinho = amigo->idUsuario;
            if (rede->usuarios[idVizinho].ativo && !visitado[idVizinho]) {
                visitado[idVizinho] = true;
                distancia[idVizinho] = distancia[idUsuarioAtual] + 1;
                fila[fimFila++] = idVizinho; // Enfileira
                printf("  %s (ID: %d) - Nível %d\n", rede->usuarios[idVizinho].nome, idVizinho, distancia[idVizinho]);
            }
            amigo = amigo->proximo;
        }
    }
}

// Função utilitária para DFS (recursiva)
void DFSUtil(RedeSocial* rede, int idUsuario, bool visitado[], int* grupo, int* contadorGrupo) {
    visitado[idUsuario] = true;
    if (grupo != NULL && contadorGrupo != NULL) {
        grupo[(*contadorGrupo)++] = idUsuario;
    } else {
         printf("  Visitando %s (ID: %d)\n", rede->usuarios[idUsuario].nome, idUsuario);
    }

    NoAmigo* amigo = rede->usuarios[idUsuario].listaAmigos;
    while (amigo != NULL) {
        int idVizinho = amigo->idUsuario;
        if (rede->usuarios[idVizinho].ativo && !visitado[idVizinho]) {
            DFSUtil(rede, idVizinho, visitado, grupo, contadorGrupo);
        }
        amigo = amigo->proximo;
    }
}

// Busca em Profundidade (DFS) a partir de um usuário
// Usado para explorar grupos e verificar conectividade
void DFS(RedeSocial* rede, int idUsuarioInicio) {
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inicial inválido para DFS.\n");
        return;
    }

    printf("\n--- DFS a partir de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);
    bool visitado[MAX_USUARIOS] = {false};
    DFSUtil(rede, idUsuarioInicio, visitado, NULL, NULL);
}


// Sugestão de Amigos (baseado em amigos de amigos - conexões de 2º grau)
void sugerirAmigos(RedeSocial* rede, int idUsuario) {
    if (idUsuario < 0 || idUsuario >= MAX_USUARIOS || !rede->usuarios[idUsuario].ativo) {
        printf("Erro: ID de usuário inválido para sugestão de amigos.\n");
        return;
    }

    printf("\n--- Sugestões de Amigos para %s (ID: %d) ---\n", rede->usuarios[idUsuario].nome, idUsuario);

    bool ehAmigoDireto[MAX_USUARIOS] = {false};
    ehAmigoDireto[idUsuario] = true; // O próprio usuário não é uma sugestão

    // Marca amigos diretos
    NoAmigo* amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    while (amigoAtual != NULL) {
        ehAmigoDireto[amigoAtual->idUsuario] = true;
        amigoAtual = amigoAtual->proximo;
    }

    int sugestoesEncontradas = 0;
    // Percorre os amigos do usuário (1º grau)
    amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    while (amigoAtual != NULL) {
        int idAmigoNivel1 = amigoAtual->idUsuario;
        if (!rede->usuarios[idAmigoNivel1].ativo) { // Pula se amigo não estiver ativo
            amigoAtual = amigoAtual->proximo;
            continue;
        }

        // Percorre os amigos do amigo (2º grau)
        NoAmigo* amigoDoAmigo = rede->usuarios[idAmigoNivel1].listaAmigos;
        while (amigoDoAmigo != NULL) {
            int idAmigoNivel2 = amigoDoAmigo->idUsuario;
            // Sugere se for ativo, não for o próprio usuário e não for amigo direto
            if (rede->usuarios[idAmigoNivel2].ativo && !ehAmigoDireto[idAmigoNivel2]) {
                // Para evitar duplicatas, podemos usar um array de "já sugerido"
                // ou simplesmente imprimir e aceitar duplicatas se um amigo de amigo
                // for amigo de múltiplos amigos diretos.
                // Por simplicidade, vamos imprimir (pode haver duplicatas se não tratado).
                // Para tratar duplicatas, usaríamos um array 'jaSugerido[MAX_USUARIOS]'
                printf("  - %s (ID: %d) (amigo de %s)\n",
                       rede->usuarios[idAmigoNivel2].nome, idAmigoNivel2,
                       rede->usuarios[idAmigoNivel1].nome);
                ehAmigoDireto[idAmigoNivel2] = true; // Marca como "sugerido" para evitar repetição nesta listagem
                sugestoesEncontradas++;
            }
            amigoDoAmigo = amigoDoAmigo->proximo;
        }
        amigoAtual = amigoAtual->proximo;
    }
    if (sugestoesEncontradas == 0) {
        printf("  Nenhuma sugestão de amigo encontrada no momento.\n");
    }
}

// Verifica a conectividade entre dois usuários usando DFS
bool verificarConectividade(RedeSocial* rede, int idUsuarioOrigem, int idUsuarioDestino) {
    if (idUsuarioOrigem < 0 || idUsuarioOrigem >= MAX_USUARIOS || !rede->usuarios[idUsuarioOrigem].ativo ||
        idUsuarioDestino < 0 || idUsuarioDestino >= MAX_USUARIOS || !rede->usuarios[idUsuarioDestino].ativo) {
        printf("Erro: IDs de usuário inválidos para verificar conectividade.\n");
        return false;
    }

    if (idUsuarioOrigem == idUsuarioDestino) return true; // Conectado a si mesmo

    bool visitado[MAX_USUARIOS] = {false};
    // Fila para DFS iterativo (ou pode usar a DFSUtil recursiva)
    int pilha[MAX_USUARIOS];
    int topoPilha = -1;

    pilha[++topoPilha] = idUsuarioOrigem;
    visitado[idUsuarioOrigem] = true;

    while (topoPilha != -1) {
        int u = pilha[topoPilha--]; // Pop

        if (u == idUsuarioDestino) return true; // Encontrou o destino

        NoAmigo* amigo = rede->usuarios[u].listaAmigos;
        while (amigo != NULL) {
            int v = amigo->idUsuario;
            if (rede->usuarios[v].ativo && !visitado[v]) {
                visitado[v] = true;
                pilha[++topoPilha] = v; // Push
            }
            amigo = amigo->proximo;
        }
    }
    return false; // Não encontrou o destino
}

// Explora e exibe os membros de um grupo social (componente conectado)
void explorarGrupos(RedeSocial* rede, int idUsuarioInicio) {
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inválido para explorar grupos.\n");
        return;
    }

    printf("\n--- Explorando Grupo Social de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);
    bool visitado[MAX_USUARIOS] = {false};
    int grupo[MAX_USUARIOS];
    int contadorGrupo = 0;

    DFSUtil(rede, idUsuarioInicio, visitado, grupo, &contadorGrupo);

    if (contadorGrupo > 0) {
        printf("Membros do grupo:\n");
        for (int i = 0; i < contadorGrupo; i++) {
            printf("  - %s (ID: %d)\n", rede->usuarios[grupo[i]].nome, grupo[i]);
        }
    } else {
        printf("Nenhum membro encontrado (o usuário pode estar isolado ou inativo).\n");
    }
}


// Libera a memória alocada para a rede social
void liberarRedeSocial(RedeSocial* rede) {
    if (rede == NULL) return;
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (rede->usuarios[i].ativo) {
            NoAmigo* atual = rede->usuarios[i].listaAmigos;
            while (atual != NULL) {
                NoAmigo* temp = atual;
                atual = atual->proximo;
                free(temp);
            }
            rede->usuarios[i].listaAmigos = NULL;
        }
    }
    free(rede);
    printf("Memória da rede social liberada.\n");
}

// --- Interface de Linha de Comando (Menu) ---
void exibirMenuRedeSocial() {
    printf("\n--- Rede Social Simulada ---\n");
    printf("1. Adicionar Usuário\n");
    printf("2. Criar Conexão (Amizade)\n");
    printf("3. Visualizar Rede de Amizades de um Usuário\n");
    printf("4. Busca em Largura (BFS) a partir de um Usuário\n");
    printf("5. Busca em Profundidade (DFS) a partir de um Usuário\n");
    printf("6. Sugerir Amigos para um Usuário\n");
    printf("7. Explorar Grupo Social de um Usuário\n");
    printf("8. Verificar Conectividade entre dois Usuários\n");
    printf("9. Listar todos os usuários ativos\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

void listarUsuariosAtivos(RedeSocial* rede) {
    printf("\n--- Usuários Ativos na Rede ---\n");
    int count = 0;
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (rede->usuarios[i].ativo) {
            printf("ID: %d, Nome: %s\n", rede->usuarios[i].id, rede->usuarios[i].nome);
            count++;
        }
    }
    if (count == 0) {
        printf("Nenhum usuário ativo na rede.\n");
    }
}


int main() { // Renomeado para main_social para evitar conflito se compilar junto
    if (setlocale(LC_ALL, "pt_BR.UTF-8") == NULL) {
        if (setlocale(LC_ALL, "pt_BR") == NULL) {
            if (setlocale(LC_ALL, "Portuguese_Brazil.1252") == NULL) {
                fprintf(stderr, "Erro ao definir a localização para o Brasil!\n");
                // Opcional: define para a localização padrão do sistema se a do Brasil falhar
                setlocale(LC_ALL, "");
            }
        }
    }

    RedeSocial* rede = criarRedeSocial();
    int opcao;
    char nome[MAX_NOME_USUARIO];
    int id1, id2;

    // Adicionando alguns usuários e conexões para teste rápido
    int idAlice = adicionarUsuario(rede, "Alice");
    int idBob = adicionarUsuario(rede, "Bob");
    int idCharlie = adicionarUsuario(rede, "Charlie");
    int idDavid = adicionarUsuario(rede, "David");
    int idEve = adicionarUsuario(rede, "Eve");
    int idFrank = adicionarUsuario(rede, "Frank"); // Usuário isolado inicialmente

    if(idAlice != -1 && idBob != -1) criarConexao(rede, idAlice, idBob);
    if(idAlice != -1 && idCharlie != -1) criarConexao(rede, idAlice, idCharlie);
    if(idBob != -1 && idDavid != -1) criarConexao(rede, idBob, idDavid);
    if(idCharlie != -1 && idDavid != -1) criarConexao(rede, idCharlie, idDavid);
    if(idDavid != -1 && idEve != -1) criarConexao(rede, idDavid, idEve);


    do {
        exibirMenuRedeSocial();
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, insira um número.\n");
            while (getchar() != '\n'); // Limpa o buffer de entrada
            opcao = -1; // Define uma opção inválida para continuar o loop
            continue;
        }
        getchar(); // Consome o newline deixado pelo scanf

        switch (opcao) {
            case 1:
                printf("Digite o nome do novo usuário: ");
                fgets(nome, MAX_NOME_USUARIO, stdin);
                nome[strcspn(nome, "\n")] = 0; // Remove o newline
                adicionarUsuario(rede, nome);
                break;
            case 2:
                printf("Digite o ID do primeiro usuário: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID do segundo usuário: ");
                if (scanf("%d", &id2) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                criarConexao(rede, id1, id2);
                break;
            case 3:
                printf("Digite o ID do usuário para ver seus amigos: ");
                 if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                visualizarRedeAmizades(rede, id1);
                break;
            case 4:
                printf("Digite o ID do usuário para iniciar BFS: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                BFS(rede, id1);
                break;
            case 5:
                printf("Digite o ID do usuário para iniciar DFS: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                DFS(rede, id1);
                break;
            case 6:
                printf("Digite o ID do usuário para sugerir amigos: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                sugerirAmigos(rede, id1);
                break;
            case 7:
                printf("Digite o ID do usuário para explorar seu grupo social: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                explorarGrupos(rede, id1);
                break;
            case 8:
                printf("Digite o ID do usuário de origem: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID do usuário de destino: ");
                if (scanf("%d", &id2) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline
                if (verificarConectividade(rede, id1, id2)) {
                    printf("%s (ID: %d) e %s (ID: %d) ESTÃO conectados.\n", rede->usuarios[id1].nome, id1, rede->usuarios[id2].nome, id2);
                } else {
                    printf("%s (ID: %d) e %s (ID: %d) NÃO ESTÃO conectados.\n", rede->usuarios[id1].nome, id1, rede->usuarios[id2].nome, id2);
                }
                break;
            case 9:
                listarUsuariosAtivos(rede);
                break;
            case 0:
                printf("Saindo da Rede Social Simulada...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarRedeSocial(rede);
    return 0;
}

// Para compilar e rodar este arquivo isoladamente, você pode descomentar
// a linha abaixo e comentar/remover a função main_social() ou vice-versa.
// Se for compilar junto com o outro .c, cada um deve ter sua main() renomeada
// e uma main() principal que chama uma das duas.
// int main() { return main_social(); }