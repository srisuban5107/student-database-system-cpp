#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <limits>

using namespace std;

/* ---------- UTILITIES ---------- */
string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return toupper(c); });
    return s;
}

int safeIntInput(const string &msg, int minVal=-1, int maxVal=-1){
    string input;
    int x;
    while(true){
        cout << msg;
        getline(cin, input);
        try { x = stoi(input); }
        catch(...){ cout << "Invalid input. Numbers only.\n"; continue; }
        if((minVal!=-1 && x<minVal) || (maxVal!=-1 && x>maxVal)){ cout << "Number out of range.\n"; continue; }
        return x;
    }
}

string safeStringInput(const string &msg, bool allowEmpty=false){
    string input;
    while(true){
        cout << msg;
        getline(cin,input);
        if(input.empty() && !allowEmpty){ cout << "Input cannot be empty.\n"; continue; }
        return input;
    }
}

bool validPhone(const string &p){
    if(p.length()!=10) return false;
    for(char c:p) if(!isdigit(c)) return false;
    return true;
}

/* ---------- DOB VALIDATION ---------- */
bool parseDOB(const string &dob,int &d,int &m,int &y){
    char sep1,sep2;
    stringstream ss(dob);
    ss >> d >> sep1 >> m >> sep2 >> y;
    return (!ss.fail() && sep1=='-' && sep2=='-');
}

int calculateAge(int d,int m,int y){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int cy = 1900 + ltm->tm_year;
    int cm = 1 + ltm->tm_mon;
    int cd = ltm->tm_mday;
    int age = cy - y;
    if(cm < m || (cm==m && cd<d)) age--;
    return age;
}

void inputAgeAndDOB(int &age,string &dob){
    while(true){
        age = safeIntInput("Age: ");
        dob = safeStringInput("DOB (DD-MM-YYYY): ");
        int d,m,y;
        if(!parseDOB(dob,d,m,y)){ cout << "Invalid format. Use DD-MM-YYYY.\n"; continue; }
        if(calculateAge(d,m,y)==age) break;
        cout << "DOB does not match age. Re-enter.\n";
    }
}

/* ---------- DATA STRUCTURES ---------- */
struct Subject{
    string name;
    vector<string> parts;
};

struct Student{
    int id;
    string name;
    int std;
    string section;
    string group;
    string gender;
    int age;
    string dob;
    string phone;
    string email;
    string address;
    vector<Subject> subjects;
};

/* ---------- GLOBAL ---------- */
vector<Student> students;
const string FILE_NAME="student_database.txt";

/* ---------- UNIQUE CHECK ---------- */
bool isUniqueID(int id){
    for(auto &s: students) if(s.id==id) return false;
    return true;
}
bool isUniqueName(string name){
    name = toUpper(name);
    for(auto &s: students) if(s.name==name) return false;
    return true;
}

/* ---------- FILE SAVE ---------- */
void saveToFile(){
    ofstream file(FILE_NAME);

    for(auto &s : students){
        file << "===========\n";
        file << "ID: " << s.id << "\n";
        file << "NAME: " << s.name << "\n";
        file << "CLASS: " << s.std << "\n";
        file << "SECTION: " << s.section << "\n";
        file << "GROUP: " << s.group << "\n";
        file << "GENDER: " << s.gender << "\n";
        file << "AGE: " << s.age << "\n";
        file << "DOB: " << s.dob << "\n";
        file << "PHONE: " << s.phone << "\n";
        file << "EMAIL: " << s.email << "\n";
        file << "ADDRESS: " << s.address << "\n";

        file << "SUBJECTS:\n";
        for(auto &sub : s.subjects){
            file << "SUBJECT: " << sub.name << "\n";
            for(int i = 0; i < sub.parts.size(); i++){
                file << "PART " << i+1 << ": " << sub.parts[i] << "\n";
            }
        }

        file << "===========\n";  
    }
}


