#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/contact_types.h"
#include "include/model.h"

#define SOURCE_JSON      "contacts.json"
#define ROUNDTRIP_JSON   "contacts_roundtrip.json"
#define FINAL_JSON       "contacts_final.json"

static int g_pass = 0;
static int g_fail = 0;

static void check(bool cond, const char *msg) {
    if (cond) {
        printf("    ✓ %s\n", msg);
        g_pass++;
    } else {
        printf("    ✗ %s\n", msg);
        g_fail++;
    }
}

static void section(const char *title) {
    printf("\n%s\n", title);
    printf("─────────────────────────────────────────────────────\n");
}

static void print_contact(const contact_t *c) {
    if (!c) {
        printf("    [Contato NULL]\n");
        return;
    }
    printf("    ID: %d\n", c->id);
    printf("    Nome: %s %s\n", c->nome, c->sobrenome);
    printf("    Cargo: %s\n", c->cargo);
    printf("    Telefone: %s | Telefone2: %s | Celular: %s\n",
           c->telefone, c->telefone2, c->celular);
    printf("    Email: %s\n", c->email);
    printf("    Instagram: %s\n", c->instagram);
    printf("    URL: %s\n", c->url);
    printf("    Endereço 1: %s, %s - %s/%s - %s (%s)\n",
           c->address1.street, c->address1.neighborhood,
           c->address1.city, c->address1.state,
           c->address1.country, c->address1.zip_code);
    printf("    Endereço 2: %s, %s - %s/%s - %s (%s)\n",
           c->address2.street, c->address2.neighborhood,
           c->address2.city, c->address2.state,
           c->address2.country, c->address2.zip_code);
}

static void print_summary_line(const contact_t *c) {
    if (!c) return;
    printf("    [ID=%-4d] %-12s %-14s | %-20s | %s\n",
           c->id, c->nome, c->sobrenome, c->celular, c->email);
}

static void print_sample(const contact_model_t *model, int n) {
    if (!model || model->total_count == 0) {
        printf("    [Base vazia]\n");
        return;
    }
    int total = model->total_count;
    int limit = (n < total) ? n : total;
    printf("    Primeiros %d contatos:\n", limit);
    for (int i = 0; i < limit; i++) {
        print_summary_line(&model->engine->data[i]);
    }
    if (total > 2 * n) {
        int mid = total / 2;
        printf("    ... (contato do meio, índice %d):\n", mid);
        print_summary_line(&model->engine->data[mid]);
    }
    if (total > n) {
        printf("    Últimos %d contatos:\n", limit);
        int start = total - limit;
        if (start < 0) start = 0;
        for (int i = start; i < total; i++) {
            print_summary_line(&model->engine->data[i]);
        }
    }
}

static void test_initialize(contact_model_t *model) {
    section("[TESTE 1] Model_Initialize()");
    printf("  Inicializando o motor de contatos...\n");
    contact_engine_t *engine = Model_Initialize(model);
    check(engine != NULL, "Motor alocado");
    check(model->engine == engine, "Painel apontando para o motor");
    check(model->total_count == 0, "total_count zerado");
    check(model->contact_pointer == 0, "contact_pointer zerado");
    if (engine) {
        check(engine->next_id == 1, "next_id inicia em 1");
        check(engine->data != NULL, "Vetor de contatos alocado");
        printf("  Estado inicial: total=%d  pointer=%d  next_id=%u\n",
               model->total_count, model->contact_pointer, engine->next_id);
    }
}

static void test_load_from_json(contact_model_t *model) {
    section("[TESTE 2] Model_LoadFromJson(\"" SOURCE_JSON "\")");
    printf("  Lendo arquivo de contatos a partir do diretório atual...\n");
    bool ok = Model_LoadFromJson(model, SOURCE_JSON);
    check(ok, "Arquivo " SOURCE_JSON " lido com sucesso");
    if (!ok) {
        printf("  [FATAL] Não foi possível ler '%s'. Abortando carga.\n",
               SOURCE_JSON);
        return;
    }
    printf("  Total de contatos carregados: %d\n", model->total_count);
    check(model->total_count > 0, "Pelo menos 1 contato carregado");
    check(model->total_count <= STATIC_VECTOR_CAPACITY,
          "Não excedeu a capacidade estática");
    check(model->engine->next_id > 1,
          "next_id ajustado a partir do maior ID carregado");
    printf("  next_id após carregamento: %u\n", model->engine->next_id);
    printf("\n  Amostra dos contatos carregados:\n");
    print_sample(model, 3);
}

