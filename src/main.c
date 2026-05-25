#include <stdio.h>
#include <string.h>
#include "../include/model.h"

contact_t new_contact;

int main(void) {
    contact_model_t *model = Model_Initialize();

    Model_LoadFromJson(model, "data/contacts.json");

    new_contact.id = 0;
    strcpy(new_contact.nome, "João");
    strcpy(new_contact.sobrenome, "Pedro");
    strcpy(new_contact.cargo, "Cerimoniário");
    strcpy(new_contact.telefone, "(21)99713-7274");
    strcpy(new_contact.telefone2, "2222-2222");
    strcpy(new_contact.celular, "(21) 99713-7274");
    strcpy(new_contact.email, "email@exemplo.com");
    strcpy(new_contact.instagram, "@exemplo");
    strcpy(new_contact.url, "www.exemplo.com");

    strcpy(new_contact.address1.street, "Rua Exemplo");
    strcpy(new_contact.address1.neighborhood, "Bairro Exemplo");
    strcpy(new_contact.address1.city, "Cidade Exemplo");
    strcpy(new_contact.address1.state, "EX");
    strcpy(new_contact.address1.country, "Pais Exemplo");
    strcpy(new_contact.address1.zip_code, "00000-000");

    strcpy(new_contact.address2.street, "Rua Exemplo 2");
    strcpy(new_contact.address2.neighborhood, "Bairro Exemplo 2");
    strcpy(new_contact.address2.city, "Cidade Exemplo 2");
    strcpy(new_contact.address2.state, "EX");
    strcpy(new_contact.address2.country, "Pais Exemplo");
    strcpy(new_contact.address2.zip_code, "11111-111");

    Model_AddContact(model, &new_contact);

    printf("=========================================\n");
    printf("O nome do contato é %s \n", model->engine->data[0].nome);
    printf("O sobrenome do contato é %s \n", model->engine->data[0].sobrenome);
    printf("O cargo do contato é %s \n", model->engine->data[0].cargo);
    printf("O sobrenome do contato é %s \n", model->engine->data[0].celular);
    printf("=========================================\n");


    return 0;
}