#include "CloudSpeechClient.h"
#include "network_param.h"
#include "Credentials.h"

#include <base64.h>
#include <ArduinoJson.h>
#define USE_SERIAL Serial
#include <Arduino.h>
#include <HTTPClient.h>

#define led_3 4
#define led_1 15
#define led_2 2

CloudSpeechClient::CloudSpeechClient(Authentication authentication) {
  this->authentication = authentication;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  client.setCACert(root_ca);

  if (!client.connect(server, 443)) {
    Serial.println("Connection failed!");
    digitalWrite(led_3, 1);
    digitalWrite(led_1, 0);
    digitalWrite(led_2, 0);
  }
}

String ans;

CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
  WiFi.disconnect();
}

void CloudSpeechClient::PrintHttpBody2(Audio* audio)
{
  String enc = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
  enc.replace("\n", "");  // delete last "\n"
  client.print(enc);      // HttpBody2
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    enc.replace("\n", "");// delete last "\n"
    client.print(enc);    // HttpBody2
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  // This first part for Google Speech-to-Text remains the same
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"" + SpeechtoText_Language + "\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n";
  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3;
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");

  client.print(HttpHeader);
  client.print(HttpBody1);
  PrintHttpBody2(audio);
  client.print(HttpBody3);

String My_Answer = "";

while (!client.available());

Serial.println("\n--- Full Google API Response ---");

while (client.available()) {
  char temp = client.read();
  My_Answer = My_Answer + temp;
  Serial.write(temp);
}

Serial.println("\n--- End of Response ---");

client.stop();

  int postion = My_Answer.indexOf('{');
  ans = My_Answer.substring(postion);
  Serial.println("Google STT Response Received.");

  DynamicJsonDocument doc(384);
  DeserializationError error = deserializeJson(doc, ans);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* transcribed_text = doc["results"][0]["alternatives"][0]["transcript"];
  if (transcribed_text) {
      Serial.print("Transcribed Text: ");
      Serial.println(transcribed_text);
  } else {
      Serial.println("Transcription failed or returned empty.");
      Serial2.print("Sorry, I could not hear you clearly. Please try again.");
      return;
  }

  // ========== MODIFIED SECTION FOR GITHUB MODELS API ==========
  {
    Serial.println("Asking AI via GitHub Models...");
    HTTPClient https;
    // Use the new GitHub Models endpoint
    const char* github_models_endpoint = "https://models.github.ai/inference/chat/completions";

    Serial.print("[HTTPS] begin...\n");
    // Note: The root_ca in network_param.h is for Google. 
    // This will work if the ESP32 doesn't strictly validate the cert.
    // If you get SSL/TLS errors, you may need to update the certificate for *.github.ai
    if (https.begin(github_models_endpoint)) {

      https.addHeader("Content-Type", "application/json");
      // Use the GitHub Token for Authorization
      String token_key = String("Bearer ") + GITHUB_MODELS_TOKEN;
      https.addHeader("Authorization", token_key);

      // Construct the JSON payload with the correct GitHub Model ID
      String payload = "{\"model\": \"" + GitHub_Model_ID + "\", "
                     + "\"messages\": [{\"role\": \"user\", \"content\": \"" + String(transcribed_text) + "\"}]"
                     + "\"temperature\": " + AI_Temperature + ", "
                     + "\"max_tokens\": " + AI_Max_Tokens + "}";

      Serial.println("Sending payload to GitHub Models:");
      Serial.println(payload);

      int httpCode = https.POST(payload);

      if (httpCode > 0) {
        String response_payload = https.getString();
        if (httpCode == HTTP_CODE_OK) {
          Serial.println("GitHub Models Response:");
          Serial.println(response_payload);

          DynamicJsonDocument doc2(2048); 
          DeserializationError error = deserializeJson(doc2, response_payload);

          if (error) {
            Serial.print("deserializeJson() for AI response failed: ");
            Serial.println(error.c_str());
            Serial2.print("There was an error processing the response.");
          } else {
            const char* only_ans = doc2["choices"][0]["message"]["content"];
            Serial.print("Extracted Answer: ");
            Serial.println(only_ans);
            Serial2.print(only_ans); // Send the final answer to the TTS ESP32
          }
        } else {
            Serial.printf("[HTTPS] POST... failed, HTTP code: %d\n", httpCode);
            Serial.println("Error response:");
            Serial.println(response_payload); // Print error for debugging
            Serial2.print("I encountered a server error. Please check your GitHub token and try again.");
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        Serial2.print("I'm having trouble connecting to the service.");
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
      Serial2.print("Connection to the AI service could not be established.");
    }

    Serial.println("\nTo ask again, press the button.");
  }
  // ========== END OF MODIFIED SECTION ==========
}