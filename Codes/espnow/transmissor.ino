#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// mac do receptor (B): 88:57:21:B7:54:08
#define WIFI_CHANNEL 1
uint8_t peerMac[] = { 0x88, 0x57, 0x21, 0xB7, 0x54, 0x08 };

static void printMac(const uint8_t *m) {
        for (int i = 0; i < 6; i++) { if (i) Serial.print(":"); Serial.printf("%02X", m[i]); }
}

void onSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {
        Serial.print("Enviado para ");
        if (info && info->des_addr) printMac(info->des_addr); else Serial.print("(desconhecido)");
        Serial.print(" -> ");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FALHA");
}

void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
        Serial.print("Resposta de ");
        if (info && info->src_addr) printMac(info->src_addr); else Serial.print("(desconhecido)");
        Serial.print(" -> ");
        Serial.write(data, len);
        Serial.println();
}

bool addPeer(const uint8_t mac[6]) {
        esp_now_peer_info_t p{};
        memcpy(p.peer_addr, mac, 6);
        p.channel = WIFI_CHANNEL;
        p.encrypt = false;
        p.ifidx = WIFI_IF_STA;
        return esp_now_add_peer(&p) == ESP_OK;
}

// --------- I/O ---------
const uint8_t BTN[4] = {18, 5, 4, 13}; // pull-up interno, ligar botão ao GND

// --------- Debounce ---------
const unsigned long DEBOUNCE_MS = 30;
uint8_t debounced[4];           // 0=solto, 1=pressionado
uint8_t lastStable[4];
uint8_t lastRaw[4];
unsigned long lastChange[4];

char lastSent[5] = "0000";

void setupButtons() {
        for (int i = 0; i < 4; i++) {
                pinMode(BTN[i], INPUT_PULLUP);
                uint8_t raw = (digitalRead(BTN[i]) == LOW) ? 1 : 0;
                debounced[i] = lastStable[i] = lastRaw[i] = raw;
                lastChange[i] = millis();
        }
}

void updateDebounce() {
        for (int i = 0; i < 4; i++) {
                uint8_t raw = (digitalRead(BTN[i]) == LOW) ? 1 : 0;
                if (raw != lastRaw[i]) {
                        lastRaw[i] = raw;
                        lastChange[i] = millis();
                } else {
                        if (millis() - lastChange[i] >= DEBOUNCE_MS && debounced[i] != raw) {
                                debounced[i] = raw;
                        }
                }
        }
}

void makeMsg(char out[5]) {
        for (int i = 0; i < 4; i++) out[i] = debounced[i] ? '1' : '0';
        out[4] = '\0';
}

void setup() {
        Serial.begin(115200);
        delay(300);

        setupButtons();

        WiFi.mode(WIFI_MODE_STA);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
        esp_wifi_set_promiscuous(false);

        if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init falhou"); while (1) delay(1000); }
        esp_now_register_send_cb(onSent);
        esp_now_register_recv_cb(onRecv);

        if (!addPeer(peerMac)) Serial.println("Falha ao adicionar peer");

        Serial.print("Meu MAC: "); Serial.println(WiFi.macAddress());
        Serial.println("Transmissor A pronto. Enviando estado dos 4 botoes quando mudar.");
}

void loop() {
        updateDebounce();

        char msg[5];
        makeMsg(msg);

        if (strncmp(msg, lastSent, 4) != 0) {
                esp_err_t r = esp_now_send(peerMac, (const uint8_t*)msg, 4);
                if (r != ESP_OK) Serial.printf("Erro send: %d\n", r);
                memcpy(lastSent, msg, 5);
                Serial.print("Estado -> "); Serial.println(msg);
        }

        delay(5); // taxa de varredura ~200 Hz
}