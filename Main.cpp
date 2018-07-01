/** \mainpage Contador de Veículos
 * \section intro_sec Introdução
 * Esse projeto consiste em um sistema capaz de contar o número de veículos que estão de passagem em uma rodovia com a utilização de técnicas aprendidas em Processamentos Digitais de Sinais
 *
 * \section install_sec Execução
 * Executar os comandos na pasta base do projeto:
 * <br>
 * g++ -o main *.cpp `pkg-config --cflags --libs opencv`
 * <br>
 * ./main
*/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include "Blob.h"

using namespace std;
using namespace cv;

/** Define escalares de cores */
const Scalar PRETO = Scalar(0.0, 0.0, 0.0); /** Representação da cor Preta*/
const Scalar BRANCO = Scalar(255.0, 255.0, 255.0); /** Representação da cor Branca*/
const Scalar AMARELO = Scalar(0.0, 255.0, 255.0); /** Representação da cor Amarela*/
const Scalar VERDE = Scalar(0.0, 200.0, 0.0); /** Representação da cor Verde*/
const Scalar VERMELHO = Scalar(0.0, 0.0, 255.0); /** Representação da cor Vermelha*/

/** Inicializa os contadores de carros na parte esqueda e direita, respectivamente */
int contEsq = 0;
int contDir = 0;

/** Calcula a distância entre os pontos */
double distPontos(Point ponto1, Point ponto2) {
  int intX = abs(ponto1.x - ponto2.x);
  int intY = abs(ponto1.y - ponto2.y);

  return (sqrt(pow(intX, 2) + pow(intY, 2)));
}

/** Define os parâmetros do blob atual no vetor de blobs existentes */
void insereBlob(Blob &blobAtual, vector<Blob> &blobsExistentes, int &intIndex) {
  blobsExistentes[intIndex].contornoAtual = blobAtual.contornoAtual;
  blobsExistentes[intIndex].quadro = blobAtual.quadro;
  blobsExistentes[intIndex].posicoesCentro.push_back(blobAtual.posicoesCentro.back());
  blobsExistentes[intIndex].diagonal = blobAtual.diagonal;
  blobsExistentes[intIndex].proporcao = blobAtual.proporcao;
  blobsExistentes[intIndex].naTela = true;
  blobsExistentes[intIndex].blobAtualNovo = true;
}

/** Adiciona um novo blob no vetor de blobs existentes */
void novoBlob(Blob &blobAtual, vector<Blob> &blobsExistentes) {
  blobAtual.blobAtualNovo = true;
  blobsExistentes.push_back(blobAtual);
}

/** Verifica se o blob atual já existe no vetor de blobs */
void verificaBlob(vector<Blob> &blobsExistentes, vector<Blob> &blobsAtuais) {
  for (auto &blobExistente : blobsExistentes) {
    blobExistente.blobAtualNovo = false;
    blobExistente.ProxPos();
  }

  /** Inicializa as variáveis de distância */
  int blobMenorDist;
  double menorDist;
  double dist;

  for (auto &blobAtual : blobsAtuais) {
    blobMenorDist = 0;
    menorDist = 100000.0;

    for (unsigned int i = 0; i < blobsExistentes.size(); i++) {
      if (blobsExistentes[i].naTela == true) {
        dist = distPontos(blobAtual.posicoesCentro.back(), blobsExistentes[i].proxPos);

        if (dist < menorDist) {
          menorDist = dist;
          blobMenorDist = i;
        }
      }
    }

    if (menorDist < blobAtual.diagonal * 0.5) {
      insereBlob(blobAtual, blobsExistentes, blobMenorDist);
    }
    else {
      novoBlob(blobAtual, blobsExistentes);
    }
  }

  for (auto &blobExistente : blobsExistentes) {
    if (blobExistente.blobAtualNovo == false) {
      blobExistente.numFrames++;
    }
    if (blobExistente.numFrames >= 5) {
      blobExistente.naTela = false;
    }
  }
}

/** Desenha o contorno dos blobs */
void Contorno(Size imageSize, vector<vector<Point>> contornos, string strImageName) {
  Mat image(imageSize, CV_8UC3, PRETO);
  drawContours(image, contornos, -1, BRANCO, -1);
}

/** Desenha o contorno dos blobs */
void Contorno(Size imageSize, vector<Blob> blobsExistentes, string strImageName) {
  Mat image(imageSize, CV_8UC3, PRETO);
  vector<vector<Point>> contours;

  for (auto &blob : blobsExistentes) {
    if (blob.naTela == true) {
      contours.push_back(blob.contornoAtual);
    }
  }

  drawContours(image, contours, -1, BRANCO, -1);
}

