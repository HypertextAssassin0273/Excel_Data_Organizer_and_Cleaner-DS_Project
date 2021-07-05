#include"AVL.hpp"
#include"String.hpp"
#include"Vector.hpp"
#include"Sorts.hpp"
#include"Global.hpp"

#define Enter 13
#define Escape 27
#define Up 72
#define Down 80
#define Right 77
#define Left 75

class data_set{//i.e. parser for .csv files
protected:
	short colomns;
	String file_name;
	Vector<String> types;
	
	data_set():colomns(0){//i.e. protected ctor
		ifstream fin("data_set_info.txt");
		if(!fin)
			throw false;
		fin>>file_name;
		String type;
		for(char reader='\0';reader!='\n'&&!fin.eof();++colomns){//i.e. reads all types
			while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof())
				type.push_back(reader);
			types.push_back(move(type));
		}
		fin.close();
		if(!colomns)
			throw false;
	}
	
	void read_row(ifstream& fin,Vector<String>& row)const{//i.e. reads data in basic-filtered mode
		char reader='\0'; String cell;
		for(short i=0;i<colomns;++i){
			if(types[i]=="string"){
				while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof())
					if(reader>='A'&&reader<='Z'||reader>='a'&&reader<='z'||reader>='0'&&reader<=':'||
					   reader>='#'&&reader<=')'||reader=='.'||reader=='-'||reader==' ')
						cell.push_back(reader);
			}
			else if(types[i]=="int"||types[i]=="float"){
				while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof())
					if(reader>='0'&&reader<='9'||reader=='.')
						cell.push_back(reader);
			}
			else{//i.e. default type: "string-list"
				reader=fin.get();//removes '"'
				if(reader=='"'){
					char reader2='\0';
					while(!('"'==(reader=fin.get())&&(','==(reader2=fin.get())||reader2=='\n'))&&!fin.eof()){
						if(reader>='A'&&reader<='Z'||reader>='a'&&reader<='z'||reader>='0'&&reader<=':'||
						   reader>='"'&&reader<=')'||reader>=','&&reader<='.'||reader==' ')
							cell.push_back(reader);
						if(reader2>='A'&&reader2<='Z'||reader2>='a'&&reader2<='z'||reader2>='0'&&reader2<=':'||
						   reader2>='#'&&reader2<=')'||reader=='"'||reader2>=','&&reader2<='.'||reader2==' ')
							cell.push_back(reader2);
					}
				}
				else if(reader!=',')
					do{
						if(reader>='A'&&reader<='Z'||reader>='a'&&reader<='z'||reader>='0'&&reader<=':'||
						   reader>='#'&&reader<=')'||reader=='.'||reader=='-'||reader==' ')
							cell.push_back(reader);
					}while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof());
			}
			row.push_back(move(cell));
		}
	}
	
	void write_row(ofstream& fout,const Vector<String>& row,const Vector<char>* colomn_flags=nullptr)const{
		bool flag;
		for(short i=0,j;i<row.size();++i){
			if(colomn_flags&&(*colomn_flags)[i])
				continue;
			flag=false;
			if(types[i]=="string"||types[i]=="int"||types[i]=="float")
				fout<<row[i];
			else{//i.e. default type: "string-list"
				for(j=0;j<row[i].size();++j)
					if(row[i][j]==','||row[i][j]=='"'){
						flag=true;
						break;
					}
				if(flag)
					fout<<'"'<<row[i]<<'"';
				else
					fout<<row[i];
			}
			if(i!=row.size()-1)
				fout<<',';
		}
	}
};


class cleaner:protected data_set{
	/* member-containers for prioritized coloumn indexes */
	Vector<int> null_values;
	struct outlier{
		int index;
		String lower_range,upper_range;//i.e. used to hold both integer & float values
		
		outlier()noexcept:index(-1){}
	};
	Vector<outlier> outliers;
	
