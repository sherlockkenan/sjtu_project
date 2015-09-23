
#include"simple_db.h"
#include"test.h"

using namespace std;



DB::DB(string filename, int flag)
{
	idx_name = filename+".idx";
	dat_name = filename + ".dat";

	if (flag == create)
	{
		errno_t err;
		err= fopen_s(&fst_idx, idx_name.c_str(), "wb+");                              //创建文件,存在的会被删除
		err = fopen_s(&fst_dat, dat_name.c_str(), "wb+");
		int root = sizeof(int)*2;
		int first = root;
		fwrite((char*)&root,sizeof(root),1,fst_idx);
		fseek(fst_idx,sizeof(int),SEEK_SET );
		fwrite((char*)&first, sizeof(root), 1, fst_idx);
		first_dat = root;
		
		idx_ptr =idxroot_ptr;
		dat_ptr = search_fault;                                         //表示没有对应的偏于量
		curr_node.isleaf = true;                           //建立空的节点
		
		curr_node.keys_size = 0;                           
		WriteNode( 8, curr_node);
		idxroot_ptr = sizeof(int) * 2;

		root_node.isleaf = true;
		root_node.keys_size = 0;

	}
	if (flag == open_file)
	{
		errno_t err;
		err= fopen_s(&fst_idx,idx_name.c_str(), "ab+");
		err= fopen_s(&fst_dat,dat_name.c_str(), "ab+");
		fread((char*)&idxroot_ptr, sizeof(int), 1, fst_idx);      //读入根的偏移量；
		fseek(fst_idx,sizeof(int),SEEK_SET);
		fread((char*)&first_dat, sizeof(int), 1, fst_idx);
	    idx_ptr = idxroot_ptr;
		dat_ptr = search_fault;

		fseek(fst_idx,idxroot_ptr,SEEK_SET);
		fread((char*)&curr_node, sizeof(Node), 1, fst_idx);

		fseek(fst_idx,idxroot_ptr,SEEK_SET);
		fread((char*)&root_node, sizeof(Node), 1,fst_idx);
		ReadNode( idxroot_ptr, curr_node);        //读入根的节点；

	}
}




/*查找函数，查找成功是db中更新dat_ptr,不成功时，db的idx_ptr也更新到对应叶子节点上，用于插入*/

void DB::search_in_bpuls( int key)
{
	int i;
	idx_ptr = idxroot_ptr;
	
	while(true)
	{
		ReadNode(idx_ptr, curr_node);                                              //读入根的节点
		for (i = 0; i < curr_node.keys_size&&key > curr_node.keys[i]; ++i){};
		if (i < curr_node.keys_size&&curr_node.isleaf&&key == curr_node.keys[i])
		{
			dat_ptr = curr_node.pointers[i];                                                 
			return;

		}
		if (curr_node.isleaf)
		{
			dat_ptr = search_fault;
			return;
		}
			
		idx_ptr = curr_node.pointers[i];

	} 
	
}


/*返回对应的key的value*/
bool DB::DB_fetch( int key)
{
	search_in_bpuls( key);
	if (dat_ptr != search_fault)
	{
		ReadData( dat_ptr, curr_dat);
		return true;
	}
	return false;
}


/*插入和删除函数*/

bool DB::DB_store(int key, string name, int score, int type)
{
	if (type == insert)
	{
		is_replace = false;
		curr_dat.name = name;
		curr_dat.score = score;
		dat_ptr = Get_Ptr(dat_name);                               //dat_ptr中保存数据文件位置指针
		WriteDate(dat_ptr, curr_dat);

		insert_in_bpuls(key);                                     //从插入位置插入索引文件中去
		return true;
	}

	if (type == replace)
	{
		is_replace = true;

		search_in_bpuls(key);
		if (dat_ptr == search_fault)
			return false;

		//构建数据类
		curr_dat.name = name;
		curr_dat.score = score;
		WriteDate(dat_ptr, curr_dat);

		insert_in_bpuls(key);
		return true;
	}
}




