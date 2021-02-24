#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

using namespace std;

/**
* Struktura główna, przechowująca zmienne z jednej linii razem ze zmienną z kontenera map<>
*/
struct Line {
	string hour;
	string day;
	int week_index;
	string group;
	string surname;
	string subject;
};

/**
* Struktura przechowująca zmienne niezbędne do walidacji danych zapisanych w pliku txt
*/
struct Valid {
	string firstPartHour_1;
	string secondPartHour_1;
	string firstPartHour_2;
	string secondPartHour_2;
	string groupV;
	string dayV;
};

/**
* Struktura przechowująca zmienne zamienionych wartości godzin (string) na intiger
*/
struct HourInt {
	int firstH1int;
	int secondH1int;
	int firstH2int;
	int secondH2int;
};

/**
* Kontener asocjacyjny map<>, który mapuje dzień tygodnia (klucz) na odpowiadająca mu wartość intiger (wartość)
*/
map<string, int> weekdays = {
	{"pn", 1},
	{"wt", 2},
	{"sr", 3},
	{"cz", 4},
	{"pt", 5},
	{"sb", 6},
	{"nd", 7}
};

/**
* Niestandardowe funkcje używanie w programie
*/
void readLine(ifstream& file, Line& line);
void readLineTab(ifstream& file, Line* lineTab, const int numOfLines);
void printLine(Line& line);
void printLineTab(Line* lineTab, const int numOfLines);
int globalCheck(string& filePath, Line* lineTab, Line& lineS, Valid& valid, HourInt& hourInt, const int numOfLines);
void lineCount(string& filePath, int& numOfLines);
bool comp(const Line& first, const Line& second);
void outFile(ifstream& file, Line* lineTab, int& numOfLines);

/**
* @brief Funkcja główna main sterująca programem.
*
* Funkcja oprócz sterowania pozostałymi funkcjami umożliwia uruchomienie programu z linii wiersza poleceń
* przy pomocy odpowiednich argumentów i odpowiedniego przełącznika.
*
* @note Funkcja posiada walidację pliku txt po otwarciu, oraz warunek sprawdzający odpowiednią wartość przełącznika.
* Jeśli zostanie podanych zbyt mało argumentów lub ich brak, program wypisze instrukcję, jak go uruchomić.
* Jeśli argumenty zostaną podane prawidłowo, ale wartość przełącznika będzie inna od tej zadeklarowanej,
* program wypiszę instrukcję z prawidłową wartością przełącznika.
*
* @param argc - liczba argumentów podanych podczas wywoływania programu
* @param argv - tablica argumentów argc
*/
int main(int argc, char** argv) {
	/** funkcja dodająca polskie znaki do konsoli */
	setlocale(LC_CTYPE, "Polish");

	int numOfLines = 0;
	ifstream file;
	string filePath;
	Line lineS;
	Valid hourS;
	HourInt hourIntS;
	/** warunki wprowadzanych argumentów i odpowiednie wyprowadzanie wiadomości o błędzie */
	if (argc == 3) {
		if (!string(argv[1]).compare("-i")) {
			filePath = argv[2];
		}
		else {
			cerr << endl;
			cerr << "  ================ ERROR! =================" << endl;
			cerr << "  Została podana zła wartość przełącznika." << endl;
			cerr << "  Prawidłowa wartość przełącznika to '-i'" << endl;
			cerr << "  =========================================" << endl;
			return 1;
		}
	}
	else {
		cerr << endl;
		cerr << "  ========================= INSTRUKCJA URUCHOMIENIA PROGRAMU ===========================" << endl;
		cerr << "  Aby uruchomić program należy w poszczególnych krokach:" << endl;
		cerr << "  1. Wpisać do konsoli przełącznik '-i'" << endl;
		cerr << "  2. Po przełączniku umieścić spację a po niej nazwę pliku (w tym przypadku 'plik.txt')" << endl;
		cerr << "  ======================================================================================" << endl;
		return 1;
	}

	lineCount(filePath, numOfLines);

	/** utworzenie tablicy dynamicznej przechowującej ilość wierszy pliku txt */
	Line* lineTab;
	lineTab= new Line[numOfLines];

	/** walidacja danych w pliku txt */
	if (globalCheck(filePath, lineTab, lineS, hourS, hourIntS, numOfLines)) {
		return 0;
	}

	/** ponowna walidacja pliku txt */
	file.open(filePath.c_str());
	if (file.fail()) {
		cerr << endl;
		cerr << "  ================= ERROR! ================" << endl;
		cerr << "  Nie udało się otworzyć pliku: " << filePath << endl;
		cerr << "  Plik jest uszkodzony lub nie istnieje. " << endl;
		cerr << "  =========================================" << endl;
		return 1;
	}

	readLineTab(file, lineTab, numOfLines);
	//printLineTab(lineTab, numOfLines);
	sort(lineTab, lineTab + numOfLines, comp);
	outFile(file, lineTab, numOfLines);

	/** usuwanie tablicy dynamicznej, w celu zwolnienia pamięci i zamknięcie pliku */
	delete[] lineTab;
	file.close();
	return 4;
}