	cleaner(){//i.e. private ctor
		ifstream fin("prioritized_colomns.txt");
		if(!fin)
			throw false;
		{//i.e. destroys index variable
			int index=-1;
			do{//i.e. reads prioritized null_value coloumn indexes
				fin>>index;
				if(index>=0&&index<=colomns)
					null_values.push_back(index);
			}while(fin.get()!='\n'&&!fin.eof());
		}
		outlier temp; char reader='\0';
		while(reader!='\n'&&!fin.eof()){//i.e. reads prioritized outliers
			fin>>temp.index;
			fin.get();//i.e. neglecting comma
			while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof())
				temp.lower_range.push_back(reader);
			while(','!=(reader=fin.get())&&reader!=' '&&reader!='\n'&&!fin.eof())
				temp.upper_range.push_back(reader);
			outliers.push_back(move(temp));//i.e. invokes move ctor for String types implicitly
		}
		fin.close();
		short outlier_limit=0;
		for(short i=0;i<types.size();++i)
			if(types[i]=="int"||types[i]=="float")
				++outlier_limit;
		if(outliers.size()>outlier_limit||
		   null_values.size()>colomns)
			throw false;
	}
	
	bool filter_data(Vector<String>& row)const{
		for(short i=0;i<colomns;++i){
			if(row[i].empty()){//i.e. deals with null values
				for(short j=0;j<null_values.size();++j)
					if(null_values[j]==i)
						return false;
				row[i]="N.I.L";
				/*Note:
					If current index matches any of the prioritized null_value index,
					then simply drop that row-entry. Otherwise, set empty cells as N.I.L. 
				*/
			}
			else if(types[i]=="int"||types[i]=="float"){//i.e. deals with outliers
				for(short j=0;j<outliers.size();++j)
					if(outliers[j].index==i&&
					  (outliers[j].lower_range!="---"&&Stoif(row[i])<Stoif(outliers[j].lower_range))||
					  (outliers[j].upper_range!="---"&&Stoif(row[i])>Stoif(outliers[j].upper_range)))
						return false;
				/*Note:
					If current index matches any of the prioritized outliers index,
					& current cell is an outlier (i.e. out of range),
					then simply drop that row-entry.
				*/
			}
		}
		return true;
	}
public:
	static void clean(){
		cleaner object;//i.e. reads 'data_set_info.txt' file
		
		ifstream fin((object.file_name+".csv\0").c_str());//Note: c-type string should be null-terminated
		if(!fin)
			throw false;
		ofstream fout((object.file_name+"_cleaned.csv\0").c_str());
		
		for(char reader='\0';(reader=fin.get())!='\n'&&!fin.eof();fout<<reader);//i.e. reads complete row_1 of data_set
		fout<<endl;
		
		Vector<String> row;
		row.reserve(object.colomns);
		while(!fin.eof()){//i.e. reads & then writes the data_set row by row
			object.read_row(fin,row);
			if(object.filter_data(row)){
				object.write_row(fout,row);
				fout<<endl;
			}
			row.clear();
		}
		fin.close();
		fout.close();
	}
};


class manager:protected data_set{
protected:
	Vector<Vector<String>> table;
	Vector<char> colomn_flags;//i.e. used only for dropping colomns
	/*Note: Vector<char> is best available custom-structure for 
			alternate to STL vector<bool> or bitwise-array */
	
	manager(bool ctor_flag=false)://i.e. private ctor
		colomn_flags(ctor_flag?colomns:0,false){
		ifstream fin((file_name+".csv\0").c_str());
		if(!fin)
			throw false;
		Vector<String> row;
		{//i.e. destroys cell object
			String cell;
			row.reserve(colomns);
			for(short i=0;i<colomns;++i){//i.e. reads complete row_1 of data_set
				for(char reader='\0';(reader=fin.get())!=','&&reader!='\n'&&!fin.eof();cell.push_back(reader));
				row.push_back(move(cell));
			}
			table.push_back(move(row));
		}
		while(!fin.eof()){
			row.reserve(colomns);
			read_row(fin,row);
			table.push_back(move(row));
		}
		fin.close();
		/*i.e. checking for cleaned file in order to exclude the last row*/
		String temp_name;
		temp_name.reserve(8);
		for(short i=1;i<9;++i)
			temp_name.push_back(file_name[file_name.size()-i]);
		if(temp_name=="denaelc_")
			table.pop_back();
	}
	
	void save_file()const{
		ofstream fout((file_name+"_modified.csv\0").c_str());
		for(uint64 i=0;i<table.size();++i)//i.e. writes the data_set row by row
			if(table[i].size()){
				write_row(fout,table[i],&colomn_flags);
				if(i!=table.size()-1)
					fout<<endl;
			}
		fout.close();
	}
	