/*插入函数，从插入的位置插入*/
void DB::insert_in_bpuls(int key)
{

	idx_ptr = idxroot_ptr;
	ReadNode(idx_ptr, curr_node);

	if (curr_node.keys_size == node_number)
	{

		File_pointer p = Get_Ptr(idx_name);                  //新的根
	
	

		WriteNode(idx_ptr, root_node);                               //原先的根写入

		root_node.pointers[0] = idx_ptr;                             //新的根
		root_node.isleaf = false;
		root_node.keys_size = 0;

		WriteNode(p, root_node);                                 //写入

		idxroot_ptr = p;                                       

		idx_ptr = idxroot_ptr;
		split_node(root_node, curr_node, 0);


		insert_in_nonfull(root_node, key);

	}
	else
		insert_in_nonfull(curr_node, key);

}

/*插入不满的树*/
void DB::insert_in_nonfull(Node& node, int key)
{
	if (node.isleaf)                                                     //直接插入
	{
		direct_insert(node, key, dat_ptr);
	
	
	    WriteNode(idx_ptr, node);
	}


	else{

		int i;
		for (i = 0; i<node.keys_size&&key > node.keys[i]; ++i){};
		Node child;
		int child_ptr = node.pointers[i];
		ReadNode(child_ptr, child);
		if (child.keys_size == node_number)
		{
			split_node(node, child, i);
			if (key > node.keys[i])
			{
				i = i + 1;
				idx_ptr = node.pointers[i];
				ReadNode(idx_ptr, child);
				insert_in_nonfull(child, key);
				return;
			}
		}
		idx_ptr = child_ptr;
		insert_in_nonfull(child, key);



	}
}


/*将child_node分裂，插入father_node的第i个位置*/
void DB::split_node(Node&father_node, Node&child_node, int i)
{
	int split_key;



	Node new_node;
	File_pointer temp_pointer = Get_Ptr(idx_name);


	if (!child_node.isleaf)
	{
		split_key = child_node.keys[child_node.keys_size / 2];


		new_node.keys_size = (child_node.keys_size - 1) / 2;                                           //新的节点的一些设置
		for (int j = 0, k = child_node.keys_size / 2 + 1; j < new_node.keys_size; ++j, ++k)
		{
			new_node.keys[j] = child_node.keys[k];
			new_node.pointers[j] = child_node.pointers[k];
		}
		new_node.pointers[new_node.keys_size] = child_node.pointers[child_node.keys_size];
		new_node.isleaf = child_node.isleaf;

		WriteNode(temp_pointer, new_node);


		child_node.keys_size = child_node.keys_size / 2;                                         //之前节点的一些更改
		WriteNode(father_node.pointers[i], child_node);
	}
	else{


		split_key = child_node.keys[(child_node.keys_size - 1) / 2];


		new_node.keys_size = child_node.keys_size / 2;                                           //新的节点的一些设置
		for (int j = 0, k = (child_node.keys_size - 1) / 2 + 1; j < new_node.keys_size; ++j, ++k)
		{
			new_node.keys[j] = child_node.keys[k];
			new_node.pointers[j] = child_node.pointers[k];
		}
		new_node.pointers[new_node.keys_size] = child_node.pointers[child_node.keys_size];
		new_node.isleaf = child_node.isleaf;

		WriteNode(temp_pointer, new_node);


		child_node.keys_size = (child_node.keys_size - 1) / 2 + 1;                                         //之前节点的一些更改
		child_node.pointers[node_number + 1] = temp_pointer;                                      //指向下一位
		WriteNode(father_node.pointers[i], child_node);
	}

	direct_insert(father_node, split_key, temp_pointer);                                  //插入到父亲节点中
	WriteNode(idx_ptr, father_node);


}

