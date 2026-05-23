#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>
#include "contact_types.h"

#define STATIC_VECTOR_CAPACITY 5000

// Estrutura do motor físico interno
// contact_engine_t em tese é uma opção da lista
typedef struct {
    contact_t *data;
    unsigned int next_id;
} contact_engine_t;

// Estrutura opaca para o motor de armazenamento
typedef struct {
    contact_engine_t *engine;    // Ponteiro para o vetor físico oculto
    int total_count;             // Quantidade atual de contatos salvos no vetor
    int contact_pointer;         // Posição atual apontada no vetor
} contact_model_t;

// --- Funções Fornecidas (Brindes) ---
contact_model_t* Model_Initialize(void);
void Model_Destroy(contact_model_t *model);
bool Model_SaveToJson(const contact_model_t *model, const char *filename);
bool Model_LoadFromJson(contact_model_t *model, const char *filename);

// --- Funções que VOCÊ precisa implementar ---
bool Model_AddContact(contact_model_t *model, const contact_t *new_contact);
contact_t* Model_GetContactById(const contact_model_t *model, unsigned int id);
bool Model_UpdateContact(contact_model_t *model, unsigned int id, const contact_t *updated_data);
bool Model_RemoveContact(contact_model_t *model, unsigned int id); // Lembre-se: shift dos elementos para não deixar buracos!
void Model_ApplyFilter(contact_model_t *model, const char *search_query);

#endif // MODEL_H