static void test_verify_loaded_data(const contact_model_t *model) {
    section("[TESTE 3] Verificação detalhada dos dados carregados");
    printf("  Inspecionando o primeiro contato (ID=1) em detalhe:\n");
    contact_t *c1 = Model_GetContactById(model, 1);
    check(c1 != NULL, "Contato com ID=1 existe");
    if (c1) {
        print_contact(c1);
        check(strlen(c1->nome) > 0, "Campo 'nome' não-vazio");
        check(strlen(c1->sobrenome) > 0, "Campo 'sobrenome' não-vazio");
        check(strlen(c1->email) > 0, "Campo 'email' não-vazio");
        check(strlen(c1->address1.city) > 0, "address1.city não-vazio");
    }
    printf("\n  Inspecionando outro contato (ID=2):\n");
    contact_t *c2 = Model_GetContactById(model, 2);
    check(c2 != NULL, "Contato com ID=2 existe");
    if (c2) {
        print_contact(c2);
    }
    printf("\n  Verificando busca por ID inexistente (ID=999999):\n");
    contact_t *cn = Model_GetContactById(model, 999999);
    check(cn == NULL, "ID inexistente retorna NULL (esperado)");
}

static void test_save_to_json(const contact_model_t *model) {
    section("[TESTE 4] Model_SaveToJson(\"" ROUNDTRIP_JSON "\")");
    printf("  Salvando %d contatos em '%s'...\n",
           model->total_count, ROUNDTRIP_JSON);
    bool ok = Model_SaveToJson(model, ROUNDTRIP_JSON);
    check(ok, "Arquivo de saída gravado com sucesso");
    FILE *f = fopen(ROUNDTRIP_JSON, "r");
    check(f != NULL, "Arquivo '" ROUNDTRIP_JSON "' existe no disco");
    if (f) {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fclose(f);
        printf("  Tamanho do arquivo gerado: %ld bytes\n", size);
        check(size > 0, "Arquivo gerado tem conteúdo");
    }
}

static void test_roundtrip(contact_model_t *model) {
    section("[TESTE 5] Roundtrip: salvar -> destruir -> recarregar");
    int original_count = model->total_count;
    unsigned int original_next_id = model->engine->next_id;
    printf("  Estado antes do roundtrip: total=%d  next_id=%u\n",
           original_count, original_next_id);
    contact_t snapshot_first = model->engine->data[0];
    contact_t snapshot_last = model->engine->data[original_count - 1];
    printf("  Destruindo modelo em memória...\n");
    Model_Destroy(model);
    check(model->engine == NULL, "Motor liberado (engine = NULL)");
    check(model->total_count == 0, "total_count zerado após destroy");
    printf("  Reinicializando modelo...\n");
    contact_engine_t *eng = Model_Initialize(model);
    check(eng != NULL, "Motor reinicializado");
    printf("  Recarregando contatos a partir de '%s'...\n", ROUNDTRIP_JSON);
    bool ok = Model_LoadFromJson(model, ROUNDTRIP_JSON);
    check(ok, "Recarregamento bem-sucedido");
    printf("  Estado após recarregar: total=%d  next_id=%u\n",
           model->total_count, model->engine->next_id);
    check(model->total_count == original_count,
          "Total de contatos preservado no roundtrip");
    contact_t *first_now = Model_GetContactById(model, snapshot_first.id);
    check(first_now != NULL, "Primeiro contato continua presente");
    if (first_now) {
        check(strcmp(first_now->nome, snapshot_first.nome) == 0,
              "Nome do primeiro contato preservado");
        check(strcmp(first_now->sobrenome, snapshot_first.sobrenome) == 0,
              "Sobrenome do primeiro contato preservado");
        check(strcmp(first_now->email, snapshot_first.email) == 0,
              "Email do primeiro contato preservado");
        check(strcmp(first_now->address1.city, snapshot_first.address1.city) == 0,
              "Cidade (address1) do primeiro contato preservada");
        printf("  Primeiro contato após roundtrip:\n");
        print_summary_line(first_now);
    }
    contact_t *last_now = Model_GetContactById(model, snapshot_last.id);
    check(last_now != NULL, "Último contato continua presente");
    if (last_now) {
        check(strcmp(last_now->nome, snapshot_last.nome) == 0,
              "Nome do último contato preservado");
        check(strcmp(last_now->sobrenome, snapshot_last.sobrenome) == 0,
              "Sobrenome do último contato preservado");
        printf("  Último contato após roundtrip:\n");
        print_summary_line(last_now);
    }
}