/*直接在节点node中插入函数*/
void DB::direct_insert(Node& node, int key,int pointer)                    
{
	int i;
	for (i = 0; i<node.keys_size&&key > node.keys[i]; ++i){};
	
	if (!node.isleaf)
	{
		
		for (int j = node.keys_size; j > i; --j)
		{
			node.keys[j] = node.keys[j - 1];
			node.pointers[j + 1] = node.pointers[j];
		}
		node.keys[i] = key;
		node.pointers[i + 1] = pointer;
		node.keys_size++;
	}
	else{

		if (i < node.keys_size &&  node.keys[i] == key)  //在B+树叶节点找到了相同关键字  
		{
			//关键字插入重复
			if (is_replace)
			{
				node.pointers[i] = pointer;
			}
			return;
		}

		for (int j = node.keys_size; j > i; --j)
		{
			node.keys[j] = node.keys[j - 1];
			node.pointers[j] = node.pointers[j-1];
		}
		node.pointers[node.keys_size + 1] = node.pointers[node.keys_size];
		node.keys[i] = key;
		node.pointers[i] = pointer;
		node.keys_size++;
	}
}



/*无回溯删除，保证孩子的keys是半满的
  六种情况
  case 1,在当前内节点找到关键字，且儿子在叶节点
  case 2,在当前内节点找到关键字，且儿子在内节点
  case 3,在当前叶节点找到关键字
  case 4,在当前内节点没找到关键字，且儿子在叶节点
  case 5,在当前内节点没找到关键字，且儿子在内节点
  case 6,在当前叶节点没找到关键字
*/


/*第1和第4种情况的*/
void DB::case1_4(Node&x, Node& child, int& i, bool is_1)
{
	int j;
	
	if (child.keys_size >node_number / 2)                            //孩子数过半
	{
		if (is_1 == true){
			x.keys[i] = child.keys[child.keys_size - 2];
			child.keys_size--;

			WriteNode(idx_ptr, x);                                       //写入更新
			WriteNode(x.pointers[i], child);

			return;
		}
	}
	else                                                              //否则孩子节点的关键字数量不过半  
	{
		if (i > 0)                                                     //有左兄弟节点  
		{
			Node lbchild;
			ReadNode(x.pointers[i - 1], lbchild);

			if (lbchild.keys_size > node_number / 2)                    //左兄弟是半满的，向左兄弟借一个  
			{
				for (j = child.keys_size; j > 0; j--)
				{
					child.keys[j] = child.keys[j - 1];
					child.pointers[j] = child.pointers[j - 1];
				}

				child.keys[0] = x.keys[i - 1];
				child.pointers[0] = lbchild.pointers[lbchild.keys_size - 1];

				child.keys_size++;

				lbchild.keys_size--;

				x.keys[i - 1] = lbchild.keys[lbchild.keys_size - 1];

				if (is_1 == true)
				{
					x.keys[i] = child.keys[child.keys_size - 2];                          //递归删除下一行
				}

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i - 1], lbchild);
				WriteNode(x.pointers[i], child);

			}
			else                                                              //左兄弟没半满时，和左兄弟合并  
			{
				for (j = 0; j < child.keys_size; j++)
				{
					lbchild.keys[lbchild.keys_size + j] = child.keys[j];
					lbchild.pointers[lbchild.keys_size + j] = child.pointers[j];

				}
				lbchild.keys_size += child.keys_size;

				lbchild.pointers[node_number + 1] = child.pointers[node_number + 1];

				//释放child节点占用的空间x.pointers[i]  

				for (j = i - 1; j < x.keys_size - 1; j++)
				{
					x.keys[j] = x.keys[j + 1];
					x.pointers[j + 1] = x.pointers[j + 2];
				}
				x.keys_size--;

				if (is_1 == true)
				{
					x.keys[i - 1] = lbchild.keys[lbchild.keys_size - 2];
				}

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i - 1], lbchild);

				i--;                                                                         //递归删除lbchild中的关键字

			}
		}
		else                                                                         //只有右兄弟节点  
		{
			Node rbchild;
			ReadNode(x.pointers[i + 1], rbchild);

			if (rbchild.keys_size > node_number / 2)                                        //和有左兄弟一样 
			{
				x.keys[i] = rbchild.keys[0];
				child.keys[child.keys_size] = rbchild.keys[0];

				child.pointers[child.keys_size] = rbchild.pointers[0];

				child.keys_size++;

				for (j = 0; j < rbchild.keys_size - 1; j++)
				{
					rbchild.keys[j] = rbchild.keys[j + 1];
					rbchild.pointers[j] = rbchild.pointers[j + 1];
				}

				rbchild.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i], child);
				WriteNode(x.pointers[i + 1], rbchild);

			}
			else                                                                         //与右兄弟合并
			{
				for (j = 0; j < rbchild.keys_size; j++)
				{
					child.keys[child.keys_size + j] = rbchild.keys[j];
					child.pointers[child.keys_size + j] = rbchild.pointers[j];
				}
				child.keys_size += rbchild.keys_size;

				child.pointers[node_number + 1] = rbchild.pointers[node_number + 1];

				//释放rbchild占用的空间x.pointers[i+1]  

				for (j = i; j < x.keys_size - 1; j++)
				{
					x.keys[j] = x.keys[j + 1];
					x.pointers[j + 1] = x.pointers[j + 2];
				}
				x.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i], child);

			}

		}

	}
}


