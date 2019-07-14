//STRUKTURA KOMUNIKATU BLUETOOTH TODO
struct _bluetooth
{
	volatile uint8_t operation;
	volatile uint8_t player1time[4];
	volatile uint8_t player2time[4];
};

//Tutaj jest stary kod bluetootha, narazie nie u�ywany
/* FUNKCJE DO OBS�?UGI MODU�?U BLUETOOTH -------------------------------------------------------------
struct _bluetooth buildBluetoothMessage(char operation)
{
	struct _bluetooth out;
	uint32_t p1t, p2t;

	p1t = PLAYER1_TIME.miliseconds + PLAYER1_TIME.seconds * 1000 + PLAYER1_TIME.minutes * 60000;
	p2t = PLAYER2_TIME.miliseconds + PLAYER2_TIME.seconds * 1000 + PLAYER2_TIME.minutes * 60000;

	out.operation = operation;
	out.player1time[0] = (p1t >> 24) & 0xFF;
	out.player1time[1] = (p1t >> 16) & 0xFF;
	out.player1time[2] = (p1t >> 8) & 0xFF;
	out.player1time[3] = p1t & 0xFF;

	out.player2time[0] = (p2t >> 24) & 0xFF;
	out.player2time[1] = (p2t >> 16) & 0xFF;
	out.player2time[2] = (p2t >> 8) & 0xFF;
	out.player2time[3] = p2t & 0xFF;

	return out;
}

void sendBluetoothMessage(char operation)
{
	struct _bluetooth message = buildBluetoothMessage(operation);
	uint16_t messageSize = 9;
	uint8_t messageBytes[9];

	messageBytes[0] = message.operation;
	messageBytes[1] = message.player1time[0];
	messageBytes[2] = message.player1time[1];
	messageBytes[3] = message.player1time[2];
	messageBytes[4] = message.player1time[3];

	messageBytes[5] = message.player2time[0];
	messageBytes[6] = message.player2time[1];
	messageBytes[7] = message.player2time[2];
	messageBytes[8] = message.player2time[3];


	HAL_UART_Transmit_IT(&huart3, messageBytes, messageSize);
}

struct _bluetooth receiveBluetoothMessage()
{
	struct _bluetooth out;
	uint16_t messageSize = 9;
	uint8_t messageBytes[9];


	HAL_UART_Receive_IT(&huart3, messageBytes, messageSize);

	out.operation = messageBytes[0];


	out.player1time[0] = messageBytes[1];
	out.player1time[1] = messageBytes[2];
	out.player1time[2] = messageBytes[3];
	out.player1time[3] = messageBytes[4];

	out.player2time[0] = messageBytes[5];
	out.player2time[1] = messageBytes[6];
	out.player2time[2] = messageBytes[7];
	out.player2time[3] = messageBytes[8];

	return out;
}

void addPresetFromBluetooth()
{
	struct _bluetooth message = receiveBluetoothMessage();

	uint32_t baseTime, increment;
	uint8_t baseTimeArr[4], incrementArr[4];

	//Konwersja komunikatu na integery
	baseTimeArr[0] = message.player1time[0];
	baseTimeArr[1] = message.player1time[1];
	baseTimeArr[2] = message.player1time[2];
	baseTimeArr[3] = message.player1time[3];

	incrementArr[0] = message.player2time[0];
	incrementArr[1] = message.player2time[1];
	incrementArr[2] = message.player2time[2];
	incrementArr[3] = message.player2time[3];


	baseTime = *(int *)baseTimeArr;
	increment = *(int *)incrementArr;

	//Uzupełnienie struktury presetu
	unsigned int divider;

	//Określenie liczby minut
	divider = baseTime / 60000;
	baseTime -= divider * 60000;
	presets[BLUETOOTH_PRESET].time.minutes = divider * 60000;

	//Określenie liczby sekund
	divider = baseTime / 1000;
	baseTime -= divider * 1000;
	presets[BLUETOOTH_PRESET].time.minutes = divider * 1000;

	//Określenie liczby milisekund
	presets[BLUETOOTH_PRESET].time.miliseconds = baseTime;

	//Przypisanie liczby sekund do inkrementu
	presets[BLUETOOTH_PRESET].increment = increment / 1000;
}
*/
