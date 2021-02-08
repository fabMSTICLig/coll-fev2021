# include <TimerOne.h>
# include <Servo.h>
# include <Adafruit_NeoPixel.h>
# ifdef __AVR__
 # include <avr/power.h> // Required for 16 MHz Adafruit Trinket
# endif

// S0 VERT
// S1 MARRON
// S2 NOIR
// S3 BLANC

# define S0				6
# define S1				5
# define S2				4
# define S3				3
# define sensorOut		2

# define SERVO_R		8
# define SERVO_L		9

# define LED_PIN		A0
# define LED_COUNT		6

# define BUTTON			7

# define SPEED_FRONT	105
# define SPEED_ZERO		90
# define SPEED_BACK		75

# define THRESHOLD_MAX	60
# define THRESHOLD_MIN	40

# define RED_MODE		0
# define GREEN_MODE		1
# define BLUE_MODE		2
# define BLACK_MODE		3
# define STOP_MODE		4

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int		rgb_array[3] = {0};
int 	mode = STOP_MODE;
int		turn_counter = 0;
Servo	myservo;
Servo	banane;

void	setLedColor(int redValue, int greenValue, int blueValue);

void	initServos()
{
	myservo.attach(SERVO_L);
	banane.attach(SERVO_R);
	delay(50);
	myservo.write(90);
	banane.write(90);
	delay(15);
}

void	initButton()
{
	pinMode(BUTTON, INPUT);
}

void	initLed()
{
	strip.begin();
	strip.show();
	strip.setBrightness(50);
	setLedColor(25, 25, 25);
	delay(50);

}

void	initSensor()
{
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(sensorOut, INPUT);
	// Setting frequency-scaling to 20%
	digitalWrite(S0,HIGH);
	digitalWrite(S1,LOW);
}

 void	setup()
 {
 	Serial.begin(9600);
 	initServos();
	initButton();
	initLed();
	initSensor();
 }

void	getMode()
{
	if (mode == RED_MODE)
	{
		setLedColor(0, 255, 0);
		mode = GREEN_MODE;
	}
	else if (mode == GREEN_MODE)
	{
		setLedColor(0, 0, 255);
		mode = BLUE_MODE;
	}
	else if (mode == BLUE_MODE)
	{
		setLedColor(255, 255, 255);
		mode = BLACK_MODE;
	}
	else if (mode == BLACK_MODE)
	{
		setLedColor(25, 25, 25);
		mode = STOP_MODE;
	}
	else if (mode == STOP_MODE)
	{
		setLedColor(255, 0, 0);
		mode = RED_MODE;
	}
}

void	setLedColor(int redValue, int greenValue, int blueValue)
{
	int32_t color = strip.Color(redValue, greenValue, blueValue);
	Serial.print("setLedColor: ");
	Serial.print(redValue);
	Serial.print(greenValue);
	Serial.println(blueValue);
	for(int i=0; i<strip.numPixels(); i++)
	{
		strip.setPixelColor(i, color);
		strip.show();
		delay(50);
	}
}

int		redDetected()
{
	if (rgb_array[1] > (rgb_array[0] + 20) && rgb_array[2] > (rgb_array[0] + 20) && rgb_array[0] < THRESHOLD_MIN)
	{
		Serial.print("ROUGE ");
		return (1);
	}
	return (0);
}

int		greenDetected()
{
	if (rgb_array[0] > (rgb_array[1] + 20) && rgb_array[2] > (rgb_array[1] + 20) && rgb_array[1] < THRESHOLD_MIN)
	{
		Serial.print("VERT ");
		return (1);
	}
	return (0);
}

int		blueDetected()
{
	if (rgb_array[0] > (rgb_array[2] + 20) && rgb_array[1] > (rgb_array[2] + 20) && rgb_array[2] < THRESHOLD_MIN)
	{
		Serial.print("BLEU ");
		return (1);
	}
	return (0);
}

