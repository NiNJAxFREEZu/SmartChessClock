//DEFINICJE PRESETÓW CZASOWYCH --------------------------------------------------------------------
#define NUMBER_OF_PRESETS		12

#define BULLET_1_0				0
#define BULLET_2_1				1
#define BLITZ_3_0				2
#define BLITZ_3_2				3
#define BLITZ_5_0				4
#define	BLITZ_5_3				5
#define	RAPID_10_0				6
#define	RAPID_15_0				7
#define	RAPID_15_15				8
#define	RAPID_25_0				9
#define	RAPID_25_10				10
#define CLASSICAL_30_0			11

//STRUKTURA CZASU
struct _time
{
	volatile uint8_t minutes;
	volatile uint8_t seconds;
	volatile uint16_t miliseconds;
};

//STRUKTURA PRESETU CZASOWEGO
struct _preset
{
	struct _time time;
	volatile uint8_t player1Increment;
	volatile uint8_t player2Increment;
};

//ZMIENNE GLOBALNE --------------------------------------------------------------------------------
struct _preset presets[NUMBER_OF_PRESETS];	//Struktura zawieraj¹ca presety czasowe
struct _time PLAYER1_TIME;	//Struktura przechowuj¹ca informacje o czasie gracza nr 1
struct _time PLAYER2_TIME;	//Struktura przechowuj¹ca informacje o czasie gracza nr 2
//-------------------------------------------------------------------------------------------------

//FUNKCJE -----------------------------------------------------------------------------------------
int decrement(struct _time* clock)	//Funkcja pomniejszaj¹ca strukturê czasu o 1ms
{
	if(clock->miliseconds <= 0)
	{
		if(clock->seconds == 0)
		{
			if(clock->minutes == 0)
			{
				return 1;		//Koniec czasu - zwracamy 1
			}
			else	{clock->minutes--; clock->seconds = 59; clock->miliseconds = 999;}

		}
		else {clock->seconds--; clock->miliseconds = 999;}

	}
	else
		clock->miliseconds--;


	return 0;
}

void increment(struct _time* clock, int secondInc)	//Funkcja inkremetuj¹ca strukturê czasow¹ o zadan¹ liczbê sekund
{
	if(secondInc == 0)	//Je¿eli inkrementacja jest "zerowa"
		return;

	clock->seconds += secondInc;

	if(clock->seconds >= 60)	//Je¿eli przekroczymy licznik minut podczas inkrementacji
	{
		clock->minutes++;
		clock->seconds -= 60;
	}
}

void presetInit()	//INICJALIZACJA PRESETÓW CZASOWYCH	WARNING: HARDCODE AHEAD
{
	//BULLET 1+0
	presets[BULLET_1_0].time.miliseconds = 0;
	presets[BULLET_1_0].time.seconds = 0;
	presets[BULLET_1_0].time.minutes = 1;
	presets[BULLET_1_0].player1Increment = 0;
	presets[BULLET_1_0].player2Increment = 0;

	//BULLET 2+1
	presets[BULLET_2_1].time.miliseconds = 0;
	presets[BULLET_2_1].time.seconds = 0;
	presets[BULLET_2_1].time.minutes = 2;
	presets[BULLET_2_1].player1Increment = 1;
	presets[BULLET_2_1].player2Increment = 1;

	//BLITZ 3+0
	presets[BLITZ_3_0].time.miliseconds = 0;
	presets[BLITZ_3_0].time.seconds = 0;
	presets[BLITZ_3_0].time.minutes = 3;
	presets[BLITZ_3_0].player1Increment = 0;
	presets[BLITZ_3_0].player2Increment = 0;

	//BLITZ 3+2
	presets[BLITZ_3_2].time.miliseconds = 0;
	presets[BLITZ_3_2].time.seconds = 0;
	presets[BLITZ_3_2].time.minutes = 3;
	presets[BLITZ_3_2].player1Increment = 2;
	presets[BLITZ_3_2].player2Increment = 2;

	//BLITZ 5+0
	presets[BLITZ_5_0].time.miliseconds = 0;
	presets[BLITZ_5_0].time.seconds = 0;
	presets[BLITZ_5_0].time.minutes = 5;
	presets[BLITZ_5_0].player1Increment = 0;
	presets[BLITZ_5_0].player2Increment = 0;

	//BLITZ 5+3
	presets[BLITZ_5_3].time.miliseconds = 0;
	presets[BLITZ_5_3].time.seconds = 0;
	presets[BLITZ_5_3].time.minutes = 5;
	presets[BLITZ_5_3].player1Increment = 3;
	presets[BLITZ_5_3].player2Increment = 3;

	//RAPID 10+0
	presets[RAPID_10_0].time.miliseconds = 0;
	presets[RAPID_10_0].time.seconds = 0;
	presets[RAPID_10_0].time.minutes = 10;
	presets[RAPID_10_0].player1Increment = 0;
	presets[RAPID_10_0].player2Increment = 0;

	//RAPID 15+0
	presets[RAPID_15_0].time.miliseconds = 0;
	presets[RAPID_15_0].time.seconds = 0;
	presets[RAPID_15_0].time.minutes = 15;
	presets[RAPID_15_0].player1Increment = 0;
	presets[RAPID_15_0].player2Increment = 0;

	//RAPID 15+15
	presets[RAPID_15_15].time.miliseconds = 0;
	presets[RAPID_15_15].time.seconds = 0;
	presets[RAPID_15_15].time.minutes = 15;
	presets[RAPID_15_15].player1Increment = 15;
	presets[RAPID_15_15].player2Increment = 15;

	//RAPID 25+0
	presets[RAPID_25_0].time.miliseconds = 0;
	presets[RAPID_25_0].time.seconds = 0;
	presets[RAPID_25_0].time.minutes = 25;
	presets[RAPID_25_0].player1Increment = 0;
	presets[RAPID_25_0].player2Increment = 0;

	//RAPID 25+10
	presets[RAPID_25_10].time.miliseconds = 0;
	presets[RAPID_25_10].time.seconds = 0;
	presets[RAPID_25_10].time.minutes = 25;
	presets[RAPID_25_10].player1Increment = 10;
	presets[RAPID_25_10].player2Increment = 10;

	//CLASSICAL 30+0
	presets[CLASSICAL_30_0].time.miliseconds = 0;
	presets[CLASSICAL_30_0].time.seconds = 0;
	presets[CLASSICAL_30_0].time.minutes = 30;
	presets[CLASSICAL_30_0].player1Increment = 0;
	presets[CLASSICAL_30_0].player2Increment = 0;
}