static void test_add_contact(contact_model_t *model) {
    section("[TESTE 6] Model_AddContact() em base já carregada");
    int before = model->total_count;
    unsigned int before_next = model->engine->next_id;
    printf("  Estado antes: total=%d  next_id=%u\n", before, before_next);
    contact_t novo = {0};
    strcpy(novo.nome, "Teste");
    strcpy(novo.sobrenome, "Automatizado");
    strcpy(novo.cargo, "QA");
    strcpy(novo.celular, "+5521900000000");
    strcpy(novo.email, "teste@example.com");
    strcpy(novo.address1.street, "Rua dos Testes, 42");
    strcpy(novo.address1.city, "Niterói");
    strcpy(novo.address1.state, "RJ");
    strcpy(novo.address1.country, "Brasil");
    printf("  Adicionando contato SEM ID (deve receber ID automático)...\n");
    bool ok = Model_AddContact(model, &novo);
    check(ok, "AddContact retornou true");
    check(model->total_count == before + 1, "total_count incrementado em 1");
    check(model->engine->next_id == before_next + 1, "next_id incrementado");
    contact_t *adicionado = &model->engine->data[model->total_count - 1];
    printf("  ID atribuído automaticamente: %d\n", adicionado->id);
    check(adicionado->id == (int)before_next, "ID recebido == next_id antigo");
    print_summary_line(adicionado);
}

static void test_update_contact(contact_model_t *model) {
    section("[TESTE 7] Model_UpdateContact()");
    contact_t *c1 = Model_GetContactById(model, 1);
    check(c1 != NULL, "Contato ID=1 localizado para atualização");
    if (!c1) return;
    char cargo_anterior[MAX_JOB_LEN];
    strncpy(cargo_anterior, c1->cargo, MAX_JOB_LEN);
    cargo_anterior[MAX_JOB_LEN - 1] = '\0';
    printf("  Cargo antes: %s\n", c1->cargo);
    contact_t atualizado = *c1;
    strcpy(atualizado.cargo, "Cargo Atualizado pelo Teste");
    strcpy(atualizado.telefone, "+5521911112222");
    bool ok = Model_UpdateContact(model, 1, &atualizado);
    check(ok, "Update retornou true");
    c1 = Model_GetContactById(model, 1);
    check(c1 != NULL, "Contato ainda existe após update");
    if (c1) {
        printf("  Cargo depois: %s\n", c1->cargo);
        printf("  Telefone depois: %s\n", c1->telefone);
        check(strcmp(c1->cargo, "Cargo Atualizado pelo Teste") == 0,
              "Cargo foi modificado");
        check(strcmp(c1->telefone, "+5521911112222") == 0,
              "Telefone foi modificado");
        check(c1->id == 1, "ID preservado após update");
    }
    printf("\n  Tentando atualizar ID inexistente (ID=888888):\n");
    contact_t dummy = {0};
    ok = Model_UpdateContact(model, 888888, &dummy);
    check(!ok, "Update em ID inexistente retorna false");
}

