
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "time.h"
#include "bluetooth.h"
#include "ledDisplay1.h"
#include "ledDisplay2.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//PREPROCESOR
#define UI_PLAYER1_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)		//Turning on player 1 diode
#define UI_PLAYER1_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)	//Turning off player 1 diode
#define UI_PLAYER1_BUTTON		GPIO_PIN_2												//Player 1 button handle

#define UI_PLAYER2_DIODE_ON		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)		//Turning on player 2 diode
#define UI_PLAYER2_DIODE_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)	//Turning off player 2 diode
#define UI_PLAYER2_BUTTON		GPIO_PIN_13												//Player 2 button handle

#define DEBUG_DIODE_ON			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET)		//Turning on the blue diode on board
#define DEBUG_DIODE_OFF			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET)	//Turning off the blue diode on board
#define DEBUG_DIODE_TOGGLE		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8)
#define UI_PAUSE_BUTTON			GPIO_PIN_1												//Pause button handle

#define BUZZER_ON				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)		//Turning on buzzer
#define BUZZER_OFF				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)	//Turning off buzzer

//ZMIENNE GLOBALNE
extern struct _preset presets[NUMBER_OF_PRESETS]; //From the file time.h
extern struct _time PLAYER1_TIME;
extern struct _time PLAYER2_TIME;

uint8_t _player1Increment;			//Holds the amount of time in seconds to increment for player 1
uint8_t _player2Increment;			//Holds the amount of time in seconds to increment for player 2
volatile uint8_t _currentPlayer;	//Indicates the current player to move (1/2)
volatile uint8_t _pause = 1;		//Indicates if the clock is paused (1) or not (0)
volatile uint8_t _gameOver = 0;		//Indicates if one of players time ran out
volatile uint8_t _refresh = 0;		//Used for refreshing the LED displays
volatile int8_t _presetSelect = 0;	//Used as a index for the array of presets and as an indicator to different phases of selecting a preset
volatile uint8_t _displayOn;		//Indicates if the displays are on or off

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM10_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* FUNCTIONS FOR MEASURING AND MANAGING TIME----------------------------------*/
void setClocks(int presetIndex)						//Funkcja ustawia czas obu zegarów z numeru presetu
{
	PLAYER1_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER1_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER1_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	PLAYER2_TIME.minutes = 		presets[presetIndex].time.minutes;
	PLAYER2_TIME.seconds = 		presets[presetIndex].time.seconds;
	PLAYER2_TIME.miliseconds = 	presets[presetIndex].time.miliseconds;

	_player1Increment = presets[presetIndex].player1Increment;
	_player2Increment = presets[presetIndex].player2Increment;
}

void clockTick()	//Invokes all logic behind reducing player's to move time
{
	if(_currentPlayer == 1)
	{
		if(decrement(&PLAYER1_TIME) == 0)
		{
			;
		}
		else	//Player 1 has ran out of time
		{

			//TIM6 interrupts upon starting! That's super intended (feature) !!!
			HAL_TIM_Base_Start_IT(&htim6);	//Starts a timer that will make buzzer stop beeping and beep again a couple times

			_pause = 1;
			_gameOver = 1;
			HAL_TIM_Base_Stop_IT(&htim2);
			HAL_TIM_Base_Stop_IT(&htim9);

		}
	}

	else if(_currentPlayer == 2)
	{
		if(decrement(&PLAYER2_TIME) == 0)
		{
			;
		}
		else	//Player 2 has ran out of time
		{
			//TIM6 interrupts upon starting! That's super intended (feature) !!!
			HAL_TIM_Base_Start_IT(&htim6);	//Starts a timer that will make buzzer stop beeping and beep again a couple times

			_pause = 1;
			_gameOver = 1;
			HAL_TIM_Base_Stop_IT(&htim2);
			HAL_TIM_Base_Stop_IT(&htim9);
		}
	}
}

void clockIncrement()	//FUNCKJA INKREMENTUJĄCA CZAS ZEGARA WYWO�?YWANA W PRZERWANIU PRZYCISKÓW i całą logikę z tym związaną
{
	if(_pause == 0)	//If the game is not paused we perform incrementation of time
	{

	if(_currentPlayer == 1)
		increment(&PLAYER1_TIME, _player1Increment);

	else if(_currentPlayer == 2)
		increment(&PLAYER2_TIME, _player2Increment);

	}
}

void incrementPreset()		//Funkcja inkrementująca kursor presetu
{
	if(_presetSelect < NUMBER_OF_PRESETS)
		_presetSelect++;
	
	else if(_presetSelect == NUMBER_OF_PRESETS)
		_presetSelect = 0;
}

