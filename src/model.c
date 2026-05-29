#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/contact_types.h"
#include "include/model.h"


contact_engine_t* Model_Initialize(contact_model_t *model) {
    if (!model) return NULL;

    // 1. Aloca a estrutura do motor físico interna
    contact_engine_t *env = malloc(sizeof(contact_engine_t));
    if (env == NULL) return NULL;

    // 2. Aloca o bloco contíguo de 5000 contatos e guarda o endereço no ponteiro .data
    env->data = malloc(sizeof(contact_t) * STATIC_VECTOR_CAPACITY);
    if (env->data == NULL) {
        free(env); // Desfaz a alocação anterior se esta falhar
        return NULL;
}

    // 3. Zera o vetor inteiro e inicializa o próximo ID
    memset(env->data, 0, sizeof(contact_t) * STATIC_VECTOR_CAPACITY);
    env->next_id = 1;

    // 4. Conecta o motor ao painel de controle e zera contadores
    model->engine = env;
    model->total_count = 0;
    model->contact_pointer = 0;

    return env;
}


void Model_Destroy(contact_model_t *model) {
    if (model && model->engine) {
        // Libera primeiro o vetor de 5000 contatos apontado pelo endereço interno
        if (model->engine->data) {
            free(model->engine->data);
            model->engine->data = NULL;
        }
        // Libera a estrutura do motor em si
        free(model->engine);

        // Reseta o painel de controle
        model->engine = NULL;
        model->total_count = 0;
        model->contact_pointer = 0;
    }
}

bool Model_AddContact(contact_model_t *model, const contact_t *new_contact) {
    // Verificar se model->total_count < STATIC_VECTOR_CAPACITY
    if (model->total_count >= STATIC_VECTOR_CAPACITY) {
        return NULL;
    }

    // A função "Model_LoadFromJson" vai preencher o vetor com os contatos existentes dentro do JSON
    // E aí o vetor que você vai inserir o contato é: model->engine->data
    // Essa função deve inserir o objeto do tipo "contact_t" dentro do vetor na última posição

    model->engine->data[model->total_count] = *new_contact;
    // printf("Contato %s adicionado com sucesso!\n", new_contact->nome);

    model->total_count++;
    model->engine->next_id = new_contact->id + 1;
    // Use model->engine->data[model->total_count] para inserir o novo contato recebido pela função na última posição do no vetor

    // Atualize model->total_count e model->engine->next_id
    //

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

static unsigned int Helper_FindMaxId(const contact_model_t *model) {
    if (!model || model->total_count == 0) return 0;
    unsigned int max_id = 0;
    for (int i = 0; i < model->total_count; i++) {
        unsigned int current_id = (unsigned int)model->engine->data[i].id;
        if (current_id > max_id) {
            max_id = current_id;
        }
    }
    return max_id;
}

static void Helper_CleanJsonStr(char *dest, const char *src, size_t max_len) {
    if (!dest || !src || max_len == 0) {
        if (dest && max_len > 0) {
            dest[0] = '\0';
        }
        return;
    }
    size_t i = 0, j = 0;
    while (src[i] != '\0' && j < max_len - 1) {
        char c = src[i];
        if (c != '"' && c != '\\' && c != ',' && c != '\n' && c != '\r') {
            dest[j++] = c;
        }
        i++;
    }
    dest[j] = '\0';
}


bool Model_SaveToJson(const contact_model_t *model, const char *filename) {
    if (!model || !model->engine || !model->engine->data || !filename) {
        return false;
    }
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "[Erro] Não foi possível abrir '%s' para escrita.\n", filename);
        return false;
    }
    fprintf(f, "{\n  \"version\": \"1.0\",\n  \"total_records\": %d,\n  \"contacts\": [\n",
            model->total_count);
    for (int i = 0; i < model->total_count; i++) {
        contact_t *c = &model->engine->data[i];
        fprintf(f, "    {\n");
        fprintf(f, "      \"id\": %d,\n", c->id);
        fprintf(f, "      \"nome\": \"%s\",\n", c->nome);
        fprintf(f, "      \"sobrenome\": \"%s\",\n", c->sobrenome);
        fprintf(f, "      \"cargo\": \"%s\",\n", c->cargo);
        fprintf(f, "      \"telefone\": \"%s\",\n", c->telefone);
        fprintf(f, "      \"telefone2\": \"%s\",\n", c->telefone2);
        fprintf(f, "      \"celular\": \"%s\",\n", c->celular);
        fprintf(f, "      \"email\": \"%s\",\n", c->email);
        fprintf(f, "      \"instagram\": \"%s\",\n", c->instagram);
        fprintf(f, "      \"url\": \"%s\",\n", c->url);
        fprintf(f, "      \"address1\": {\n");
        fprintf(f, "        \"street\": \"%s\",\n", c->address1.street);
        fprintf(f, "        \"neighborhood\": \"%s\",\n", c->address1.neighborhood);
        fprintf(f, "        \"city\": \"%s\",\n", c->address1.city);
        fprintf(f, "        \"state\": \"%s\",\n", c->address1.state);
        fprintf(f, "        \"country\": \"%s\",\n", c->address1.country);
        fprintf(f, "        \"zip_code\": \"%s\"\n", c->address1.zip_code);
        fprintf(f, "      },\n");
        fprintf(f, "      \"address2\": {\n");
        fprintf(f, "        \"street\": \"%s\",\n", c->address2.street);
        fprintf(f, "        \"neighborhood\": \"%s\",\n", c->address2.neighborhood);
        fprintf(f, "        \"city\": \"%s\",\n", c->address2.city);
        fprintf(f, "        \"state\": \"%s\",\n", c->address2.state);
        fprintf(f, "        \"country\": \"%s\",\n", c->address2.country);
        fprintf(f, "        \"zip_code\": \"%s\"\n", c->address2.zip_code);
        fprintf(f, "      }\n");
        fprintf(f, "    }%s\n", (i == model->total_count - 1) ? "" : ",");
    }
    fprintf(f, "  ]\n}\n");
    fclose(f);
    printf("[Info] %d contatos salvos em '%s'.\n", model->total_count, filename);
    return true;
}