/**
* @brief Funkcja void wczytująca dane z jednej linii pliku txt
*
* Głównym zadaniem funkcji jest wczytanie pojedyńczej linii i zapisania wartości do odpowiednich zmiennych.
* Funkcja nie sprawdza poprawności wczytywania danych.
* Dodatkowo funkcja przypisuje wartość z kontenera map<> do zmiennej week_index
*
* @param file - odwołanie do pliku txt
* @param line - odwołanie się do struktury Line
*/
void readLine(ifstream& file, Line& line) {
	file >> line.hour >> line.day >> line.group >> line.surname >> line.subject;
	line.week_index = weekdays[line.day];  //operator indeksowania
}

/**
* @brief Funkcja void wczytująca dane z wszystkich wierszy pliku txt
*
* Funkcja nie sprawdza poprawności wczytywania danych.
*			
* @param lineTab - odwołanie do tablicy dynamicznej przez wskaźnik
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej
*/
void readLineTab(ifstream& file, Line* lineTab, const int numOfLines) {
	for (int i = 0; i < numOfLines; i++) {
		readLine(file, lineTab[i]);
	}
}

/**
* @brief Funkcja void drukująca na ekran zawartość jednej linii pliku txt.
*
* UWAGA! Funkcja jest funkcją sprawdzającą i nie jest niezbędnia do prawidłowego działania programu.
* Funkcja nie sprawdza poprawności wyświetlanych danych.
*
* @param lineTab - odwołanie do tablicy dynamicznej przez wskaźnik
*/
void printLine(Line& line) {
	cout << line.hour << " " << line.day << " " << line.group << " " << line.surname << " " << line.subject << endl;
}

/**
* @brief Funkcja void drukująca na ekran zawartość wszystkich wierszy z pliku txt.
* 
* UWAGA! Funkcja jest funkcją sprawdzającą i nie jest niezbędnia do prawidłowego działania programu.
* Funkcja nie sprawdza poprawności wyświetlanych danych. 
* 
* @param lineTab - odwołanie do tablicy dynamicznej przez wskaźnik
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej
*/ 
void printLineTab(Line* lineTab, const int numOfLines) {
	for (int i = 0; i < numOfLines; i++) {
		printLine(lineTab[i]);
	}
}

/**
* @brief Funkcja void walidująca dane z pliku txt.
* 
* Funkcja rozpoczyna od walidacji ogólnej pliku.
* Następnie sprawdza godzinę, wzgędem tego, czy wartość minut i/lub godzin nie została przekroczona.
* Poźniej upewnia się, czy godzina zakończenia nie jest mniejsza od godziny rozpoczęcia.
* Następnie sprawdza, czy dzień tygodnia jest wpisany prawidłowo.
* Po czym waliduję nazwę grupy, jeżeli w pliku przypadkowo znalazła się literówka.
* Jeśli nie wykryto żadnych błędów, funkcja wyświetla w konsoli komunikat o poprawnej walidacji pliku.
* 
* @param filePath - ścieżka do pliku txt
* @param lineTab - odwołanie do tablicy dynamicznej przez wskaźnik
* @param lineS - odwołanie do struktury Line
* @param valid - odwołanie do struktury Valid
* @param hourInt - odwołanie do struktury HourInt
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej 
* 
* @return - dla wartości 0, program wykonuje się dalej; dla wartości != 0 program wyświetla komunikat o błędzie
*/
int globalCheck(string& filePath, Line* lineTab, Line& lineS, Valid& valid, HourInt& hourInt, const int numOfLines) {
	ifstream file;
	file.open(filePath.c_str());
	if (file.fail()) {
		return 1;
	}
	else {
		for (int i = 0; i < numOfLines; i++) {
			string line;
			getline(file, line);
			/** wycięcie dwóch części (h i min) godziny rozpoczęcia i zamiana na intiger */
			valid.firstPartHour_1 = line.substr(0, 2);
			valid.secondPartHour_1 = line.substr(3, 2);
			hourInt.firstH1int = stoi(valid.firstPartHour_1);
			hourInt.secondH1int = stoi(valid.secondPartHour_1);
			/** wycięcie dwóch części (h i min) godziny zakończenia i zamiana na intiger */
			valid.firstPartHour_2 = line.substr(6, 2);
			valid.secondPartHour_2 = line.substr(9, 2);
			hourInt.firstH2int = stoi(valid.firstPartHour_2);
			hourInt.secondH2int = stoi(valid.secondPartHour_2);
			/** zamiana godziny na liczbę całkowitą */
			string globalH1 = valid.firstPartHour_1 + valid.secondPartHour_1;
			int globalH1int = stoi(globalH1);
			string globalH2 = valid.firstPartHour_2 + valid.secondPartHour_2;
			int globalH2int = stoi(globalH2);
			/** wycięcie dnia tygodnia */
			valid.dayV = line.substr(12, 2);
			/** wycięcie nazwy grupy */
			valid.groupV = line.substr(15, 2);
			/** walidacja zakresu godziny */
			if ((hourInt.firstH1int > 24 || hourInt.secondH1int >= 60) || (hourInt.firstH2int > 24 || hourInt.secondH2int >= 60)) {
				cerr << endl;
				cerr << "  ========================= ERROR! ==========================" << endl;
				cerr << "  Błąd wartości godzinowych w " << i + 1 << " wierszu!" << endl;
				cerr << "  Wartość godziny wykracza poza skalę!" << endl;
				cerr << "  ===========================================================" << endl;
				return 2;
			}
			/** walidacja zakresu dwóch godzin */
			else if (globalH2int - globalH1int <= 0) {
				cerr << endl;
				cerr << "  ========================= ERROR! ==========================" << endl;
				cerr << "  Błąd wartości godzinowych w " << i + 1 << " wierszu!" << endl;
				cerr << "  Godzina rozpoczęcia jest późniejsza od godziny zakończenia!" << endl;
				cerr << "  ===========================================================" << endl;
				return 3;
			}
			/** walidacja nazwy grupy */
			else if (valid.groupV != "gr") {
				cerr << endl;
				cerr << "  ========================= ERROR! ==========================" << endl;
				cerr << "  Błąd w nazwie grupy w " << i + 1 << " wierszu!" << endl;
				cerr << "  Podana nazwa grupy nie istnieje!" << endl;
				cerr << "  ===========================================================" << endl;
				return 4;
			}
			/** walidacja nazwy dni tygodnia */
			else if ((valid.dayV != "pn") && (valid.dayV != "wt") && (valid.dayV != "sr") && 
					 (valid.dayV != "cz") && (valid.dayV != "pt") && (valid.dayV != "sb") && (valid.dayV != "nd")) {
				cerr << endl;
				cerr << "  ========================= ERROR! ==========================" << endl;
				cerr << "  Błąd w nazwach dni tygodnia w " << i + 1 << " wierszu!" << endl;
				cerr << "  Podana nazwa dnia tygodnia nie istnieje!" << endl;
				cerr << "  ===========================================================" << endl;
				return 5;
			}
		}
	}
	cout << endl;
	cout << "  ==================== SUKCES! =====================" << endl;
	cout << "  Walidacja pliku wejściowego przebiegła pomyślnie." << endl;
	cout << "  ==================================================" << endl;
	file.close();
	return 0;
}

