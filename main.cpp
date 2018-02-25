// Robert Hayden Anderson
//CS440 Assignment 6
//Join Algorithm by Thomas Noelcke
//Because the join algorithm was created by Thomas Noelcke
//I will source the top of each function/struct that he made for his join
//Each that I wrote or changed for my purposes, I will mark as mine.


#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<fstream>
#include<stdlib.h>
#include<algorithm>
#include <sys/stat.h>
#include <unistd.h>
#define TEST_CRASH true
#define STABILITY 35


//source: Brian Wiltse https://piazza.com/class/jc67dl3u3yr4u?cid=119
void crash_test() {
	if (TEST_CRASH && rand() % STABILITY == 0) {
		abort();
	}
}

using namespace std;
const int STRING_LEN = 40;
 
//source: Thomas Noelcke
//this struct represents an employee in our employee relation.
typedef struct employee
{
    long int eid;
    char ename[STRING_LEN];
    long int age;
    double salary;
    
} employee;
//source: Thomas Noelcke
//this struct represents a department in our department relation.
typedef struct department 
{
	long int managerId;
    long int did;
    char dname[STRING_LEN];
    double budget;
   
} department;
//source: Thomas Noelcke
typedef struct empDepartment 
{
    long int did;
    long int eid;
    char dname[STRING_LEN];
    char ename[STRING_LEN];
    double budget;
    double salary;
	long int age;
    long int managerId;    
} empDepartment;

//Mine
typedef struct logRecord{
    long int dept_position;
    long int emp_position;
    long int join_position;
    int write;


}logRecord;






//Function Headers
//Source: Thomas Noelcke
employee* getEmpTouple(FILE* file, logRecord* log);
department* getDeptTouple(FILE* file, logRecord* log);
empDepartment* copy(department* dep, employee* emp);
FILE* openFile(const char* fname,const char* args);
void displayEmp(employee* emp);
void displayDep(department* dep);
void writeEmpDepartment(FILE* file, empDepartment* join, logRecord* log);
void freeEmp(employee* emp);
void freeDep(department* dep);

//Source originally was Thomas Noelcke,I edited to add recovery
void join(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile);


//Source Mine
void writeToLog(FILE* LogFile, logRecord* tuple);
void set_fp(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile, logRecord* record);

//source: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool FileExists (const string& name);



//Constants
const int NUM_BLOCKS = 22;
const char EMP_FNAME[] = "Emp.csv";
const char DEPT_FNAME[] = "Dept.csv";
const char OUTPUT_FNAME[] = "join.csv";
const char LOG_FNAME[] = "joinLog";

// Main function
int main(){
    FILE* depFile = openFile(DEPT_FNAME, "r");
    FILE* empFile = openFile(EMP_FNAME, "r");
    FILE* outFile;
    FILE* logFile;
    logRecord* temp_record =NULL;
    temp_record = new logRecord;

    if (FileExists(LOG_FNAME)){
        logFile = openFile(LOG_FNAME, "a+");
        outFile = openFile(OUTPUT_FNAME, "a+");
        set_fp(outFile, depFile, empFile, logFile, temp_record);
    }else{
        logFile = openFile(LOG_FNAME, "a+");
        outFile = openFile(OUTPUT_FNAME, "w+");
    }
    join(outFile, depFile, empFile, logFile);

    fclose(depFile);
    fclose(empFile);
    fclose(outFile);
    fclose(logFile);
    //if finished program remove existing joinLog
    remove(LOG_FNAME);
    exit(0);
}


//Source:Mine
void writeToLog(FILE* LogFile, logRecord* tuple){
    char write_or_read[6];
    if (tuple->write == 0){strcpy(write_or_read, "d_read");}else if(tuple->write == -1){strcpy(write_or_read, "e_read");}else{ strcpy(write_or_read, "write");}
    fprintf(LogFile, "%ld,%ld,%ld,%s\n", tuple->dept_position, tuple->emp_position, tuple->join_position, write_or_read);
    fflush(LogFile);

}
//Source:Mine
void set_fp(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile, logRecord* record){
    //look in logfile
    //put fp at end SEEKEND
    fseek (logFile, 0, SEEK_END);
    int nl=0;
    int converter_int;
    int i=ftell(logFile);
    char temp;
    
    while (nl < 2){
        converter_int = fgetc(logFile);
        temp = converter_int;
        if( temp == '\n'){nl++;}
        i--;
        fseek (logFile, i, SEEK_SET);
    }
    //backup until second /n
    //move up 1
    fseek (logFile, (i + 2), SEEK_SET);
    //set_fps to 
    fscanf(logFile, "%ld,%ld,%ld,%d\n", &record->dept_position, &record->emp_position, &record->join_position, &record->write);
    fseek(depFile, record->dept_position, SEEK_SET);
    fseek(empFile, record->emp_position, SEEK_SET);
    fseek(output, record->join_position, SEEK_SET);

}