int		blackDetected()
{
	if (rgb_array[0] < THRESHOLD_MAX && rgb_array[1] < THRESHOLD_MAX && rgb_array[2] < THRESHOLD_MAX)
	{
		Serial.print("NOIR ");
		return (1);
	}
	return(0);
}

void	stopMotors()
{
	Serial.println("stopMotors");
	myservo.write(SPEED_ZERO);
	banane.write(SPEED_ZERO);
}

void	avancer()
{
	Serial.println("AVANCER");
	myservo.write(SPEED_FRONT);
	banane.write(SPEED_BACK);
}

void	reculer()
{
	Serial.println("RECULER");
	myservo.write(SPEED_BACK);
	banane.write(SPEED_FRONT);
}

void	tournerDroite()
{
	myservo.write(90);
	banane.write(100);
}

void	tournerGauche()
{
	myservo.write(80);
	banane.write(90);
}


void	tourner()
{
	Serial.println("TOURNER");
	if (turn_counter < 20)
	{
		tournerGauche();
	}
	else
	{
		tournerDroite();
	}
	turn_counter += 1;
	if (turn_counter > 40)
	{
		turn_counter = 0;
	}
}

void	folowRedLine()
{
	Serial.print(" value => ");
	Serial.print(rgb_array[0]);
	Serial.print("\tgreen ( ");
	Serial.print(rgb_array[1]);
	Serial.print(")\tblue(");
	Serial.print(rgb_array[2]);
	Serial.println(")");
	if (redDetected())
	{
		avancer();
	}
	else
	{
		tourner();
	}
}

void	folowGreenLine()
{
	Serial.print(" value => ");
	Serial.println(rgb_array[1]);
	if (greenDetected())
	{
		avancer();
	}
	else
	{
		tourner();
	}
}

void	folowBlueLine()
{
	Serial.print(" value => ");
	Serial.println(rgb_array[2]);
	if (blueDetected())
	{
		avancer();
	}
	else
	{
		tourner();
	}
}

void	folowBlackLine()
{
	Serial.print("BLACK:\n\tred value => ");
	Serial.println(rgb_array[0]);
	Serial.print("\tgreen value => ");
	Serial.println(rgb_array[0]);
	Serial.print("\tblue value => ");
	Serial.println(rgb_array[0]);
	if (blackDetected())
	{
		avancer();
	}
	else
	{
		tourner();
	}
}

void	getColorValue()
{
	// Setting Red filtered photodiodes to be read
	digitalWrite(S2,LOW);
	digitalWrite(S3,LOW);
	rgb_array[0] = pulseIn(sensorOut, LOW);
	delay(50);
	// Setting Green filtered photodiodes to be read
	digitalWrite(S2,HIGH);
	digitalWrite(S3,HIGH);
	rgb_array[1] = pulseIn(sensorOut, LOW);
	delay(50);
	// Setting Blue filtered photodiodes to be read
	digitalWrite(S2,LOW);
	digitalWrite(S3,HIGH);
	rgb_array[2] = pulseIn(sensorOut, LOW);
	delay(50);
}

void	loop()
{
	getColorValue();
	Serial.println(F("___"));

 	if (digitalRead(BUTTON))
	{
		getMode();
	}
	Serial.print("Mode: ");
	if (mode = RED_MODE)
	{
		Serial.print(F("Red\t"));
		folowRedLine();
	}
	else if (mode = GREEN_MODE)
	{
		Serial.print(F("Green\t"));
		folowGreenLine();
	}
	else if (mode = BLUE_MODE)
	{
		Serial.print(F("Blue\t"));
		folowBlueLine();
	}
	else if (mode = BLACK_MODE)
	{
		Serial.print(F("Black\t"));
		folowBlackLine();
	}
	else
	{
		Serial.println(F("STOP MODE"));
		stopMotors();
		Serial.print(F("Red:\t"));
		Serial.println(rgb_array[0]);
		Serial.print(F("Green:\t"));
		Serial.println(rgb_array[1]);
		Serial.print(F("Blue:\t"));
		Serial.println(rgb_array[2]);
	}
 }
