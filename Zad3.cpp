#include "NXT++.h"
#include <iostream>
#include <conio.h>
using namespace std;
using namespace NXT;

Comm::NXTComm comm;

//PRZED U¯YCIEM SPRZETU DOK£ADNIE SPRAWDZAMY: 
//DO KTÓRYCH PORTÓW WEJŒCIOWYCH (IN) POD£¥CZONE S¥ SENSORY,\
//DO KTÓRYCH PORTÓW WYJŒCIOWYCH (OUT) POD£¥CZONE S¥ SERVO-MOTORY,
//STARAMY SIÊ PISAC KOD Z MO¯LIWOŒCI¥ ZATRZYMANIA ROBOTA W KA¯DEJ CHWILI,
//ROBOT POWIENIEN BYC PRZEMIESZCZONY W MIEJSCE TESTÓW ZA POŒREDNICTWEM ZDALNEGO STEROWANIA,
//CZUJNIKI SA INICJOWANE PRZED PETLA NIESKONCZONA,
//CZUJNIKI SA DEAKTYWOWANE ZA PETLA NIESKONCZONA, LUB PO KOMENDZIE k,

int cte;
int ksi = 1;


int convert(int k, int orient) {
	if (k < 180) {
		if (orient <= k + 180) {
			orient = orient - k;
		}
		else orient = (orient - k) - 360;
	}
	if (k >= 180) {
		if (orient >= k - 180) {
			orient = orient - k;
		}
		else orient = 360 - (k - orient);
	}
	return orient;
}


