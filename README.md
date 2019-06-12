# Smart Chess Clock
Projekt oprogramowania do inteligentnego zegara szachowego

## Cechy charakterystyczne zegara
  - Zasilanie: Kabel USB/5V
  - Trzy przyciski sterujące (po jednym dla każdego z dwóch graczy oraz jeden przycisk start/stop)
  - Podświetlenie przycisków graczy (LED)
  - Możliwość wyboru jednego z wielu presetów czasowych oraz tworzenia własnych presetów (customizacja in progress)
  - Moduł bluetooth do komunikacji z aplikacją android (in progress)
  - Buzzer wydający dźwięk po zakończeniu czasu jednego z graczy (in progress)
  - 2 wyświetlacze LED siedmiosegmentowe z dwukropkiem
  - Sercem zegara jest płytka z mikrokontrolerem firmy ST Microelectronics
  - Czas graczy jest pamiętany z dokładnością do 1ms
  - Wysoka responsywność przycisków na poziomie 5ms
  
 ## Kwestie programowe
  - Czas pamiętany jest w strukturze godziny/minuty/sekundy/milisekundy (godziny in progress)
  - Presety czasowe są pamiętane w tablicy zawierającej w/w strukturę czasu oraz pole inkrementacji (w sekundach)
  - Po uruchomieniu zegara wykonywana jest procedura wyboru presetu czasowego za pomocą przycisków sterujących w następujący sposób:
  -- (PLAYER1_BUTTON -> zmniejszenie licznika presetu)
  -- (START/STOP -> potwierdzenie wybranego presetu)
  -- (PLAYER1_BUTTON -> zwiększenie licznika presetu)
  - Zegar można zresetować w dowolnej chwili wciskając wszystkie 3 przyciski.
  
