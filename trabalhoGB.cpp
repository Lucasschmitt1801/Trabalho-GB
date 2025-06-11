#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_EQUIPE 5

typedef struct {
    char classe[20];
    int vida;
    int ataque;
    int defesa;
    int habilidade_ativa; 
} Personagem;

void inicializar_equipe(Personagem equipe[], int numero);
int escolher_atacante(Personagem equipe[]);
int escolher_alvo(Personagem equipe[]);
void aplicar_ataque(Personagem* atacante, Personagem* defensor);
void imprimir_estado(Personagem time1[], Personagem time2[]);
int time_vivo(Personagem equipe[]);
int habilidade_ativada(const char* classe);

int main() {
    srand(time(NULL));

    Personagem time1[TAM_EQUIPE];
    Personagem time2[TAM_EQUIPE];

    inicializar_equipe(time1, 1);
    inicializar_equipe(time2, 2);

    int rodada = 1;
    int turno = rand() % 2; 

    while (time_vivo(time1) > 0 && time_vivo(time2) > 0) {
        printf("\n>>> Rodada %d:\n", rodada);

        if (turno == 0) {
            int idx_atacante = escolher_atacante(time1);
            int idx_alvo = escolher_alvo(time2);
            printf("Time 1 — %s (Vida: %d) ataca %s do Time 2 (Vida: %d)\n",
                   time1[idx_atacante].classe, time1[idx_atacante].vida,
                   time2[idx_alvo].classe, time2[idx_alvo].vida);
            aplicar_ataque(&time1[idx_atacante], &time2[idx_alvo]);
        } else {
            int idx_atacante = escolher_atacante(time2);
            int idx_alvo = escolher_alvo(time1);
            printf("Time 2 — %s (Vida: %d) ataca %s do Time 1 (Vida: %d)\n",
                   time2[idx_atacante].classe, time2[idx_atacante].vida,
                   time1[idx_alvo].classe, time1[idx_alvo].vida);
            aplicar_ataque(&time2[idx_atacante], &time1[idx_alvo]);
        }

        imprimir_estado(time1, time2);

        rodada++;
        turno = 1 - turno; // alterna o time
    }

    // Resultado final
    if (time_vivo(time1) > 0 && time_vivo(time2) == 0) {
        printf("\n>>> Fim de jogo! Time 1 venceu.\n");
    } else if (time_vivo(time2) > 0 && time_vivo(time1) == 0) {
        printf("\n>>> Fim de jogo! Time 2 venceu.\n");
    } else {
        printf("\n>>> Fim de jogo! Empate.\n");
    }

    return 0;
}

void inicializar_equipe(Personagem equipe[], int numero) {
    char* classes[5] = {"Guerreiro", "Mago", "Caçador", "Paladino", "Bárbaro"};
    int ataques[5] = {20, 30, 18, 15, 25};
    int defesas[5] = {10, 5, 8, 12, 6};

    for (int i = 0; i < TAM_EQUIPE; i++) {
        strcpy(equipe[i].classe, classes[i]);
        equipe[i].vida = 100;
        equipe[i].ataque = ataques[i];
        equipe[i].defesa = defesas[i];
        equipe[i].habilidade_ativa = 0;
    }
}

int escolher_atacante(Personagem equipe[]) {
    int idx = -1;
    float melhor = 0.0;

    for (int i = 0; i < TAM_EQUIPE; i++) {
        if (equipe[i].vida > 0) {
            float razao = (float)equipe[i].vida / equipe[i].ataque;
            if (razao > melhor) {
                melhor = razao;
                idx = i;
            }
        }
    }
    return idx;
}

int escolher_alvo(Personagem equipe[]) {
    int vivos[TAM_EQUIPE];
    int n = 0;
    for (int i = 0; i < TAM_EQUIPE; i++) {
        if (equipe[i].vida > 0) vivos[n++] = i;
    }
    if (n == 0) return -1;
    return vivos[rand() % n];
}

int habilidade_ativada(const char* classe) {
    if (strcmp(classe, "Guerreiro") == 0)
        return rand() % 100 < 20;
    if (strcmp(classe, "Mago") == 0)
        return rand() % 100 < 25;
    if (strcmp(classe, "Caçador") == 0)
        return rand() % 100 < 15;
    if (strcmp(classe, "Paladino") == 0)
        return rand() % 100 < 30;
    if (strcmp(classe, "Bárbaro") == 0)
        return 1; // Sempre ativa
    return 0;
}

void aplicar_ataque(Personagem* atacante, Personagem* defensor) {
    int erro_ataque = rand() % 100 < 20;
    int falha_defesa = rand() % 100 < 20;
    int ativou_habilidade = habilidade_ativada(atacante->classe);

    // Bárbaro nunca erra
    if (strcmp(atacante->classe, "Bárbaro") == 0) erro_ataque = 0;

    if (erro_ataque) {
        printf("→ Ataque errou! Nenhum dano aplicado.\n");
        return;
    }

    int ataque_total = atacante->ataque;

    // Habilidades
    if (ativou_habilidade) {
        if (strcmp(atacante->classe, "Guerreiro") == 0) {
            ataque_total *= 2;
            printf("→ Habilidade especial do Guerreiro ativada: Golpe Crítico\n");
        } else if (strcmp(atacante->classe, "Mago") == 0) {
            falha_defesa = 1;
            printf("→ Habilidade especial do Mago ativada: Bola de Fogo\n");
        } else if (strcmp(atacante->classe, "Caçador") == 0) {
            printf("→ Habilidade especial do Caçador ativada: Ataque Duplo\n");
            aplicar_ataque(atacante, defensor);
        }
    }

    int defesa_real = falha_defesa ? 0 : defensor->defesa;
    int dano = ataque_total - defesa_real;
    if (dano < 0) dano = 0;

    defensor->vida -= dano;
    if (defensor->vida < 0) defensor->vida = 0;

    printf("→ Dano aplicado: %d\n", dano);

    // Paladino pode regenerar após sofrer dano
    if (strcmp(defensor->classe, "Paladino") == 0 && defensor->vida > 0) {
        if (habilidade_ativada(defensor->classe)) {
            int vida_perdida = 100 - defensor->vida;
            int recuperado = vida_perdida * 0.2;
            defensor->vida += recuperado;
            printf("→ Habilidade especial do Paladino ativada: Regeneração (+%d vida)\n", recuperado);
        }
    }
}

void imprimir_estado(Personagem time1[], Personagem time2[]) {
    printf("\n>>> Estado Atual:\n");
    printf("Time 1:\n");
    for (int i = 0; i < TAM_EQUIPE; i++) {
        printf("  %s: Vida %d\n", time1[i].classe, time1[i].vida);
    }
    printf("Time 2:\n");
    for (int i = 0; i < TAM_EQUIPE; i++) {
        printf("  %s: Vida %d\n", time2[i].classe, time2[i].vida);
    }
}

int time_vivo(Personagem equipe[]) {
    int vivos = 0;
    for (int i = 0; i < TAM_EQUIPE; i++) {
        if (equipe[i].vida > 0) vivos++;
    }
    return vivos;
}

