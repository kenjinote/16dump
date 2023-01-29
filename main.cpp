/*!
 *  @file      dump.c
 *  @breif     �t�@�C���̂P�U�i�_���v��\�����܂�
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
char    szAppTitle[] = "�t�@�C���̂P�U�i�_���v��\�����܂�";

/* global stuff */
PSTR    szCommandLine = NULL;

/* function prototype declarations */
int         InitApplication(HINSTANCE, char *);
HWND        InitInstance(HINSTANCE, int);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

/*!
 * @fn     WinMain
 * @brief  Window�v���O�����̃G���g���E�|�C���g
 * @param  hInstance    Instance�n���h��
 * @param  hPrevInstance  ���݂͎g���Ă��Ȃ�
 * @param  szCmdLine    �R�}���h�s�̈���
 * @param  iCmdShow    �����\����Ԏw���̒l���i�[
 * @return returns the value from PostQauitMessage
 */
int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR      szCmdLine, int       iCmdShow)
{
  MSG        msg;      /* ���b�Z�[�W�\���� */

  if (!hPrevInstance) {
    szCommandLine = szCmdLine;

    if (!InitApplication(hInstance, szAppName)) {  /* Window�N���X�̓o�^  */
      MessageBox(HWND_DESKTOP, "Window�N���X�̓o�^�Ɏ��s���܂���",
                 "�G���[", MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
    }
  }

  /* Window�̍쐬  */
  if ((hWndMain = InitInstance(hInstance, iCmdShow)) == NULL) {
    MessageBox(HWND_DESKTOP, "Window�̍쐬�Ɏ��s���܂���",
               "�G���[", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  /* start message loop  */
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam; /* PostQuitMessage�̖߂�l��Ԃ� */
}

/*!
 * @fn     InitApplication
 * @brief  Window�N���X�̓o�^
 * @param  hInstance  Instance�n���h��
 * @param  class_name  Window�N���X�̖��O�ւ̃|�C���^
 * @return RegisterClass�֐��̖߂�l��Ԃ�
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
 * @brief  Window�̍쐬
 * @param  hInstance  Instance�n���h��
 * @param  iCmdShow  Window�̏����\����Ԃ��w������
 * @return CreateWindow�֐��̖߂�l��Ԃ�
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
 * @brief  Main Window Procedure(�t�@�C���̐擪256�o�C�g���_���v�����)
 * @param  hWnd      Window�n���h��
 * @param  msg        ���b�Z�[�WID
 * @param  wParam      ��P����
 * @param  lParam      ��Q����
 * @return 0�܂���0�ȊO��Ԃ�
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int     count;
  static int     file_flag; /* file�����s���̎��A�P�ɂ��� */
  static int     seek_pos;  /* file��seek�ʒu��\���B */
  static char    in_buffer[256];
  static HFONT   hFont;
  static FILE    *fp;
  static struct  _finddata_t f_info;
  HDC            hdc;      /* �f�o�C�X�E�R���e�L�X�g�E�n���h�� */
  PAINTSTRUCT    paint;
  HFONT          hPreFont;
  long           hFile;
  int            i, j;
  char           out_buffer[256];
  char           tmp_buffer[16];

  switch (msg) {
    case WM_CHAR   :    /* �L�[�{�[�h�������ꂽ */
      switch ((TCHAR)wParam) {
        case 'n':  /* ����256�o�C�g��\�� */
          if (f_info.size > (size_t)(seek_pos * 256)) {
            seek_pos++;
          } else {
            break;
          }
          fseek(fp, seek_pos * 256, SEEK_SET); /* �ړ� */
          /* �t�@�C�����e��ϐ�bufer�ɓǍ��� */
          count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
          /* ��ʍX�V */
          InvalidateRect(hWnd, NULL, FALSE);
          break;
        case 'p':  /* ����256�o�C�g��\�� */
          if (seek_pos > 0) {
            seek_pos--;
          } else {
            break;
          }
          fseek(fp, seek_pos * 256, SEEK_SET); /* �ړ� */
          /* �t�@�C�����e��ϐ�bufer�ɓǍ��� */
          count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
          /* ��ʍX�V */
          InvalidateRect(hWnd, NULL, FALSE);
          break;
        case 'q':  /* �v���O�������I������ */
          PostMessage(hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
          break;
        default :  /* �������Ȃ� */
          break;
      }
      break;
    case WM_PAINT  :    /* ��ʂ̕`��X�V�v�������� */
      hdc = BeginPaint(hWnd, &paint); /* �`��̊J�n */
      /* �t�H���g���Œ�s�b�`��"�l�r �S�V�b�N"�t�H���g�ɕύX���� */
      hPreFont = (HFONT)SelectObject(hdc, (HFONT)hFont);
      /* �����Acount��0�ȊO�ł���΁A
       * dump�C���[�W���쐬���ăe�L�X�g�Ƃ��ďo�͂���
       */
      strcpy(out_buffer,
             "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
      strcat(out_buffer, "  0123456789ABCDEF");
      TextOut(hdc, 10, 10, out_buffer, strlen(out_buffer));
      if (count > 0) {
        for (i = 0; i < 16; i++) {
          /* out_buffer��0x00�ŃN���A���� */
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
      /* �t�H���g�����ɖ߂� */
      SelectObject(hdc, (HFONT)hPreFont);
      /* BeginPaint�֐��ɂ���ĊJ�n�����`����I������ */
      EndPaint(hWnd, &paint);
      break;
    case WM_CREATE :    /* Window���\�������O�ɗ��郁�b�Z�[�W */
      /* �f�o�C�X�R���e�L�X�g�n���h�����擾���� */
      hdc = GetDC(hWnd);
      /* �_��Font�̍쐬 */
      hFont = CreateFont(
                 MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                 0, 0, 0, FW_BOLD, 0, 0, 0, SHIFTJIS_CHARSET,
                 OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
                 FIXED_PITCH | FF_SWISS, "�l�r �S�V�b�N");
      /* hdc�̊J�� */
      ReleaseDC(hWnd, hdc);
      /* �t�@�C������f�[�^��Ǎ��� */
      file_flag = 0;
      count = 0; /* �t�@�C������Ǎ��񂾕�������\���ϐ������������� */
      memset(in_buffer, 0, sizeof(in_buffer)); /* in_buffer���N���A���� */
      /* �o�C�i���Ǎ����[�h�Ńt�@�C�����J�� */
      if (szCommandLine[0] == NULL) {
        fp = fopen("data.dat", "rb");
        if (fp == NULL) {
          file_flag = 1;
          MessageBox(hWnd, "�t�@�C��data.dat���J���܂���ł���",
                     "�G���[", MB_OK | MB_ICONEXCLAMATION);
          break;
        }
      } else {
        /* �R�}���h�s�Ŏw�肳�ꂽ�t�@�C���̑��݃`�F�b�N */
        hFile = _findfirst(szCommandLine, &f_info);
        if (hFile != -1) {
          _findclose(hFile);
        } else {
          file_flag = 1;
          if (errno == ENOENT) {
            sprintf(out_buffer, "�t�@�C��%s��������܂���ł���", szCommandLine);
            MessageBox(hWnd, out_buffer,
                       "�G���[", MB_OK | MB_ICONEXCLAMATION);
          }
          if (errno == EINVAL) {
            sprintf(out_buffer, "�t�@�C����%s�������ł��B", szCommandLine);
            MessageBox(hWnd, out_buffer,
                       "�G���[", MB_OK | MB_ICONEXCLAMATION);
          }
          break; /* �G���[������A�������Ȃ� */
        }
        /* �R�}���h�s�Ŏw�肳�ꂽ�A�t�@�C�����J�� */
        fp = fopen(szCommandLine, "rb");
        if (fp == NULL) {
          file_flag = 1;
          sprintf(out_buffer, "�t�@�C��%s���J���܂���ł���", szCommandLine);
          MessageBox(hWnd, out_buffer,
                     "�G���[", MB_OK | MB_ICONEXCLAMATION);
          break;
        }
      }
      /* �t�@�C���̐擪256�o�C�g��ϐ�bufer�ɓǍ��� */
      count = fread(in_buffer, sizeof(char), sizeof(in_buffer), fp);
      break;
    case WM_DESTROY :
      /* �t�@�C������� */
      if (fp != NULL) {
        fclose(fp);
      }
      /* �_���t�H���g�̊J�� */
      DeleteObject(hFont);
      PostQuitMessage(0);  /* ���b�Z�[�W���[�v�𔲂������� */
      break;
    case WM_CLOSE :
      if (MessageBox( hWnd, "�I�����Ă���낵���ł���?", "�I���m�F",
          MB_ICONQUESTION | MB_OKCANCEL ) != IDOK)
        break ;
      /* CANCEL���I�����ꂽ��A�P�[�Xdefault�����s���� */
    default :
      return (LRESULT)DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return (LRESULT)0;
}