//Source: originally was Thomas Noelcke,I edited to add recovery
//this function preforms the join by reading in one tuple from each file
//and attempting the join. IN the future this function will also be responsible for
//preforming actions to log all steps.
void join(FILE* output, FILE* depFile, FILE* empFile, FILE* logFile){
    //set up variables to hold read in values
    employee* emp;
    department* dep;
    logRecord* record =NULL;
    record = new logRecord;
     
    //this value will get set to true if we hit EOF on either file.
    emp = getEmpTouple(empFile, record);
    writeToLog(logFile, record);
    dep = getDeptTouple(depFile, record);
    writeToLog(logFile, record);
    crash_test();
    bool isDone = emp == NULL || dep == NULL;
    do {
        displayDep(dep);
        displayEmp(emp);
        if(emp != NULL && dep != NULL){
            //if the ID's match join the two and out put them
            if(emp->eid == dep->managerId){
                cout << "FOUND A MATCH\n";
                empDepartment* toOutput = copy(dep, emp);            
                writeEmpDepartment(output, toOutput, record);
                writeToLog(logFile,record);
                delete toOutput;
                delete dep;
                dep = getDeptTouple(depFile, record);
                writeToLog(logFile,record);
                crash_test();
            //if emp is > dep discard of dep and read in a new emp
            } else if(emp->eid > dep->managerId){
                cout << "EMP > DEP \n";
                delete dep;
                dep = getDeptTouple(depFile, record);
                writeToLog(logFile,record);
                crash_test();
             //if dep > emp discard emp and read in a new emp.
            } else {
                cout << "DEP > EMP\n";
                delete emp;
                emp = getEmpTouple(empFile, record);
                writeToLog(logFile,record);
                crash_test();
            }
        }
        //if either emp or dep are set to null we have hit EOF
        isDone = emp == NULL || dep == NULL;
    } while(!isDone);
}





//Source: originally was Thomas Noelcke,I edited to add recovery
//this function takes a file pointer and a merged empDepartment pointer
//and writes this out to the file.
void writeEmpDepartment(FILE* file, empDepartment* join, logRecord* record)
{
    record->write = 1;
    record->join_position = ftell(file);
	fprintf(file,"\"%ld\",\"%ld\",\"%s\",\"%s\",\"%.2lf\",\"%.2lf\",\%li\",\"%li\"\n", join->did, join->eid, join->dname, join->ename, join->budget, join->salary, join->managerId, join->age);	
    fflush(file);
}
//Source: originally was Thomas Noelcke,I edited to add recovery
//this function takes a file pointer and returns a pointer to an employee object.
employee* getEmpTouple(FILE* file, logRecord* record)
{
	employee* emp = NULL;
	if(!feof(file)){
		emp = new employee;
        record->write = -1;
        record->emp_position = ftell(file);
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%ld\",\"%la\"\n", &emp->eid, emp->ename, &emp->age, &emp->salary);
	} else {
		emp = NULL;
	}
	return emp;
}
//Source: originally was Thomas Noelcke,I edited to add recovery
//this function takes a file point and returns a pointer to a department object.
department* getDeptTouple(FILE* file, logRecord* record)
{	
	department* dep = NULL;
	if(!feof(file)){
		dep = new department;
        record->write = 0;
        record->dept_position = ftell(file);
		fscanf(file, "\"%ld\",\"%[^\"]\",\"%la\",\"%ld\"\n", &dep->did, dep->dname, &dep->budget, &dep->managerId);
	}
    return dep;
}













//Source: Thomas Noelcke
//this function takes a file name and a string of args
//opens up the file and stops the program if the file
//doesn't open. Takes either r or w for args.
FILE* openFile(const char* fname,const char* args){
	FILE* fp = fopen(fname, args);
	if(!fp){
		perror("Error opening file");
		exit(1);
	}
}
//Source: Thomas Noelcke
//frees the memory for a single employee
void freeEmp(employee* emp){
  if(emp != NULL){
    delete emp;
    emp = NULL;
  }
}
//Source: Thomas Noelcke
//frees the memeory for a single department.
void freeDep(department* dep){
  if(dep != NULL){
    delete dep;
    dep = NULL;
  }
}
//Source: Thomas Noelcke
//this function takes a department pointer and an employee pointer
//and creates a new empDepartment by doing a deep copy of dep and emp.
empDepartment* copy(department* dep, employee* emp)
{   
    //checks if the function arguments are null.
    if(!dep || !emp){
        return NULL;
    }
    //copy the data over to the new struct.
    empDepartment* copy = new empDepartment;
    copy->did = dep->did;
    copy->eid = emp->eid;
    strcpy(copy->dname, dep->dname);
    strcpy(copy->ename, emp->ename);
    copy->budget = dep->budget;
    copy->salary = emp->salary;
    copy->managerId = dep->managerId;
	copy->age = emp-> age;

    return copy;
}
//Source: Thomas Noelcke
//Takes a department pointer and displays that department to standard out.
void displayDep(department* dep){
	if(dep){
		cout<<"did: "<<dep->did<<" name: " << dep->dname << " budget: " << dep->budget << " Managerid: " << dep->managerId << "\n";
	} else {
		cout << "No Department to display\n";
	}
}
//Source: Thomas Noelcke
//Takes an employee pointer and displays the employee to stdout.
void displayEmp(employee* emp){
	if(emp){
		cout<<"eid: "<< emp->eid <<" name: " << emp->ename << " age: " << emp->age << " salary: " << emp->salary << "\n";
	} else{
		cout << "No Employee to display\n";
	}
}
//check if file exists
//source: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
inline bool FileExists (const string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}