#include<iostream>
#include<string>
#include"simple_db.h"

using namespace std;

void call_insert(DB&db);
void call_replace(DB&db);
void call_delete(DB&db);
void call_fetch(DB&db);
void test(DB&db);
void insert_number(DB&db);

void use(){

	cout << "请输入要打开数据库的名字，新建为1，打开为2，例如输入“1 new_db” ："<<endl;
	int flag;
	string db_name;
	cin >> flag >> db_name;
	DB db(db_name, flag);

	while (true)
	{
		cout << "菜单：" << endl;
		cout << "插入   :  1" << endl;
		cout << "修改   :  2" << endl;
		cout << "删除   :  3" << endl;
		cout << "查找   :  4" << endl;
		cout << "测试    :  5"<< endl;
		cout << "大量插入数据    :  6" << endl;
		cout << "退出    :7" << endl;
		cout << endl << endl;
		cout << "请输入你要执行的操作的数字" << endl;
		int fl;
		cin >> fl;
		switch (fl)
		{
		case 1:
		{
			call_insert(db); 
			break;
		}
		case 2:
		{
			call_replace(db); 
			break;
		}
		case 3:
		{
			call_delete(db);
			break;
		}
		case 4:
		{
			call_fetch(db);
			break;
		}
		case 5:
		{
			test(db);
			break;
		}
		case 6:
		{
			insert_number(db);
			break;
		}
		case 7:
		{
			db.DB_close();
			exit(0);
		}
		default:
			break;
		}
	}
}
void call_insert(DB&db)
{
	int key,score;
	string name;
	cout << "请输入学号，不能超过2^31：" << endl;
	cin >> key;
	cout << "请输入你的姓名：" << endl;
	cin >> name;
	cout << "请输入你的分数：" << endl;
	cin >> score;

	clock_t time = clock();
	db.DB_store( key,  name, score, 1);
	cout << "用时：" << clock() - time;
	cout << endl << endl;
}
void call_replace(DB&db)
{
	int key, score;
	string name;
	cout << "请输入学号，不能超过2^31：" << endl;
	cin >> key;
	cout << "请输入你要替代的姓名：" << endl;
	cin >> name;
	cout << "请输入你要替代的分数：" << endl;
	cin >> score;

	clock_t time = clock();
	db.DB_store(key, name, score, 2);
	cout << "用时：" << clock() - time;
	cout << endl << endl;
}
void call_delete(DB&db)
{
	int key;
	cout << "请输入要删除数据的学号，不能超过2^31：" << endl;
	cin >> key;

	clock_t time = clock();
	db.DB_delete(key);
	cout << "用时：" << clock() - time;
	cout << endl << endl;
}
void call_fetch(DB&db)
{
	int key;
	cout << "请输入你要查找的学号，不能超过2^31：" << endl;
	cin >> key;

	clock_t time = clock();
	if (db.DB_fetch(key) == true)
	{
		cout << "姓名：\t" << db.curr_dat.name << endl;
		cout << "分数：\t" << db.curr_dat.score << endl;
	}
	else
		cout << "search false" << endl;

	cout << "用时：" << clock() - time;
	cout << endl << endl;
}
void test1(DB&db)
{
	for (int i = 1; i <= 1000000; ++i)
	{
		db.DB_store(i, "keke", 100, insert);

	}


	for (int i = 10; i <= 999990; ++i){
		db.DB_delete(i);

	}
	for (int i = 1; i <= 20; ++i)
	{
		if (db.DB_fetch(i) == true)
		{
			cout << i << "   " << db.curr_dat.name << "  " << db.curr_dat.score << endl;
		}
		else
			cout << i << "   " << "search false" << endl;

	}
	for (int i = 999980; i <= 1000000; ++i)
	{
		if (db.DB_fetch(i) == true)
		{
			cout << i << "   " << db.curr_dat.name << "  " << db.curr_dat.score << endl;
		}
		else
			cout << i << "   " << "search false" << endl;

	}

	cout << endl;
	cout << "遍历所有的数据:" << endl;
	int adress = db.first_dat;
	while (true)
	{
		Node node;

		db.ReadNode(adress, node);
		for (int i = 0; i < node.keys_size; ++i)
		{
			cout << node.keys[i] << endl;
		}
		if (node.pointers[node_number + 1] <= 0)
			break;
		adress = node.pointers[node_number + 1];
	}
}

void test2(DB&db)
{
	cout << "请输入你要测试的数据量大小：" << endl;
	int number;
	cin >> number;

	

	for (int i = 0; i < number; ++i)
	{
		db.DB_store(i, "keke", i % 100, 1);
	}

	db.Visit();


	for (int i = 0; i < 5 * number; ++i){

		int j = rand() % number;

		db.DB_fetch(j);

		if (i % 37 == 0){
			int j = rand() % (number * 2);
			db.DB_delete(j);
		}
		if (i % 11 == 0)
		{
			int j = rand() % (2 * number);
			db.DB_store(j, "keke", j % 100, insert);
			db.DB_fetch(j);

		}

		if (i % 17 == 0)
		{
			int j = rand() % (2 * number);
			if (i % 34 == 0)
			{
				db.DB_store(j, "ab", j % 100, replace);
			}
			else{
				db.DB_store(j, "kenan", j % 100, replace);
			}
		}
	}

	int adress = db.first_dat;
	while (true)
	{
		Node node;

		db.ReadNode(adress, node);
		db.DB_delete(node.keys[node.keys_size - 1]);

		for (int i = 0; i < 10; ++i)
		{
			int j = rand() % (2 * number);
			db.DB_fetch(j);
		}

		if (node.keys_size == 0)
			break;
		int adress = db.first_dat;
	}
	cout << "test2 pass" << endl;
	
}
void test(DB&db)                                                  //测试正确性的函数
{
	
	/*cout << "test1:" << endl;
	test1(db);*/


	
	cout << "test2:" << endl;
	test2(db);
	
	
}

void insert_number(DB&db)
{
	cout << "请输入你要插入的数据量大小：" << endl;
	int number;
	cin >> number;
	clock_t time = clock();

	for (int i = 0; i < number; ++i)
	{
		db.DB_store(i, "keke", i % 100, 1);
	}


	cout << "用时：" << clock() - time;
	cout << endl << endl;
}