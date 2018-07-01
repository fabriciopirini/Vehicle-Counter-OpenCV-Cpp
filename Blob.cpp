#include "Blob.h"

// Inicializa os parâmetros do blob
Blob::Blob(vector<Point> contorno) {
    contornoAtual = contorno;
    quadro = boundingRect(contornoAtual);

    Point centro;

    // Calcula o centro do blob
    centro.x = (quadro.x + quadro.x + quadro.width) / 2;
    centro.y = (quadro.y + quadro.y + quadro.height) / 2;

    posicoesCentro.push_back(centro);

    // Calcula a diagonal do quadro do blob
    diagonal = sqrt(pow(quadro.width, 2) + pow(quadro.height, 2));

    // Calcula a proporção do quadro do blob
    proporcao = (float)quadro.width / (float)quadro.height;

    // Define que o blob está na tela e é atual/novo
    naTela = true;
    blobAtualNovo = true;

    numFrames = 0;
}

// Calcula a próxima posição do centro do blob
void Blob::ProxPos(void) {
    int numPos = (int)posicoesCentro.size();

    if (numPos == 1) {
        proxPos.x = posicoesCentro.back().x;
        proxPos.y = posicoesCentro.back().y;
    }
    else if (numPos == 2) {
        int deltaX = posicoesCentro[1].x - posicoesCentro[0].x;
        int deltaY = posicoesCentro[1].y - posicoesCentro[0].y;

        proxPos.x = posicoesCentro.back().x + deltaX;
        proxPos.y = posicoesCentro.back().y + deltaY;
    }
    else if (numPos == 3) {
        int numModX = ((posicoesCentro[2].x - posicoesCentro[1].x) * 2) + ((posicoesCentro[1].x - posicoesCentro[0].x) * 1);
        int deltaX = (int)round((float)numModX / 3.0);
        int somaModY = ((posicoesCentro[2].y - posicoesCentro[1].y) * 2) + ((posicoesCentro[1].y - posicoesCentro[0].y) * 1);
        int deltaY = (int)round((float)somaModY / 3.0);

        proxPos.x = posicoesCentro.back().x + deltaX;
        proxPos.y = posicoesCentro.back().y + deltaY;
    }
    else if (numPos == 4) {
        int numModX = ((posicoesCentro[3].x - posicoesCentro[2].x) * 3) + ((posicoesCentro[2].x - posicoesCentro[1].x) * 2) + ((posicoesCentro[1].x - posicoesCentro[0].x) * 1);
        int deltaX = (int)round((float)numModX / 6.0);
        int somaModY = ((posicoesCentro[3].y - posicoesCentro[2].y) * 3) + ((posicoesCentro[2].y - posicoesCentro[1].y) * 2) + ((posicoesCentro[1].y - posicoesCentro[0].y) * 1);
        int deltaY = (int)round((float)somaModY / 6.0);

        proxPos.x = posicoesCentro.back().x + deltaX;
        proxPos.y = posicoesCentro.back().y + deltaY;
    }
    else if (numPos >= 5) {
        int numModX = ((posicoesCentro[numPos - 1].x - posicoesCentro[numPos - 2].x) * 4) + ((posicoesCentro[numPos - 2].x - posicoesCentro[numPos - 3].x) * 3) + ((posicoesCentro[numPos - 3].x - posicoesCentro[numPos - 4].x) * 2) + ((posicoesCentro[numPos - 4].x - posicoesCentro[numPos - 5].x) * 1);
        int deltaX = (int)round((float)numModX / 10.0);
        int somaModY = ((posicoesCentro[numPos - 1].y - posicoesCentro[numPos - 2].y) * 4) + ((posicoesCentro[numPos - 2].y - posicoesCentro[numPos - 3].y) * 3) + ((posicoesCentro[numPos - 3].y - posicoesCentro[numPos - 4].y) * 2) + ((posicoesCentro[numPos - 4].y - posicoesCentro[numPos - 5].y) * 1);
        int deltaY = (int)round((float)somaModY / 10.0);

        proxPos.x = posicoesCentro.back().x + deltaX;
        proxPos.y = posicoesCentro.back().y + deltaY;
    }
    else {}
}