static void test_remove_contact(contact_model_t *model) {
    section("[TESTE 8] Model_RemoveContact()");
    int before = model->total_count;
    printf("  Estado antes: total=%d\n", before);
    contact_t *c2 = Model_GetContactById(model, 2);
    check(c2 != NULL, "Contato ID=2 existe antes da remoção");
    printf("  Removendo contato ID=2...\n");
    bool ok = Model_RemoveContact(model, 2);
    check(ok, "Remove retornou true");
    check(model->total_count == before - 1, "total_count decrementado em 1");
    contact_t *gone = Model_GetContactById(model, 2);
    check(gone == NULL, "Contato ID=2 não mais localizável");
    contact_t *c1 = Model_GetContactById(model, 1);
    check(c1 != NULL, "Contato ID=1 sobreviveu à remoção");
    printf("\n  Tentando remover ID inexistente (ID=777777):\n");
    ok = Model_RemoveContact(model, 777777);
    check(!ok, "Remove em ID inexistente retorna false");
}

static void test_apply_filter(contact_model_t *model) {
    section("[TESTE 9] Model_ApplyFilter()");
    model->contact_pointer = 7;
    printf("  contact_pointer antes do filtro: %d\n", model->contact_pointer);
    Model_ApplyFilter(model, "Silva");
    printf("  contact_pointer depois do filtro: %d\n", model->contact_pointer);
    check(model->contact_pointer == 0, "Filtro resetou o cursor para 0");
    printf("\n  Varredura manual por 'Silva' no sobrenome:\n");
    int count = 0;
    for (int i = 0; i < model->total_count; i++) {
        contact_t *c = &model->engine->data[i];
        if (strstr(c->sobrenome, "Silva") != NULL) {
            print_summary_line(c);
            count++;
        }
    }
    printf("  Total de 'Silva' encontrados: %d\n", count);
}

static void test_final_save(contact_model_t *model) {
    section("[TESTE 10] Persistência final em \"" FINAL_JSON "\"");
    printf("  Estado final do modelo: total=%d  next_id=%u\n",
           model->total_count, model->engine->next_id);
    printf("  Salvando estado final em '%s'...\n", FINAL_JSON);
    bool ok = Model_SaveToJson(model, FINAL_JSON);
    check(ok, "Estado final gravado com sucesso");
    printf("  Reabrindo '%s' em um modelo limpo para verificar persistência...\n",
           FINAL_JSON);
    contact_model_t verifier = {0};
    Model_Initialize(&verifier);
    bool loaded = Model_LoadFromJson(&verifier, FINAL_JSON);
    check(loaded, "Arquivo final pôde ser recarregado");
    check(verifier.total_count == model->total_count,
          "Quantidade de contatos no arquivo final está correta");
    contact_t *c1 = Model_GetContactById(&verifier, 1);
    check(c1 != NULL, "Contato modificado (ID=1) presente no arquivo final");
    if (c1) {
        check(strcmp(c1->cargo, "Cargo Atualizado pelo Teste") == 0,
              "Update do TESTE 7 persistiu no arquivo final");
        printf("  Cargo lido do arquivo final: %s\n", c1->cargo);
    }
    contact_t *removed = Model_GetContactById(&verifier, 2);
    check(removed == NULL, "Contato removido (ID=2) ausente no arquivo final");
    Model_Destroy(&verifier);
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   TESTES DO MODELO DE CONTATOS - GENESIS CONTACT MANAGER     ║\n");
    printf("║       Carregamento e persistência de %-12s             ║\n", SOURCE_JSON);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    contact_model_t model = {0};
    test_initialize(&model);
    test_load_from_json(&model);
    if (model.total_count == 0) {
        printf("\n[ABORT] Sem contatos carregados de '%s'. Encerrando.\n",
               SOURCE_JSON);
        Model_Destroy(&model);
        return EXIT_FAILURE;
    }
    test_verify_loaded_data(&model);
    test_save_to_json(&model);
    test_roundtrip(&model);
    test_add_contact(&model);
    test_update_contact(&model);
    test_remove_contact(&model);
    test_apply_filter(&model);
    test_final_save(&model);

    section("[LIMPEZA] Model_Destroy()");
    Model_Destroy(&model);
    check(model.engine == NULL, "Motor liberado");
    check(model.total_count == 0, "total_count zerado");

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RESUMO DOS TESTES                          ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║   Asserções OK   : %-6d                                     ║\n", g_pass);
    printf("║   Asserções FAIL : %-6d                                     ║\n", g_fail);
    printf("║   Arquivos gerados: %s, %s\n", ROUNDTRIP_JSON, FINAL_JSON);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    return (g_fail == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}