/* ---------- FILE LOAD ---------- */
void loadFromFile(){
    ifstream file(FILE_NAME);
    if(!file) return;

    students.clear();
    string line;
    Student s;
    bool inStudent = false;

    while(getline(file, line)){
        if(line == "==========="){
            if(inStudent){
                students.push_back(s);
                s = Student();
                inStudent = false;
            } else {
                s = Student();
                inStudent = true;
            }
            continue;
        }

        if(!inStudent) continue;

        if(line.rfind("ID:",0)==0) s.id = stoi(line.substr(4));
        else if(line.rfind("NAME:",0)==0) s.name = line.substr(6);
        else if(line.rfind("CLASS:",0)==0) s.std = stoi(line.substr(7));
        else if(line.rfind("SECTION:",0)==0) s.section = line.substr(9);
        else if(line.rfind("GROUP:",0)==0) s.group = line.substr(7);
        else if(line.rfind("GENDER:",0)==0) s.gender = line.substr(8);
        else if(line.rfind("AGE:",0)==0) s.age = stoi(line.substr(5));
        else if(line.rfind("DOB:",0)==0) s.dob = line.substr(5);
        else if(line.rfind("PHONE:",0)==0) s.phone = line.substr(7);
        else if(line.rfind("EMAIL:",0)==0) s.email = line.substr(7);
        else if(line.rfind("ADDRESS:",0)==0) s.address = line.substr(9);

        else if(line.rfind("SUBJECT:",0)==0){
            Subject sub;
            sub.name = line.substr(9);
            s.subjects.push_back(sub);
        }
        else if(line.rfind("PART",0)==0){
            size_t pos = line.find(":");
            if(pos != string::npos && !s.subjects.empty()){
                s.subjects.back().parts.push_back(line.substr(pos+2));
            }
        }
    }
}




/* ---------- SUBJECT INPUT ---------- */
void inputSubjects(vector<Subject>& subs){
    subs.clear();
    int n = safeIntInput("Number of subjects: ",0);
    for(int i=0;i<n;i++){
        Subject s;
        s.name = toUpper(safeStringInput("Subject " + to_string(i+1) + ": "));
        int ch;
        while(true){
            ch = safeIntInput("Has sub-parts? (0=No,1=Yes): ");
            if(ch==0||ch==1) break;
            cout << "Invalid choice\n";
        }
        if(ch==1){
            int p = safeIntInput("Number of sub-parts: ",1);
            for(int j=0;j<p;j++){
                string part = toUpper(safeStringInput("Sub-part " + to_string(j+1) + ": "));
                s.parts.push_back(part);
            }
        }
        subs.push_back(s);
    }
}

/* ---------- ADD STUDENT ---------- */
void addStudent(){
    Student s;
    do{
        s.id = safeIntInput("ID: ");
        if(!isUniqueID(s.id)) cout << "ID already exists\n";
    }while(!isUniqueID(s.id));

    do{
        s.name = safeStringInput("Name: ");
        s.name = toUpper(s.name);
        if(!isUniqueName(s.name)) cout << "Name already exists\n";
    }while(!isUniqueName(s.name));

    s.std = safeIntInput("Class (1-12): ",1,12);
    s.section = toUpper(safeStringInput("Section: "));

    if(s.std==11 || s.std==12) s.group = toUpper(safeStringInput("Group: "));
    else s.group="N/A";

    while(true){
        int g = safeIntInput("Gender (0=Male,1=Female): ");
        if(g==0){ s.gender="MALE"; break; }
        if(g==1){ s.gender="FEMALE"; break; }
        cout<<"Invalid choice\n";
    }

    inputAgeAndDOB(s.age,s.dob);

    while(true){
        s.phone = safeStringInput("Phone (10 digits): ");
        if(validPhone(s.phone)) break;
        cout<<"Phone number compulsory and must be 10 digits\n";
    }

    s.email = safeStringInput("Email (0=None): ",true);
    if(s.email=="0") s.email="NONE";

    while(true){
        s.address = toUpper(safeStringInput("Address: "));
        if(!s.address.empty()) break;
        cout<<"Address is compulsory\n";
    }

    inputSubjects(s.subjects);
    students.push_back(s);
    saveToFile();
    cout << "Student added successfully\n";
}