/*第2和第5种情况的*/
void DB::case2_5(Node&x,Node&child,int key, int& i){
	//找到key在B+树叶节点的左兄弟关键字,将这个关键字取代key的位置  
	int j;


	if (child.keys_size > node_number / 2)                                                 //孩子的关键字大于过半，直接向下
	{
	}
	else                                                          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
	{
		if (i > 0)                                                  //x.key[i]有左兄弟  
		{
			Node lbchild;
			ReadNode(x.pointers[i - 1], lbchild);

			if (lbchild.keys_size > node_number / 2)                //向左兄弟子借，左兄弟为内节点
			{
				for (j = child.keys_size; j > 0; j--)
				{
					child.keys[j] = child.keys[j - 1];
					child.pointers[j + 1] = child.pointers[j];
				}
				child.pointers[1] = child.pointers[0];
				child.keys[0] = x.keys[i - 1];
				child.pointers[0] = lbchild.pointers[lbchild.keys_size];

				child.keys_size++;

				x.keys[i - 1] = lbchild.keys[lbchild.keys_size - 1];
				lbchild.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i - 1], lbchild);
				WriteNode(x.pointers[i], child);



			}
			else                                                  //  与左兄弟合并
			{
				lbchild.keys[lbchild.keys_size] = x.keys[i - 1];   //将孩子节点复制到其左兄弟的末尾  
				lbchild.keys_size++;

				for (j = 0; j < child.keys_size; j++)      //将child节点拷贝到lbchild节点的末尾,  
				{
					lbchild.keys[lbchild.keys_size + j] = child.keys[j];
					lbchild.pointers[lbchild.keys_size + j] = child.pointers[j];
				}
				lbchild.pointers[lbchild.keys_size + j] = child.pointers[j];
				lbchild.keys_size += child.keys_size;        //已经将child拷贝到lbchild节点  


				//释放child节点的存储空间,x.pointers[i]  


				//将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
				for (j = i - 1; j < x.keys_size - 1; j++)
				{
					x.keys[j] = x.keys[j + 1];
					x.pointers[j + 1] = x.pointers[j + 2];
				}
				x.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i - 1], lbchild);

				i--;

			}

		}
		else                                                                  //否则x.key[i]只有右兄弟  
		{
			Node rbchild;
			ReadNode(x.pointers[i + 1], rbchild);

			if (rbchild.keys_size > node_number / 2)                                            //向内节点的右兄弟借
			{

				child.keys[child.keys_size] = x.keys[i];
				child.keys_size++;

				child.pointers[child.keys_size] = rbchild.pointers[0];
				x.keys[i] = rbchild.keys[0];

				for (j = 0; j < rbchild.keys_size - 1; j++)
				{
					rbchild.keys[j] = rbchild.keys[j + 1];
					rbchild.pointers[j] = rbchild.pointers[j + 1];
				}
				rbchild.pointers[j] = rbchild.pointers[j + 1];
				rbchild.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i], child);
				WriteNode(x.pointers[i + 1], rbchild);

			}
			else                                                             //  与右兄弟合并
			{
				child.keys[child.keys_size] = x.keys[i];
				child.keys_size++;

				for (j = 0; j < rbchild.keys_size; j++)                 //将rbchild节点合并到child节点后  
				{
					child.keys[child.keys_size + j] = rbchild.keys[j];
					child.pointers[child.keys_size + j] = rbchild.pointers[j];
				}
				child.pointers[child.keys_size + j] = rbchild.pointers[j];

				child.keys_size += rbchild.keys_size;

				//释放rbchild节点所占用的空间,x,pointers[i+1]  

				for (j = i; j < x.keys_size - 1; j++)    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
				{
					x.keys[j] = x.keys[j + 1];
					x.pointers[j + 1] = x.pointers[j + 2];
				}
				x.keys_size--;

				WriteNode(idx_ptr, x);
				WriteNode(x.pointers[i], child);

			}

		}
	}

}

