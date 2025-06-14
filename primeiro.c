#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_SENSORES 5
#define LEITURAS_POR_SENSOR 2000
#define TAM_ID 20
#define TAM_TIPO 10

typedef struct {
    int dia, mes, ano;
} Data;

typedef struct {
    int hora, min, seg;
} Hora;

int validarFormatoData(const char *data, Data *d) {
    if (sscanf(data, "%d/%d/%d", &d->dia, &d->mes, &d->ano) != 3) return 0;
    if (d->dia < 1 || d->dia > 31 || d->mes < 1 || d->mes > 12 || d->ano < 1970) return 0;
    return 1;
}

int validarFormatoHora(const char *hora, Hora *h) {
    if (sscanf(hora, "%d:%d:%d", &h->hora, &h->min, &h->seg) != 3) return 0;
    if (h->hora < 0 || h->hora > 23 || h->min < 0 || h->min > 59 || h->seg < 0 || h->seg > 59) return 0;
    return 1;
}

int validarTipoDado(const char *tipo) {
    return strcmp(tipo, "int") == 0 || strcmp(tipo, "float") == 0 || strcmp(tipo, "bool") == 0 || strcmp(tipo, "string") == 0;
}

time_t montarTimestamp(Data d, Hora h) {
    struct tm t;
    t.tm_year = d.ano - 1900;
    t.tm_mon = d.mes - 1;
    t.tm_mday = d.dia;
    t.tm_hour = h.hora;
    t.tm_min = h.min;
    t.tm_sec = h.seg;
    t.tm_isdst = -1;
    return mktime(&t);
}

time_t lerDataHoraValidaComLimite(const char *tipo, time_t inicio, Data *dataInicio, Hora *horaInicio, int verificarLimite) {
    char entrada[100];
    Data d;
    Hora h;
    time_t timestamp;

    while (1) {
        printf("Digite a data de %s (dd/mm/aaaa):\n", tipo);
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = '\0';
        if (!validarFormatoData(entrada, &d)) {
            printf("Formato da data inválido. Tente novamente.\n");
            continue;
        }

        while (1) {
            printf("Digite a hora de %s (hh:mm:ss):\n", tipo);
            fgets(entrada, sizeof(entrada), stdin);
            entrada[strcspn(entrada, "\n")] = '\0';
            if (!validarFormatoHora(entrada, &h)) {
                printf("Formato da hora inválido. Tente novamente.\n");
                continue;
            }
            break;
        }

        timestamp = montarTimestamp(d, h);

        if (verificarLimite) {
            if (timestamp < inicio) {
                printf("A data de fim não pode ser anterior à de início.\n");
                continue;
            } else if (timestamp == inicio) {
                if (d.dia == dataInicio->dia && d.mes == dataInicio->mes && d.ano == dataInicio->ano) {
                    if (h.hora < horaInicio->hora || (h.hora == horaInicio->hora && h.min < horaInicio->min) ||
                        (h.hora == horaInicio->hora && h.min == horaInicio->min && h.seg < horaInicio->seg)) {
                        printf("A data de fim não pode ser anterior à de início.\n");
                        continue;
                    }
                }
            }
        }

        break;
    }

    return timestamp;
}


void gerarValorFormatado(const char *tipo, char *saida) {
    if (strcmp(tipo, "int") == 0) {
        int valor = rand() % 100;
        sprintf(saida, "%d", valor);
    } else if (strcmp(tipo, "float") == 0) {
        float valor = ((float)rand() / RAND_MAX) * 100.0f;
        sprintf(saida, "%.2f", valor);
    } else if (strcmp(tipo, "bool") == 0) {
        int b = rand() % 2;
        strcpy(saida, b ? "true" : "false");
    } else if (strcmp(tipo, "string") == 0) {
        int len = 6 + rand() % 5;
        for (int i = 0; i < len; i++) {
            saida[i] = 'a' + rand() % 26;
        }
        saida[len] = '\0';
    } else {
        strcpy(saida, "erro");
    }
}

int main() {
    char sensores[MAX_SENSORES][TAM_ID];
    char tipos[MAX_SENSORES][TAM_TIPO];
    int qtdSensores;
    Data dataInicio;
    Hora horaInicio;

    srand(time(NULL));

    while (1) {
        printf("Quantos sensores deseja simular (1 a %d)? ", MAX_SENSORES);
        if (scanf("%d", &qtdSensores) != 1 || qtdSensores < 1 || qtdSensores > MAX_SENSORES) {
            printf("Quantidade inválida. Digite um número entre 1 e %d.\n", MAX_SENSORES);
            while (getchar() != '\n'); 
            continue;
        }
        while (getchar() != '\n'); // impede estrouro de buffer =)
        break;
    }

    for (int i = 0; i < qtdSensores; i++) {
        printf("Nome do sensor %d: ", i + 1);
        fgets(sensores[i], sizeof(sensores[i]), stdin);
        sensores[i][strcspn(sensores[i], "\n")] = '\0';

        while (1) {
            printf("Tipo de dado (int, float, bool, string) para %s: ", sensores[i]);
            fgets(tipos[i], sizeof(tipos[i]), stdin);
            tipos[i][strcspn(tipos[i], "\n")] = '\0';
            if (validarTipoDado(tipos[i])) break;
            printf("Tipo inválido. Use apenas: int, float, bool ou string.\n");
        }
    }

    time_t inicio = lerDataHoraValidaComLimite("início", 0, &dataInicio, &horaInicio, 0);
    time_t fim = lerDataHoraValidaComLimite("fim", inicio, &dataInicio, &horaInicio, 1);

    FILE *arquivo = fopen("Parte1DoTrabalho.txt", "w");
    if (!arquivo) {
        printf("Erro ao criar o arquivo.\n");
        return 1;
    }

    char valorFormatado[64];
    for (int i = 0; i < qtdSensores; i++) {
        for (int j = 0; j < LEITURAS_POR_SENSOR; j++) {
            time_t t = inicio + rand() % (fim - inicio + 1);
            gerarValorFormatado(tipos[i], valorFormatado);
            fprintf(arquivo, "%ld %s %s\n", t, sensores[i], valorFormatado);
        }
    }

    fclose(arquivo);
    printf("Arquivo Parte1DoTrabalho.txt criado com sucesso!\n");
    return 0;
}
