#ifndef _WIDGET_CLOCK_H_
#define _WIDGET_CLOCK_H_

#include <ShellWidget/UiWidget.h>

// Widget DLL 中必须通过 .def 文件导出此函数
// 此函数创建并返回Widget对象
UiWidget* CreateWidgetInstance(void* lpVoid);

#endif  //_WIDGET_CLOCK_H_
