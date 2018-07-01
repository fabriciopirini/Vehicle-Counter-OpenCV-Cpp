#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#ifndef MY_BLOB
#define MY_BLOB

class Blob {
	public:
		Rect quadro;					/** Contorno retangular do blob */
		Point proxPos;					/** Próxima posição do centro do blob */
		vector<Point> posicoesCentro;	/** Coordenadas do centro do blob */
		vector<Point> contornoAtual;	/** Contorno do blob */
		double diagonal;				/** Diagonal do quadro do blob */
		double proporcao;				/** Proporção do quadro do blob */
		bool blobAtualNovo;				/** Verifica se o blob é atual/novo ou antigo */
		bool naTela;					/** Verifica se o blob ainda está na tela */
		int numFrames;					/** Número consecutivo de frames sem parecer com o blob */

		Blob(vector<Point> contorno);
		void ProxPos(void);
};

#endif    /** MY_BLOB */