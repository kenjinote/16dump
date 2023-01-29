/*!
 *  @file      dump.c
 *  @breif     ファイルの１６進ダンプを表示します
 *  @author    S.Kajikawa
 *  @date      2009/02/28
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <math.h>
#include <string.h>
#include <memory.h>

/* macro definitions  */

/* global stuff */
HINSTANCE  hInst;    /*!< instance handle  */
HWND       hWndMain;  /*!< main window handle  */

/*! window class name  */
char    szAppName[]  = "WINDUMP";
/*! Application Title  */
char    szAppTitle[] = "ファイルの１６進ダンプを表示します";

/* global stuff */
PSTR    szCommandLine = NULL;

/* function prototype declarations */
int         InitApplication(HINSTANCE, char *);
HWND        InitInstance(HINSTANCE, int);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

/*!
 * @fn     WinMain
 * @brief  Windowプログラムのエントリ・ポイント
 * @param  hInstance    Instanceハンドル
 * @param  hPrevInstance  現在は使われていない
 * @param  szCmdLine    コマンド行の引数
 * @param  iCmdShow    初期表示状態指示の値を格納
 * @return returns the value from PostQauitMessage
 */
int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR      szCmdLine, int       iCmdShow)
{
  MSG        msg;      /* メッセージ構造体 */

  if (!hPrevInstance) {
    szCommandLine = szCmdLine;

    if (!InitApplication(hInstance, szAppName)) {  /* Windowクラスの登録  */
      MessageBox(HWND_DESKTOP, "Windowクラスの登録に失敗しました",
                 "エラー", MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
    }
  }

  /* Windowの作成  */
  if ((hWndMain = InitInstance(hInstance, iCmdShow)) == NULL) {
    MessageBox(HWND_DESKTOP, "Windowの作成に失敗しました",
               "エラー", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  /* start message loop  */
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam; /* PostQuitMessageの戻り値を返す */
}

/*!
 * @fn     InitApplication
 * @brief  Windowクラスの登録
 * @param  hInstance  Instanceハンドル
 * @param  class_name  Windowクラスの名前へのポインタ
 * @return RegisterClass関数の戻り値を返す
 */
int InitApplication(HINSTANCE hInstance, char *class_name)
{
  WNDCLASS  wndclass;

  wndclass.style          = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc    = WndProc;
  wndclass.cbClsExtra     = 0;
  wndclass.cbWndExtra     = 0;
  wndclass.hInstance      = hInstance;
  wndclass.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
  wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName   = NULL;
  wndclass.lpszClassName  = class_name;

  return (int)RegisterClass(&wndclass);
}

/*!
 * @fn     InitInstance
 * @brief  Windowの作成
 * @param  hInstance  Instanceハンドル
 * @param  iCmdShow  Windowの初期表示状態を指示する
 * @return CreateWindow関数の戻り値を返す
 */
HWND InitInstance(HINSTANCE hInstance, int iCmdShow)
{
  HWND    hWnd;

  /* save the instance-handle */
  hInst = hInstance;

  /* Create the Window */
  hWnd = CreateWindow(szAppName,  szAppTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,        NULL,
            hInstance,   NULL);

  ShowWindow(hWnd, iCmdShow);
  UpdateWindow(hWnd);

  return hWnd;
}

/*!
 * @fn     WndProc
 * @brief  Main Window Procedure(ファイルの先頭256バイトをダンプする版)
 * @param  hWnd      Windowハンドル
 * @param  msg        メッセージID
 * @param  wParam      第１引数
 * @param  lParam      第２引数
 * @return 0または0以外を返す
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int     count;
  static int     file_flag; /* file名が不正の時、１にする */
  static int     seek_pos;  /* fileのseek位置を表す。 */
  static char    in_buffer[256];
  static HFONT   hFont;
  static FILE    *fp;
  static struct  _finddata_t f_info;
  HDC            hdc;      /* デバイス・コンテキスト・ハンドル */
  PAINTSTRUCT    paint;
  HFONT          hPreFont;
  long           hFile;
  int            i, j;
  char           out_buffer[256];
  char           tmp_buffer[16];

  switch (msg) {
    case WM_CHAR   :    /* キーボードが押された */
      switch ((TCHAR)wParam) {
        case 'n':  /* 次の256バイトを表示 */
          if (f_info.size > (size_t)(seek_pos * 256)) {
            seek_pos++;
          } else {
            break;
          }
          fseek(fp, seek_pos * 256, SEEK_SET); /* 移動 */
          /* ファイル内容を変数buferに読込む */
          count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
          /* 画面更新 */
          InvalidateRect(hWnd, NULL, FALSE);
          break;
        case 'p':  /* 次の256バイトを表示 */
          if (seek_pos > 0) {
            seek_pos--;
          } else {
            break;
          }
          fseek(fp, seek_pos * 256, SEEK_SET); /* 移動 */
          /* ファイル内容を変数buferに読込む */
          count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
          /* 画面更新 */
          InvalidateRect(hWnd, NULL, FALSE);
          break;
        case 'q':  /* プログラムを終了する */
          PostMessage(hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
          break;
        default :  /* 何もしない */
          break;
      }
      break;
    case WM_PAINT  :    /* 画面の描画更新要求が来た */
      hdc = BeginPaint(hWnd, &paint); /* 描画の開始 */
      /* フォントを固定ピッチの"ＭＳ ゴシック"フォントに変更する */
      hPreFont = (HFONT)SelectObject(hdc, (HFONT)hFont);
      /* もし、countが0以外であれば、
       * dumpイメージを作成してテキストとして出力する
       */
      strcpy(out_buffer,
             "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
      strcat(out_buffer, "  0123456789ABCDEF");
      TextOut(hdc, 10, 10, out_buffer, strlen(out_buffer));
      if (count > 0) {
        for (i = 0; i < 16; i++) {
          /* out_bufferを0x00でクリアする */
          memset(out_buffer, 0, sizeof(out_buffer));
          sprintf(tmp_buffer, "%02x: ", (i * 16 + seek_pos * 256));
          strcpy(out_buffer, tmp_buffer);
          for (j = 0; j < 16; j++) {
            sprintf(tmp_buffer, "%02x ", (in_buffer[i * 16 + j] & 0xff));
            strcat(out_buffer, tmp_buffer);
          }
          strcat(out_buffer, " ");
          for (j = 0; j < 16; j++) {
            if (isprint(in_buffer[i * 16 + j] & 0xff) != 0) {
              sprintf(tmp_buffer, "%c", (in_buffer[i * 16 + j] & 0xff));
            } else {
              sprintf(tmp_buffer, ".");
            }
            strcat(out_buffer, tmp_buffer);
          }
          TextOut(hdc, 10, i * 20 + 30, out_buffer, strlen(out_buffer));
        }
      }
      /* フォントを元に戻す */
      SelectObject(hdc, (HFONT)hPreFont);
      /* BeginPaint関数によって開始した描画を終了する */
      EndPaint(hWnd, &paint);
      break;
    case WM_CREATE :    /* Windowが表示される前に来るメッセージ */
      /* デバイスコンテキストハンドルを取得する */
      hdc = GetDC(hWnd);
      /* 論理Fontの作成 */
      hFont = CreateFont(
                 MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                 0, 0, 0, FW_BOLD, 0, 0, 0, SHIFTJIS_CHARSET,
                 OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
                 FIXED_PITCH | FF_SWISS, "ＭＳ ゴシック");
      /* hdcの開放 */
      ReleaseDC(hWnd, hdc);
      /* ファイルからデータを読込む */
      file_flag = 0;
      count = 0; /* ファイルから読込んだ文字数を表す変数を初期化する */
      memset(in_buffer, 0, sizeof(in_buffer)); /* in_bufferをクリアする */
      /* バイナリ読込モードでファイルを開く */
      if (szCommandLine[0] == NULL) {
        fp = fopen("data.dat", "rb");
        if (fp == NULL) {
          file_flag = 1;
          MessageBox(hWnd, "ファイルdata.datが開けませんでした",
                     "エラー", MB_OK | MB_ICONEXCLAMATION);
          break;
        }
      } else {
        /* コマンド行で指定されたファイルの存在チェック */
        hFile = _findfirst(szCommandLine, &f_info);
        if (hFile != -1) {
          _findclose(hFile);
        } else {
          file_flag = 1;
          if (errno == ENOENT) {
            sprintf(out_buffer, "ファイル%sが見つかりませんでした", szCommandLine);
            MessageBox(hWnd, out_buffer,
                       "エラー", MB_OK | MB_ICONEXCLAMATION);
          }
          if (errno == EINVAL) {
            sprintf(out_buffer, "ファイル名%sが無効です。", szCommandLine);
            MessageBox(hWnd, out_buffer,
                       "エラー", MB_OK | MB_ICONEXCLAMATION);
          }
          break; /* エラー処理後、何もしない */
        }
        /* コマンド行で指定された、ファイルを開く */
        fp = fopen(szCommandLine, "rb");
        if (fp == NULL) {
          file_flag = 1;
          sprintf(out_buffer, "ファイル%sが開けませんでした", szCommandLine);
          MessageBox(hWnd, out_buffer,
                     "エラー", MB_OK | MB_ICONEXCLAMATION);
          break;
        }
      }
      /* ファイルの先頭256バイトを変数buferに読込む */
      count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
      break;
    case WM_DESTROY :
      /* ファイルを閉じる */
      if (fp != NULL) {
        fclose(fp);
      }
      /* 論理フォントの開放 */
      DeleteObject(hFont);
      PostQuitMessage(0);  /* メッセージループを抜けさせる */
      break;
    case WM_CLOSE :
      if (MessageBox( hWnd, "終了してもよろしいですか?", "終了確認",
          MB_ICONQUESTION | MB_OKCANCEL ) != IDOK)
        break ;
      /* CANCELが選択されたら、ケースdefaultを実行する */
    default :
      return (LRESULT)DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return (LRESULT)0;
}