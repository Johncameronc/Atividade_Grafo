// Inclui a biblioteca padrão de entrada e saída (para funções como printf, scanf).
#include <stdio.h>
// Inclui a biblioteca padrão para alocação de memória, controle de processos e outras utilidades (para funções como malloc, free, exit).
#include <stdlib.h>
// Inclui a biblioteca para manipulação de strings (para funções como strcpy, strlen).
#include <string.h>
// Inclui a biblioteca para usar o tipo booleano (bool, true, false).
#include <stdbool.h>
// Inclui a biblioteca para configurações de localidade (para acentuação e formatação regional).
#include <locale.h>

// Define o número máximo de usuários que a rede social pode ter.
#define MAX_USUARIOS 100
// Define o tamanho máximo para o nome de um usuário.
#define MAX_NOME_USUARIO 50

// Define a estrutura para um nó da lista de amigos.
// Cada nó armazena o ID de um usuário amigo e um ponteiro para o próximo amigo na lista.
typedef struct NoAmigo {
    int idUsuario;          // ID do usuário amigo.
    struct NoAmigo* proximo; // Ponteiro para o próximo nó amigo na lista.
} NoAmigo;

// Define a estrutura para um usuário da rede social.
typedef struct Usuario {
    int id;                             // Identificador único do usuário.
    char nome[MAX_NOME_USUARIO];      // Nome do usuário.
    NoAmigo* listaAmigos;             // Ponteiro para o início da lista de amigos do usuário.
    bool ativo;                       // Flag que indica se o usuário está ativo (true) ou não (false).
} Usuario;

// Define a estrutura para a rede social.
// Contém um array de usuários e o número de usuários ativos.
typedef struct RedeSocial {
    Usuario usuarios[MAX_USUARIOS]; // Array para armazenar todos os usuários.
    int numUsuariosAtivos;          // Contador de usuários atualmente ativos na rede.
} RedeSocial;

// Função para criar um novo nó de amigo.
// Aloca memória para o nó, atribui o ID do usuário amigo e inicializa o próximo ponteiro como NULL.
NoAmigo* criarNoAmigo(int idUsuario) {
    // Aloca memória dinamicamente para um novo nó de amigo.
    NoAmigo* novoNo = (NoAmigo*)malloc(sizeof(NoAmigo));
    // Verifica se a alocação de memória foi bem-sucedida.
    if (!novoNo) {
        // Se falhar, exibe uma mensagem de erro e encerra o programa.
        perror("Erro ao alocar memória para NoAmigo");
        exit(EXIT_FAILURE);
    }
    // Define o ID do usuário amigo no novo nó.
    novoNo->idUsuario = idUsuario;
    // Define o ponteiro para o próximo nó como NULL, pois este é o último nó adicionado (por enquanto).
    novoNo->proximo = NULL;
    // Retorna o ponteiro para o nó recém-criado.
    return novoNo;
}

// Função para adicionar uma amizade à lista de amigos de um usuário.
// A amizade é mútua, então esta função normalmente seria chamada duas vezes (uma para cada usuário).
// Evita adicionar amizades duplicadas.
void adicionarAmizadeNaLista(Usuario* usuario, int idAmigo) {
    // Percorre a lista de amigos do usuário para verificar se a amizade já existe.
    NoAmigo* atual = usuario->listaAmigos;
    while (atual != NULL) {
        // Se o amigo já estiver na lista, não faz nada e retorna.
        if (atual->idUsuario == idAmigo) {
            return;
        }
        atual = atual->proximo;
    }

    // Se a amizade não existir, cria um novo nó para o amigo.
    NoAmigo* novoAmigo = criarNoAmigo(idAmigo);
    // Adiciona o novo amigo no início da lista de amigos do usuário.
    novoAmigo->proximo = usuario->listaAmigos;
    usuario->listaAmigos = novoAmigo;
}