void decrementPreset()		//Funkcja dekrementująca kursor presetu
{
	if(_presetSelect > 0)
		_presetSelect--;
		
	else if(_presetSelect == 0)
		_presetSelect = NUMBER_OF_PRESETS;
}

/*-------------------------------------------------------------------------------------------------
FUNKCJE UI I PRZERWANIA--------------------------------------------------------------------------*/
void onDisplays()
{
	tm1637SetBrightness('8');
	tm1637SetBrightness2('8');
	_displayOn = 1;
}

void offDisplays()
{
	tm1637SetBrightness('0');
	tm1637SetBrightness2('0');
	_displayOn = 0;
}

void buzzerBeeper()
{
	static int beepCounter = 0;

	if(beepCounter % 2 == 0)	//if there is no beep
		BUZZER_ON;
	else
		BUZZER_OFF;

	++beepCounter;

	if(beepCounter >= 6)
		HAL_TIM_Base_Stop_IT(&htim6);
}

int timeToDisplay(struct _time* clock)	//Fukcja zwraca integera do wyświetlenia na ekranie
{
	return clock->minutes * 100 + clock->seconds;
}

void switchTimer()				//Funkcja przełączająca timer podczas pauzowania/wznawiania gry
{
	if(_gameOver == 0)
	{
	   	DEBUG_DIODE_TOGGLE;

	   	if(_pause == 1)								//Wnowienie gry po zatrzymaniu
	   	{
	   		_pause = 0;

	   		if(_currentPlayer == 1)
	   		{
	   			HAL_TIM_Base_Start_IT(&htim2);		//Wznowienie timera odmierzającego czas graczowi 1
	   		}
	   		else if(_currentPlayer == 2)
	   		{
	   			HAL_TIM_Base_Start_IT(&htim9);
	   		}

	   	}

	   	else if(_pause == 0)						//Spauzowanie gry
	   	{
	   		_pause = 1;
	   		HAL_TIM_Base_Stop_IT(&htim2);			//Zatrzymanie timera odmierzającego czas
	   		HAL_TIM_Base_Stop_IT(&htim9);
	  	}
	 }
}

