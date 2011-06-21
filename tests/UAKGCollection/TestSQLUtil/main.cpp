#include <Utils/SQLTextUtils.h>
#include <Utils/SzArray.h>

void test0()
{
	const char* x_const= "a";
	char* y="b";
	x_const = y;
//	y=x;              -- Error convertion
}

void test1()
{
	SzArray szar;
	szar.append_sz_dup("select ");
	szar.append_sz_dup("from ");
	szar.append_sz_dup("where ");
	szar.append_sz_dup("order by ");
	szar.append_sz_dup("with key ");

	SzArray* res = SQLTextUtils::create_sql_parts_szarray(
//		"select v1, v2 from test order by v2 with key v3", szar);
//		"select F3 from UAKGTEST where F1='aa' and F3='a1' order by F2", szar);
		"select F1,F2 from UAKGTEST where 1=1", szar);
	cout << "all done" << endl;
}

void main()
{ test1(); }