/* ---------- VIEW ---------- */
void viewStudents(){
    if(students.empty()){ cout<<"No student found\n"; return; }
    for(auto &s: students){
        cout << "===========\nID: " << s.id << "\nNAME: " << s.name
             << "\nCLASS: " << s.std << " SECTION: " << s.section
             << " GROUP: " << s.group << "\nGENDER: " << s.gender
             << "\nAGE: " << s.age << " DOB: " << s.dob
             << "\nPHONE: " << s.phone << " EMAIL: " << s.email
             << "\nADDRESS: " << s.address << "\nSUBJECTS:\n";
        for(auto &sub: s.subjects){
            cout << "- " << sub.name << "\n";
            for(auto &p: sub.parts) cout << "   * " << p << "\n";
        }
    }
}

/* ---------- SEARCH ---------- */
void searchStudent(){
    if(students.empty()){ cout<<"No student found\n"; return; }
    int id = safeIntInput("Enter ID: ");
    for(auto &s: students) if(s.id==id){ 
        cout<<"Found: "<<s.name<<" Class "<<s.std<<" Section "<<s.section<<"\n"; return; 
    }
    cout<<"Student not found\n";
}

/* ---------- UPDATE ---------- */
void updateStudent(){
    if(students.empty()){
        cout << "No student found\n";
        return;
    }

    int id = safeIntInput("Enter ID to update: ");
    for(auto &s : students){
        if(s.id == id){
            int ch;
            do {
                cout << "\nUpdate Menu for ID: " << s.id << " (" << s.name << ")\n";
                cout << "1. Name\n2. Class & Section\n3. Phone\n4. Address\n5. Subjects\n6. Exit update\n";
                ch = safeIntInput("Choice: ");

                switch(ch){
                    case 1: {
                        while(true){
                            string newName = safeStringInput("New Name: ");
                            newName = toUpper(newName);

                            bool unique = true;
                            for(auto &o : students){
                                if(o.id != s.id && o.name == newName){
                                    unique = false;
                                    break;
                                }
                            }
                            if(!unique){
                                cout << "Name already exists\n";
                                continue;
                            }
                            s.name = newName;
                            cout << "Name updated\n";
                            break;
                        }
                        break;
                    }

                    case 2:
                        s.std = safeIntInput("Class (1-12): ",1,12);
                        s.section = toUpper(safeStringInput("Section: "));
                        if(s.std==11 || s.std==12)
                            s.group = toUpper(safeStringInput("Group: "));
                        else
                            s.group = "N/A";
                        cout << "Class & Section updated\n";
                        break;

                    case 3:
                        while(true){
                            s.phone = safeStringInput("Phone (10 digits): ");
                            if(validPhone(s.phone)) break;
                            cout << "Invalid phone\n";
                        }
                        cout << "Phone updated\n";
                        break;

                    case 4:
                        s.address = toUpper(safeStringInput("Address: "));
                        cout << " Address updated\n";
                        break;

                    case 5:
                        inputSubjects(s.subjects);
                        cout << "Subjects updated\n";
                        break;

                    case 6:
                        cout << "Exiting update...\n";
                        break;

                    default:
                        cout << "Invalid choice\n";
                }

            } while(ch != 6);

            saveToFile();   
            cout << "Changes saved to file\n";
            return;
        }
    }
    cout << "Student ID not found\n";
}



/* ---------- DELETE ---------- */
void deleteStudent(){
    if(students.empty()){ cout<<"No student found\n"; return; }
    int id = safeIntInput("Enter ID to delete: ");
    for(int i=0;i<students.size();i++){
        if(students[i].id==id){ students.erase(students.begin()+i); saveToFile(); cout<<"Deleted successfully\n"; return; }
    }
    cout<<"Student not found\n";
}

/* ---------- MAIN ---------- */
int main(){
    loadFromFile();
    while(true){
        cout<<"\n==============================\nSTUDENT DATABASE SYSTEM\n==============================\n";
        cout<<"1. Add Student\n2. View All Students\n3. Search Student\n4. Update Student\n5. Delete Student\n6. Exit\n";
        int choice = safeIntInput("Enter choice: ");
        switch(choice){
            case 1: addStudent(); break;
            case 2: viewStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: cout<<"Exiting...\n"; return 0;
            default: cout<<"Invalid choice\n";
        }
    }
}