bool Model_LoadFromJson(contact_model_t *model, const char *filename) {
    if (!model || !model->engine || !model->engine->data || !filename) {
        fprintf(stderr, "[Erro] Model_LoadFromJson: parametros inválidos\n");
        return false;
    }
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "[Aviso] Banco de dados '%s' não localizado. "
                "Iniciando base em branco.\n", filename);
        return false;
    }
    model->total_count = 0;
    model->contact_pointer = 0;
    char linha[256];
    contact_t temp_contact;
    memset(&temp_contact, 0, sizeof(contact_t));
    bool dentro_do_contato = false;
    int endereco_atual = 0;
    while (fgets(linha, sizeof(linha), f) &&
           model->total_count < STATIC_VECTOR_CAPACITY) {
        if (Helper_IsContactObjectStart(linha)) {
            dentro_do_contato = true;
            endereco_atual = 0;
            memset(&temp_contact, 0, sizeof(contact_t));
            continue;
        }
        int address_num = 0;
        if (Helper_IsAddressBlockStart(linha, &address_num)) {
            endereco_atual = address_num;
            continue;
        }
        if (strchr(linha, '}')) {
            if (endereco_atual > 0) {
                endereco_atual = 0;
                continue;
            }
            if (dentro_do_contato) {
                if (model->total_count < STATIC_VECTOR_CAPACITY) {
                    contact_t *dest = &model->engine->data[model->total_count];
                    memcpy(dest, &temp_contact, sizeof(contact_t));
                    model->total_count++;
                }
                dentro_do_contato = false;
            }
            continue;
        }
        if (dentro_do_contato) {
            char chave[64], valor[128];
            int sscanf_result = 0;
            sscanf_result = sscanf(linha, " \"%63[^\"]\": \"%127[^\"]\"",
                                   chave, valor);
            if (sscanf_result != 2) {
                sscanf_result = sscanf(linha, " \"%63[^\"]\": %127s",
                                       chave, valor);
            }
            if (sscanf_result == 2) {
                if (endereco_atual == 1) {
                    if (strcmp(chave, "street") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.street, valor, MAX_STREET_LEN);
                    }
                    else if (strcmp(chave, "neighborhood") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.neighborhood, valor, MAX_NEIGHBORHOOD_LEN);
                    }
                    else if (strcmp(chave, "city") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.city, valor, MAX_CITY_LEN);
                    }
                    else if (strcmp(chave, "state") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.state, valor, MAX_STATE_LEN);
                    }
                    else if (strcmp(chave, "country") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.country, valor, MAX_COUNTRY_LEN);
                    }
                    else if (strcmp(chave, "zip_code") == 0) {
                        Helper_CleanJsonStr(temp_contact.address1.zip_code, valor, MAX_ZIP_LEN);
                    }
                }
                else if (endereco_atual == 2) {
                    if (strcmp(chave, "street") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.street, valor, MAX_STREET_LEN);
                    }
                    else if (strcmp(chave, "neighborhood") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.neighborhood, valor, MAX_NEIGHBORHOOD_LEN);
                    }
                    else if (strcmp(chave, "city") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.city, valor, MAX_CITY_LEN);
                    }
                    else if (strcmp(chave, "state") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.state, valor, MAX_STATE_LEN);
                    }
                    else if (strcmp(chave, "country") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.country, valor, MAX_COUNTRY_LEN);
                    }
                    else if (strcmp(chave, "zip_code") == 0) {
                        Helper_CleanJsonStr(temp_contact.address2.zip_code, valor, MAX_ZIP_LEN);
                    }
                }
                else if (endereco_atual == 0) {
                    if (strcmp(chave, "id") == 0) {
                        temp_contact.id = atoi(valor);
                    }
                    else if (strcmp(chave, "nome") == 0) {
                        Helper_CleanJsonStr(temp_contact.nome, valor, MAX_NAME_LEN);
                    }
                    else if (strcmp(chave, "sobrenome") == 0) {
                        Helper_CleanJsonStr(temp_contact.sobrenome, valor, MAX_NAME_LEN);
                    }
                    else if (strcmp(chave, "cargo") == 0) {
                        Helper_CleanJsonStr(temp_contact.cargo, valor, MAX_JOB_LEN);
                    }
                    else if (strcmp(chave, "telefone") == 0) {
                        Helper_CleanJsonStr(temp_contact.telefone, valor, MAX_PHONE_LEN);
                    }
                    else if (strcmp(chave, "telefone2") == 0) {
                        Helper_CleanJsonStr(temp_contact.telefone2, valor, MAX_PHONE_LEN);
                    }
                    else if (strcmp(chave, "celular") == 0) {
                        Helper_CleanJsonStr(temp_contact.celular, valor, MAX_PHONE_LEN);
                    }
                    else if (strcmp(chave, "email") == 0) {
                        Helper_CleanJsonStr(temp_contact.email, valor, MAX_EMAIL_LEN);
                    }
                    else if (strcmp(chave, "instagram") == 0) {
                        Helper_CleanJsonStr(temp_contact.instagram, valor, MAX_SOCIAL_LEN);
                    }
                    else if (strcmp(chave, "url") == 0) {
                        Helper_CleanJsonStr(temp_contact.url, valor, MAX_URL_LEN);
                    }
                }
            }
        }
    }
    fclose(f);
    if (model->total_count == 0) {
        fprintf(stderr, "[Aviso] Nenhum contato válido encontrado em '%s'\n", filename);
        return false;
    }
    unsigned int max_id = Helper_FindMaxId(model);
    if (max_id > 0) {
        model->engine->next_id = max_id + 1;
    }
    fprintf(stdout, "[Info] %d contatos carregados com sucesso de '%s'\n",
            model->total_count, filename);
    return true;
}