void switchPlayers()				//Funkcja zmienajaca któremu graczowi ma uplywac czas
{
	if(_gameOver == 0)				//Jeżeli gra nie dobiegła jeszcze końca
	{
		if(_currentPlayer == 1)
		{
			UI_PLAYER1_DIODE_OFF;
			UI_PLAYER2_DIODE_ON;
			clockIncrement();	//Inkrementacja czasu jest w osobnej funkcji żeby umożliwic zmianę graczy na pauzie bez dodawania czasu
			_currentPlayer = 2;
			return;
		}

		if(_currentPlayer == 2)
		{
			UI_PLAYER1_DIODE_ON;
			UI_PLAYER2_DIODE_OFF;
			clockIncrement();
			_currentPlayer = 1;
			return;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 	//Przerwania GPIO
{
   	if(GPIO_Pin == UI_PAUSE_BUTTON)				//Wcisniecie przycisku START/STOP
   			HAL_TIM_Base_Start_IT(&htim10);

   	if(GPIO_Pin == UI_PLAYER1_BUTTON)			//Wcisniecie przycisku PLAYER1
   			HAL_TIM_Base_Start_IT(&htim10);

   	if(GPIO_Pin == UI_PLAYER2_BUTTON)			//Wcisniecie przycisku PLAYER2
   			HAL_TIM_Base_Start_IT(&htim10);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//Przerwania Timerów
{
   	if(htim->Instance == TIM2)	//Przepełnienie timera nr 2 -> upłynięcie milisekundy gracza 1
         clockTick();
         //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);	Można na oscyloskopie sprawdzic, czy timer odmierza dokładnie 1 ms

   	if(htim->Instance == TIM9)	//Przepełnienie timera nr 9 -> upłynięcie milisekundy gracza 2
   	     clockTick();


   	if(htim->Instance == TIM3)	//Przepełnienie timera nr 3 -> odświeżenie wyświetlaczy
   		_refresh = 1;

   	if(htim->Instance == TIM6)	//BEEP BOOP na koniec gry
   		buzzerBeeper();

   	if(htim->Instance == TIM10)	//Przepełnienie timera nr 10 -> debouncing przycisków
   	{
   		//FUNKCJA RESET - WSZYSTKIE TRZY PRZYCISKI WCIŚNI�?TE
   		if(HAL_GPIO_ReadPin(GPIOB, UI_PAUSE_BUTTON) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, UI_PLAYER1_BUTTON) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOB, UI_PLAYER2_BUTTON) == GPIO_PIN_RESET)
   		{
   			//Na razie programowo
   			int i;
   			for(i = 0; i < 1000000; ++i);

   			NVIC_SystemReset();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PAUSE_BUTTON) == GPIO_PIN_RESET)			//Przycisk pauzy
   		{
   			if(_presetSelect != -1)		//Wciśnięcie przysicku pauzy rozpoczyna rozgrywkę
   			{
   				setClocks(_presetSelect);
   				_presetSelect = -1;
   				_pause = 1;
   			}

   			else
   				switchTimer();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PLAYER1_BUTTON) == GPIO_PIN_RESET)		//Gracz 1
   		{
   			if(_presetSelect != -1)
   				decrementPreset();

   			else if(_currentPlayer == 1)
   			   	switchPlayers();
   		}

   		if(HAL_GPIO_ReadPin(GPIOB, UI_PLAYER2_BUTTON) == GPIO_PIN_RESET)		//Gracz 2
   		{
   			if(_presetSelect != -1)
   			   	incrementPreset();

   			else if(_currentPlayer == 2)
   				switchPlayers();
   		}

   	   	HAL_TIM_Base_Stop_IT(&htim10);
   	   	TIM10->CNT = 0;
   	}
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  //INICJALIACJA WYŚWIETLACZY I EXTI
  tm1637Init();
  tm1637Init2();
  onDisplays();										//Włączenie obu wyświetlaczy
  __HAL_GPIO_EXTI_CLEAR_IT(UI_PLAYER1_BUTTON);		//Odświeżenie EXTI
  __HAL_GPIO_EXTI_CLEAR_IT(UI_PLAYER2_BUTTON);
  __HAL_GPIO_EXTI_CLEAR_IT(UI_PAUSE_BUTTON);

  //TIM6 interrupts upon starting! That's super intended (feature) !!!
  //Clearing the interrupt flags after timer inits ensures that timer interrupts won't be triggered uopn starting the timers
  __HAL_TIM_CLEAR_FLAG(&htim2, TIM_SR_UIF);
  __HAL_TIM_CLEAR_FLAG(&htim3, TIM_SR_UIF);
  __HAL_TIM_CLEAR_FLAG(&htim9, TIM_SR_UIF);
  __HAL_TIM_CLEAR_FLAG(&htim10, TIM_SR_UIF);

  //INICJALIZACJA ZEGARA---------------------------------------------------------------------------
  presetInit();						//Wypełnienie pamięci presetami
  HAL_TIM_Base_Start_IT(&htim3);	//Uruchomienie timera odświeżającego wyświetlacze


  //WYBÓR PRESETU CZASOWEGO------------------------------------------------------------------------
  while(1)		//Wciśnięcie przycisku pauzy rozpocznie rozgrywkę z wybranym presetem
  {
	  if(_refresh == 1)
	  {
		  tm1637DisplayDecimal(_presetSelect + 1, 0);
		  tm1637DisplayDecimal2(presets[_presetSelect].time.minutes * 100 + presets[_presetSelect].player1Increment, 1);
	  	  _refresh = 0;
	  }

	  if(_presetSelect == -1)
		  break;
  }

  _currentPlayer = 1;				//Domyślnie gracz 1 "ma ruch"
  UI_PLAYER1_DIODE_ON;				//Włączenie diody gracza 1

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //ROZGRYWKA--------------------------------------------------------------------------------------
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  if(_refresh == 1)
	  {
		  tm1637DisplayDecimal(timeToDisplay(&PLAYER1_TIME), !((PLAYER1_TIME.miliseconds / 500) && (_currentPlayer == 1)) || _pause);
		  tm1637DisplayDecimal2(timeToDisplay(&PLAYER2_TIME), !((PLAYER2_TIME.miliseconds / 500) && (_currentPlayer == 2)) || _pause);
		  _refresh = 0;
	  }
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* TIM6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* TIM10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM10_IRQn);
  /* TIM9_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM9_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM9_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 159;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM6 init function */
static void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 799;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 9999;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM9 init function */
static void MX_TIM9_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 15;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 999;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim9, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM10 init function */
static void MX_TIM10_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 159;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 499;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim10, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BUZZER_Pin|PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_TIMER_GPIO_Port, DEBUG_TIMER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAUSE_BUTTON_Pin PLAYER1_BUTTON_Pin PLAYER2_BUTTON_Pin */
  GPIO_InitStruct.Pin = PAUSE_BUTTON_Pin|PLAYER1_BUTTON_Pin|PLAYER2_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PLAYER1_DIODE_Pin PLAYER2_DIODE_Pin */
  GPIO_InitStruct.Pin = PLAYER1_DIODE_Pin|PLAYER2_DIODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_TIMER_Pin */
  GPIO_InitStruct.Pin = DEBUG_TIMER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_TIMER_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
