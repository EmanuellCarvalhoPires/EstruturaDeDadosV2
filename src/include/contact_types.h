#ifndef CONTACT_TYPES_H
#define CONTACT_TYPES_H

// Limites de tamanho expandidos para acomodar os novos campos
#define MAX_NAME_LEN 32
#define MAX_JOB_LEN 64
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 64
#define MAX_SOCIAL_LEN 64
#define MAX_URL_LEN 128

// Novos limites específicos para o endereço
#define MAX_STREET_LEN 96
#define MAX_NEIGHBORHOOD_LEN 48
#define MAX_CITY_LEN 42
#define MAX_STATE_LEN 16
#define MAX_COUNTRY_LEN 32
#define MAX_ZIP_LEN 16

// Estrutura de Endereço Atualizada
typedef struct {
    char street[MAX_STREET_LEN];
    char neighborhood[MAX_NEIGHBORHOOD_LEN];
    char city[MAX_CITY_LEN];
    char state[MAX_STATE_LEN];
    char country[MAX_COUNTRY_LEN];
    char zip_code[MAX_ZIP_LEN];
} address_t;

// Tipo de Contato
typedef struct {
    int id;
    char nome[MAX_NAME_LEN];
    char sobrenome[MAX_NAME_LEN];
    char cargo[MAX_JOB_LEN];
    char telefone[MAX_PHONE_LEN];
    char telefone2[MAX_PHONE_LEN];
    char celular[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char instagram[MAX_SOCIAL_LEN];
    char url[MAX_URL_LEN];

    // Endereços (Residencial / Comercial)
    address_t address1;
    address_t address2;
} contact_t; // Renomeado para contact_t por padronização com o model.c

#endif // CONTACT_TYPES_H