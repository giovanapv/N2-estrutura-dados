// esse era o antigo arquivo 1, mas como o arquivo 3 tinha que ser rodado primeiro entt o 3 virou o 1, o 1 viroi o 2 e o 2 virou o 3
// nesse eu primeiro fez as melhorias que o professor comentou no meu primeiro código e tentei ficar bem atenta com as validacoes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define QuantidadeMaximaDeSensores 10
#define QuantidadeMaximaDeLinhas 256
#define TamanhoDoId 60
#define TAM_MAX_STRING 17
#define TAM_MAX_VALOR 64

typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_BOOL,
    TIPO_STRING
} TipoDado;

typedef union {
    int inteiro;
    float racional;
    char booleano[6];
    char texto[TAM_MAX_STRING];
} ValorSensor;

typedef struct {
    long timestamp;
    ValorSensor valor;
} Leitura;

typedef struct {
    char IdDoSensor[TamanhoDoId];
    TipoDado tipo;
    Leitura *leituras;
    int QuantidadeDeLeitura;
    int capacidade;
} Sensor;


TipoDado DescobrirTipo(const char *valor) {
    if (strcmp(valor, "true") == 0 || strcmp(valor, "false") == 0) {
        return TIPO_BOOL;
    }

    char *endptr;

    strtol(valor, &endptr, 10);
    if (*endptr == '\0') return TIPO_INT;

  
    strtof(valor, &endptr);
    if (*endptr == '\0') return TIPO_FLOAT;

    return TIPO_STRING;
}

int BuscarSensor(const Sensor sensores[], int qtdSensores, const char *Id) {
    for (int i = 0; i < qtdSensores; i++) {
        if (strcmp(sensores[i].IdDoSensor, Id) == 0) return i;
    }
    return -1;
}

int CriarSensor(Sensor sensores[], int *qtdSensores, const char *Id, TipoDado tipo) {
    if (*qtdSensores >= QuantidadeMaximaDeSensores) {
        printf("Número máximo de sensores atingido.\n");
        exit(1);
    }

    strncpy(sensores[*qtdSensores].IdDoSensor, Id, TamanhoDoId - 1);
    sensores[*qtdSensores].IdDoSensor[TamanhoDoId - 1] = '\0';
    sensores[*qtdSensores].tipo = tipo;
    sensores[*qtdSensores].leituras = NULL;
    sensores[*qtdSensores].QuantidadeDeLeitura = 0;
    sensores[*qtdSensores].capacidade = 0;

    return (*qtdSensores)++;
}

int CompararLeitura(const void *a, const void *b) {
    const Leitura *la = (const Leitura *)a;
    const Leitura *lb = (const Leitura *)b;
    return (la->timestamp - lb->timestamp);
}

void AdicionarLeitura(Sensor *sensor, long timestamp, const char *valorStr) {
    if (!valorStr || strnlen(valorStr, TAM_MAX_VALOR) >= TAM_MAX_VALOR) {
        printf("Valor inválido ou muito longo.\n");
        exit(1);
    }

    if (sensor->QuantidadeDeLeitura == sensor->capacidade) {
        sensor->capacidade = (sensor->capacidade == 0) ? 10 : sensor->capacidade * 2;
        sensor->leituras = realloc(sensor->leituras, sensor->capacidade * sizeof(Leitura));
        if (!sensor->leituras) {
            printf("Erro de memória.\n");
            exit(1);
        }
    }

    sensor->leituras[sensor->QuantidadeDeLeitura].timestamp = timestamp;

    switch (sensor->tipo) {
        case TIPO_INT: {
            char *endptr;
            sensor->leituras[sensor->QuantidadeDeLeitura].valor.inteiro = strtol(valorStr, &endptr, 10);
            if (*endptr != '\0') {
                printf("Valor inteiro inválido: %s\n", valorStr);
                exit(1);
            }
            break;
        }
        case TIPO_FLOAT: {
            char *endptr;
            sensor->leituras[sensor->QuantidadeDeLeitura].valor.racional = strtof(valorStr, &endptr);
            if (*endptr != '\0') {
                printf("Valor float inválido: %s\n", valorStr);
                exit(1);
            }
            break;
        }
        case TIPO_BOOL:
            strncpy(sensor->leituras[sensor->QuantidadeDeLeitura].valor.booleano, valorStr, 5);
            sensor->leituras[sensor->QuantidadeDeLeitura].valor.booleano[5] = '\0';
            break;
        case TIPO_STRING:
            strncpy(sensor->leituras[sensor->QuantidadeDeLeitura].valor.texto, valorStr, TAM_MAX_STRING - 1);
            sensor->leituras[sensor->QuantidadeDeLeitura].valor.texto[TAM_MAX_STRING - 1] = '\0';
            break;
    }

    sensor->QuantidadeDeLeitura++;
}

