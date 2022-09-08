#include "Header.h"


void SetConst(Output* OutPut, Input* InPut, LOGFONT lf, RECT rect, LPARAM lParam)
{
    InPut->FileName=(LPSTR)(((CREATESTRUCT*)lParam)->lpCreateParams);
    lf.lfHeight += 5;
    OutPut->hfont1=CreateFont(lf.lfHeight, lf.lfWidth,
                              lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
                              lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
                              lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision,
                              lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
    OutPut->lf = lf;
    OutPut->SizeWindow.x=(int)rect.right;
    OutPut->SizeWindow.y=(int)rect.bottom;
    OutPut->MoodWork=TRUE;
    OutPut->ScrollInfo.x=0;
    OutPut->ScrollInfo.y=0;
}

void recountStr(int left, int right, Input* InPut, Output OutPut, int* i)
{
    if(right-left>OutPut.SizeWindow.x/OutPut.lf.lfWidth)
    {
        InPut->EndOfStrWithCoding[(*i)]=left+OutPut.SizeWindow.x/OutPut.lf.lfWidth;
        InPut->CountStr++;
        (*i)++;
        InPut->EndOfStrWithCoding=(int*)realloc(InPut->EndOfStrWithCoding, sizeof(int)*((*i)+1));
        recountStr(left+OutPut.SizeWindow.x/OutPut.lf.lfWidth+1, right, InPut, OutPut, i);
    }
    else
    {
        InPut->EndOfStrWithCoding[(*i)]=right;
        (*i)++;
        InPut->EndOfStrWithCoding=(int*)realloc(InPut->EndOfStrWithCoding, sizeof(int)*((*i)+1));
    }
    return;
}

void MyReadFile(Output* OutPut, Input* InPut, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    FILE* fp=fopen(InPut->FileName,"r");
    if(!fp)
        return;
    rewind(fp);
    fseek(fp, 0L, SEEK_END);
    InPut->FileSize = ftell(fp);
    rewind(fp);
    InPut->Buf=(char*)malloc(sizeof(char)*InPut->FileSize);
    InPut->FileSize = fread(InPut->Buf,sizeof(char), InPut->FileSize, fp);
    unsigned int j=1;
    InPut->EndOfStr=(int*)malloc(sizeof(int)*2);
    InPut->EndOfStr[0]=0;
    InPut->MaxSizeStr=0;

    int SizeStr=0;
    for(unsigned int i=0; i<InPut->FileSize; i++)
    {
        SizeStr++;
        if(InPut->Buf[i]=='\n')
        {
            if(SizeStr>InPut->MaxSizeStr)
            {
                InPut->MaxSizeStr=SizeStr;
            }
            SizeStr=0;
            int* pointer=InPut->EndOfStr;
            InPut->EndOfStr[j]=i+1;
            j++;
            InPut->EndOfStr=(int*)realloc(InPut->EndOfStr, sizeof(int)*(j+1));
            if(InPut->EndOfStr==NULL)
            {
                InPut->EndOfStr=pointer;
                break;
            }
        }
    }
    if(SizeStr>InPut->MaxSizeStr)
    {
        InPut->MaxSizeStr=SizeStr;
    }
    InPut->CountStr=j;
    InPut->prevCountStr=InPut->CountStr;
    InPut->EndOfStr[j]=InPut->FileSize+1;
    fclose(fp);
}

void SettingScrollRangeWithoutCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    int myScrollSize=1000;
    OutPut->SizeWindow.y=HIWORD(lParam);
    OutPut->SizeWindow.x=LOWORD(lParam);
    InPut->CountStr=InPut->prevCountStr;
    OutPut->ScrollInfo.x=0;
    OutPut->ScrollInfo.y=0;
    SetScrollPos(hwnd,SB_VERT, OutPut->ScrollInfo.y, TRUE);
    SetScrollPos(hwnd,SB_HORZ, OutPut->ScrollInfo.x, TRUE);
    if(InPut->CountStr<myScrollSize)
    {
        if((OutPut->SizeWindow.y-(InPut->CountStr*OutPut->lf.lfHeight)-1)>0)
        {
            SetScrollRange(hwnd,SB_VERT,0,0,FALSE);
        }
        else
        {
            SetScrollRange(hwnd,SB_VERT,0,InPut->CountStr-OutPut->SizeWindow.y/OutPut->lf.lfHeight,FALSE);

        }
        if(OutPut->SizeWindow.x-(InPut->MaxSizeStr*OutPut->lf.lfWidth)<0)
        {
            SetScrollRange(hwnd, SB_HORZ, 0, InPut->MaxSizeStr-OutPut->SizeWindow.x/OutPut->lf.lfWidth, FALSE);

        }
        else
        {
            SetScrollRange(hwnd,SB_HORZ,0,0,FALSE);
        }
    }
    else
    {
        SetScrollRange(hwnd,SB_VERT,0,myScrollSize,FALSE);

        if(OutPut->SizeWindow.x-(InPut->MaxSizeStr*OutPut->lf.lfWidth)<0)
        {
            SetScrollRange(hwnd, SB_HORZ, 0, InPut->MaxSizeStr-OutPut->SizeWindow.x/OutPut->lf.lfWidth, FALSE);

        }
        else
        {
            SetScrollRange(hwnd,SB_HORZ,0,0,FALSE);
        }
    }
    InvalidateRect(hwnd,NULL,TRUE);
}


