#pragma once
#include "stdafx.h"

extern "C"
{
	//////////////////////网口通信函数
    _declspec(dllexport) unsigned int LED_DIST_Connect(char* address, int port);//打开网口
    _declspec(dllexport) unsigned int LED_DIST_Disconnect(unsigned int socket);//关闭网口

    //////////////////////参数设置函数
    _declspec(dllexport) int LED_DIST_New_SetArea(unsigned int socket, int style, int area, int value);//设置单区域
    _declspec(dllexport) int LED_DIST_New_SetAll(unsigned int socket, int style, int arr[60]);//设置所有区域
    _declspec(dllexport) int LED_DIST_New_SelectStyle(unsigned int socket, int style);//设置条纹样式（1/2）
    _declspec(dllexport) int LED_DIST_New_SaveAll(unsigned int socket);//设置保存条纹
    _declspec(dllexport) int LED_DIST_New_SetLightTime(unsigned int socket, int time);//设置点亮时长us
    
    //////////////////////触发设置函数
    _declspec(dllexport) int LED_DIST_New_SoftTri(unsigned int socket);//软触发
    _declspec(dllexport) int LED_DIST_New_SetTriTime(unsigned int socket, int time);//设置内触发周期
    _declspec(dllexport) int LED_DIST_New_SoftTriStart(unsigned int socket);//开始内触发
    _declspec(dllexport) int LED_DIST_New_SoftTriStop(unsigned int socket);//停止内触发
    
    //////////////////////读取函数
    _declspec(dllexport) int LED_DIST_New_ReadInfo(unsigned int socket, char* arr);//读取光源状态
    _declspec(dllexport) int LED_DIST_New_ReadAll(unsigned int socket, int style, char* arr);//读取条纹设置
    _declspec(dllexport) int LED_DIST_New_ReadVersion(unsigned int socket, char* arr);//读取光源版本号
}
