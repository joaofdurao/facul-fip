#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_NUM 65
#define MAX_DATA 30
#define HISTORY_FILE "historico_conversoes.txt"
#define HISTORY_HEADER "data_hora|valor_original|base_origem|base_destino|resultado\n"

typedef struct {
    char data_hora[MAX_DATA];
    char valor_original[MAX_NUM];
    int base_origem; // 2, 8, 10 ou 16
    int base_destino;
    char resultado[MAX_NUM];
} Conversao;

void current_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tp = localtime(&now);
    if (tp) {
        strftime(buffer, size, "%Y-%m-%d %H:%M", tp);
    } else {
        strncpy(buffer, "0000-00-00 00:00", size - 1);
        buffer[size - 1] = '\0';
    }
}

void decimal_to_binary_recursive(unsigned long long n, char *resultado, int *idx) {
    if (n > 1) {
        decimal_to_binary_recursive(n / 2, resultado, idx);
    }
    resultado[(*idx)++] = '0' + (n % 2);
}

void decimal_to_hex_recursive(unsigned long long n, char *resultado, int *idx) {
    const char hex_digits[] = "0123456789ABCDEF";
    if (n > 15) {
        decimal_to_hex_recursive(n / 16, resultado, idx);
    }
    resultado[(*idx)++] = hex_digits[n % 16];
}

void decimal_to_octal_recursive(unsigned long long n, char *resultado, int *idx) {
    if (n > 7) {
        decimal_to_octal_recursive(n / 8, resultado, idx);
    }
    resultado[(*idx)++] = '0' + (n % 8);
}

int validate_binary(const char *valor) {
    if (valor[0] == '\0') return 0;
    for (size_t i = 0; i < strlen(valor); i++) {
        if (valor[i] != '0' && valor[i] != '1') {
            return 0;
        }
    }
    return 1;
}

int validate_hex(const char *valor) {
    if (valor[0] == '\0') return 0;
    for (size_t i = 0; i < strlen(valor); i++) {
        char ch = toupper(valor[i]);
        if (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F'))) {
            return 0;
        }
    }
    return 1;
}

unsigned long long binary_to_decimal(const char *valor) {
    unsigned long long total = 0;
    for (size_t i = 0; i < strlen(valor); i++) {
        total = total * 2 + (valor[i] - '0');
    }
    return total;
}

unsigned long long hex_to_decimal(const char *valor) {
    unsigned long long total = 0;
    for (size_t i = 0; i < strlen(valor); i++) {
        char ch = toupper(valor[i]);
        unsigned long long digit;
        if (ch >= '0' && ch <= '9') {
            digit = ch - '0';
        } else {
            digit = 10 + (ch - 'A');
        }
        total = total * 16 + digit;
    }
    return total;
}

void log_conversion(const Conversao *registro) {
    FILE *file = fopen(HISTORY_FILE, "a+");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo de historico.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fprintf(file, HISTORY_HEADER);
    }

    fprintf(file, "%s|%s|%d|%d|%s\n",
            registro->data_hora,
            registro->valor_original,
            registro->base_origem,
            registro->base_destino,
            registro->resultado);

    fclose(file);
}

void show_history(void) {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        printf("Nenhum historico encontrado.\n");
        return;
    }

    char line[256];
    printf("\nHistorico de conversoes:\n");
    printf("Data/Hora           | Valor | Origem | Destino | Resultado\n");
    printf("--------------------------------------------------------\n");
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "data_hora|", 10) == 0) {
            continue;
        }

        char data[MAX_DATA];
        char valor[MAX_NUM];
        int orig;
        int dest;
        char resultado[MAX_NUM];

        if (sscanf(line, "%29[^|]|%64[^|]|%d|%d|%64[^\n]", data, valor, &orig, &dest, resultado) == 5) {
            printf("%s | %s | %3d | %3d | %s\n", data, valor, orig, dest, resultado);
        }
    }
    fclose(file);
}

void show_statistics(void) {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        printf("Nenhum historico para gerar estatisticas.\n");
        return;
    }

    int total = 0;
    int counts[6] = {0};
    // indices: 1=10->2, 2=10->16, 3=10->8, 4=2->10, 5=16->10
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "data_hora|", 10) == 0) {
            continue;
        }

        char data[MAX_DATA];
        char valor[MAX_NUM];
        int orig;
        int dest;
        char resultado[MAX_NUM];

        if (sscanf(line, "%29[^|]|%64[^|]|%d|%d|%64[^\n]", data, valor, &orig, &dest, resultado) == 5) {
            total++;
            if (orig == 10 && dest == 2) counts[1]++;
            else if (orig == 10 && dest == 16) counts[2]++;
            else if (orig == 10 && dest == 8) counts[3]++;
            else if (orig == 2 && dest == 10) counts[4]++;
            else if (orig == 16 && dest == 10) counts[5]++;
        }
    }
    fclose(file);

    printf("\nEstatisticas:\n");
    printf("Total de conversoes: %d\n", total);
    printf("Conversoes por tipo:\n");
    printf("  Decimal -> Binario : %d\n", counts[1]);
    printf("  Decimal -> Hex     : %d\n", counts[2]);
    printf("  Decimal -> Octal   : %d\n", counts[3]);
    printf("  Binario -> Decimal : %d\n", counts[4]);
    printf("  Hex -> Decimal     : %d\n", counts[5]);

    int best_index = 1;
    for (int i = 2; i <= 5; i++) {
        if (counts[i] > counts[best_index]) {
            best_index = i;
        }
    }

    const char *most_used = "Nenhuma conversao ainda";
    if (total > 0) {
        switch (best_index) {
            case 1: most_used = "Decimal -> Binario"; break;
            case 2: most_used = "Decimal -> Hexadecimal"; break;
            case 3: most_used = "Decimal -> Octal"; break;
            case 4: most_used = "Binario -> Decimal"; break;
            case 5: most_used = "Hexadecimal -> Decimal"; break;
        }
        printf("Conversao mais usada: %s (%d vezes)\n", most_used, counts[best_index]);
    } else {
        printf("Conversao mais usada: %s\n", most_used);
    }
}

