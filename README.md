# Smart Chess Clock
Projekt oprogramowania do inteligentnego zegara szachowego

## Cechy charakterystyczne zegara
  - Zasilanie: Kabel USB/5V
  - Trzy przyciski sterujące (po jednym dla każdego z dwóch graczy oraz jeden przycisk start/stop)
  - Podświetlenie przycisków graczy (LED)
  - Możliwość wyboru jednego z wielu presetów czasowych oraz tworzenia własnych presetów (customizacja in progress)
  - Moduł bluetooth do komunikacji z aplikacją android (in progress)
  - Buzzer wydający dźwięk po zakończeniu czasu jednego z graczy
  - 2 wyświetlacze LED siedmiosegmentowe z dwukropkiem
  - Sercem zegara jest płytka z mikrokontrolerem firmy ST Microelectronics
  - Czas graczy jest pamiętany z dokładnością do 1ms
  - Wysoka responsywność przycisków na poziomie 5ms
  
 ## Kwestie programowe
  - Czas pamiętany jest w strukturze godziny/minuty/sekundy/milisekundy (godziny in progress)
  - Presety czasowe są pamiętane w tablicy zawierającej w/w strukturę czasu oraz pole inkrementacji (w sekundach)
  - Po uruchomieniu zegara wykonywana jest procedura wyboru presetu czasowego za pomocą przycisków sterujących w następujący sposób:
    - (PLAYER1_BUTTON -> zmniejszenie licznika presetu)
    - (START/STOP -> potwierdzenie wybranego presetu)
    - (PLAYER1_BUTTON -> zwiększenie licznika presetu)
  - Zegar można zresetować w dowolnej chwili wciskając wszystkie 3 przyciski.
  
  ### Podłączanie przewodów
 
 *DISP_1*
- ![#0ab858](https://placehold.it/15/0ab858/000000?text=+) `PB6`
- ![#0a4aff](https://placehold.it/15/0a4aff/000000?text=+) `PB9`
- ![#8b05c7](https://placehold.it/15/8b05c7/000000?text=+) `VDD`
- ![#999999](https://placehold.it/15/999999/000000?text=+) `GND`

*DISP_2*
- ![#fffa5c](https://placehold.it/15/fffa5c/000000?text=+) `PB10`
- ![#ff7d00](https://placehold.it/15/ff7d00/000000?text=+) `PB11`
- ![#ff2400](https://placehold.it/15/ff2400/000000?text=+) `VDD`
- ![#654321](https://placehold.it/15/654321/000000?text=+) `GND`

*BUZZER*
- ![#ff2400](https://placehold.it/15/ff2400/000000?text=+) `PB0`
- ![#808000](https://placehold.it/15/808000/000000?text=+) `GND`


__*Przyciski*__

Przy podłączaniu przycisków należy patrzeć od strony zegarów i przyjąć przycisk po lewej stronie za odpowiednie dla gracza pierwszego, a po prawej dla gracza drugiego

*P1*
 - ![#ff2400](https://placehold.it/15/ff2400/000000?text=+) `PB2`
 - ![#000000](https://placehold.it/15/000000/000000?text=+) `PB12`

*P2*
 - ![#ff2400](https://placehold.it/15/ff2400/000000?text=+) `PB13`
 - ![#000000](https://placehold.it/15/000000/000000?text=+) `PB14`
 
*Pause*
- ![#000000](https://placehold.it/15/000000/000000?text=+) `PB1`

*Wspólna masa*
- ![#000000](https://placehold.it/15/000000/000000?text=+) `GND`


### Warte uwagi

- **Timer 6 ma specyficzną zależność przy której przerwanie następuje od razu po wywołaniu funkcji, która go uruchamia.** Na ten moment okazuje się, że jest to efekt przydatny i wykorzystywany, jednak w ramach przeciwdziałania wszystkie timery (oprócz 6) mają w kodzie wyłączone flagi za to odpowiedzialne.
    
- **Kiedy STM korzysta ze źródła zasilania innego niż ???gniazdko???, taktowanie procesora jest zmniejszone o połowę.** W ramach rekompensaty zmieniono częstotliwość taktowania STM'a bezpośrednio w programie CUBE. Należy jednak o tym pamiętać w przypadku chęci zmiany źródła zasilania.