/**
* @brief Funkcja void zliczająca ilość wierszy.
* 
* Głównym celem funkcji jest zliczenie ilości wierszy w pliku txt.
* Funkcja posiada podstawową walidację odczytu pliku. Jeśli plik nie istnieje, wyprowadza informację o błędzie.
* 
* @param filePath - ścieżka do pliku txt
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej
*/
void lineCount(string& filePath, int& numOfLines) {
	ifstream file;
	file.open(filePath.c_str()); {
		if (file.fail()) {
			cerr << endl;
			cerr << "  ================= ERROR! ================" << endl;
			cerr << "  Nie udało się otworzyć pliku: " << filePath << endl;
			cerr << "  Plik jest uszkodzony lub nie istnieje. " << endl;
			cerr << "  =========================================" << endl;
			file.close();
		}
		else {
			string line;
			while (getline(file, line)) {
				numOfLines++;
			}
			file.close();
		}
	}
}

/**
* @brief Funkcja bool porównująca zmienne z linii
*
* Głównym celem funkcji jest zliczenie ilości wierszy w pliku txt.
* Funkcja nie posiada walidacji pliku.
*
* @param filePath - ścieżka do pliku txt
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej
*/
bool comp(const Line& first, const Line& second) {
	if (first.surname < second.surname) {
		return true;
	}
	if (first.surname > second.surname) {
		return false;
	}
	if (first.week_index < second.week_index) {
		return true;
	}
	if (first.week_index > second.week_index) {
		return false;
	}
	if (first.hour < second.hour) {
		return true;
	}
	return false;
}

/**
* @brief Funkcja bool porównująca zmienne z linii
*
* Głównym celem funkcji jest zliczenie ilości wierszy w pliku txt.
* Funkcja nie posiada walidacji pliku.
*
* @param filePath - ścieżka do pliku txt
* @param numOfLines - ilość wierszy w pliku txt w postaci wartości liczbowej
*/
void outFile(ifstream& file, Line* lineTab, int& numOfLines) {
	string surname;
	ofstream outFiles;
	for (const Line* li = lineTab; li < lineTab + numOfLines; li++) {
		if (li->surname != surname) {
			if (outFiles.is_open()) outFiles.close();
			outFiles.open(li->surname + string(".txt"));
			surname = li->surname;
		}
		outFiles << li->hour << " " << li->day << " " << li->group << " " << li->subject << endl;
	}
	outFiles.close();
}
