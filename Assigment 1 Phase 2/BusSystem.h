#include<iostream>
#include<sstream>
#include<string>
#include<ctime>
#include<vector>
#include<algorithm>

#define MAX 999999999  

using namespace std;

class FragmentLinkedList{
	public:
    	class Node;
	
	protected:
   	 	Node **fragmentPointers;
   	 	int fragmentMaxSize;
   		int count;
   		int capacity;
    		
	public:
    	FragmentLinkedList(){
    		this->capacity=9999;
       		this->count = 0;
       		this->fragmentMaxSize = 0;
       		this->fragmentPointers = new Node *[9999];
       		for(int i=0; i<9999; i++)	this->fragmentPointers[i]=NULL;
    	}

	public:
    	class Node{
    		protected:
       			string CODE;
    			string LP;
    			bool CASE;
   				int TIME_A;
   				int TIME_B;
    		public:
       			Node *next;
       			Node *prev;
        		friend class FragmentLinkedList;
           		friend class BusSystem;

    		public:
       			Node(){
       				CODE="";
       				LP="";
   					CASE=0;
       				TIME_A=-1;
       				TIME_B=-1;
       				next = NULL;
       				prev = NULL;
       			}
        		Node(Node *next, Node *prev){
            		this->next = next;
            		this->prev = prev;
        		}
        		void set(string CODE, string LP, bool CASE, int TIME_A, int TIME_B){
			       	this->CODE=CODE;
		   	     	this->LP=LP;
		        	this->CASE=CASE;
       				this->TIME_A=TIME_A;
       				this->TIME_B=TIME_B;
           			this->next = NULL;
           			this->prev = NULL;
       			}
    	};
};

  
class BusSystem : public FragmentLinkedList {
public:
	// private valriables
	bool Is_init;
	vector<int>	CODE_size;
	vector<string> CODE;
	int max_size;
	
private:
	// private functions
	int  	command_hash(string command);
	bool 	calling_func(int code, string instruction, string& return_str);
	int  	str_to_int(string str);
	bool 	taking_data(string arr[], int N, string instruction, int start, int skip);
  	int  	CODE_index(string CODE);
	bool 	verify_add(int code_index, string LP, bool CASE, int TIME_A);
  	void 	add(int code_index, string CODE, string LP, bool CASE, int TIME_A, int TIME_B);
  	int   	Delete(int code_index, string CODE, int TIME_A, int TIME_B);
	string 	toStr(int N);
	
	//Cac func cho BTL
	string SQ(int N);
	string INS(string CODE, string LP, bool CASE, int TIME_A, int TIME_B);
	string DEL(string CODE, int TIME_A, int TIME_B);
	string CS(string CODE, int TIME, bool CASE, bool Is_both);
	string CE(string CODE, int TIME, bool CASE, bool Is_both);
	string GS(string CODE, int TIME, bool CASE, bool Is_both);
	string GE(string CODE, int TIME, bool CASE, bool Is_both);
		
public:
  void print_2_console(){
    stringstream ss;
    Node* pWalker=this->fragmentPointers[0];
    if(pWalker!=NULL){
      while(pWalker!=NULL){
        ss<<"["<<pWalker->CODE<<" "<<pWalker->LP<<" "<<pWalker->CASE<<" "<<pWalker->TIME_A<<" "<<pWalker->TIME_B<<"]";
        if(pWalker->next!=NULL)ss<<" -> ";
        pWalker=pWalker->next;
      }
    }
    else  ss<<"[]";
    cout<<ss.str()<<endl;
  }

	BusSystem(){
		this->max_size=0;
		this->Is_init=0;
	}