void DB::_delete(Node&x,int key)
{
	int i, j;


	for (i = 0; i < x.keys_size && key > x.keys[i]; i++);

	/*case 1,在当前内节点找到关键字，且儿子在叶节点*/
	if (i < x.keys_size && x.keys[i] == key)                                       //在当前节点找到关键字  
	{
		if (!x.isleaf)                                                            //在内节点找到关键字  
		{
			Node child;
			ReadNode(x.pointers[i], child);

			if (child.isleaf)                                                     //如果孩子是叶节点  
			{
				case1_4(x, child, i, true);
			}

			/*case 2,在当前内节点找到关键字，且儿子在内节点*/
			else                                                                             //当前关键字的孩子是内孩子
			{
				//找到key在B+树叶节点的左兄弟关键字,将这个关键字取代key的位置  
				int temp = idx_ptr;
				int last_ptr;
				search_in_bpuls(key);
				last_ptr = idx_ptr;
				idx_ptr = temp;
				Node last;

				ReadNode(last_ptr, last);
				x.keys[i] = last.keys[last.keys_size - 2];
				WriteNode(idx_ptr, x);

				case2_5(x,child, key, i);
			}

			idx_ptr = x.pointers[i];                                                //向下递归
			_delete(child,key);
		}

		/*case 3,在当前叶子节点找到关键字*/
		else                                                                   //当前在叶子节点，直接删除 
		{
			for (j = i; j < x.keys_size - 1; j++)
			{
				x.keys[j] = x.keys[j + 1];
				x.pointers[j] = x.pointers[j + 1];                            
			}
			x.keys_size--;

			WriteNode(idx_ptr, x);

			return;
		}
	}

	  /*case 4,在当前内节点没找到关键字，孩子节点是外节点*/
	else                                                                         //在当前节点没找到关键字，基本如上，往下递归     
	{
		if (!x.isleaf)        //没找到关键字,则关键字必然包含在以pointers[i]为根的子树中  
		{
			Node child;
			ReadNode(x.pointers[i], child);
			if (child.isleaf)                                                      //如果其孩子节点是外节点  
			{
				case1_4(x, child, i, false);
			}

			/*case 5,在当前内节点没找到关键字，孩子节点是外节点*/
			else                                                        //否则其孩子节点是内节点  
			{
				case2_5(x, child, key, i);
			}
			/*case 6 当前叶子节点没找到关键字，不用操作，没有这个数据*/
			idx_ptr = x.pointers[i];
			_delete(child,key);
		}
	}
}

void    DB::DB_delete(int key)                                             //在B+中删除一个关键字  
{
	idx_ptr = idxroot_ptr;
	Node x;
	ReadNode(idx_ptr, x);
	_delete(x,key);

	

	if (!root_node.isleaf && root_node.keys_size == 0)    //如果删除关键字后根节点不是叶节点，并且关键字数量为0时根节点也应该被删除  
	{
		//释放ROOT节点占用的空间                                                
		File_pointer temp = root_node.pointers[0];         //根节点发生改变
		ReadNode(root_node.pointers[0], root_node);
		idxroot_ptr =temp;         //根节点下移,B+树高度减1  

	}

}


void DB::DB_close()
{
	fseek(fst_idx,0,SEEK_SET);
	fwrite((char*)&idxroot_ptr, sizeof(int), 1, fst_idx);
	fseek(fst_idx,sizeof(int),SEEK_SET);
	fwrite((char*)&first_dat, sizeof(int), 1, fst_idx);
	fseek(fst_idx,idxroot_ptr,SEEK_SET );
	fwrite((char*)&root_node, sizeof(Node), 1, fst_idx);
	fclose(fst_idx);
	fclose(fst_dat);

}




