// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CODIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o codigo em funcoes especializadas.
// - Implementar um sistema de missoes para um jogador.
// - Criar uma funcao para verificar se a missao foi cumprida.
// - Utilizar passagem por referencia (ponteiros) para modificar dados e
//   passagem por valor/referencia constante (const) para apenas ler.
// - Foco em: Design de software, modularizacao, const correctness, logica de jogo.
//
// ============================================================================

// Inclusao das bibliotecas padrao necessarias para entrada/saida, alocacao de memoria, manipulacao de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
// Definem valores fixos para o numero de territorios, missoes e tamanho maximo de strings, facilitando a manutencao.
#define NUM_TERRITORIOS 42
#define NUM_MISSOES 4
#define TAM_NOME 50
#define TAM_COR 20

// --- Estrutura de Dados ---
// Define a estrutura para um territorio, contendo seu nome, a cor do exercito que o domina e o numero de tropas.
typedef struct {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
} Territorio;

// --- Prototipos das Funcoes ---
// Declaracoes antecipadas de todas as funcoes que serao usadas no programa, organizadas por categoria.
// Funcoes de setup e gerenciamento de memoria:
Territorio* alocarMapa();
void inicializarTerritorios(Territorio* mapa);
void liberarMemoria(Territorio* mapa);

// Funcoes de interface com o usuario:
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa);
void exibirMissao(int missaoId);

// Funcoes de logica principal do jogo:
void faseDeAtaque(Territorio* mapa);
void simularAtaque(Territorio* mapa, int origem, int destino);
int sortearMissao();
int verificarVitoria(const Territorio* mapa, int missaoId, const char* corJogador);

// Funcao utilitaria:
void limparBufferEntrada();

