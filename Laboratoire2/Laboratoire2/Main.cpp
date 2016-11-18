#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
using namespace std;


#define CHAR unsigned char

class DisqueDur{
private:
	const CHAR bitMap = 251;
	const CHAR FAT = 252;

	// Un seul fichier est �crit par le programme en ex�cution (HD.DH).
	string nomFichier;//HD.DH
	ifstream fichierLecture;
	ofstream fichierEcriture;
public:
	DisqueDur() {

	}

	DisqueDur(string fichier) {
		nomFichier = fichier;
	}

	~DisqueDur() {

	}

	void readBlock(int numBlock, string tampLecture);
	void writeBlock(int numBloc, string tampLecture);
};

void UpdateFiles();
void UpdateScreen();
void read(string nomFichier, fpos_t position, int nbChar, string TampLecture);
void write(string nomFichier, fpos_t position, int nbChar, string TampLecture);
void deleteEOF(string nomFichier, fpos_t position);

int main() {
	int timer = 0;
	string buffer;

	while (true) {
		UpdateFiles();
		if (timer >= 50000) {
			UpdateScreen();
			timer = 0;
		}
		timer++;
	}

	return 0;
}

// FONCTIONS MAIN

void UpdateFiles() {
	/*
	Votre programme principal �crit al�atoirement des fichiers nomm�es a.txt, b.txt, etc. qui contiennent des a,b, c, etc. Il efface al�atoirement des fichiers et des fins de fichiers.
	*/
}

void UpdateScreen() {
	/*
	L'ensemble de son fichier HD.DH soit 256 lettres majuscules ou minuscules selon que le bloc est plein ou incomplet. 
	Par exemple, �crire un A (majuscule) pour repr�senter un bloc plein de "a" et un a (minuscule) pour un bloc incomplet de "a".
	La liste des fichiers et des blocs qu'ils occupent.
	*/
}

// FONCTIONS DES INTERACTIONS AVEC LES FICHIERS

void read(string nomFichier, fpos_t position, int nbChar, string TampLecture) {
	// ouvre un fichier (s'il existe) et lit (selon les param�tres) les donn�es pour les mettre dans TampLecture puis le referme.
}

void write(string nomFichier, fpos_t position, int nbChar, string TampLecture) {
	// ouvre un fichier ou le cr�e au besoin et �crit (selon les param�tres) TampEcriture puis le referme.
}

void deleteEOF(string nomFichier, fpos_t position) {
	// ouvre un fichier existant et le coupe � "position" puis le referme. Si position est 0, le fichier est effac�.
}

// FONCTIONS DU DISQUE DUR

void DisqueDur::readBlock(int numBlock, string tampLecture) {

}

void DisqueDur::writeBlock(int numBloc, string tampLecture) {

}