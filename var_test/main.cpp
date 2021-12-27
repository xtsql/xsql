#include "dynamic_var.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*

#ifndef DYNAMIC_VAR_MUTE_OPNONE
throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE

#ifndef DYNAMIC_VAR_MUTE_INVALID_ARG
throw std::invalid_argument("nullptr receieved");
#endif // !DYNAMIC_VAR_MUTE_INVALID_ARG

#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA

*/
int main()
{
	char t0 = 5;
	short t1 = 11;
	int t2 = 20;
	long long t3 = 30;
	float t4 = 60.006f;
	double t5 = 5.0005;
	bool t6 = true;
	bool t7 = false;
	const char t8[] = " -30.30 ";
	unsigned char t9[] = { 65,66,67 };  //ABC
	const char t10[] = "he ";
	const char t11[] = "123F ";
	const char t12[] = "2.2.2 ";
	const char t13[] = "ab12 ";
	const char t14[] = " \t ";
	const char t15[] = "";
	const char t16[] = " 5 ";
	const char t17[] = " 150 ";
	const char t18[] = " 35000 ";
	var v[] = { t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18 };
	for (int i = 0; i < 19; i++)
	{
		system("cls");
		for (int j = 0; j < 19; j++)
		{
			printf("v[%2d] %%= v[%2d] is: ", i, j);
			try
			{
				var a1 = v[i];
				a1 %= v[j];
				printf("[%-15s]\t%s\n", a1.type_name().c_str(), a1.to_string().c_str());
			}
			catch (var::ViA){	cout << "ViA!\n";}
			catch (var::opNone){	cout << "opNone!\n";}
			catch (...){	cout << "unexpected error\n";}
		}
		cout << endl;
		system("pause");
	}
	return 0;
}