void SalvarEmArquivo(Sensor sensor) {
    char nome_arquivo[TamanhoDoId + 30];
    const char *tipoExt;

    switch (sensor.tipo) {
        case TIPO_INT: tipoExt = "int"; break;
        case TIPO_FLOAT: tipoExt = "float"; break;
        case TIPO_BOOL: tipoExt = "bool"; break;
        case TIPO_STRING: tipoExt = "string"; break;
        default: tipoExt = "desconhecido"; break;
    }

    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s_%s.txt", sensor.IdDoSensor, tipoExt);

    FILE *saida = fopen(nome_arquivo, "w");
    if (!saida) {
        printf("Erro ao criar arquivo %s\n", nome_arquivo);
        return;
    }

    for (int i = 0; i < sensor.QuantidadeDeLeitura; i++) {
        fprintf(saida, "%ld %s ", sensor.leituras[i].timestamp, sensor.IdDoSensor);
        switch (sensor.tipo) {
            case TIPO_INT:
                fprintf(saida, "%d\n", sensor.leituras[i].valor.inteiro); break;
            case TIPO_FLOAT:
                fprintf(saida, "%.2f\n", sensor.leituras[i].valor.racional); break;
            case TIPO_BOOL:
                fprintf(saida, "%s\n", sensor.leituras[i].valor.booleano); break;
            case TIPO_STRING:
                fprintf(saida, "%s\n", sensor.leituras[i].valor.texto); break;
        }
    }

    fclose(saida);
    printf("Arquivo criado: %s\n", nome_arquivo);
}

int main() {
    char nomeArquivo[100];
    FILE *entrada = NULL;

    while (1) {
        printf("Digite o nome do arquivo de entrada (ex: Parte1DoTrabalho.txt): ");
        fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
        nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';

        entrada = fopen(nomeArquivo, "r");
        if (!entrada) {
            printf("Erro ao abrir o arquivo '%s'. Tente novamente.\n", nomeArquivo);
        } else {
            break;
        }
    }

    Sensor sensores[QuantidadeMaximaDeSensores];
    int qtdSensores = 0;
    char linha[QuantidadeMaximaDeLinhas];

    while (fgets(linha, sizeof(linha), entrada)) {
        long timestamp;
        char id[TamanhoDoId];
        char valor[TAM_MAX_VALOR];

        if (sscanf(linha, "%ld %59s %63s", &timestamp, id, valor) != 3) {
            fprintf(stderr, "Linha mal formatada ignorada: %s", linha);
            continue;
        }

        int indice = BuscarSensor(sensores, qtdSensores, id);
        if (indice == -1) {
            TipoDado tipo = DescobrirTipo(valor);
            indice = CriarSensor(sensores, &qtdSensores, id, tipo);
        } else {
        
        }

        AdicionarLeitura(&sensores[indice], timestamp, valor);
    }

    fclose(entrada);

    for (int i = 0; i < qtdSensores; i++) {
        qsort(sensores[i].leituras, sensores[i].QuantidadeDeLeitura, sizeof(Leitura), CompararLeitura);
        SalvarEmArquivo(sensores[i]);
        free(sensores[i].leituras);
    }

    printf("\nOrganização feita com sucesso!\n");
    return 0;
}
