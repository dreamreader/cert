//FPProcess.h ���������� ��������������� ������ � 2 ������� ���������� (������������ � �����������)

#ifndef _FP_PROCESS_H
#define _FP_PROCESS_H

#include "../nb/nb.h"
#include "CFPMinutiae.h"

//�������������� ����������� ��������� ������ � 2 ������� ����������:
//����������� � ����������
//  img    - [��] 256-������� ����� ����������� nWt*nHt [320 * 480]
//  �param - [���] ������ ����������� ���������� [416]
//  dparam - [���] ������ ���������� ���������� [96], ����������� 1 � 0
//����������: ������� �������� ��������� �����������
bool fp_process(int img[], FPPoint minutiae[], int &numberMinutiae, float cparam[], int dparam[]);

#endif
