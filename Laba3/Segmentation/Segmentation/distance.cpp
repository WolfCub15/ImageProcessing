#include "header.h"


void DistanceTransform(InputArray _src,
    OutputArray _dst,
    int distType,
    int maskSize) {

    Mat src = _src.getMat();
    _dst.create(src.size(), CV_32F);
    Mat dst = _dst.getMat();

    const int a = 1.0f * 100000;
    const int b = 1.4f * 100000;
    const int c = 2.1969f * 100000;

    Size src_size = src.size();
    //увеличиваем на 2 с каждой стороны
    Mat temp(src_size.height + 2 * 2, src_size.width + 2 * 2, CV_32SC1);
    int* ttemp = temp.ptr<int>();
    int temp_step = static_cast<int>(temp.step / sizeof(ttemp[0]));

    for (int i = 0; i < 2; ++i) {
        int* rowi = temp.ptr<int>(i);//указатель на i строку матрицы
        for (int j = 0; j < temp.size().width; ++j) {
            rowi[j] = 0;
        }
    }
    for (int i = 0; i < 2; ++i) {
        int* rowi = temp.ptr<int>(temp.size().height - i - 1);//указатель на i строку c конца матрицы
        for (int j = 0; j < temp.size().width; ++j) {
            rowi[j] = 0;
        }
    }

    uchar* ssrc = src.ptr(); //указатель на 0 строку матрицы
    int src_step = static_cast<int>(src.step / sizeof(ssrc[0]));

    for (int i = 0; i < _src.size().height; i++) {
        const uchar* curr = ssrc + i * src_step;
        int* tmp = static_cast<int*>(ttemp + (i + 2) * temp_step) + 2;
        for (int j = 0; j < _src.size().width; j++) {
            if (curr[j] == 0) tmp[j] = 0;
            else {
                tmp[j] = min({ tmp[j - temp_step * 2 - 1] + c ,
                    tmp[j - temp_step * 2 + 1] + c ,
                    tmp[j - temp_step - 2] + c ,
                    tmp[j - temp_step - 1] + b ,
                    tmp[j - temp_step] + a ,
                    tmp[j - temp_step + 1] + b ,
                    tmp[j - temp_step + 2] + c ,
                    tmp[j - 1] + a });
            }
        }
    }

    float* ddist = dst.ptr<float>();
    int dst_step = static_cast<int>(dst.step / sizeof(ddist[0]));

    for (int i = _src.size().height - 1; i >= 0; --i) {
        float* distance = static_cast<float*>(ddist + i * dst_step);
        int* tmp = static_cast<int*>(ttemp + (i + 2) * temp_step) + 2;
        for (int j = _src.size().width - 1; j >= 0; --j) {
            int t0 = tmp[j];
            if (t0 > a) {
                t0 = min({ tmp[j], tmp[j + temp_step * 2 + 1] + c,
                        tmp[j + temp_step * 2 - 1] + c,
                        tmp[j + temp_step - 2] + c,
                        tmp[j + temp_step + 2] + c,
                        tmp[j + temp_step + 1] + b,
                        tmp[j + temp_step] + a,
                        tmp[j + temp_step - 1] + b,
                        tmp[j + 1] + a });
                tmp[j] = t0;
            }
            distance[j] = static_cast<float>(t0 * 1.f / (100000));
        }
    }
}

