int ASignal = A0;

// LED pins
#define RED_LED_PIN D1
#define GREEN_LED_PIN D2
#define BLUE_LED_PIN D3

void setup()
{
    pinMode(ASignal, INPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    int sensorValue = analogRead(ASignal);

    // Convert the sensor value to a percentage (0 - 100%)
    float humidityPercent = 100 - ((sensorValue / 1023.0) * 100);

    // Print sensor value and humidity percentage to Serial
    Serial.print("Moisture: ");
    Serial.print(sensorValue);
    Serial.print(" | Humidity: ");
    Serial.print(humidityPercent, 1);
    Serial.println("%");

    // Control LED based on humidity level
    if (humidityPercent > 70)
    {
        digitalWrite(BLUE_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
    }
    else if (humidityPercent >= 30 && humidityPercent <= 70)
    {
        digitalWrite(BLUE_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
    }
    else
    {
        digitalWrite(BLUE_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
    }

    delay(3000);
}