  ~BusSystem(){
    if(this->count != 0){
      Node* pWalker= this->fragmentPointers[0];
      while(pWalker!=NULL){
        Node* d_pointer= new Node;
        d_pointer=pWalker;
        pWalker=pWalker->next;
        if(d_pointer->next != NULL){
          d_pointer->next=NULL;
        } 
        if(pWalker!=NULL && pWalker->prev != NULL){
          pWalker->prev=NULL;
        } 
        delete d_pointer;
        this->count--;
      }
    }
    delete this->fragmentPointers;
    this->CODE.erase(this->CODE.begin(), this->CODE.end());
    this->CODE_size.erase(this->CODE_size.begin(), this->CODE_size.end());
    this->max_size=0;
    this->Is_init=0;
  }

  string query(string instruction) {
    int length= instruction.length();
    if(instruction[length-1]==' ' || instruction[0]==' ')	return "-1";
    string command;
    
    for(int i=0; i<length; i++){
      if(instruction[i] == ' ')	break;
      else command.push_back(instruction[i]);
	}
	int command_code=this->command_hash(command);
    string return_str="";
	if(this->calling_func(command_code, instruction, return_str)==0)	return "-1";
	else{
		return return_str;
	}
  }
};

int BusSystem::command_hash(string command){
	if(command=="SQ")	return 1;
	if(command=="INS")	return 2;
	if(command=="DEL")	return 3;
	if(command=="CS")	return 4;
	if(command=="CE")	return 5;
	if(command=="GS")	return 6;
	if(command=="GE")	return 7;
	return -1;
}

int BusSystem::str_to_int(string str){
	int N=0;
	if(str.length() > 9)	return -9999;
  	int length= str.length();
	for(int i=0; i<length; i++){
		if((int)str[i]>=0 && (int)str[i]<=57)
			N=N*10 + ((int)str[i] - 48);
		else return -9999;
	}
	return N;
}

bool BusSystem::taking_data(string arr[], int N, string instruction,int start, int skip){
	int index=0;
	bool flag=1;
  	int length=instruction.length();
	for(int i=start; i<length; i++){
		flag=0;
		if(instruction[i]==' '){
			if(instruction[i+1] == ' ')	return 0;
			else	index++;
		}
		else{
			arr[index].push_back(instruction[i]);
		}
		if(index>=N)	return 0;
	}
	if(index<N-skip-1 || arr[0].length()>5 || flag==1)	return 0;
	return 1;
}

bool BusSystem::calling_func(int code, string instruction, string &return_str){
	switch (code){
		case 1:{ //SQ
			if(this->Is_init)	return 0;
			string Number=instruction.substr(3, instruction.length()-1);
			for(unsigned int i=0; i<Number.length(); i++)	if(Number[i]==' ')	return 0;
			int N=str_to_int(Number);
			if(N<0)	return 0;
			return_str=this->SQ(N);
			break;
		}
		
		case 2:{ //INS
			string parameters[5]={"","","","",""};
			if(this->taking_data(parameters, 5, instruction, 4, 1)==1){
				string CODE=parameters[0];
				bool CASE=0;
				int	TIME_A=0,
					TIME_B=0;
				string LP="";
				if(parameters[1].length()>10)	return 0;
				else	LP=parameters[1];
				int temp=str_to_int(parameters[2]);
				if(parameters[4] == ""){
					if(temp<48 && temp>57)	return 0;
					TIME_A=temp;
					TIME_B=str_to_int(parameters[3]);
				}
				else{
					if(temp != 0 && temp !=1)	return 0;
					CASE=temp;
					TIME_A=str_to_int(parameters[3]);
					TIME_B=str_to_int(parameters[4]);
				}	
				if(TIME_A <0 || TIME_B< 0 || TIME_A>=TIME_B)	return 0;
				return_str=this->INS(CODE, LP, CASE, TIME_A, TIME_B);
			}
			else return 0;
			break;
		}
		
		case 3:{ // DEL
			string parameters[3]={"","",""};
			if(this->taking_data(parameters, 3, instruction, 4, 2) == 1){
				string CODE=parameters[0];;
				int	TIME_A=0,
					TIME_B=100000000;
				if(parameters[1]!=""){
					TIME_A=str_to_int(parameters[1]);
					if(parameters[2]!="")	TIME_B=str_to_int(parameters[2]);
          			else TIME_B=TIME_A;
				}
				if(TIME_A<0 || TIME_B<0 || TIME_A>TIME_B)	return 0;
        		return_str=this->DEL(CODE, TIME_A, TIME_B);
			}
			else return 0;
			break;
		}
		
		default:{ // REST
			if(code==-1)	return 0;
			string parameters[3]={"","",""};
			if(this->taking_data(parameters, 3, instruction, 3, 1) == 1){
				string CODE=parameters[0];
				int	TIME=str_to_int(parameters[1]);
				bool CASE=0;
				int temp=0;
        		bool Is_both=0;
				if(parameters[2] != "")	temp=str_to_int(parameters[2]);
        		else Is_both=1;
				if(TIME<0 || (temp!=0 && temp!=1))	return 0;
				CASE=temp;
				if(code == 4) return_str=this->CS(CODE, TIME, CASE, Is_both);
				else{
					if(code==5)	return_str=this->CE(CODE, TIME, CASE, Is_both);
					else if(code==6)	return_str=this->GS(CODE, TIME, CASE, Is_both);
						 else if(code==7)	return_str=this->GE(CODE, TIME ,CASE, Is_both);
				}
			}
			else return 0;
			break;
		}
	}	
  return 1;
}