// Função para criar e inicializar uma nova rede social.
// Aloca memória para a estrutura RedeSocial e inicializa todos os usuários como inativos.
RedeSocial* criarRedeSocial() {
    // Aloca memória dinamicamente para a estrutura da rede social.
    RedeSocial* rede = (RedeSocial*)malloc(sizeof(RedeSocial));
    // Verifica se a alocação de memória foi bem-sucedida.
    if (!rede) {
        // Se falhar, exibe uma mensagem de erro e encerra o programa.
        perror("Erro ao alocar memória para RedeSocial");
        exit(EXIT_FAILURE);
    }
    // Inicializa cada usuário no array de usuários da rede.
    for (int i = 0; i < MAX_USUARIOS; i++) {
        rede->usuarios[i].id = i;                         // Atribui um ID único ao usuário.
        strcpy(rede->usuarios[i].nome, "");             // Inicializa o nome do usuário como uma string vazia.
        rede->usuarios[i].listaAmigos = NULL;           // Inicializa a lista de amigos como vazia.
        rede->usuarios[i].ativo = false;                  // Define o usuário como inativo.
    }
    // Inicializa o contador de usuários ativos como 0.
    rede->numUsuariosAtivos = 0;
    // Informa que a rede social foi inicializada.
    printf("Rede social inicializada.\n");
    // Retorna o ponteiro para a rede social criada.
    return rede;
}

// Função para adicionar um novo usuário à rede social.
// Procura por um slot de usuário inativo, o ativa e atribui o nome fornecido.
// Retorna o ID do usuário adicionado ou -1 se a rede estiver cheia.
int adicionarUsuario(RedeSocial* rede, const char* nome) {
    // Verifica se o número máximo de usuários foi atingido.
    if (rede->numUsuariosAtivos >= MAX_USUARIOS) {
        printf("Limite máximo de usuários (%d) atingido. Não é possível adicionar mais.\n", MAX_USUARIOS);
        return -1; // Retorna -1 indicando falha.
    }

    // Procura por um slot de usuário inativo para adicionar o novo usuário.
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (!rede->usuarios[i].ativo) { // Se encontrar um usuário inativo.
            rede->usuarios[i].ativo = true; // Ativa o usuário.
            // Copia o nome fornecido para o usuário, garantindo que não exceda o tamanho máximo e termine com '\0'.
            strncpy(rede->usuarios[i].nome, nome, MAX_NOME_USUARIO - 1);
            rede->usuarios[i].nome[MAX_NOME_USUARIO - 1] = '\0'; // Garante a terminação nula.
            rede->usuarios[i].listaAmigos = NULL; // Inicializa a lista de amigos do novo usuário como vazia.
            rede->numUsuariosAtivos++; // Incrementa o contador de usuários ativos.
            printf("Usuário '%s' (ID: %d) adicionado com sucesso.\n", nome, i);
            return i; // Retorna o ID do usuário adicionado.
        }
    }
    return -1; // Caso (improvável, devido à verificação anterior) não encontre slot.
}

// Função para criar uma conexão (amizade) entre dois usuários.
// Adiciona cada usuário à lista de amigos do outro.
void criarConexao(RedeSocial* rede, int idUsuario1, int idUsuario2) {
    // Verifica se os IDs dos usuários são válidos e se os usuários estão ativos.
    if (idUsuario1 < 0 || idUsuario1 >= MAX_USUARIOS || !rede->usuarios[idUsuario1].ativo ||
        idUsuario2 < 0 || idUsuario2 >= MAX_USUARIOS || !rede->usuarios[idUsuario2].ativo) {
        printf("Erro: ID de usuário inválido ou usuário não ativo.\n");
        return;
    }
    // Verifica se um usuário está tentando adicionar a si mesmo como amigo.
    if (idUsuario1 == idUsuario2) {
        printf("Erro: Um usuário não pode ser amigo de si mesmo.\n");
        return;
    }

    // Adiciona a amizade na lista de ambos os usuários (conexão mútua).
    adicionarAmizadeNaLista(&rede->usuarios[idUsuario1], idUsuario2);
    adicionarAmizadeNaLista(&rede->usuarios[idUsuario2], idUsuario1);
    printf("Conexão criada entre %s (ID: %d) e %s (ID: %d).\n",
           rede->usuarios[idUsuario1].nome, idUsuario1,
           rede->usuarios[idUsuario2].nome, idUsuario2);
}

