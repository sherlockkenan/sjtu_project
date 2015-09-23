#ifndef _DB_H
#define _DB_H

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<time.h>
#include<stdio.h>

using namespace std;


#define node_number 256        //一个节点key的个数
#define File_pointer   int            //文件的指针

#define search_fault -1




struct Node
{
	int keys[node_number + 1];
	int pointers[node_number + 2];              //pointers leaf的指向的是value，interior指向的是儿子的指针,为了统一，叶子中key【i】对应的是pointer【i】
	int keys_size;
	bool isleaf;

};

struct Data
{
	string name;
	int score;
};

class DB{
public:

	DB(string filename, int flag);
	void DB_close();
	bool DB_fetch(int key);
	bool DB_store(int key, string name, int score, int type);
	void DB_delete(int key);
	void _delete(Node&x,int key);

	void search_in_bpuls(int key);
	
	/*分裂节点*/
	void split_node(Node&node, Node&, int i);

	/*直接插入*/
	void direct_insert(Node& node, int key, File_pointer pointer);

	/*插入函数，从插入的位置插入*/
	void insert_in_bpuls(int key);

	/*插入没满的节点*/
	void insert_in_nonfull(Node& node, int key);

	/*在磁盘上分配一块B+树节点空间或者是数据的空间*/
	File_pointer Get_Ptr(string name);

	/*将一个B+树节点写入address地址*/
	void WriteNode(File_pointer address, Node &r);

	/*读取address地址上的一块B+树节点*/
	void ReadNode(File_pointer address, Node &r);

	/*将一个数据写入address地址*/
	void WriteDate(File_pointer address, Data &r);

	/*读取address地址上的一一个数据*/
	void ReadData(File_pointer address, Data &r);

	void case1_4(Node&node,Node&child,int& i,bool is_5);
	void case2_5(Node&node,Node&child,int key,int& i);
	void case3(Node&node);

	
	/*遍历函数*/
	void Visit();

	Data curr_dat;                     //提取出来的数据
	int first_dat;                   //叶子节点第一个叶子的指针
private:

	string idx_name;                    //打开数据库的名字
	string dat_name;
	FILE* fst_idx;
	FILE* fst_dat;
	File_pointer idxroot_ptr;                //索引B+树的根的偏移量     
	File_pointer idx_ptr;                      //索引的偏移量
	File_pointer dat_ptr;                      //文件的偏移量	
	Node curr_node;
	Node root_node;                              //根节点，根节点不用读入，保存在内存中
	bool is_replace;                 //用于是否替换



};


/*define the type of store*/
#define insert 1                     //insert new record
#define replace 2                    //replace old record
#define store 3                      //replace or insert

/*define the type of flag*/
#define create 1                       //create a new file with the name of filename
#define open_file  2                        //open the file with the name of filename


#endif