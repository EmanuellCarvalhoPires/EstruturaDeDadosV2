#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "contact_types.h"

typedef struct contact_engine {
    contact_t *data;
    unsigned int next_id;
} contact_engine_t;

typedef struct {
    contact_engine_t *engine;
    int total_count;
    int contact_pointer;
} contact_model_t;

contact_engine_t* Model_Initialize(contact_model_t *model);
void Model_Destroy(contact_model_t *model);

bool Model_AddContact(contact_model_t *model, const contact_t *new_contact);
contact_t* Model_GetContactById(const contact_model_t *model, unsigned int id);
bool Model_UpdateContact(contact_model_t *model, unsigned int id,
                         const contact_t *updated_data);
bool Model_RemoveContact(contact_model_t *model, unsigned int id);
void Model_ApplyFilter(contact_model_t *model, const char *query);

bool Model_SaveToJson(const contact_model_t *model, const char *filename);
bool Model_LoadFromJson(contact_model_t *model, const char *filename);

#endif /* MODEL_H */
