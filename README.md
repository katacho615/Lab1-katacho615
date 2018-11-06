# Pakman
Program „Pakman” jest odwzorowaniem popularnej gry Pacman. Stworzony przeze mnie
“Pacman” będzie jedno planszową grą. Plansza ta będzie miała wymiary 30x40, gdzie miarą
jednostki jest jeden kwadrat. Kwadrat będzie ścieżką po której może poruszać się tytułowy
Pakman. W grze będziemy sterować bohaterem starając się zebrać punkty rozsiane po planszy.
Punkty będą na planszy w postaci pojedynczych kółeczek lub jako “owoce” na środku
kwadratu. W zbieraniu punktów przeszkadzać będą wrogowie w postaci duszków. Duszki będą
poruszały się w naszym kierunku, a po styku z naszym bohaterem gra zakończy się
niepowodzeniem. Gdy wszystkie okrągłe punkty zostaną zebrane, a od początku do zebrania
ostatniego punktu, bohater nie zetknął się z wrogiem ani razu, gra kończy się sukcesem.

W grze będą klasy takie jak:
- bohater (możliwy tryb dwuosobowy)
- wróg (zawiera wszystkich wrogów)
- punkty okrągłe (zawiera punkty pojawienia się punktów okrągłych)
- punkty owoce (zawiera punkty pojawienia się punktów owocowych)
- plansza (obiekt planszy będzie zawierał informacje o wyglądzie planszy)

Użyte zostanie dziedziczenie. Klasy wróg i punkty (okrągłe i owoce) będą dziedziczyły z klasy
bohater. Klasa wróg odziedziczy możliwość poruszania się, a klasy punktowe odziedziczą
specyficzne dla każdego obiektu miejsce na planszy ale będą miały wyłączoną możliwość
ruszania się. Klasy bohater, wróg oraz klasy punktowe będą wspólnie dziedziczyły z klasy
plansza. Odziedziczą planszę, a wraz z nią puste miejsca na planszy w których mogą się
utworzyć kolejne obiekty.

Będzie użyty polimorfizm do funkcji poruszającej Pakmanem, "zdobywającej" (liczącej)
punkty oraz kończącą grę przy styku z wrogiem, będzie to pojedyncza funkcja która będzie
wywoływana przy każdym ruchu Pakmana lub jednego z wrogów. Funkcja wywoła się różnie
zależnie od tego czy Pakman będzie stał na polu z punktem okrągłym, punktem owocowym czy
wrogiem.

Strumień wejścia wyjścia zostanie użyty do sterowania Pakmanem oraz menu głównym. 