void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void read_line(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

int main(void) {
    int opcao = 0;
    char entrada[MAX_NUM];

    do {
        printf("\n=== Conversor de Bases Numericas ===\n");
        printf("1) Decimal -> Binario\n");
        printf("2) Decimal -> Hexadecimal\n");
        printf("3) Decimal -> Octal\n");
        printf("4) Binario -> Decimal\n");
        printf("5) Hexadecimal -> Decimal\n");
        printf("6) Ver historico\n");
        printf("7) Estatisticas\n");
        printf("8) Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        Conversao registro = {0};
        switch (opcao) {
            case 1:
                printf("Digite um numero decimal: ");
                read_line(entrada, sizeof(entrada));
                if (entrada[0] == '\0') {
                    printf("Entrada vazia.\n");
                    break;
                }
                {
                    char *endptr;
                    unsigned long long valor = strtoull(entrada, &endptr, 10);
                    if (*endptr != '\0') {
                        printf("Decimal invalido. Use apenas digitos.\n");
                        break;
                    }
                    if (valor == 0) {
                        strcpy(registro.resultado, "0");
                    } else {
                        int idx = 0;
                        decimal_to_binary_recursive(valor, registro.resultado, &idx);
                        registro.resultado[idx] = '\0';
                    }
                    strcpy(registro.data_hora, "");
                    current_timestamp(registro.data_hora, sizeof(registro.data_hora));
                    strncpy(registro.valor_original, entrada, sizeof(registro.valor_original) - 1);
                    registro.base_origem = 10;
                    registro.base_destino = 2;
                    log_conversion(&registro);
                    printf("Resultado: %s\n", registro.resultado);
                }
                break;
            case 2:
                printf("Digite um numero decimal: ");
                read_line(entrada, sizeof(entrada));
                if (entrada[0] == '\0') {
                    printf("Entrada vazia.\n");
                    break;
                }
                {
                    char *endptr;
                    unsigned long long valor = strtoull(entrada, &endptr, 10);
                    if (*endptr != '\0') {
                        printf("Decimal invalido. Use apenas digitos.\n");
                        break;
                    }
                    if (valor == 0) {
                        strcpy(registro.resultado, "0");
                    } else {
                        int idx = 0;
                        decimal_to_hex_recursive(valor, registro.resultado, &idx);
                        registro.resultado[idx] = '\0';
                    }
                    current_timestamp(registro.data_hora, sizeof(registro.data_hora));
                    strncpy(registro.valor_original, entrada, sizeof(registro.valor_original) - 1);
                    registro.base_origem = 10;
                    registro.base_destino = 16;
                    log_conversion(&registro);
                    printf("Resultado: %s\n", registro.resultado);
                }
                break;
            case 3:
                printf("Digite um numero decimal: ");
                read_line(entrada, sizeof(entrada));
                if (entrada[0] == '\0') {
                    printf("Entrada vazia.\n");
                    break;
                }
                {
                    char *endptr;
                    unsigned long long valor = strtoull(entrada, &endptr, 10);
                    if (*endptr != '\0') {
                        printf("Decimal invalido. Use apenas digitos.\n");
                        break;
                    }
                    if (valor == 0) {
                        strcpy(registro.resultado, "0");
                    } else {
                        int idx = 0;
                        decimal_to_octal_recursive(valor, registro.resultado, &idx);
                        registro.resultado[idx] = '\0';
                    }
                    current_timestamp(registro.data_hora, sizeof(registro.data_hora));
                    strncpy(registro.valor_original, entrada, sizeof(registro.valor_original) - 1);
                    registro.base_origem = 10;
                    registro.base_destino = 8;
                    log_conversion(&registro);
                    printf("Resultado: %s\n", registro.resultado);
                }
                break;
            case 4:
                printf("Digite um numero binario: ");
                read_line(entrada, sizeof(entrada));
                if (!validate_binary(entrada)) {
                    printf("Binario invalido. Use apenas 0 e 1.\n");
                    break;
                }
                {
                    unsigned long long decimal = binary_to_decimal(entrada);
                    snprintf(registro.resultado, sizeof(registro.resultado), "%llu", decimal);
                    current_timestamp(registro.data_hora, sizeof(registro.data_hora));
                    strncpy(registro.valor_original, entrada, sizeof(registro.valor_original) - 1);
                    registro.base_origem = 2;
                    registro.base_destino = 10;
                    log_conversion(&registro);
                    printf("Resultado: %s\n", registro.resultado);
                }
                break;
            case 5:
                printf("Digite um numero hexadecimal: ");
                read_line(entrada, sizeof(entrada));
                if (!validate_hex(entrada)) {
                    printf("Hexadecimal invalido. Use digitos 0-9 e A-F.\n");
                    break;
                }
                {
                    unsigned long long decimal = hex_to_decimal(entrada);
                    snprintf(registro.resultado, sizeof(registro.resultado), "%llu", decimal);
                    current_timestamp(registro.data_hora, sizeof(registro.data_hora));
                    strncpy(registro.valor_original, entrada, sizeof(registro.valor_original) - 1);
                    registro.base_origem = 16;
                    registro.base_destino = 10;
                    log_conversion(&registro);
                    printf("Resultado: %s\n", registro.resultado);
                }
                break;
            case 6:
                show_history();
                break;
            case 7:
                show_statistics();
                break;
            case 8:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 8);

    return 0;
}