File_pointer DB::Get_Ptr(string filename)
{
	
	if (filename == idx_name)
	{
		fseek(fst_idx,0,SEEK_END );
		return (ftell(fst_idx));
		
	}
	else{
		fseek(fst_dat, 0, SEEK_END);
		return (ftell(fst_dat));
	}
	
}


void  DB::WriteNode(File_pointer address, Node &r)
{
	if (address == idxroot_ptr)
	{
		root_node = r;
		return;
	}
	fseek(fst_idx,address,SEEK_SET);
	fwrite((char*)(&r), sizeof(Node), 1, fst_idx);


}


void  DB::ReadNode(File_pointer address, Node& r)
{
	if (address == idxroot_ptr)
	{
		r = root_node;
		return;
	}
	fseek(fst_idx, address, SEEK_SET);
	fread((char*)(&r), sizeof(Node), 1, fst_idx);
	
	
}


void  DB::WriteDate(File_pointer address, Data &r)
{
	
	fseek(fst_dat, address, SEEK_SET);
	fwrite((char*)(&r), sizeof(Data), 1, fst_dat);
	
}


void  DB::ReadData(File_pointer address, Data &r)
{
	
	fseek(fst_dat, address, SEEK_SET);
	fread((char*)(&r), sizeof(Data), 1, fst_dat);

	

}

void DB::Visit()
{
	int adress = first_dat;
	while (true)
	{
		Node node;

		ReadNode(adress, node);
		for (int i = 0; i < node.keys_size; ++i)
		{
			
		}
		if (node.pointers[node_number + 1] <= 0)
			break;
		adress = node.pointers[node_number + 1];
	}
}
void main()
{

	use();
	

	//获取测试的数据
	/*DB db("8", create);


	int number = 2000000;
	clock_t time = clock();
	for (int i = 0; i <number; ++i)
	{
		db.DB_store(i, "a", i, insert);
	}
	cout << clock() - time<<endl;
	time = clock();


	int n = 10000;
	for (int i = 0; i < n; ++i)
	{
		db.DB_store(i + number, "keke", 100, insert);
	}
	cout << clock() - time << endl;
	time = clock();


	for (int i = 0; i < n; ++i)
	{
		db.DB_store(i + number, "kekeke", 10, replace);
	}
	cout << clock() - time << endl;
	time = clock();


	for (int i = 0; i < n; ++i)
	{
		db.DB_fetch(i + number);
	}

	cout << clock() - time << endl;
	time = clock();

	
	for (int j = 0; j < n; ++j)
	{
		db.DB_delete(j + number);
	}

	cout << clock() - time << endl;
	time = clock();*/
	/*for (int i = 1; i <= 100000; ++i)
	{
		db.DB_store(i, "keke", 100, insert);

	}
	

	for (int i =10; i <= 99990; ++i){
		db.DB_delete(i);
		
	}
	for (int i = 1; i <=20; ++i)
	{
	if (db.DB_fetch(i) == true)
	{
	cout <<i<<"   "<< db.curr_dat.name << "  " << db.curr_dat.score << endl;
	}
	else
		cout << i << "   " << "search false" << endl;

	}
	for (int i = 99980; i <=100000; ++i)
	{
		if (db.DB_fetch(i) == true)
		{
			cout <<i<<"   "<< db.curr_dat.name << "  " << db.curr_dat.score << endl;
		}
		else
			cout <<i<<"   "<< "search false" << endl;

	}

	cout << endl;
	cout << "遍历所有的数据:" << endl;
	int adress =db.first_dat;
	while (true)
	{
		Node node;

		db.ReadNode(adress, node);
		for (int i = 0; i < node.keys_size; ++i)
		{
			cout << node.keys[i]<<endl;
		}
		if (node.pointers[node_number + 1] <= 0)
			break;
		adress = node.pointers[node_number + 1];
	}*/
	

	

	
	//db.DB_close();

}
