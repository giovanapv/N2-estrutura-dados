#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define TAM_MAX_STRING 17
#define TAM_ID 60
#define BUFFER_DATA 20
#define BUFFER_HORA 20

typedef struct {
    long timestamp;
    char valor[TAM_MAX_STRING];
} Leitura;

int buscarMaisProxima(Leitura *leituras, int tamanho, long alvo) {
    int inicio = 0, fim = tamanho - 1, meio;
    int maisProximo = 0;
    long menorDiferenca = labs(leituras[0].timestamp - alvo);

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        long dif = labs(leituras[meio].timestamp - alvo);

        if (dif < menorDiferenca) {
            menorDiferenca = dif;
            maisProximo = meio;
        }

        if (leituras[meio].timestamp < alvo) {
            inicio = meio + 1;
        } else if (leituras[meio].timestamp > alvo) {
            fim = meio - 1;
        } else {
            return meio;
        }
    }
    return maisProximo;
}

int main() {
    char sensor[TAM_ID];
    char data[BUFFER_DATA];
    char hora[BUFFER_HORA];
    int dia, mes, ano;
    int h, m, s;
    struct tm t = {0};
    long alvo;
    char nomeArquivo[100];

    printf("Digite o nome do sensor (sem extensão, vulgo '.txt'): ");
    fgets(sensor, sizeof(sensor), stdin);
    sensor[strcspn(sensor, "\n")] = '\0';

    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s.txt", sensor);
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo '%s'. Verifique se o sensor existe.\n", nomeArquivo);
        return 1;
    }

    // Validação separada de formato e valor da data formato e se a hora e a data existem
    while (1) {
        printf("Digite a data da consulta (dd/mm/aaaa): ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = '\0';

        if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') {
            printf("Formato inválido. Use dd/mm/aaaa para data.\n\n");
            continue;
        }

        if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) {
            printf("Formato inválido. Use dd/mm/aaaa para data.\n\n");
            continue;
        }

        if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1970) {
            printf("Essa data nem existe seu doido\n\n");
            continue;
        }

        break;
    }

    
    while (1) {
        printf("Digite a hora da consulta (hh:mm:ss): ");
        fgets(hora, sizeof(hora), stdin);
        hora[strcspn(hora, "\n")] = '\0';

        if (strlen(hora) != 8 || hora[2] != ':' || hora[5] != ':') {
            printf("Formato inválido. Use hh:mm:ss para hora.\n\n");
            continue;
        }

        if (sscanf(hora, "%d:%d:%d", &h, &m, &s) != 3) {
            printf("Formato inválido. Use hh:mm:ss para hora.\n\n");
            continue;
        }

        if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
            printf("Essa hora nem existe seu doido\n\n");
            continue;
        }

        break;
    }

    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = h;
    t.tm_min = m;
    t.tm_sec = s;
    t.tm_isdst = -1;

    alvo = mktime(&t);
    if (alvo == -1) {
        printf("Erro ao converter para timestamp.\n");
        fclose(arquivo);
        return 1;
    }

    Leitura *leituras = NULL;
    int capacidade = 0, quantidade = 0;

    while (1) {
        if (quantidade == capacidade) {
            capacidade = (capacidade == 0) ? 100 : capacidade * 2;
            leituras = realloc(leituras, capacidade * sizeof(Leitura));
            if (!leituras) {
                printf("Erro de memória.\n");
                fclose(arquivo);
                return 1;
            }
        }

        if (fscanf(arquivo, "%ld %*s %16s", &leituras[quantidade].timestamp, leituras[quantidade].valor) != 2)
            break;

        quantidade++;
    }

    fclose(arquivo);

    if (quantidade == 0) {
        printf("Nenhuma leitura disponível no arquivo.\n");
        free(leituras);
        return 1;
    }

    int indice = buscarMaisProxima(leituras, quantidade, alvo);
    printf("\nLeitura mais próxima encontrada:\nTimestamp: %ld\nValor: %s\n",
           leituras[indice].timestamp, leituras[indice].valor);

    free(leituras);
    return 0;
}