/** Verifica se o veículo cruzou a linha da direita */
bool cruzouLinhaDir(vector<Blob> &blobsExistentes, int &linhaHor, int &contDir) {
  bool blobCruzou = false;    /** Verifica se pelo menos 1 blob cruzou a linha */
  int frameAnt;               /** Índice do frame anterior */
  int frameAtual;             /** Índice do frame atual */

  for (auto blob : blobsExistentes) {
    if (blob.naTela == true && blob.posicoesCentro.size() >= 2) {
      frameAnt = (int)blob.posicoesCentro.size() - 2;
      frameAtual = (int)blob.posicoesCentro.size() - 1;

      /** Se um blob cruzou a linha, incrementa o contador da via direita */
      if (blob.posicoesCentro[frameAnt].y > linhaHor && blob.posicoesCentro[frameAtual].y <= linhaHor && blob.posicoesCentro[frameAtual].x > 350) {
        contDir++;
        blobCruzou = true;
      }
    }
  }
  return blobCruzou;
}

/** Verifica se o veículo cruzou a linha da esquerda */
bool cruzouLinhaEsq(vector<Blob> &blobsExistentes, int &linhaHor, int &contEsq) {
  bool blobCruzou = false;    /** Verifica se pelo menos 1 blob cruzou a linha */
  int frameAnt;               /** Índice do frame anterior */
  int frameAtual;             /** Índice do frame atual */

  for (auto blob : blobsExistentes) {
    if (blob.naTela == true && blob.posicoesCentro.size() >= 2) {
      frameAnt = (int)blob.posicoesCentro.size() - 2;
      frameAtual = (int)blob.posicoesCentro.size() - 1;

      /** Se um blob cruzou a linha, incrementa o contador da via esquerda */
      if (blob.posicoesCentro[frameAnt].y <= linhaHor && blob.posicoesCentro[frameAtual].y > linhaHor && blob.posicoesCentro[frameAtual].x < 350 && blob.posicoesCentro[frameAtual].x > 0) {
        contEsq++;
        blobCruzou = true;
      }
    }
  }
  return blobCruzou;
}

/** Imprime as informações do blob na tela */
void blobInfo(vector<Blob> &blobsExistentes, Mat &cloneFrame2) {
  /** Parâmetros da fonte */
  int tipoFonte;
  int espFonte;
  double escFonte;

  for (unsigned int i = 0; i < blobsExistentes.size(); i++) {
    if (blobsExistentes[i].naTela == true) {
      rectangle(cloneFrame2, blobsExistentes[i].quadro, VERMELHO, 2);
    }
  }
}

/** Imprime o contador na imagem */
void contador(int &contDir, Mat &cloneFrame2) {
  int tipoFonte = CV_FONT_HERSHEY_SIMPLEX;
  double escFonte = (cloneFrame2.rows * cloneFrame2.cols) / 650000.0;
  int espFonte = (int)round(escFonte * 2.5);

  /** Contador da via direita */
  Size tamTextoD = getTextSize(to_string(contDir), tipoFonte, escFonte, espFonte, 0);
  putText(cloneFrame2, "Contador direita:" + to_string(contDir), Point(800, 50), tipoFonte, escFonte, VERMELHO, espFonte);

  /** Contador da via esquerda */
  Size tamTextoE = getTextSize(to_string(contEsq), tipoFonte, escFonte, espFonte, 0);
  putText(cloneFrame2, "Contador esquerda:" + to_string(contEsq), Point(10, 50), tipoFonte, escFonte, AMARELO, espFonte);
}

