#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "Menu.h"
typedef struct{
    int x;//Положение по х
    int y;//Положение по y
}Point;

typedef struct{
    LPSTR FileName;//Имя файла
    unsigned int FileSize;//Размер файла в байтах
    int CountStr;//Количество строк в файле
    char* Buf;//Буфер в котором хранится содержимое файла
    int prevCountStr;//Количество строк до выполнения вёрстки
    int* EndOfStr;//Положение \n без вёрстки
    int* EndOfStrWithCoding;//Положение \n с вёрсткой
    int MaxSizeStr;//Максимальная длина строки
}Input;

typedef struct{
    HFONT hfont1;
    bool MoodWork;//Режим работы
    LOGFONT lf;
    Point SizeWindow;//Размеры окна приложения
    Point ScrollInfo;//Информация о текущем положении скролла
}Output;

void MyReadFile(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//Функция читает файл, записывает его в буфер и ваставляет положение \n до вёрстви

void SetConst(Output* OutPut, Input* InPut, LOGFONT lf, RECT rect, LPARAM lParam);//Функция выставляет начальные значения для моделей(шрифт, размеры окна, начальное положение скролла, режимм работы)

void SettingScrollRangeWithoutCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//Функция считает количество положений скролла без вёрстки

void SettingScrollRangeWithCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//Функция считает количество положений скролла с вёрсткой

void PaintWithoutLCoding(Output OutPut, Input InPut, HDC hdc);//Функция выпполняет отрисовку без вёрсткой

void PaintWithCoding(Output OutPut, Input InPut, HDC hdc);//функция выполняет отрисовку с вёрсткой

void clearEverything(Output* OutPut, Input* InPut);//функция очищает всю динамически выделенную память перед открытием нового файла или закрытием приложения

void recountStr(int left, int right, Input* InPut, Output OutPut, int* i);//Функция выполняет саму вёрстку, то есть расставляет новые значения \n