	void sort_data(){
		short choice,col; bool order_flag,stoif_flag;
		Validate_Input(choice,1,3,"Select which sort you want to use?"
								  "\n1. Bubble     (works best for sorting in presorted data)"
								  "\n2. Insertion  (gives slightly better performance than flag-based bubble-sort)"
								  "\n3. Selection  (works best for sorting integers/floats in small/medium range)"
								  "\n==> ");
		Validate_Input(order_flag,0,1,"\nSort in which Order?\n1 -> ascending\n0 -> descending\n==> ");
		Validate_Input(stoif_flag,0,1,"\nSort in integer/float converted string format?\n1 -> yes\n0 -> no\n==> ");
		Validate_Input(col,1,colomns,"\nEnter colomn number (1-",colomns,"): ");
		--col;
		switch(choice){
			case 1:
				Sort::Bubble(table,col,order_flag,stoif_flag||types[col]=="int"||types[col]=="float");
				break;
			case 2:
				Sort::Insertion(table,col,order_flag,stoif_flag||types[col]=="int"||types[col]=="float");
				break;
			case 3:
				Sort::Selection(table,col,order_flag,stoif_flag||types[col]=="int"||types[col]=="float");
				break;
		}
	}
public:
	static void manage(){
		manager object;
		uchar hit,col_coord=33; short cursor_index=0,cursor_pos=1; uint64 index_1=0,index_2=0;
		while(true){
			system("cls");
			cout<<"\n\t\t\t\t\t\t  !!! DATA MANAGER !!!"
				  "\n\t\t\t\t\t\t  ^^^^^^^^^^^^^^^^^^^^"
				  "\n\t\t\t\t\t(Source File Name: "<<object.file_name<<".csv)";
			Display_Boxes(4,28,7,30);
			Display_Boxes(4,62,7,30);
			Gotoxy(30,8);
			cout<<"1.    Drop Row   (random)";
			Gotoxy(64,8);
			cout<<"2.   Drop Colomn (random)";
			Gotoxy(30,12);
			cout<<"3.    Swap Rows  (random)";
			Gotoxy(64,12);
			cout<<"4.   Swap Colomns (random)";
			Gotoxy(30,16);
			cout<<"5.    Insert Row  (last)";
			Gotoxy(64,16);
			cout<<"6.   Insert Colomn  (last)";
			Gotoxy(30,20);
			cout<<"7.    Sort Data  (random)";
			Gotoxy(64,20);
			cout<<"8.   Save Changes & Return";
			do{
				if(col_coord==33){
					Gotoxy(col_coord,(cursor_pos-cursor_index)*4+4);
					Cursor_Pointer(col_coord,(cursor_pos-cursor_index)*4+4);
				}
				else{
					Gotoxy(col_coord,cursor_pos*2+4);
					Cursor_Pointer(col_coord,cursor_pos*2+4);
				}
				hit=getch();
				if (hit==Down){
					++cursor_index;
					cursor_pos+=2;
				}
				else if(hit==Up){
					--cursor_index;
					cursor_pos-=2;
				}
				else if(hit==Right||hit==Left)
					if(col_coord==33){
						++cursor_pos;
						col_coord=67;
					}
					else{
						--cursor_pos;
						col_coord=33;
					}
				else if(hit==Enter)
					break;
				else if(hit>='1'&&hit<='8'){
					cursor_pos=hit-'0';
					break;
				}
				else if(hit==Escape){
					cursor_pos=8;
					break;
				}
				if (cursor_pos>8){
					cursor_index=0;
					cursor_pos=(col_coord==33?1:2);
				}
				else if(cursor_pos<1){
					cursor_index=3;
					cursor_pos=(col_coord==33?7:8);
				}
			}while(true);
			system("cls");
			switch (cursor_pos){
				case 1:
					Validate_Input(index_1,2,object.table.size(),"Enter Index (2-",object.table.size(),"): ");
					object.table[index_1-1].clear();
					break;
				case 2:
					Validate_Input(index_1,1,object.colomns,"Enter Index (1-",object.colomns,"): ");
					for(index_2=0;index_2<object.table.size();++index_2)
						object.table[index_2][index_1-1].clear();
					object.colomn_flags[index_1-1]=true;
					break;
				case 3:
					Validate_Input(index_1,2,object.table.size(),"Enter Index-1 (2-",object.table.size(),"): ");
					Validate_Input(index_2,2,object.table.size(),"Enter Index-2 (2-",object.table.size(),"): ");
					if(index_1!=index_2&&object.table[index_1-1].size()&&object.table[index_2-1].size())
						Swap(object.table[index_1-1],object.table[index_2-1]);
					else{
						cout<<"Warning: Same Indexes provided or any one of them is dropped!\n\a";
						system("pause");
						}
					break;
				case 4:
					Validate_Input(index_1,2,object.colomns,"Enter Index-1 (2-",object.colomns,"): ");
					Validate_Input(index_2,2,object.colomns,"Enter Index-2 (2-",object.colomns,"): ");
					if(index_1!=index_2&&object.table[index_1-1].size()&&object.table[index_2-1].size())
						for(uint64 i=0;i<object.table.size();++i)
							Swap(object.table[i][index_1-1],object.table[i][index_2-1]);
					else{
						cout<<"Warning: Same Indexes provided or any one of them is dropped!\n\a";
						system("pause");
						}
					break;
				case 5:{
					ifstream fin("new_data.txt");
					if(!fin){
						cout<<"Error: No File found!\a";
						break;
					}
					Vector<String> row;
					object.read_row(fin,row);
					object.table.push_back(move(row));
					break;
				}
				case 6:{
					ifstream fin("new_data.txt");
					if(!fin){
						cout<<"Error: No File found!\a";
						break;
					}
					String cell; char reader='\0',reader2;
					for(index_1=0;reader!='\n'&&!fin.eof()&&index_1<object.table.size();++index_1){
						reader=fin.get();//removes '"'
						if(reader=='"'){
							reader2='\0';
							while(!('"'==(reader=fin.get())&&(','==(reader2=fin.get())||reader2=='\n'))&&!fin.eof()){
								if(reader>='A'&&reader<='Z'||reader>='a'&&reader<='z'||reader>='0'&&reader<=':'||
								   reader>='"'&&reader<=')'||reader>=','&&reader<='.'||reader==' '||reader=='_')
									cell.push_back(reader);
								if(reader2>='A'&&reader2<='Z'||reader2>='a'&&reader2<='z'||reader2>='0'&&reader2<=':'||
								   reader2>='#'&&reader2<=')'||reader=='"'||reader2>=','&&reader2<='.'||reader2==' '||reader=='_')
									cell.push_back(reader2);
							}
						}
						else if(reader!=',')
							do{
								if(reader>='A'&&reader<='Z'||reader>='a'&&reader<='z'||reader>='0'&&reader<=':'||
								   reader>='#'&&reader<=')'||reader=='.'||reader=='-'||reader==' '||reader=='_')
									cell.push_back(reader);
							}while(','!=(reader=fin.get())&&reader!='\n'&&!fin.eof());
						object.table[index_1].push_back(move(cell));
					}
					cell="N.I.L";
					for(;index_1<object.table.size();object.table[index_1++].push_back(cell));
					++object.colomns;
					object.colomn_flags.push_back(false);
					object.types.emplace_back("string-list");
					fin.close();
					break;
				}
				case 7:
					object.sort_data();
					break;
				case 8:
					Display_Boxes(1,44,8,33,5,'$');
					Gotoxy(47,9);
					cout<<"Save Changes to the File???";
					Gotoxy(54,10);
					cout<<"Press Y -> Yes";
					Gotoxy(54,11);
					cout<<"Press N -> No";
					hit=getch();
					system("cls");
					if(hit=='Y'||hit=='y'){
						object.save_file();
						cout<<"File is successfully modified!";
					}
					return;
			}
		}
	}	
};


