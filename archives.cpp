#include <fstream>
#include <iostream>
#include <string.h>


using namespace std;
struct Aluno
{
    char name[100];
    int idade;
    float nota;
};

int main () {
    /*ofstream arquivo("teste.txt");

    arquivo << "testando o arquivo estou escrevendo\n";

    arquivo.close();

    cout << "arquivo criado com sucesso\n";
    return 0;
    

    ifstream arquivo("teste.txt");

    string linha;

    while(getline(arquivo, linha)) {
        cout << linha << endl;
    }
    arquivo.close();
    return 0;
    */
    ofstream arquivo("alunos.dat", ios::binary);

    Aluno a1 = {"Lucas", 19, 9.9};
    Aluno a2 = {"Barbara", 19, 10.0};

    arquivo.write((char*)&a1, sizeof(Aluno));
    arquivo.write((char*)&a2, sizeof(Aluno));

    arquivo.close();

    cout << "registros escritos com sucessos" << endl;
}