// --- Funcao Principal (main) ---
// Funcao principal que orquestra o fluxo do jogo, chamando as outras funcoes em ordem.
int main() {
    // 1. Configuracao Inicial (Setup):
    // - Define o locale para portugues do Brasil.
    char* locale_result = setlocale(LC_ALL, "Portuguese_Brazil");
    if (locale_result == NULL) {
        // Tenta alternativa se "Portuguese_Brazil" nao estiver disponivel
        locale_result = setlocale(LC_ALL, "pt_BR");
        if (locale_result == NULL) {
            locale_result = setlocale(LC_ALL, "Portuguese");
        }
    }
    
    // - Inicializa a semente para geracao de numeros aleatorios com base no tempo atual.
    srand(time(NULL));
    
    // - Aloca a memoria para o mapa do mundo e verifica se a alocacao foi bem-sucedida.
    Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro: Nao foi possivel alocar memoria para o mapa!\n");
        return 1;
    }
    
    // - Preenche os territorios com seus dados iniciais (tropas, donos, etc.).
    inicializarTerritorios(mapa);
    
    // - Define a cor do jogador e sorteia sua missao secreta.
    char corJogador[TAM_COR] = "Azul";
    int missaoId = sortearMissao();
    
    printf("=== BEM-VINDO AO JOGO WAR ===\n");
    printf("Voce esta jogando com o exercito %s!\n", corJogador);
    printf("Sua missao secreta foi sorteada...\n\n");

    // 2. Laco Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua ate o jogador sair (opcao 0) ou vencer.
    int opcao;
    int vitoria = 0;
    
    do {
        // - A cada iteracao, exibe o mapa, a missao e o menu de acoes.
        exibirMapa(mapa);
        exibirMissao(missaoId);
        exibirMenuPrincipal();
        
        // - Le a escolha do jogador e usa um 'switch' para chamar a funcao apropriada:
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBufferEntrada();
        
        switch (opcao) {
            case 1:
                // - Opcao 1: Inicia a fase de ataque.
                faseDeAtaque(mapa);
                break;
            case 2:
                // - Opcao 2: Verifica se a condicao de vitoria foi alcancada e informa o jogador.
                vitoria = verificarVitoria(mapa, missaoId, corJogador);
                if (vitoria) {
                    printf("\n*** PARABENS! Voce cumpriu sua missao e venceu o jogo! ***\n");
                } else {
                    printf("\n*** Voce ainda nao cumpriu sua missao. Continue tentando! ***\n");
                }
                break;
            case 0:
                printf("\nObrigado por jogar! Ate a proxima!\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
                break;
        }
        
        // - Pausa a execucao para que o jogador possa ler os resultados antes da proxima rodada.
        if (opcao != 0 && !vitoria) {
            printf("\nPressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0 && !vitoria);

    // 3. Limpeza:
    // - Ao final do jogo, libera a memoria alocada para o mapa para evitar vazamentos de memoria.
    liberarMemoria(mapa);

    return 0;
}

// --- Implementacao das Funcoes ---

// alocarMapa():
// Aloca dinamicamente a memoria para o vetor de territorios usando calloc.
// Retorna um ponteiro para a memoria alocada ou NULL em caso de falha.
Territorio* alocarMapa() {
    Territorio* mapa = (Territorio*)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada territorio no mapa (nome, cor do exercito, numero de tropas).
// Esta funcao modifica o mapa passado por referencia (ponteiro).
void inicializarTerritorios(Territorio* mapa) {
    // Nomes dos territorios do jogo War (sem acentos para evitar problemas de encoding)
    char nomes[NUM_TERRITORIOS][TAM_NOME] = {
        "Alaska", "Alberta", "America Central", "America do Sul", "Argentina", "Brasil",
        "Groenlandia", "Mackenzie", "Nova York", "Ontario", "Quebec", "Territorio Noroeste",
        "Venezuela", "Africa do Sul", "Congo", "Egito", "Madagascar", "Norte da Africa",
        "Africa Oriental", "Afeganistao", "China", "India", "Irkutsk", "Japao",
        "Kamchatka", "Mongolia", "Oriente Medio", "Siberia", "Sudeste Asiatico", "Siam",
        "Ural", "Yakutsk", "Australia Oriental", "Indonesia", "Nova Guine", "Australia Ocidental",
        "Europa Ocidental", "Europa Oriental", "Gra-Bretanha", "Islandia", "Escandinavia", "Ucrania"
    };
    
    // Cores dos exercitos (distribuidas aleatoriamente)
    char cores[6][TAM_COR] = {"Azul", "Vermelho", "Verde", "Amarelo", "Rosa", "Roxo"};
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        strcpy(mapa[i].cor, cores[rand() % 6]);
        mapa[i].tropas = 1 + (rand() % 5); // Entre 1 e 5 tropas
    }
}

// liberarMemoria():
// Libera a memoria previamente alocada para o mapa usando free.
void liberarMemoria(Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

// exibirMenuPrincipal():
// Imprime na tela o menu de acoes disponiveis para o jogador.
void exibirMenuPrincipal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Atacar territorio\n");
    printf("2. Verificar vitoria\n");
    printf("0. Sair do jogo\n");
    printf("=====================\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territorios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a funcao apenas leia os dados do mapa, sem modifica-los.
void exibirMapa(const Territorio* mapa) {
    printf("\n=== MAPA DO MUNDO ===\n");
    printf("%-3s | %-25s | %-10s | %s\n", "ID", "Territorio", "Exercito", "Tropas");
    printf("----|---------------------------|------------|--------\n");
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("%-3d | %-25s | %-10s | %d\n", 
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("===============================\n");
}

// exibirMissao():
// Exibe a descricao da missao atual do jogador com base no ID da missao sorteada.
void exibirMissao(int missaoId) {
    printf("\n=== SUA MISSAO SECRETA ===\n");
    
    switch (missaoId) {
        case 1:
            printf("*** MISSAO: Destruir o exercito VERMELHO ***\n");
            printf("   Objetivo: Eliminar todas as tropas vermelhas do mapa\n");
            break;
        case 2:
            printf("*** MISSAO: Destruir o exercito VERDE ***\n");
            printf("   Objetivo: Eliminar todas as tropas verdes do mapa\n");
            break;
        case 3:
            printf("*** MISSAO: Conquistar 18 territorios ***\n");
            printf("   Objetivo: Controlar pelo menos 18 territorios com seu exercito\n");
            break;
        case 4:
            printf("*** MISSAO: Conquistar 24 territorios ***\n");
            printf("   Objetivo: Controlar pelo menos 24 territorios com seu exercito\n");
            break;
        default:
            printf("*** MISSAO: Conquistar 15 territorios ***\n");
            printf("   Objetivo: Controlar pelo menos 15 territorios com seu exercito\n");
            break;
    }
    printf("==========================\n");
}

// faseDeAtaque():
// Gerencia a interface para a acao de ataque, solicitando ao jogador os territorios de origem e destino.
// Chama a funcao simularAtaque() para executar a logica da batalha.
void faseDeAtaque(Territorio* mapa) {
    int origem, destino;
    
    printf("\n=== FASE DE ATAQUE ===\n");
    printf("Escolha o territorio de origem (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &origem);
    limparBufferEntrada();
    
    printf("Escolha o territorio de destino (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &destino);
    limparBufferEntrada();
    
    // Validacao basica
    if (origem < 1 || origem > NUM_TERRITORIOS || destino < 1 || destino > NUM_TERRITORIOS) {
        printf("*** Territorios invalidos! Tente novamente. ***\n");
        return;
    }
    
    if (origem == destino) {
        printf("*** Voce nao pode atacar o mesmo territorio! Tente novamente. ***\n");
        return;
    }
    
    // Verificar se o territorio de origem pertence ao jogador (Azul)
    if (strcmp(mapa[origem-1].cor, "Azul") != 0) {
        printf("*** Voce so pode atacar com territorios que pertencem ao seu exercito (Azul)! ***\n");
        return;
    }
    
    // Verificar se o territorio de destino nao pertence ao jogador
    if (strcmp(mapa[destino-1].cor, "Azul") == 0) {
        printf("*** Voce nao pode atacar seus proprios territorios! ***\n");
        return;
    }
    
    // Verificar se o territorio de origem tem tropas suficientes
    if (mapa[origem-1].tropas < 2) {
        printf("*** Voce precisa de pelo menos 2 tropas para atacar! ***\n");
        return;
    }
    
    simularAtaque(mapa, origem-1, destino-1);
}

// simularAtaque():
// Executa a logica de uma batalha entre dois territorios.
// Realiza validacoes, rola os dados, compara os resultados e atualiza o numero de tropas.
// Se um territorio for conquistado, atualiza seu dono e move uma tropa.
void simularAtaque(Territorio* mapa, int origem, int destino) {
    printf("\n*** BATALHA: %s vs %s ***\n", mapa[origem].nome, mapa[destino].nome);
    printf("Exercito atacante: %s (%d tropas)\n", mapa[origem].cor, mapa[origem].tropas);
    printf("Exercito defensor: %s (%d tropas)\n", mapa[destino].cor, mapa[destino].tropas);
    
    // Simular dados (1-6 para cada tropa)
    int tropasAtacante = mapa[origem].tropas - 1; // -1 porque uma tropa fica no territorio
    int tropasDefensor = mapa[destino].tropas;
    
    printf("\n*** Rolando dados... ***\n");
    printf("Tropas atacantes: %d | Tropas defensoras: %d\n", tropasAtacante, tropasDefensor);
    printf("----------------------------------------\n");
    
    // Simular batalha
    int vitoriasAtacante = 0;
    int vitoriasDefensor = 0;
    
    // Cada tropa rola um dado
    for (int i = 0; i < tropasAtacante && i < tropasDefensor; i++) {
        int dadoAtacante = 1 + (rand() % 6);
        int dadoDefensor = 1 + (rand() % 6);
        
        printf("Confronto %d: Atacante rola %d | Defensor rola %d", i+1, dadoAtacante, dadoDefensor);
        
        if (dadoAtacante > dadoDefensor) {
            vitoriasAtacante++;
            printf(" -> VITORIA DO ATACANTE! (+1)\n");
        } else {
            vitoriasDefensor++;
            printf(" -> VITORIA DO DEFENSOR! (+1)\n");
        }
    }
    
    printf("----------------------------------------\n");
    
    printf("\n*** RESULTADO DA BATALHA: ***\n");
    printf("Vitorias do atacante: %d\n", vitoriasAtacante);
    printf("Vitorias do defensor: %d\n", vitoriasDefensor);
    
    if (vitoriasAtacante > vitoriasDefensor) {
        printf("*** RESULTADO: ATACANTE VENCEU! ***\n");
    } else if (vitoriasDefensor > vitoriasAtacante) {
        printf("*** RESULTADO: DEFENSOR VENCEU! ***\n");
    } else {
        printf("*** RESULTADO: EMPATE! ***\n");
    }
    
    // Atualizar tropas
    mapa[destino].tropas -= vitoriasAtacante;
    mapa[origem].tropas -= vitoriasDefensor;
    
    if (mapa[destino].tropas <= 0) {
        // Territorio conquistado!
        printf("\n*** TERRITORIO CONQUISTADO! ***\n");
        printf("%s agora pertence ao exercito %s!\n", mapa[destino].nome, mapa[origem].cor);
        
        // Transferir o territorio para o atacante
        strcpy(mapa[destino].cor, mapa[origem].cor);
        mapa[destino].tropas = 1; // Minimo de 1 tropa
        
        // Mover uma tropa do territorio de origem
        if (mapa[origem].tropas > 1) {
            mapa[origem].tropas--;
            mapa[destino].tropas++;
        }
    } else {
        printf("\n*** Territorio defendido com sucesso! ***\n");
    }
    
    printf("\nEstado final:\n");
    printf("%s: %d tropas (%s)\n", mapa[origem].nome, mapa[origem].tropas, mapa[origem].cor);
    printf("%s: %d tropas (%s)\n", mapa[destino].nome, mapa[destino].tropas, mapa[destino].cor);
}

// sortearMissao():
// Sorteia e retorna um ID de missao aleatorio para o jogador.
int sortearMissao() {
    return 1 + (rand() % NUM_MISSOES);
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missao atual.
// Implementa a logica para cada tipo de missao (destruir um exercito ou conquistar um numero de territorios).
// Retorna 1 (verdadeiro) se a missao foi cumprida, e 0 (falso) caso contrario.
int verificarVitoria(const Territorio* mapa, int missaoId, const char* corJogador) {
    switch (missaoId) {
        case 1: // Destruir exercito vermelho
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor, "Vermelho") == 0) {
                    return 0; // Ainda ha tropas vermelhas
                }
            }
            return 1; // Todas as tropas vermelhas foram eliminadas
            
        case 2: // Destruir exercito verde
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor, "Verde") == 0) {
                    return 0; // Ainda ha tropas verdes
                }
            }
            return 1; // Todas as tropas verdes foram eliminadas
            
        case 3: // Conquistar 18 territorios
            {
                int territoriosConquistados = 0;
                for (int i = 0; i < NUM_TERRITORIOS; i++) {
                    if (strcmp(mapa[i].cor, corJogador) == 0) {
                        territoriosConquistados++;
                    }
                }
                return (territoriosConquistados >= 18) ? 1 : 0;
            }
            
        case 4: // Conquistar 24 territorios
            {
                int territoriosConquistados = 0;
                for (int i = 0; i < NUM_TERRITORIOS; i++) {
                    if (strcmp(mapa[i].cor, corJogador) == 0) {
                        territoriosConquistados++;
                    }
                }
                return (territoriosConquistados >= 24) ? 1 : 0;
            }
            
        default: // Conquistar 15 territorios
            {
                int territoriosConquistados = 0;
                for (int i = 0; i < NUM_TERRITORIOS; i++) {
                    if (strcmp(mapa[i].cor, corJogador) == 0) {
                        territoriosConquistados++;
                    }
                }
                return (territoriosConquistados >= 15) ? 1 : 0;
            }
    }
}

// limparBufferEntrada():
// Funcao utilitaria para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
