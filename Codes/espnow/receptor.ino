#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// mac do transmissor (A): 24:0A:C4:57:BE:F0
#define WIFI_CHANNEL 1
uint8_t peerMac[] = { 0x24, 0x0A, 0xC4, 0x57, 0xBE, 0xF0 };

static void printMac(const uint8_t *m) {
        for (int i = 0; i < 6; i++) { if (i) Serial.print(":"); Serial.printf("%02X", m[i]); }
}

void onSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {
        Serial.print("Enviado para ");
        if (info && info->des_addr) printMac(info->des_addr); else Serial.print("(desconhecido)");
        Serial.print(" -> ");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FALHA");
}

// LEDs correspondentes aos 4 caracteres recebidos
const uint8_t LEDS[4] = {23, 22, 21, 19};

bool addPeer(const uint8_t mac[6]) {
        esp_now_peer_info_t p{};
        memcpy(p.peer_addr, mac, 6);
        p.channel = WIFI_CHANNEL;
        p.encrypt = false;
        p.ifidx = WIFI_IF_STA;
        return esp_now_add_peer(&p) == ESP_OK;
}

void applyCmd(const uint8_t* data, int len) {
        if (len < 4) return;
        for (int i = 0; i < 4; i++) {
                uint8_t v = (data[i] == '1') ? HIGH : LOW;
                digitalWrite(LEDS[i], v);
        }
}

void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
        Serial.print("Recebido de ");
        if (info && info->src_addr) printMac(info->src_addr); else Serial.print("(desconhecido)");
        Serial.print(" -> ");
        Serial.write(data, len);
        Serial.println();

        applyCmd(data, len);

        const char* ack = "Comando recebido!";
        esp_now_send(peerMac, (const uint8_t*)ack, strlen(ack));
}

void setup() {
        Serial.begin(115200);
        delay(300);

        for (int i = 0; i < 4; i++) { pinMode(LEDS[i], OUTPUT); digitalWrite(LEDS[i], LOW); }

        WiFi.mode(WIFI_MODE_STA);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);

        if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init falhou"); while (1) delay(1000); }
        esp_now_register_send_cb(onSent);
        esp_now_register_recv_cb(onRecv);

        if (!addPeer(peerMac)) Serial.println("Falha ao adicionar peer");

        Serial.print("Meu MAC: "); Serial.println(WiFi.macAddress());
        Serial.println("Receptor B pronto. Aguardando comandos de 4 chars ('0'/'1').");
}

void loop() {
        delay(10);
}