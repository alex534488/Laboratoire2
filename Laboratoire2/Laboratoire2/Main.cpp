#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
using namespace std;

#define CHAR unsigned char
const CHAR bitMap = 251;
const CHAR FAT = 252;
const int blockSize = 64;

const CHAR bitMap = 251;
const CHAR FAT = 252;

class DisqueDur{
private:

	// Un seul fichier est �crit par le programme en ex�cution (HD.DH).
	fstream hd;
public:
	DisqueDur(string fichier) {
		hd.open(fichier, fstream::in | fstream::out | fstream::binary);
		if (!hd.is_open) {
			string erreur = "Erreur lors de l'ouverture de " + fichier;
			cout << erreur;
		}
	}

	~DisqueDur() {

	}

	void readBlock(CHAR numBlock, CHAR* tampLecture);
	void writeBlock(CHAR numBlock, CHAR* tampLecture);
	CHAR GetBlockLibre();
	bool IsBlockLibre(CHAR numBlock);
};

void UpdateFiles();
void UpdateScreen();
void read(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void write(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture);
void deleteEOF(CHAR* nomFichier, fpos_t position);

DisqueDur* dur;

int main() {
	dur = new DisqueDur("hd.dh");

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

void read(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	CHAR premierBlock;
	// ouvre un fichier (s'il existe) et lit (selon les param�tres) les donn�es pour les mettre dans TampLecture puis le referme.
	premierBlock = FindFichier(nomFichier);
}

CHAR FindFichier(CHAR* nomFichier) {
	CHAR* blockBuffer1;
	CHAR* blockBuffer2;
	for (CHAR i = FAT; i < 256; i++) {
		dur->readBlock(i, blockBuffer1);
		for (int j = 0; j < blockSize; j++) {
			dur->readBlock(blockBuffer1[j], blockBuffer2);
			if(nomFichier == blockBuffer2) {

			}
		}
	}
}

void write(CHAR* nomFichier, fpos_t position, int nbChar, CHAR* TampLecture) {
	// ouvre un fichier ou le cr�e au besoin et �crit (selon les param�tres) TampEcriture puis le referme.
}

void deleteEOF(CHAR* nomFichier, fpos_t position) {
	// ouvre un fichier existant et le coupe � "position" puis le referme. Si position est 0, le fichier est effac�.
}

// FONCTIONS DU DISQUE DUR

void DisqueDur::readBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekg(pos);
	hd.read((char*)tampLecture, blockSize);
}

void DisqueDur::writeBlock(CHAR numBlock, CHAR* tampLecture) {
	streampos pos = numBlock * blockSize;
	hd.seekp(pos);
	hd.write((char*)tampLecture, blockSize);

}

// ouput seekp
// input seekg