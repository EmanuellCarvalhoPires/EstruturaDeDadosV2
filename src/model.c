#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/model.h"

// ============================================================================
// BRINDES: CÓDIGO FORNECIDO
// ============================================================================

contact_model_t* Model_Initialize(void) {
    // Aloca a estrutura principal
    contact_model_t *model = malloc(sizeof(contact_model_t));
    if (!model) return NULL;

    // 1. Aloca a estrutura do motor físico interna
    model->engine = malloc(sizeof(contact_engine_t));
    if (model->engine == NULL) {
        free(model);
        return NULL;
    }

    // 2. Aloca o bloco contíguo de 5000 contatos
    model->engine->data = malloc(sizeof(contact_t) * STATIC_VECTOR_CAPACITY);
    if (model->engine->data == NULL) {
        free(model->engine);
        free(model);
        return NULL;
    }

    model->engine->next_id = 1;
    model->total_count = 0;
    model->contact_pointer = 0;

    return model;
}

void Model_Destroy(contact_model_t *model) {
    if (model && model->engine) {
        // Libera primeiro o vetor de 5000 contatos apontado pelo endereço interno
        if (model->engine->data) {
            free(model->engine->data);
        }
        // Libera a estrutura do motor em si
        free(model->engine);

        // Reseta o painel de controle e libera o model
        model->engine = NULL;
        model->total_count = 0;
        model->contact_pointer = 0;
        free(model);
    }
}

static void Helper_CleanJsonStr(char *dest, const char *src, size_t max_len) {
    size_t i = 0, j = 0;
    while (src[i] != '\0' && j < max_len - 1) {
        if (src[i] != '"' && src[i] != '\\' && src[i] != ',' && src[i] != '\n' && src[i] != '\r') {
            dest[j++] = src[i];
        }
        i++;
    }
    dest[j] = '\0';
}

bool Model_LoadFromJson(contact_model_t *model, const char *filename) {
    if (!model || !model->engine || !model->engine->data) return false;

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("[Aviso] Banco de dados '%s' nao localizado. Iniciando base em branco.\n", filename);
        return false;
    }

    // Reseta os contadores do contexto antes de preencher a memória
    model->total_count = 0;
    model->contact_pointer = 0;

    char linha[256];
    contact_t temp_contact;
    memset(&temp_contact, 0, sizeof(contact_t));

    bool dentro_do_contato = false;
    bool dentro_do_endereco = false;

    // Varre o arquivo de texto sequencialmente linha por linha
    while (fgets(linha, sizeof(linha), f) && model->total_count < STATIC_VECTOR_CAPACITY) {

        // Identifica a abertura do objeto de contato
        if (strstr(linha, "{") && strstr(linha, "version") == NULL && strstr(linha, "address1") == NULL) {
            dentro_do_contato = true;
            memset(&temp_contact, 0, sizeof(contact_t));
            continue;
        }

        // Identifica que entrou na leitura do sub-objeto de endereço
        if (strstr(linha, "\"address1\"")) {
            dentro_do_endereco = true;
            continue;
        }

        // Identifica o fechamento de blocos lógicos
        if (strstr(linha, "}")) {
            if (dentro_do_endereco) {
                dentro_do_endereco = false; // Saiu do bloco do endereço, volta para o contato
                continue;
            }

            if (dentro_do_contato) {
                // Copia fisicamente a struct temporária para a região de memória do vetor
                contact_t *dest = &model->engine->data[model->total_count];
                memcpy(dest, &temp_contact, sizeof(contact_t));

                model->total_count++;

                // Sincroniza o gerador automático para evitar colisão de IDs nas próximas inserções
                if ((unsigned int)dest->id >= model->engine->next_id) {
                    model->engine->next_id = dest->id + 1;
                }
                dentro_do_contato = false;
            }
        }

        // Parser manual de extração de chaves e valores textuais
        if (dentro_do_contato) {
            char chave[64], valor[128];
            if (sscanf(linha, " \"%[^\"]\": \"%[^\"]\"", chave, valor) == 2 ||
                sscanf(linha, " \"%[^\"]\": %s", chave, valor) == 2) {

                if (dentro_do_endereco) {
                    if (strcmp(chave, "street") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.street, valor, MAX_STREET_LEN);
                    else if (strcmp(chave, "neighborhood") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.neighborhood, valor, MAX_NEIGHBORHOOD_LEN);
                    else if (strcmp(chave, "city") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.city, valor, MAX_CITY_LEN);
                    else if (strcmp(chave, "state") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.state, valor, MAX_STATE_LEN);
                    else if (strcmp(chave, "country") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.country, valor, MAX_COUNTRY_LEN);
                    else if (strcmp(chave, "zip_code") == 0)
                        Helper_CleanJsonStr(temp_contact.address1.zip_code, valor, MAX_ZIP_LEN);
                } else {
                    if (strcmp(chave, "id") == 0)
                        temp_contact.id = atoi(valor);
                    else if (strcmp(chave, "nome") == 0)
                        Helper_CleanJsonStr(temp_contact.nome, valor, MAX_NAME_LEN);
                    else if (strcmp(chave, "sobrenome") == 0)
                        Helper_CleanJsonStr(temp_contact.sobrenome, valor, MAX_NAME_LEN);
                    else if (strcmp(chave, "cargo") == 0)
                        Helper_CleanJsonStr(temp_contact.cargo, valor, MAX_JOB_LEN);
                    else if (strcmp(chave, "celular") == 0)
                        Helper_CleanJsonStr(temp_contact.celular, valor, MAX_PHONE_LEN);
                    else if (strcmp(chave, "email") == 0)
                        Helper_CleanJsonStr(temp_contact.email, valor, MAX_EMAIL_LEN);
                }
            }
        }
    }

    fclose(f);
    return true;
}