class AMO:protected manager{
	Vector<AVL<String>> colomn_wise_table;//vector of trees for colomn wise search
	
	AMO(){
		AVL<String> colomn_tree;
		for(uint64 i,j=0;j<colomns;++j){
			for(i=0;i<table.size();++i)
				colomn_tree.insert(move(table[i][j]));
			colomn_wise_table.push_back(move(colomn_tree));
		}
	}
public:
	static void advance_management_operations(){
		AMO object;//i.e. not efficient method for searching by multiple atrributes
		//object.colomn_wise_table[2].in_order_traversal();
	}
};


int main(){
	try{
		uchar hit,cursor_pos=1; 
		while(true){
			Gotoxy(41,3);
			cout<<"vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
			Gotoxy(41,4);
			cout<<"!!! EXCEL DATA ORGANIZER & CLEANER !!!";
			Gotoxy(41,5);
			cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
			Display_Boxes(4,46,8,27);
			Gotoxy(48,9);
			cout<<"1.     Clean Data";
			Gotoxy(48,13);
			cout<<"2.    Manage Data";
			Gotoxy(48,17);
			cout<<"3. Advance Operations";
			Gotoxy(48,21);
			cout<<"4.       Close   ";
			Gotoxy(48,26);
			do{
				Gotoxy(44,4*cursor_pos+5);
				Cursor_Pointer(44,4*cursor_pos+5);
				hit=getch();
				if(hit==Down||hit==Right)
					++cursor_pos;
				else if(hit==Up||hit==Left)
					--cursor_pos;
				else if(hit==Enter)
					break;
				else if(hit==Escape){
					cursor_pos=4;
					break;
				}
				else if(hit>='1'&&hit<='4'){
					cursor_pos=hit-'0';
					break;
				}
				if(cursor_pos==5)
					cursor_pos=1;
				else if(!cursor_pos)
					cursor_pos=4;
			}while(true);
			system("cls");
			switch (cursor_pos){
					case 1:
						cleaner::clean();
						cout<<"File is successfully cleaned!";
						break;
					case 2:
						manager::manage();
						break;
					case 3:
						AMO::advance_management_operations();//i.e. efficient search by multiple atrributes
						break;
					case 4:
						return 0;
			}
		}
	}
	catch(bool){
		cout<<"Error: Either files are not found or wrong info is provided!\n\a";
		system("pause");
		return 0;
	}
}