int BusSystem::CODE_index(string CODE){
  if(!this->CODE.empty()){
    for(unsigned int i=0; i<this->CODE.size(); i++){
	  	if(this->CODE[i]==CODE){
	  		if(this->CODE_size[i] > this->max_size)	return -1; 
		  	else{
		  		return i;
		  	} 
		  } 
		else{
		  	if(i==this->CODE.size()-1){
		  		this->CODE.push_back(CODE);
		  		this->CODE_size.push_back(0);
		  		return i+1;
		  	}
		}
	}
  }
  else{
    this->CODE.push_back(CODE);
		this->CODE_size.push_back(0);
  }
  return 0;
}

void BusSystem::add(int code_index, string CODE, string LP, bool CASE, int TIME_A, int TIME_B){
	Node* N_bus= new Node;
	N_bus->FragmentLinkedList::Node::set(CODE, LP, CASE, TIME_A, TIME_B);
	if(this->count==0){
		this->fragmentPointers[0]=N_bus;
    	this->fragmentPointers[1]=N_bus;
	}
	else{
		Node* pWalker=this->fragmentPointers[code_index];
		int pos=this->CODE_size[code_index];
    	if(pos==0){
      		this->fragmentPointers[code_index]->next=N_bus;
      		N_bus->prev=this->fragmentPointers[code_index];
      		this->fragmentPointers[code_index]=N_bus;
      		this->fragmentPointers[code_index+1]=N_bus;
    	}
    	else{
      		for(int i=0; i<pos-1; i++)	pWalker=pWalker->next;
      		if(this->fragmentPointers[code_index+1] == pWalker) this->fragmentPointers[code_index+1]=N_bus;
		  	N_bus->next=pWalker->next;
		 	N_bus->prev=pWalker;
      		if(pWalker->next !=NULL)  pWalker->next->prev=N_bus;
		  	pWalker->next=N_bus;
    	}
	}
  	this->CODE_size[code_index]++;
  	this->count++;
}

bool BusSystem::verify_add(int code_index, string LP, bool CASE, int TIME_A){
  if((this->CODE_size[code_index] +1) > this->max_size || this->max_size==0)	return 0;
  if(this->count!=0){
    Node* pWalker=this->fragmentPointers[code_index];
    int length=this->CODE_size[code_index];
    for(int i=0; i<length; i++){
    	if(pWalker->LP == LP){
    		if(TIME_A <= pWalker->TIME_B)	return 0;
      	}
    	else{
        	if(pWalker->TIME_A==TIME_A && pWalker->CASE==CASE)  return 0;
      	}
      	pWalker=pWalker->next;
    }
  }
  return 1;
}

