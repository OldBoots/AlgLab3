#include <QCoreApplication>
#include <iostream>
#include <QFile>
#include <QVector>
#include "fraction.h"

using namespace std;

void read_matrix(QVector<QVector<fract>>& matrix){
    QFile file(":/matrix");
    if(!file.size()){
        qDebug() << "File is empty!";
        exit(0);
    }
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QStringList elMat;
        QStringList elMatI;
        QString lineStr;
        QVector<fract> lineEl;
        while(!file.atEnd()){
            lineStr = file.readLine();
            elMat = lineStr.split("__");
            lineEl.clear();
            for(int i=0; i<elMat.size(); i++){
                fract oneEl;
                if(elMat[i].contains('/')){
                    elMatI = elMat[i].split("/");
                    if(elMatI[1] == "0"){
                        qDebug("Division on null!");
                        exit(0);
                    }
                    oneEl.u_num = elMatI[0].toInt();
                    oneEl.d_num = elMatI[1].toInt();
                    if(oneEl.d_num < 0){
                        oneEl.d_num *= -1;
                        oneEl.u_num *= -1;
                    }
                } else {
                    oneEl.u_num = elMat[i].toInt();
                    oneEl.d_num = 1;
                }

                lineEl.push_back(oneEl);
            }
            matrix.push_back(lineEl);
        }
    }
}

void cout_matrix(QVector<QVector<fract>> matrix){
    for(int i = 0; i < matrix.size(); i++){
        for(int j = 0; j <matrix[0].size(); j++){
            if(matrix[i][j].u_num > -10000){
                if(matrix[i][j].d_num == 1){
                    cout<<matrix[i][j].u_num<<"\t";
                }else{
                    cout<<matrix[i][j].u_num<<"/"<<matrix[i][j].d_num<<"\t";
                }
            } else {
                cout << "--\t";
            }

        }
        cout << endl << endl;
    }
    cout << endl;
}

int comparison(fract frst, fract scnd){
    frst.u_num = scnd.d_num * frst.u_num;
    scnd.u_num = frst.d_num * scnd.u_num;
    if(abs(frst.u_num) < abs(scnd.u_num)){ //?
        return -1;
    }
    if(abs(frst.u_num) > abs(scnd.u_num)){
        return 1;
    }
    return 0;
}

void balancing_matrix(int flag, QVector<QVector<fract>> matrix, QVector<QVector<fract>> &matrix_balanced, fract diff){
    fract f;
    f.u_num = 0;
    f.d_num = 1;
    QVector<fract> dima;
    if(flag < 0){
        for(int i = 0; i < matrix.size(); i++){
            if(i == matrix.size() - 1){
                for(int j = 0; j < matrix[i].size() - 1; j++){
                    dima << f;
                }
                dima << diff;
                matrix_balanced << dima;
            }
            matrix_balanced << matrix[i];
        }
    }
    if(flag > 0){
        for(int i = 0; i < matrix.size(); i++){
            dima.clear();
            for(int j = 0; j < matrix[i].size(); j++){
                if(j == matrix[i].size() - 1){
                    if(i == matrix.size() - 1){
                        dima << diff;
                    } else {
                        dima << f;
                    }
                }
                dima << matrix[i][j];

            }
            matrix_balanced << dima;
        }
    }
    if(flag == 0){
        matrix_balanced = matrix;
    }
}

void cout_fract(fract f){
    cout << f.u_num;
    if(f.d_num != 1){
        cout << "/" << f.d_num;
    }
}

void calculate(QVector<QVector<fract>> &matrix, QVector<QVector<fract>> &matrix_balanced, Fraction frct){
    QVector<fract> dima;
    fract f_last_col, f_last_row, f;
    f.u_num = -INFINITY;
    f.d_num = 1;
    f_last_col.u_num = 0;
    f_last_col.d_num = 1;
    f_last_row.u_num = 0;
    f_last_row.d_num = 1;
    for(int i = 0; i < matrix.size(); i++){
        f_last_col = frct.sum(f_last_col, matrix[i][matrix[i].size() - 1]);
    }
    for(int i = 0; i < matrix[matrix.size() - 1].size(); i++){
        f_last_row = frct.sum(f_last_row, matrix[matrix.size() - 1][i]);
    }
    balancing_matrix(comparison(f_last_col, f_last_row), matrix, matrix_balanced, frct.abs(frct.sum(f_last_row, f_last_col, 1)));
    qDebug("Balanced matrix:");
    cout_matrix(matrix_balanced);
    matrix.clear();
    for(int i = 0; i < matrix_balanced.size() - 1; i++){
        dima.clear();
        for(int j = 0; j < matrix_balanced[i].size() - 1; j++){
            dima << f;
        }
        matrix << dima;
    }
    f.u_num = 0;
    f.d_num = 1;
    for(int i = 0, row = 0, col = 0; col < matrix[row].size() - 1; i++){
        // [matrix_balanced.size() - 1][col] - потребности
        // [row][matrix_balanced[row].size() - 1] - запас
        cout << i << ") row = " << row << " col = " << col << " DDDD\n";
        f = frct.sum(matrix_balanced[row][matrix_balanced[row].size() - 1], matrix_balanced[matrix_balanced.size() - 1][col], 1);
        if(f.u_num < 0){
            matrix[row][col] = matrix_balanced[row][matrix_balanced[row].size() - 1];
            matrix_balanced[row][matrix_balanced[row].size() - 1].u_num = 0;
            matrix_balanced[matrix_balanced.size() - 1][col] = frct.abs(f);
            if(row < matrix.size() - 1){
                row++;
            }
        }
        qDebug("if1");
        if(f.u_num > 0){
            matrix[row][col] = matrix_balanced[matrix_balanced.size() - 1][col];
            matrix_balanced[matrix_balanced.size() - 1][col].u_num = 0;
            matrix_balanced[row][matrix_balanced[row].size() - 1] = f;
            col++;
        }
        qDebug("if2");
        if(f.u_num == 0){
            matrix[row][col] = matrix_balanced[matrix_balanced.size() - 1][col];
            matrix_balanced[matrix_balanced.size() - 1][col].u_num = 0;
            matrix_balanced[row][matrix_balanced[row].size() - 1].u_num = 0;
            col++;
            matrix[row][col].u_num = 0;
            if(row < matrix.size() - 1){
                row++;
            }
        }
        qDebug("if3");
        cout << i << ") row = " << row << " ";
        cout_fract(f);
        cout << endl;
        cout_matrix(matrix);
    }
}

void show_answer(QVector<QVector<fract>> matrix, QVector<QVector<fract>> matrix_balanced, Fraction frct){
    fract f, ff;
    f.u_num = 0;
    f.d_num = 1;
    cout << "F = ";
    for(int i = 0; i < matrix.size(); i++){
        for(int j = 0; j < matrix[i].size(); j++){
            if(matrix[i][j].u_num > -10000){
                ff = frct.mult(matrix[i][j], matrix_balanced[i][j]);
                f = frct.sum(f, ff);
                cout_fract(ff);
                if(j < matrix[i].size() - 1){
                    cout << " + ";
                }
            }
        }
    }
    cout << " = ";
    cout_fract(f);
    cout << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QVector<QVector<fract>> matrix;
    QVector<QVector<fract>> matrix_balanced;
    Fraction frct;
    read_matrix(matrix);
    qDebug("Origin matrix:");
    cout_matrix(matrix);
    calculate(matrix, matrix_balanced, frct);
    qDebug("Reference matrix:");
    cout_matrix(matrix);
    qDebug("Price transit:");
    show_answer(matrix, matrix_balanced, frct);
    return a.exec();
}