int main()
{
	srand(time(NULL));

	cout << "Szukanie sygnalu... to moze zajac chwile.\n";
	if (NXT::OpenBT(&comm)) //initialize the NXT and continue if it succeeds
	{
		cout << "Sygnal znaleziony!";
		cout << "\nPoziom baterii = " << NXT::BatteryLevel(&comm);
		cout << "\nDostepna pamiec flash = " << NXT::GetAvailableFlash(&comm);

		int x = 30;//szybkoœæ jazdy w przód i ty³
		int xtemp = 30;//potrzebna do sprawdzania czy predkosc sie zmienila

		int y = 20;//szybkoœæ obrotu w lewo, prawo
		int ytemp = 20;//potrzebna do sprawdzania czy predkosc sie zmienila

		char last_value;//zapamietana ostatnia instrukcja steruj¹ca

		//Przed uzyciem sprawdzamy numer portu do którego jest podl¹czony sensor, 
		//NXT::Sensor::SetTouch(&comm, IN_1);//inicjacja sensora dotyku

		//aktywacja czujnika koloru
		NXT::Sensor::SetLight(&comm, IN_1, 1);
		int color;

		// kompas
		NXT::Sensor::SetSonar(&comm, IN_3);

		//aktywacja sonaru,
		//NXT::Sensor::SetSonar(&comm, IN_3);
		//int sonar;

		//poczatek programu
		NXT::StartProgram(&comm, "program1");

		//zmienna uywana do sterowania
		char decyzja;

		//zmienna do orientacji
		int orient;
		int k1 = 30;
		int k2 = 30;
		int k = 60;

		cout << "\n ___Podstawowe sterowanie do uzupelnienia___";
		cout << "\n A - jazda w lewo";
		cout << "\n D - jazda w prawo";
		cout << "\n W - jazda na wprost";
		cout << "\n S - jazda do tylu";
		cout << "\n U - odczyt z sonaru";
		cout << "\n C - odczyt nasycenia koloru";
		cout << "\n H - Zatrzymanie silnikow";
		cout << "\n T - Jedz w wybranym kierunku";
		cout << "\n O - odczyt orientacji";
		cout << "\n K - Koniec Programu";

		do
		{
			//Przed uzyciem sprawdzamy numer portu do którego jest podl¹czony sensor, 
		/*
		if(NXT::Sensor::GetValue(&comm, IN_1)==1)
			{
			for(int i=1;i<1000;i=i+10)
				{
				NXT::PlayTone(&comm, i, 1);
				Wait(1);
				}
			}
		*/

			if (kbhit() == true)
			{
				decyzja = getch();

				if (decyzja == 'T' || decyzja == 't')
				{
					cout << "\nProgram kompas - start" << endl;
					cout << "\nkierunek = " << k;
					while (true)
					{
						orient = 2 * NXT::Sensor::GetSonarValue(&comm, IN_3);
						cte = convert(k, orient);
						if (cte < -1)
						{
							if (abs(cte) > k1) { k2 = 0; }
							else { k2 = k1 + (ksi * cte); }
						}
						else if (cte > 1)
						{
							if (cte > k2) { k1 = 0; }
							else { k1 = k2 - (ksi * cte); }
						}
						else
						{
							k1 = 30;
							k2 = 30;
						}
						cout << "\norient = " << orient << ", cte = " << cte;
						cout << "\nk1 = " << k1 << ", k2 = " << k2;


						NXT::Motor::SetForward(&comm, OUT_B, k2);
						NXT::Motor::SetForward(&comm, OUT_C, k1);


						if (kbhit() == true) { break; }
					}

					NXT::Motor::SetForward(&comm, OUT_B, 0);
					NXT::Motor::SetForward(&comm, OUT_C, 0);

					k1 = 30;
					k2 = 30;
				}

				if (decyzja == 'O' || decyzja == 'o')
				{
					orient = 2 * NXT::Sensor::GetSonarValue(&comm, IN_2);
					cte = convert(k, orient);
					cout << "\norient = " << orient << ", cte = " << cte;
				}

				if (decyzja == 'N' || decyzja == 'n')
				{
					x -= 10;
				}

				if (decyzja == 'M' || decyzja == 'm')
				{
					x += 10;
				}

				if (decyzja == 'V' || decyzja == 'v')
				{
					y--;
				}

				if (decyzja == 'B' || decyzja == 'b')
				{
					y++;
				}
				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'A' || decyzja == 'a' || (ytemp != y && last_value == 'a'))
				{
					ytemp = y;
					last_value = 'a';
					NXT::Motor::SetForward(&comm, OUT_B, y);
					NXT::Motor::SetReverse(&comm, OUT_C, y);
					//cout<<"SonarValue = "<<NXT::Sensor::GetSonarValue(&comm, IN_4);
				}

				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'D' || decyzja == 'd' || (ytemp != y && last_value == 'd'))
				{
					ytemp = y;
					last_value = 'd';
					NXT::Motor::SetReverse(&comm, OUT_B, y);
					NXT::Motor::SetForward(&comm, OUT_C, y);
				}

				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'W' || decyzja == 'w' || (xtemp != x && last_value == 'w'))
				{
					xtemp = x;
					last_value = 'w';
					NXT::Motor::SetForward(&comm, OUT_B, x);
					NXT::Motor::SetForward(&comm, OUT_C, x);
				}

				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'S' || decyzja == 's' || (xtemp != x && last_value == 's'))
				{
					xtemp = x;
					last_value = 's';
					NXT::Motor::SetReverse(&comm, OUT_B, x);
					NXT::Motor::SetReverse(&comm, OUT_C, x);
				}

				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'H' || decyzja == 'h')
				{
					NXT::Motor::SetForward(&comm, OUT_B, 0);
					NXT::Motor::SetForward(&comm, OUT_C, 0);
					NXT::Motor::Stop(&comm, OUT_B, 0);
					NXT::Motor::Stop(&comm, OUT_C, 0);
				}

				//Przed uzyciem serwo sprawdzamy numer portu do którego jest podl¹czony,
				if (decyzja == 'K' || decyzja == 'k')
				{
					NXT::Motor::Stop(&comm, OUT_B, 0);
					NXT::Motor::Stop(&comm, OUT_C, 0);
					NXT::Motor::Stop(&comm, OUT_A, 0);
					NXT::Sensor::SetSonarOff(&comm, IN_3);
					NXT::Sensor::SetSonarOff(&comm, IN_2);
					NXT::Sensor::SetLight(&comm, IN_1, 0);

					break;
				}

				//Przed uzyciem sprawdzamy numer portu do którego jest podl¹czony sensor, 
				if (decyzja == 'U' || decyzja == 'u')
				{
					cout << " Wskazanie sonaru = " << NXT::Sensor::GetSonarValue(&comm, IN_3);
				}

				//Przed uzyciem sprawdzamy numer portu do którego jest podlaczony sensor, 
				if (decyzja == 'C' || decyzja == 'c')
				{
					color = NXT::Sensor::GetValue(&comm, IN_1);
					Wait(50);
					cout << "\nNasycenie swaitla = " << color;
				}
			}

		} while (decyzja != 'k' && decyzja != 'K');

		NXT::StopProgram(&comm);

	}
	NXT::Close(&comm); //close communication with NXT

	cout << "\n\nPress ENTER to close program";
	getchar();

	return 0;
}


//opcje dotyczace Mindstorm NXT v. 1.0
//if(decyzja=='L' || decyzja=='l')
// {
//  NXT::Sensor::SetLight(&comm, IN_3, 1);
//  Wait(50);
//  cout<<"LightValue = "<<NXT::Sensor::GetValue(&comm, IN_3);
//  Wait(50);
//  NXT::Sensor::SetLight(&comm, IN_3, 0);
// }

	//if(decyzja=='O' || decyzja=='o')
// {
// NXT::Sensor::SetSound(&comm, IN_2);
// Wait(50);
// cout<<"SoundValue = "<<NXT::Sensor::GetValue(&comm, IN_2);
// }