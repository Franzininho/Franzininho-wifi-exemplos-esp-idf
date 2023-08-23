/*
    Autor: Equipe Embarcados
*/
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#define TAG "spiffs"
#define NUM_MAX_FILES 5

void app_main(void)
{
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = NUM_MAX_FILES,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    
    ESP_LOGW(TAG, "Criando um novo arquivo: franzininho.txt");
    FILE *f = fopen("/spiffs/franzininho.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Falha ao tentar escrever no arquivo");
        return;
    }

    ESP_LOGW(TAG, "Escrevendo no arquivo: franzininho.txt");
    fprintf(f, "Franzininho - SPIFFS demo\n");
    fclose(f);
    ESP_LOGI(TAG, "Arquivo escrito");


    ESP_LOGW(TAG, "Lendo o arquivo: franzininho.txt");
    FILE *file = fopen("/spiffs/franzininho.txt", "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Arquivo não existe");
    }
    else
    {
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL)
        {
            printf(line);
        }
        fclose(file);
    }
    ESP_LOGW(TAG, "Arquivo lido: franzininho.txt");
    esp_vfs_spiffs_unregister(NULL);
}