int BusSystem::Delete(int code_index, string CODE, int TIME_A, int TIME_B){
	Node* pWalker= this->fragmentPointers[code_index];
	int N=0;
	for(int i=0; i<this->CODE_size[code_index]; i++){
    	if(pWalker->TIME_A >= TIME_A && pWalker->TIME_A<= TIME_B){
      		Node* deleted_bus= new Node;
        	deleted_bus=pWalker;
        	if(pWalker == this->fragmentPointers[code_index]){
        	  if(pWalker == this->fragmentPointers[code_index+1]){
        	  	this->fragmentPointers[code_index+1]=NULL;
        	  	this->fragmentPointers[code_index]=this->fragmentPointers[code_index]->prev;
        	  	pWalker=pWalker->next;
        	  	if(this->fragmentPointers[code_index]!=NULL)	this->fragmentPointers[code_index]->next=NULL;
        	  	deleted_bus->prev=NULL;
			  }
			  else{
			  	this->fragmentPointers[code_index]=this->fragmentPointers[code_index]->next;
        	 	pWalker=pWalker->next;
        	  	deleted_bus->next=NULL;
          		if(deleted_bus->prev!=NULL){
        			if(this->fragmentPointers[code_index]!=NULL)
        		    	this->fragmentPointers[code_index]->prev=deleted_bus->prev;
        		    deleted_bus->prev->next= pWalker;	
		  		}
		  		else{
		  			if(this->fragmentPointers[code_index]!=NULL)
          		 	 	this->fragmentPointers[code_index]->prev=NULL;
		  		}	
			  }
        	}
        	else{
        		if(pWalker == this->fragmentPointers[code_index+1])	this->fragmentPointers[code_index+1]=pWalker->prev;
        		pWalker=pWalker->next;
        		deleted_bus->prev->next=pWalker;
        		if(pWalker!=NULL)	pWalker->prev=deleted_bus->prev;
        			deleted_bus->next=deleted_bus->prev=NULL;
        	}  
        	delete deleted_bus;
        	this->CODE_size[code_index]--;
        	N++;
        	i--;
    	}
    	else  pWalker=pWalker->next;
    }
    if(this->CODE_size[code_index]==0){
    	this->CODE.erase(this->CODE.begin()+ code_index);
    	this->CODE_size.erase(this->CODE_size.begin()+code_index);
    	for(int i=code_index; this->fragmentPointers[i]!=NULL; i++){
        	if(code_index != (int)(this->CODE.size()))	this->fragmentPointers[i]=this->fragmentPointers[i+1];
      	}	
    }
	this->count--;
	return N;
}

string BusSystem::toStr(int N){
  stringstream STR;
  STR<<N;
  return STR.str();
}

string BusSystem::SQ(int N){
	this->max_size=N;
	this->Is_init=1;
	return "1";
}

string BusSystem::INS(string CODE, string LP, bool CASE, int TIME_A, int TIME_B){
  int index=this->CODE_index(CODE);
  if(index<0) return "-1";
  if(this->verify_add(index, LP, CASE, TIME_A) == 0) return "-1";
  else  this->add(index, CODE, LP, CASE, TIME_A, TIME_B);
  return this->toStr(this->CODE_size[index]);
}

string BusSystem::DEL(string CODE, int TIME_A, int TIME_B){
  int index= this->CODE_index(CODE);
  if(index<0) return "-1";
  else{
  	if(this->CODE_size[index] == 0){
  		this->CODE.erase(this->CODE.end() - 1);
    	this->CODE_size.erase(this->CODE_size.end() - 1);
    	return "0";
	}
    else	return this->toStr(this->Delete(index, CODE, TIME_A, TIME_B));
  }
}

