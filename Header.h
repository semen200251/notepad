#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "Menu.h"
typedef struct{
    int x;//��������� �� �
    int y;//��������� �� y
}Point;

typedef struct{
    LPSTR FileName;//��� �����
    unsigned int FileSize;//������ ����� � ������
    int CountStr;//���������� ����� � �����
    char* Buf;//����� � ������� �������� ���������� �����
    int prevCountStr;//���������� ����� �� ���������� ������
    int* EndOfStr;//��������� \n ��� ������
    int* EndOfStrWithCoding;//��������� \n � �������
    int MaxSizeStr;//������������ ����� ������
}Input;

typedef struct{
    HFONT hfont1;
    bool MoodWork;//����� ������
    LOGFONT lf;
    Point SizeWindow;//������� ���� ����������
    Point ScrollInfo;//���������� � ������� ��������� �������
}Output;

void MyReadFile(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//������� ������ ����, ���������� ��� � ����� � ���������� ��������� \n �� ������

void SetConst(Output* OutPut, Input* InPut, LOGFONT lf, RECT rect, LPARAM lParam);//������� ���������� ��������� �������� ��� �������(�����, ������� ����, ��������� ��������� �������, ������ ������)

void SettingScrollRangeWithoutCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//������� ������� ���������� ��������� ������� ��� ������

void SettingScrollRangeWithCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam);//������� ������� ���������� ��������� ������� � �������

void PaintWithoutLCoding(Output OutPut, Input InPut, HDC hdc);//������� ���������� ��������� ��� �������

void PaintWithCoding(Output OutPut, Input InPut, HDC hdc);//������� ��������� ��������� � �������

void clearEverything(Output* OutPut, Input* InPut);//������� ������� ��� ����������� ���������� ������ ����� ��������� ������ ����� ��� ��������� ����������

void recountStr(int left, int right, Input* InPut, Output OutPut, int* i);//������� ��������� ���� ������, �� ���� ����������� ����� �������� \n