// Função para visualizar a lista de amigos de um usuário específico.
void visualizarRedeAmizades(RedeSocial* rede, int idUsuario) {
    // Verifica se o ID do usuário é válido e se o usuário está ativo.
    if (idUsuario < 0 || idUsuario >= MAX_USUARIOS || !rede->usuarios[idUsuario].ativo) {
        printf("Erro: ID de usuário inválido ou usuário não ativo.\n");
        return;
    }

    // Exibe o nome e ID do usuário cujos amigos serão listados.
    printf("Amigos de %s (ID: %d):\n", rede->usuarios[idUsuario].nome, idUsuario);
    // Percorre a lista de amigos do usuário.
    NoAmigo* amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    // Verifica se o usuário não tem amigos.
    if (amigoAtual == NULL) {
        printf("  Nenhum amigo encontrado.\n");
        return;
    }
    // Itera sobre a lista de amigos e imprime o nome e ID de cada amigo.
    while (amigoAtual != NULL) {
        printf("  - %s (ID: %d)\n", rede->usuarios[amigoAtual->idUsuario].nome, amigoAtual->idUsuario);
        amigoAtual = amigoAtual->proximo;
    }
}

// Função para realizar uma Busca em Largura (BFS) na rede social a partir de um usuário inicial.
// Mostra os usuários alcançáveis e a distância (nível) em relação ao usuário inicial.
void BFS(RedeSocial* rede, int idUsuarioInicio) {
    // Verifica se o ID do usuário inicial é válido e se ele está ativo.
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inicial inválido para BFS.\n");
        return;
    }

    printf("\n--- BFS a partir de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);

    // Array para marcar os usuários visitados durante a BFS.
    bool visitado[MAX_USUARIOS] = {false};
    // Fila para gerenciar os usuários a serem visitados.
    int fila[MAX_USUARIOS];
    // Índices para o início e fim da fila.
    int inicioFila = 0, fimFila = 0;
    // Array para armazenar a distância de cada usuário em relação ao usuário inicial.
    int distancia[MAX_USUARIOS];

    // Inicializa as distâncias de todos os usuários como -1 (não alcançado).
    for(int i=0; i<MAX_USUARIOS; i++) distancia[i] = -1;

    // Adiciona o usuário inicial à fila, marca como visitado e define sua distância como 0.
    fila[fimFila++] = idUsuarioInicio;
    visitado[idUsuarioInicio] = true;
    distancia[idUsuarioInicio] = 0;

    printf("Usuários alcançáveis e suas distâncias (níveis):\n");
    printf("  %s (ID: %d) - Nível 0 (você mesmo)\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);

    // Loop principal da BFS: continua enquanto a fila não estiver vazia.
    while (inicioFila < fimFila) {
        // Remove o primeiro usuário da fila (FIFO).
        int idUsuarioAtual = fila[inicioFila++];

        // Percorre a lista de amigos do usuário atual.
        NoAmigo* amigo = rede->usuarios[idUsuarioAtual].listaAmigos;
        while (amigo != NULL) {
            int idVizinho = amigo->idUsuario;
            // Se o vizinho estiver ativo e não tiver sido visitado ainda.
            if (rede->usuarios[idVizinho].ativo && !visitado[idVizinho]) {
                visitado[idVizinho] = true; // Marca como visitado.
                distancia[idVizinho] = distancia[idUsuarioAtual] + 1; // Calcula a distância.
                fila[fimFila++] = idVizinho; // Adiciona à fila para visita futura.
                printf("  %s (ID: %d) - Nível %d\n", rede->usuarios[idVizinho].nome, idVizinho, distancia[idVizinho]);
            }
            amigo = amigo->proximo; // Move para o próximo amigo.
        }
    }
}

// Função utilitária recursiva para a Busca em Profundidade (DFS).
// Visita um usuário, marca-o como visitado e, opcionalmente, o adiciona a um grupo.
// Em seguida, chama recursivamente para todos os vizinhos não visitados.
void DFSUtil(RedeSocial* rede, int idUsuario, bool visitado[], int* grupo, int* contadorGrupo) {
    // Marca o usuário atual como visitado.
    visitado[idUsuario] = true;
    // Se os ponteiros 'grupo' e 'contadorGrupo' não forem NULL, adiciona o usuário ao grupo.
    // Isso é usado pela função explorarGrupos.
    if (grupo != NULL && contadorGrupo != NULL) {
        grupo[(*contadorGrupo)++] = idUsuario;
    } else {
        // Caso contrário (chamado diretamente pela DFS), apenas imprime que está visitando.
         printf("  Visitando %s (ID: %d)\n", rede->usuarios[idUsuario].nome, idUsuario);
    }

    // Percorre a lista de amigos do usuário atual.
    NoAmigo* amigo = rede->usuarios[idUsuario].listaAmigos;
    while (amigo != NULL) {
        int idVizinho = amigo->idUsuario;
        // Se o vizinho estiver ativo e não tiver sido visitado.
        if (rede->usuarios[idVizinho].ativo && !visitado[idVizinho]) {
            // Chama recursivamente a DFSUtil para o vizinho.
            DFSUtil(rede, idVizinho, visitado, grupo, contadorGrupo);
        }
        amigo = amigo->proximo; // Move para o próximo amigo.
    }
}

// Função principal para realizar uma Busca em Profundidade (DFS) na rede social a partir de um usuário inicial.
void DFS(RedeSocial* rede, int idUsuarioInicio) {
    // Verifica se o ID do usuário inicial é válido e se ele está ativo.
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inicial inválido para DFS.\n");
        return;
    }

    printf("\n--- DFS a partir de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);
    // Array para marcar os usuários visitados durante a DFS.
    bool visitado[MAX_USUARIOS] = {false};
    // Chama a função utilitária DFSUtil para iniciar a busca.
    // Passa NULL para 'grupo' e 'contadorGrupo' pois aqui o objetivo é apenas mostrar a ordem de visitação.
    DFSUtil(rede, idUsuarioInicio, visitado, NULL, NULL);
}

// Função para sugerir amigos a um usuário.
// Sugere amigos de amigos que ainda não são amigos diretos do usuário.
void sugerirAmigos(RedeSocial* rede, int idUsuario) {
    // Verifica se o ID do usuário é válido e se ele está ativo.
    if (idUsuario < 0 || idUsuario >= MAX_USUARIOS || !rede->usuarios[idUsuario].ativo) {
        printf("Erro: ID de usuário inválido para sugestão de amigos.\n");
        return;
    }

    printf("\n--- Sugestões de Amigos para %s (ID: %d) ---\n", rede->usuarios[idUsuario].nome, idUsuario);

    // Array para marcar quem já é amigo direto ou o próprio usuário (para não sugerir).
    bool ehAmigoDireto[MAX_USUARIOS] = {false};
    // Marca o próprio usuário para não ser sugerido.
    ehAmigoDireto[idUsuario] = true;

    // Percorre a lista de amigos diretos do usuário e marca-os.
    NoAmigo* amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    while (amigoAtual != NULL) {
        ehAmigoDireto[amigoAtual->idUsuario] = true;
        amigoAtual = amigoAtual->proximo;
    }

    int sugestoesEncontradas = 0; // Contador de sugestões.

    // Itera sobre os amigos do usuário (amigos de nível 1).
    amigoAtual = rede->usuarios[idUsuario].listaAmigos;
    while (amigoAtual != NULL) {
        int idAmigoNivel1 = amigoAtual->idUsuario; // ID de um amigo direto.
        // Pula se o amigo de nível 1 não estiver ativo (embora improvável se for amigo).
        if (!rede->usuarios[idAmigoNivel1].ativo) {
            amigoAtual = amigoAtual->proximo;
            continue;
        }

        // Itera sobre os amigos do amigo de nível 1 (amigos de nível 2).
        NoAmigo* amigoDoAmigo = rede->usuarios[idAmigoNivel1].listaAmigos;
        while (amigoDoAmigo != NULL) {
            int idAmigoNivel2 = amigoDoAmigo->idUsuario; // ID de um amigo do amigo.
            // Se o amigo de nível 2 estiver ativo e NÃO for amigo direto (nem o próprio usuário).
            if (rede->usuarios[idAmigoNivel2].ativo && !ehAmigoDireto[idAmigoNivel2]) {
                printf("  - %s (ID: %d) (amigo de %s)\n",
                       rede->usuarios[idAmigoNivel2].nome, idAmigoNivel2,
                       rede->usuarios[idAmigoNivel1].nome);
                // Marca como 'ehAmigoDireto' para não sugerir novamente se for amigo de outro amigo.
                ehAmigoDireto[idAmigoNivel2] = true;
                sugestoesEncontradas++;
            }
            amigoDoAmigo = amigoDoAmigo->proximo;
        }
        amigoAtual = amigoAtual->proximo;
    }
    // Se nenhuma sugestão for encontrada.
    if (sugestoesEncontradas == 0) {
        printf("  Nenhuma sugestão de amigo encontrada no momento.\n");
    }
}

// Verifica a conectividade entre dois usuários usando uma implementação iterativa de DFS (com pilha).
// Retorna true se existe um caminho entre o usuário de origem e o de destino, false caso contrário.
bool verificarConectividade(RedeSocial* rede, int idUsuarioOrigem, int idUsuarioDestino) {
    // Valida os IDs dos usuários e se estão ativos.
    if (idUsuarioOrigem < 0 || idUsuarioOrigem >= MAX_USUARIOS || !rede->usuarios[idUsuarioOrigem].ativo ||
        idUsuarioDestino < 0 || idUsuarioDestino >= MAX_USUARIOS || !rede->usuarios[idUsuarioDestino].ativo) {
        printf("Erro: IDs de usuário inválidos para verificar conectividade.\n");
        return false;
    }

    // Se origem e destino são o mesmo usuário, estão conectados.
    if (idUsuarioOrigem == idUsuarioDestino) return true;

    // Array para marcar usuários visitados.
    bool visitado[MAX_USUARIOS] = {false};
    // Pilha para a DFS iterativa.
    int pilha[MAX_USUARIOS];
    // Topo da pilha.
    int topoPilha = -1;

    // Empilha o usuário de origem e marca como visitado.
    pilha[++topoPilha] = idUsuarioOrigem;
    visitado[idUsuarioOrigem] = true;

    // Loop enquanto a pilha não estiver vazia.
    while (topoPilha != -1) {
        // Desempilha um usuário.
        int u = pilha[topoPilha--];

        // Se o usuário desempilhado é o destino, eles estão conectados.
        if (u == idUsuarioDestino) return true;

        // Percorre os amigos do usuário 'u'.
        NoAmigo* amigo = rede->usuarios[u].listaAmigos;
        while (amigo != NULL) {
            int v = amigo->idUsuario;
            // Se o amigo 'v' está ativo e não foi visitado.
            if (rede->usuarios[v].ativo && !visitado[v]) {
                visitado[v] = true; // Marca como visitado.
                pilha[++topoPilha] = v; // Empilha para visitar.
            }
            amigo = amigo->proximo;
        }
    }
    // Se o loop terminar e o destino não foi alcançado, não há conexão.
    return false;
}

// Função para explorar o grupo social (componente conectado) de um usuário.
// Utiliza DFSUtil para encontrar todos os membros do mesmo grupo que o usuário inicial.
void explorarGrupos(RedeSocial* rede, int idUsuarioInicio) {
    // Verifica se o ID do usuário inicial é válido e se ele está ativo.
    if (idUsuarioInicio < 0 || idUsuarioInicio >= MAX_USUARIOS || !rede->usuarios[idUsuarioInicio].ativo) {
        printf("Erro: ID de usuário inválido para explorar grupos.\n");
        return;
    }

    printf("\n--- Explorando Grupo Social de %s (ID: %d) ---\n", rede->usuarios[idUsuarioInicio].nome, idUsuarioInicio);
    // Array para marcar usuários visitados.
    bool visitado[MAX_USUARIOS] = {false};
    // Array para armazenar os IDs dos usuários no mesmo grupo.
    int grupo[MAX_USUARIOS];
    // Contador para o número de membros no grupo.
    int contadorGrupo = 0;

    // Chama DFSUtil para encontrar todos os usuários conectados ao usuário inicial.
    // Os membros encontrados serão armazenados no array 'grupo'.
    DFSUtil(rede, idUsuarioInicio, visitado, grupo, &contadorGrupo);

    // Se membros foram encontrados no grupo.
    if (contadorGrupo > 0) {
        printf("Membros do grupo:\n");
        // Lista todos os membros do grupo.
        for (int i = 0; i < contadorGrupo; i++) {
            printf("  - %s (ID: %d)\n", rede->usuarios[grupo[i]].nome, grupo[i]);
        }
    } else {
        printf("Nenhum membro encontrado (o usuário pode estar isolado ou inativo).\n");
    }
}

// Função para liberar toda a memória alocada dinamicamente para a rede social.
// Libera as listas de amigos de cada usuário e depois a própria estrutura da rede.
void liberarRedeSocial(RedeSocial* rede) {
    // Se a rede for NULL, não há nada a fazer.
    if (rede == NULL) return;
    // Itera por todos os possíveis slots de usuário.
    for (int i = 0; i < MAX_USUARIOS; i++) {
        // Se o usuário estiver ativo (e, portanto, pode ter uma lista de amigos alocada).
        if (rede->usuarios[i].ativo) {
            NoAmigo* atual = rede->usuarios[i].listaAmigos;
            // Percorre a lista de amigos do usuário, liberando cada nó.
            while (atual != NULL) {
                NoAmigo* temp = atual; // Guarda o nó atual.
                atual = atual->proximo; // Avança para o próximo.
                free(temp);             // Libera o nó guardado.
            }
            // Define a lista de amigos como NULL após liberar todos os nós.
            rede->usuarios[i].listaAmigos = NULL;
        }
    }
    // Libera a memória da própria estrutura da rede social.
    free(rede);
    printf("Memória da rede social liberada.\n");
}

// Função para exibir o menu principal de opções da rede social.
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

// Função para listar todos os usuários ativos na rede social.
void listarUsuariosAtivos(RedeSocial* rede) {
    printf("\n--- Usuários Ativos na Rede ---\n");
    int count = 0; // Contador de usuários ativos encontrados.
    // Itera por todos os possíveis slots de usuário.
    for (int i = 0; i < MAX_USUARIOS; i++) {
        // Se o usuário no slot 'i' estiver ativo.
        if (rede->usuarios[i].ativo) {
            printf("ID: %d, Nome: %s\n", rede->usuarios[i].id, rede->usuarios[i].nome);
            count++;
        }
    }
    // Se nenhum usuário ativo for encontrado.
    if (count == 0) {
        printf("Nenhum usuário ativo na rede.\n");
    }
}


// Função principal do programa.
int main() {
    // Tenta configurar a localidade para pt_BR.UTF-8 para exibir caracteres acentuados corretamente.
    // Faz algumas tentativas com diferentes strings de localidade comuns para sistemas Windows e Linux.
    if (setlocale(LC_ALL, "pt_BR.UTF-8") == NULL) {
        if (setlocale(LC_ALL, "pt_BR") == NULL) {
            if (setlocale(LC_ALL, "Portuguese_Brazil.1252") == NULL) {
                // Se todas as tentativas falharem, informa um erro e usa a localidade padrão do sistema.
                fprintf(stderr, "Erro ao definir a localização para o Brasil!\n");
                setlocale(LC_ALL, ""); // Define para a localidade padrão do ambiente.
            }
        }
    }

    // Cria a rede social.
    RedeSocial* rede = criarRedeSocial();
    // Variáveis para armazenar a opção do menu, nome e IDs de usuários.
    int opcao;
    char nome[MAX_NOME_USUARIO];
    int id1, id2;

    // Adiciona alguns usuários iniciais para teste e demonstração.
    int idAlice = adicionarUsuario(rede, "Alice");
    int idBob = adicionarUsuario(rede, "Bob");
    int idCharlie = adicionarUsuario(rede, "Charlie");
    int idDavid = adicionarUsuario(rede, "David");
    int idEve = adicionarUsuario(rede, "Eve");
    int idFrank = adicionarUsuario(rede, "Frank"); // Usuário isolado inicialmente

    // Cria algumas conexões iniciais entre os usuários, verificando se os IDs são válidos.
    if(idAlice != -1 && idBob != -1) criarConexao(rede, idAlice, idBob);
    if(idAlice != -1 && idCharlie != -1) criarConexao(rede, idAlice, idCharlie);
    if(idBob != -1 && idDavid != -1) criarConexao(rede, idBob, idDavid);
    if(idCharlie != -1 && idDavid != -1) criarConexao(rede, idCharlie, idDavid);
    if(idDavid != -1 && idEve != -1) criarConexao(rede, idDavid, idEve);


    // Loop principal do menu da aplicação. Continua até o usuário escolher a opção 0 (Sair).
    do {
        exibirMenuRedeSocial(); // Mostra o menu.
        // Lê a opção do usuário.
        if (scanf("%d", &opcao) != 1) { // Verifica se a entrada é um número.
            printf("Entrada inválida. Por favor, insira um número.\n");
            while (getchar() != '\n'); // Limpa o buffer de entrada.
            opcao = -1; // Define uma opção inválida para continuar o loop.
            continue;   // Volta para o início do loop.
        }
        getchar(); // Consome o caractere newline deixado pelo scanf.

        // Estrutura switch para tratar a opção escolhida pelo usuário.
        switch (opcao) {
            case 1: // Adicionar Usuário
                printf("Digite o nome do novo usuário: ");
                fgets(nome, MAX_NOME_USUARIO, stdin); // Lê o nome.
                nome[strcspn(nome, "\n")] = 0;      // Remove o newline do final da string lida por fgets.
                adicionarUsuario(rede, nome);
                break;
            case 2: // Criar Conexão
                printf("Digite o ID do primeiro usuário: ");
                // Lê e valida o primeiro ID.
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID do segundo usuário: ");
                // Lê e valida o segundo ID.
                if (scanf("%d", &id2) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                criarConexao(rede, id1, id2);
                break;
            case 3: // Visualizar Rede de Amizades
                printf("Digite o ID do usuário para ver seus amigos: ");
                 if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                visualizarRedeAmizades(rede, id1);
                break;
            case 4: // Busca em Largura (BFS)
                printf("Digite o ID do usuário para iniciar BFS: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                BFS(rede, id1);
                break;
            case 5: // Busca em Profundidade (DFS)
                printf("Digite o ID do usuário para iniciar DFS: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                DFS(rede, id1);
                break;
            case 6: // Sugerir Amigos
                printf("Digite o ID do usuário para sugerir amigos: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                sugerirAmigos(rede, id1);
                break;
            case 7: // Explorar Grupo Social
                printf("Digite o ID do usuário para explorar seu grupo social: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                explorarGrupos(rede, id1);
                break;
            case 8: // Verificar Conectividade
                printf("Digite o ID do usuário de origem: ");
                if (scanf("%d", &id1) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                printf("Digite o ID do usuário de destino: ");
                if (scanf("%d", &id2) != 1) { printf("ID inválido.\n"); while (getchar() != '\n'); break; }
                getchar(); // Consome o newline.
                // Verifica a conectividade e imprime o resultado.
                if (verificarConectividade(rede, id1, id2)) {
                    printf("%s (ID: %d) e %s (ID: %d) ESTÃO conectados.\n", rede->usuarios[id1].nome, id1, rede->usuarios[id2].nome, id2);
                } else {
                    printf("%s (ID: %d) e %s (ID: %d) NÃO ESTÃO conectados.\n", rede->usuarios[id1].nome, id1, rede->usuarios[id2].nome, id2);
                }
                break;
            case 9: // Listar todos os usuários ativos
                listarUsuariosAtivos(rede);
                break;
            case 0: // Sair
                printf("Saindo da Rede Social Simulada...\n");
                break;
            default: // Opção inválida
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0); // Continua o loop até que a opção seja 0.

    // Libera a memória alocada para a rede social antes de encerrar o programa.
    liberarRedeSocial(rede);
    // Retorna 0 indicando que o programa terminou com sucesso.
    return 0;
}