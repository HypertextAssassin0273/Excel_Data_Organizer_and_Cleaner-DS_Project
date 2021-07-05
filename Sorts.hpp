#if !VECTOR_GUARD
#include"Vector.hpp"
#endif

#if !STRING_GUARD
#include"String.hpp"
#endif

#if !GLOBAL_GUARD
#include"Global.hpp"//i.e. provides 'Stoif' func
using namespace Global;
#endif

#if __cplusplus >= 201103L
template<typename T>//i.e. T->type
void Swap(T &xp,T &yp){//i.e. works best for pointer-based objects, like strings, etc
    T temp=move(xp);
    xp=move(yp);
    yp=move(temp);
	//i.e. here, move ctor gets invoked for pointed obj
}
#else
template<typename T>
void Swap(T &xp,T &yp){
    T temp=xp;
    xp=yp;
    yp=temp;
}
#endif

namespace Sort{//i.e. sorts for Vector class
	void Bubble(const Vector<Vector<String>>& table,short col,bool direction_flag,bool stoif_flag){//i.e. improved bubble-sort 
		bool swapped=true;
		if(!stoif_flag){
			if(direction_flag)//i.e. ascending order
				for(__int64 i=0,j;swapped&&(i<table.size()-1);++i){
					swapped=false;
				    for (j=1;j<table.size()-i-1;++j)//i.e. neglecting row-1 by iterating 'j' from 1
						if(table[j][col]>table[j+1][col]){
    			    		Swap(table[j],table[j+1]);
    			    		swapped=true;
    			    	}
				}
			else//i.e. descending order
				for(__int64 i=0,j;swapped&&(i<table.size()-1);++i){
					swapped=false;
				    for (j=1;j<table.size()-i-1;++j)
						if(table[j][col]<table[j+1][col]){
    						Swap(table[j],table[j+1]);
    			    		swapped=true;
    			    	}
				}
		}
		else{
			if(direction_flag)
				for(__int64 i=0,j;swapped&&(i<table.size()-1);++i){
					swapped=false;
				    for (j=1;j<table.size()-i-1;++j)
						if(Stoif(table[j][col])>Stoif(table[j+1][col])){
    			    		Swap(table[j],table[j+1]);
    			    		swapped=true;
    			    	}
				}
			else
				for(__int64 i=0,j;swapped&&(i<table.size()-1);++i){
					swapped=false;
				    for (j=1;j<table.size()-i-1;++j)
						if(Stoif(table[j][col])<Stoif(table[j+1][col])){
    						Swap(table[j],table[j+1]);
    			    		swapped=true;
    			    	}
				}
		}
	}
	
	void Insertion(const Vector<Vector<String>>& table,short col,bool direction_flag,bool stof_flag){//i.e. insertion-sort
    	if(!stof_flag){
			if(direction_flag)//i.e. ascending
				for(__int64 j,i=2;i<table.size();++i){
      				Vector<String> key=move(table[i]);
					for (j=i-1;j>=0&&table[j][col]>key[col];--j)
						table[j+1]=move(table[j]);
					table[j+1]=move(key);
    			}
    		else//i.e. descending
    			for(__int64 j,i=2;i<table.size();++i){
      				Vector<String> key=move(table[i]);
					for (j=i-1;j>=0&&table[j][col]<key[col];--j)
						table[j+1]=move(table[j]);
					table[j+1]=move(key);
    			}
		}
		else{
			if(direction_flag)
				for(__int64 j,i=2;i<table.size();++i){
      				Vector<String> key=move(table[i]);
					for (j=i-1;j>=0&&Stoif(table[j][col])>Stoif(key[col]);--j)
						table[j+1]=move(table[j]);
					table[j+1]=move(key);
    			}
    		else
    			for(__int64 j,i=2;i<table.size();++i){
      				Vector<String> key=move(table[i]);
					for (j=i-1;j>=0&&Stoif(table[j][col])<Stoif(key[col]);--j)
						table[j+1]=move(table[j]);
					table[j+1]=move(key);
    			}
		}
	}
	
	void Selection(const Vector<Vector<String>>& table,short col,bool direction_flag,bool Stoif_flag){//i.e. selection sort
		if(!Stoif_flag){
			if(direction_flag)//i.e. ascending
				for(__int64 i=1,j,min_index;i<table.size()-1;++i){//i.e. neglecting row-1 by iterating 'i' from 1
        			min_index=i;
        			for(j=i+1;j<table.size();++j)
						if(table[j][col]<table[min_index][col])
        		       		min_index=j;
    		   		Swap(table[i],table[min_index]);
    			}
    		else//i.e. descending
    			for(__int64 i=1,j,min_index;i<table.size()-1;++i){
        			min_index=i;
        			for(j=i+1;j<table.size();++j)
						if(table[j][col]>table[min_index][col])
        		       		min_index=j;
    		   		Swap(table[i],table[min_index]);
    			}
		}
		else{
			if(direction_flag)
				for(__int64 i=1,j,min_index;i<table.size()-1;++i){
        			min_index=i;
        			for(j=i+1;j<table.size();++j)
						if(Stoif(table[j][col])<Stoif(table[min_index][col]))
        		       		min_index=j;
    		   		Swap(table[i],table[min_index]);
    			}
    		else
    			for(__int64 i=1,j,min_index;i<table.size()-1;++i){
        			min_index=i;
        			for(j=i+1;j<table.size();++j)
						if(Stoif(table[j][col])>Stoif(table[min_index][col]))
        		       		min_index=j;
    		   		Swap(table[i],table[min_index]);
    			}
		}
	}
	
}