void SettingScrollRangeWithCoding(Output* OutPut, Input* InPut,HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    int myScrollSize=1000;
    RECT rect;
    GetClientRect(hwnd,&rect);
    OutPut->SizeWindow.y=rect.bottom-rect.top;
    OutPut->SizeWindow.x=rect.right-rect.left;
    OutPut->ScrollInfo.x=0;
    OutPut->ScrollInfo.y=0;
    SetScrollPos(hwnd,SB_VERT, OutPut->ScrollInfo.y, TRUE);
    SetScrollPos(hwnd,SB_HORZ, OutPut->ScrollInfo.x, TRUE);
    if(InPut->EndOfStrWithCoding!=NULL)
    {
        free(InPut->EndOfStrWithCoding);
    }
    InPut->EndOfStrWithCoding=(int*)malloc(sizeof(int)*2);
    int i=0;
    InPut->EndOfStrWithCoding[i]=0;
    i++;
    InPut->CountStr=InPut->prevCountStr;
    for(int j=0; j<InPut->prevCountStr; j++)
    {
        recountStr(InPut->EndOfStr[j],InPut->EndOfStr[j+1], InPut, *OutPut, &i);
    }
    InPut->EndOfStrWithCoding[i-1]=InPut->FileSize;
    SetScrollRange(hwnd,SB_HORZ,0,0,FALSE);
    if(InPut->CountStr<myScrollSize)
    {
        if((OutPut->SizeWindow.y-(InPut->CountStr*OutPut->lf.lfHeight)-1)>0)
        {
            SetScrollRange(hwnd,SB_VERT,0,0,FALSE);
        }
        else
        {
            SetScrollRange(hwnd,SB_VERT,0,InPut->CountStr-OutPut->SizeWindow.y/OutPut->lf.lfHeight,FALSE);
        }
    }
    else
    {
        SetScrollRange(hwnd,SB_VERT,0,myScrollSize,FALSE);
    }
}

void PaintWithoutLCoding(Output OutPut, Input InPut, HDC hdc)
{
    for(unsigned int i=OutPut.ScrollInfo.y; i<OutPut.ScrollInfo.y+OutPut.SizeWindow.y/OutPut.lf.lfHeight; i++)
    {
        if(i==InPut.CountStr)
            break;
        TextOut(hdc, 0, 0+OutPut.lf.lfHeight*(i-OutPut.ScrollInfo.y), InPut.Buf+InPut.EndOfStr[i]+(int)OutPut.ScrollInfo.x, InPut.EndOfStr[i+1]-InPut.EndOfStr[i]-OutPut.ScrollInfo.x);
    }
}

void PaintWithCoding(Output OutPut, Input InPut, HDC hdc)
{
    for(unsigned int i=OutPut.ScrollInfo.y; i<OutPut.ScrollInfo.y+OutPut.SizeWindow.y/OutPut.lf.lfHeight; i++)
    {
        if(i>=InPut.CountStr)
        {
            i=InPut.CountStr-1;
            TextOut(hdc, 0, 0+OutPut.lf.lfHeight*(i-OutPut.ScrollInfo.y), InPut.Buf+InPut.EndOfStrWithCoding[i]+(int)OutPut.ScrollInfo.x, InPut.EndOfStrWithCoding[i+1]-InPut.EndOfStrWithCoding[i]-OutPut.ScrollInfo.x);
            break;
        }
        TextOut(hdc, 0, 0+OutPut.lf.lfHeight*(i-OutPut.ScrollInfo.y), InPut.Buf+InPut.EndOfStrWithCoding[i]+(int)OutPut.ScrollInfo.x, InPut.EndOfStrWithCoding[i+1]-InPut.EndOfStrWithCoding[i]-OutPut.ScrollInfo.x);
    }
}


void clearEverything(Output* OutPut, Input* InPut){
    OutPut->ScrollInfo.x=0;
    OutPut->ScrollInfo.y=0;
    free(InPut->Buf);
    InPut->Buf=NULL;
    InPut->CountStr=0;
    free(InPut->EndOfStr);
    InPut->EndOfStr=NULL;
    free(InPut->EndOfStrWithCoding);
    InPut->EndOfStrWithCoding=NULL;
    InPut->FileName=NULL;
    InPut->FileSize=0;
    InPut->MaxSizeStr=0;
    InPut->prevCountStr=0;
}