string BusSystem::CS( string CODE, int TIME, bool CASE, bool Is_both){
  int index=this->CODE_index(CODE);
  if(this->CODE_size[index] == 0){
    this->CODE.erase(this->CODE.end()-1);
    this->CODE_size.erase(this->CODE_size.end()-1);
    return "0";
  }
  int nums=0;
  Node* pWalker= this->fragmentPointers[index];
  int Frag_length=this->CODE_size[index];
  for(int i=0; i<Frag_length; i++){
    if(pWalker->TIME_A<= TIME && pWalker->TIME_B >= TIME){
      if(Is_both==1)  nums++;
      else  if(pWalker->CASE==CASE) nums++;
    }
    pWalker=pWalker->next;
  }
  return toStr(nums);
}

string BusSystem::CE(string CODE, int TIME, bool CASE, bool Is_both){
  int index=this->CODE_index(CODE);
  if(this->CODE_size[index] == 0){
    this->CODE.erase(this->CODE.end()-1);
    this->CODE_size.erase(this->CODE_size.end()-1);
    return "0";
  }
  int nums=0;
  Node* pWalker= this->fragmentPointers[index];
  int Frag_length=this->CODE_size[index];
  for(int i=0; i<Frag_length; i++){
    if(pWalker->TIME_B < TIME){
      if(Is_both==1)  nums++;
      else  if(pWalker->CASE==CASE) nums++;
    }
    pWalker=pWalker->next;
  }
  return toStr(nums);
}

string BusSystem::GS(string CODE, int TIME, bool CASE, bool Is_both){
  int index=this->CODE_index(CODE);
  if(this->CODE_size[index] == 0){
    this->CODE.erase(this->CODE.end()-1);
    this->CODE_size.erase(this->CODE_size.end()-1);
    return "-1";
  }
  Node* pWalker= this->fragmentPointers[index];
  string return_LP="";
  int min= MAX;
  int Frag_length=this->CODE_size[index];
  for(int i=0; i<Frag_length; i++){
    if(min >= (TIME - pWalker->TIME_A) && TIME >= pWalker->TIME_A){
      if(Is_both==1){
        if(min == (TIME - pWalker->TIME_A)){
          if(pWalker->CASE==0) return_LP= pWalker->LP; 
        }
        else{
          min=TIME - pWalker->TIME_A;
          return_LP= pWalker->LP;
        }
      }
      else{
        if(pWalker->CASE==CASE){
          min=TIME - pWalker->TIME_A;
          return_LP= pWalker->LP;
        }
      }
    }
    pWalker=pWalker->next;
  }
  if(min != MAX) return return_LP;
  else return "-1";
}

string BusSystem::GE(string CODE , int TIME, bool CASE, bool Is_both){
  int index=this->CODE_index(CODE);
  if(this->CODE_size[index] == 0){
    this->CODE.erase(this->CODE.end()-1);
    this->CODE_size.erase(this->CODE_size.end()-1);
    return "-1";
  }
  Node* pWalker= this->fragmentPointers[index];
  string return_LP="";
  int min= MAX;
  int start_time=-1;
  bool bus_case=CASE;
  int Frag_length=this->CODE_size[index];
  for(int i=0; i<Frag_length; i++){
    if(min >= (TIME - pWalker->TIME_B) && TIME > pWalker->TIME_B){
      if(min==(TIME - pWalker->TIME_B)){
        if(pWalker->CASE == bus_case){
          return_LP=(pWalker->TIME_A > start_time)?pWalker->LP:return_LP;
          start_time=(pWalker->TIME_A > start_time)?pWalker->TIME_A:start_time;
        }
        else{
          return_LP=(Is_both==1 && pWalker->CASE==0)?pWalker->LP:return_LP;
        }
      }
      else{
        if(Is_both == 0){
          if(pWalker->CASE == CASE){
            min=(TIME - pWalker->TIME_B);
            return_LP=pWalker->LP;
            bus_case=pWalker->CASE;
            start_time=pWalker->TIME_A;
          }
        }
        else{
          min=(TIME - pWalker->TIME_B);
          return_LP=pWalker->LP;
          bus_case=pWalker->CASE;
          start_time=pWalker->TIME_A;
        }
      }
    }
    pWalker=pWalker->next;
  }
  if(min != MAX) return return_LP;
  else return "-1";
}