int main(void) {
  VideoCapture capVideo;
  Mat frame1, frame2;
  vector<Blob> blobsExistentes;

  /** Inicializa as linhas de referência da via direita e da esquerda, respectivamente */
  Point linhaDir[2];
  Point linhaEsq[2];

  capVideo.open("video_transito.mp4");

  /** Caso não seja possível abrir o vídeo - Erro */
  if (!capVideo.isOpened()) {
    cout << "Erro: Não foi possível abrir o vídeo!" << endl << endl;
    return 0;
  }

  /** Caso o vídeo tenha apenas 1 frame (imagem) - Erro */
  if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
    cout << "Erro: O vídeo deve ter 2 frames ou mais!";
    return 0;
  }

  /** Captura 2 frames do vídeo */
  capVideo.read(frame1);
  capVideo.read(frame2);

  /** Controla a linha da direita */
  int linhaHorDir = (int)round((double)frame1.rows * 0.3);

  linhaDir[0].x = 350;
  linhaDir[0].y = linhaHorDir;

  linhaDir[1].x = 1000;
  linhaDir[1].y = linhaHorDir;

  /** Controla a linha da esquerda */
  int linhaHorEsq = (int)round((double)frame1.rows * 0.15);
  linhaEsq[0].x = 10;
  linhaEsq[0].y = linhaHorEsq;

  linhaEsq[1].x = 360;
  linhaEsq[1].y = linhaHorEsq;

  /** Inicializa os parâmetros para comparação dos frames */
  char chCheckForEscKey = 0;
  bool primeiroFrame = true, blobCruzou;
  int contFrame = 2;
  float normalizado[256];
  vector<Blob> blobsAtuais;
  Mat cloneFrame1, cloneFrame2, subtracao, limiar, limiar1, limiar2, cloneLimiar, hist;
  vector<vector<Point>> contours;

  while (capVideo.isOpened() && chCheckForEscKey != 27) {
    cloneFrame1 = frame1.clone();
    cloneFrame2 = frame2.clone();
    hist = frame1.clone();

    /** Converte os frames para a escala de cinza */
    cvtColor(cloneFrame1, cloneFrame1, CV_BGR2GRAY);
    cvtColor(cloneFrame2, cloneFrame2, CV_BGR2GRAY);

    /** Filtro de suavização gaussiano */
    GaussianBlur(cloneFrame1, cloneFrame1, Size(5, 5), 0);
    GaussianBlur(cloneFrame2, cloneFrame2, Size(5, 5), 0);

    /** Subtração da imagem */
    absdiff(cloneFrame1, cloneFrame2, limiar);

    /** Limiarização */
    threshold(limiar, limiar, 30, 255.0, CV_THRESH_BINARY);

    /** Elemento estruturante + Filtros morfológicos x2 (dilatação + erosão) */
    Mat elemEstruturante = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(limiar, limiar, elemEstruturante, Point(-1, -1), 2);
    erode(limiar, limiar, elemEstruturante, Point(-1, -1), 1);
    dilate(limiar, limiar, elemEstruturante, Point(-1, -1), 2);
    erode(limiar, limiar, elemEstruturante, Point(-1, -1), 1);

    /** Define os contornos dos blobs */
    cloneLimiar = limiar.clone();
    findContours(cloneLimiar, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Contorno(limiar.size(), contours, "Contorno");

    /** Define o envoltório convexo dos blobs */
    vector<vector<Point>> convexHulls(contours.size());

    for (unsigned int i = 0; i < contours.size(); i++) {
      convexHull(contours[i], convexHulls[i]);
    }

    Contorno(limiar.size(), convexHulls, "Envoltório");

    for (auto &convexHull : convexHulls) {
      Blob possibleBlob(convexHull);

      if (possibleBlob.quadro.area() > 400 && possibleBlob.proporcao > 0.2 && possibleBlob.proporcao < 4.0 && possibleBlob.quadro.width > 30 && possibleBlob.quadro.height > 30 && possibleBlob.diagonal > 60.0 && (contourArea(possibleBlob.contornoAtual) / (double)possibleBlob.quadro.area()) > 0.50) {
        blobsAtuais.push_back(possibleBlob);
      }
    }

    Contorno(limiar.size(), blobsAtuais, "imgCurrentFrameBlobs");

    if (primeiroFrame == true) {
      for (auto &blobAtual : blobsAtuais) {
        blobsExistentes.push_back(blobAtual);
      }
    } else {
      verificaBlob(blobsExistentes, blobsAtuais);
    }

    Contorno(limiar.size(), blobsExistentes, "imgBlobs");

    cloneFrame2 = frame2.clone();

    blobInfo(blobsExistentes, cloneFrame2);

    /** Verifica a linha da direita */
    blobCruzou = cruzouLinhaDir(blobsExistentes, linhaHorDir, contDir);

    /** Verifica a linha da esquerda */
    blobCruzou = cruzouLinhaEsq(blobsExistentes, linhaHorEsq, contEsq);

    /** Via direita */
    if (blobCruzou == true) {
      line(cloneFrame2, linhaDir[0], linhaDir[1], VERDE, 2);
    }
    else if (blobCruzou == false) {
      line(cloneFrame2, linhaDir[0], linhaDir[1], VERMELHO, 2);
    }

    /** Via esquerda */
    if (blobCruzou == true) {
      line(cloneFrame2, linhaEsq[0], linhaEsq[1], VERDE, 2);
    }
    else if (blobCruzou == false) {
      line(cloneFrame2, linhaEsq[0], linhaEsq[1], VERMELHO, 2);
    }

    contador(contDir, cloneFrame2);

    imshow("Tela principal", cloneFrame2);

    blobsAtuais.clear();

    /** Passa o frame 2 para o frame 1 */
    frame1 = frame2.clone();

    /** Verifica se o vídeo chegou ao fim */
    if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
      capVideo.read(frame2);
    }
    else {
      cout << "Fim do vídeo\n";
      break;
    }

    primeiroFrame = false;
    contFrame++;
    chCheckForEscKey = waitKey(1);
  }

  if (chCheckForEscKey != 27) {
    waitKey(0);
  }

  return 0;
}