bool Model_SaveToJson(const contact_model_t *model, const char *filename) {
    if (!model || !model->engine || !model->engine->data) return false;

    FILE *f = fopen(filename, "w");
    if (!f) return false;

    // Escrita estruturada dos metadados globais da base de dados
    fprintf(f, "{\n  \"version\": \"1.0\",\n  \"total_records\": %d,\n  \"contacts\": [\n", model->total_count);

    for (int i = 0; i < model->total_count; i++) {
        contact_t *c = &model->engine->data[i];

        fprintf(f, "    {\n");
        fprintf(f, "      \"id\": %d,\n", c->id);
        fprintf(f, "      \"nome\": \"%s\",\n", c->nome);
        fprintf(f, "      \"sobrenome\": \"%s\",\n", c->sobrenome);
        fprintf(f, "      \"cargo\": \"%s\",\n", c->cargo);
        fprintf(f, "      \"celular\": \"%s\",\n", c->celular);
        fprintf(f, "      \"email\": \"%s\",\n", c->email);

        fprintf(f, "      \"address1\": {\n");
        fprintf(f, "        \"street\": \"%s\",\n", c->address1.street);
        fprintf(f, "        \"neighborhood\": \"%s\",\n", c->address1.neighborhood);
        fprintf(f, "        \"city\": \"%s\",\n", c->address1.city);
        fprintf(f, "        \"state\": \"%s\",\n", c->address1.state);
        fprintf(f, "        \"country\": \"%s\",\n", c->address1.country);
        fprintf(f, "        \"zip_code\": \"%s\"\n", c->address1.zip_code);
        fprintf(f, "      }\n");

        // Garante a vírgula de separação apenas se não for o último registro
        fprintf(f, "    }%s\n", (i == model->total_count - 1) ? "" : ",");
    }

    fprintf(f, "  ]\n}\n");
    fclose(f);
    return true;
}

// ============================================================================
// SUAS FUNÇÕES (Implemente a lógica abaixo)
// ============================================================================

bool Model_AddContact(contact_model_t *model, const contact_t *new_contact) {
    // Dica: Verifique se model->total_count < STATIC_VECTOR_CAPACITY
    // Dica: Use model->engine->data[model->total_count] para inserir
    // Dica: Atualize model->total_count e model->engine->next_id
    return false;
}

contact_t* Model_GetContactById(const contact_model_t *model, unsigned int id) {
    // Dica: Faça um loop de 0 até model->total_count buscando o id
    return NULL;
}

bool Model_UpdateContact(contact_model_t *model, unsigned int id, const contact_t *updated_data) {
    // Dica: Use Model_GetContactById para achar o ponteiro e sobrescreva os dados
    return false;
}

bool Model_RemoveContact(contact_model_t *model, unsigned int id) {
    // Dica: Encontre o índice do contato
    // Dica: Faça um loop copiando o elemento i+1 para a posição i até o final
    // Dica: Reduza model->total_count em 1
    return false;
}

void Model_ApplyFilter(contact_model_t *model, const char *search_query) {
    // Lógica para filtro de busca (opcional dependendo do requisito exato)
}