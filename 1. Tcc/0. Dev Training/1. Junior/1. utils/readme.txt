. ���ʹ�� __stdcall , ��tcc����Ͳ������def�ļ�,  ����� __declspec(dllexport), tcc �������dll��ͬʱ�����def�ļ�.

#define DLL_EXPORT __declspec(dllexport)   //__stdcall

. ��makefile�ļ���, �������ǰ����@, �ͻ᲻����ʾ�������ִ��(ʵ���Ѿ�ִ����)

. ����makefile �� nmake

. 2019.01.10

 1. Astring.dll  �鲢��str_tools.dll ��.

 2. FileApi.dll  �ϲ��� file_tools.dll��

 3. 

. 2019.03.07

  ������ pdf_tools ���ڴ�й¶����. ԭ����  ��һ�α����ı���ʱ�� cellMap_p->txtTotal ������ı��Ƿ���cell ������, ���ǲ�û������ΪtxtIDs_p �����ڴ沢��Ŷ�ӦtextID, ����printCellMap()����ȷʵֱ�ӷ���  txtIDs_p[i], ��ô�ͳ������ڴ�й¶.  �޸�printCellMap()�� ���ж�txtTotal �� �����ж�txtIDs_p �Ƿ��Ѿ������ڴ���, �پ����Ƿ���ʾcell �е��ı����.

  