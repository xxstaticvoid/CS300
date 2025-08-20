//============================================================================
// Name        : SNHU CS-300 Project Two | Loading Data Structures 
// Author      : Joseph Ebersole
// Version     : 1.0
// Copyright   : Copyright � 2023 SNHU COCE
// Description : 16-AUG-2025    Program for CS300 Module 7
//============================================================================


#include <algorithm>
#include <iostream>
#include <time.h>	//for time()
#include <string>	
#include <vector>
#include <climits>

#include <fstream> //work with files


using namespace std;


const unsigned int DEFAULT_SIZE = 10;


struct Course {
    string courseNumber;
    string courseTitle;
    vector <string> prerequisites;

    Course(){}

    Course(const string &number, const string &title, const vector<string> &prereqs) {  //constructor
        courseNumber = number;
        courseTitle = title;
        prerequisites = prereqs;
    }

};


/*
 *  Define HashMap class for courses
 */
class HashMap {

private:
    //typical node structure
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node (Course c) : Node() {
            course = c;
        }

        Node(Course c, unsigned int aKey) : Node(c) {
            key = aKey;
        }

    };

    vector<Node> buckets;

    unsigned int tableSize = DEFAULT_SIZE;
    unsigned int hash(const string& key);

    vector<Course> getAllCourses(); //convert to vector

public:
    HashMap();
    HashMap(unsigned int size);
    virtual ~HashMap();
    void insertNode(const Course& c);
    void printAll();

    void printAllSorted();
    Course search(string courseNumber);
};


//class methods
HashMap::HashMap() {
    buckets.resize(DEFAULT_SIZE);
}

HashMap::HashMap(unsigned int newSize) {
    buckets.resize(newSize);
    tableSize = newSize;
}

HashMap::~HashMap() {
    buckets.clear();    //delete all head nodes from buckets
}

unsigned int HashMap::hash(const string& courseNumber) {
    int ASCIIsval = 0;
    for (char c : courseNumber) {
        ASCIIsval = 31 * ASCIIsval + c; //polyniomial rolling hash (credit chatGPT)
    }

    return ASCIIsval % tableSize;
}

void HashMap::insertNode(const Course& course) {

    unsigned int tempHashIndex = this->hash(course.courseNumber);

    //look at bucket
    Node& tempNode = buckets.at(tempHashIndex);

    //if no entry found for key
    if (tempNode.key == UINT_MAX) {
        //assign to position
        tempNode = Node(course, tempHashIndex);
    // else there is a collision
    } else {
        Node* currentNode = &tempNode;
        while (currentNode->next != nullptr) {
            currentNode = currentNode->next;
        }
        //point to new node in bucket
        currentNode->next = new Node(course, tempHashIndex);

    }

}


//print all courses in structure
void HashMap::printAll() {


    for (unsigned int i = 0; i < tableSize; i++) {
        Node* currentNode = &buckets.at(i);

        if (currentNode->key != UINT_MAX) {
            //print course info
            cout << currentNode->course.courseNumber << " [] " << currentNode->course.courseTitle << endl;


            //loop through linked list
            while (currentNode->next != nullptr) {
                currentNode = currentNode->next;
                //print course info
                cout << currentNode->course.courseNumber << " [] " << currentNode->course.courseTitle << endl;

            }

        }

    }

}


vector<Course> HashMap::getAllCourses() {
    vector<Course> courses;
    for (unsigned int i = 0; i < tableSize; i++) {
        Node* currentNode = &buckets.at(i);

        if (currentNode->key != UINT_MAX) {
            courses.push_back(currentNode->course);


            //loop through linked list
            while (currentNode->next != nullptr) {
                currentNode = currentNode->next;
                courses.push_back(currentNode->course);
            }
        }

    }
    return courses;
}

void HashMap::printAllSorted() {
    vector<Course> courses = this->getAllCourses();

    sort(courses.begin(), courses.end(),
        [](const Course& course1, const Course& course2) {  //lambda comparison
            return course1.courseNumber < course2.courseNumber;
        });

    for (const Course& course : courses) {  //output in order
        cout << course.courseNumber << " [] " << course.courseTitle << endl;
    }


}




Course HashMap::search(string courseNumber) {
    Course tempCourse;

    unsigned int tempKey = hash(courseNumber);  //recrete key

    Node* tempNode = &buckets.at(tempKey);  //search row
    while (tempNode != nullptr) {
        if (tempNode->course.courseNumber == courseNumber) {    //if found
            return tempNode->course;
        }

        tempNode = tempNode->next;
    }

    return tempCourse;  //return empty
}


void diplayMenu(int& choice) {  //update choice
    cout << endl << "Menu:" << endl;
    cout << "  1. Load Data Structure of Choice" << endl;
    cout << "  2. Print Course List" << endl;
    cout << "  3. Find Course Info" << endl;
    cout << "  4. Print Courses in Order" << endl; //much slower
    cout << "  9. Exit" << endl;
    cout << "Enter choice -> ";
    cin >> choice;
}

vector<string> parseCSVLine(const string& line) {
    vector<string> tokens;
    string token;

    for (char c : line) {
        if (c == ',') {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }

    }
    tokens.push_back(token);
    return tokens;

}


int loadDataFromFile(string& fileName, HashMap* hashMap) {

    ifstream file(fileName);  //read only permission

    if (!file.is_open()) {
        return -1;
    }

    string currentLine, title, number;
    vector<string> prereqs;

    Course tempCourse;

    while (getline(file, currentLine)) {
        vector<string> row = parseCSVLine(currentLine);

        //parse data
        number = row[0];
        title = row[1];
        if (row.size() > 2) {
            for (unsigned int i = 2; i < row.size(); ++i) {
                if (! row[i].empty()) {
                    prereqs.push_back(row[i]);
                }
            }

        }

        //change info
        tempCourse.courseNumber = number;
        tempCourse.courseTitle = title;
        tempCourse.prerequisites = prereqs;

        //load to structure
        hashMap->insertNode(tempCourse);

        //clear vector
        prereqs.clear();

    }


    file.close();
    return 0;
}

void displayCourseInfo(Course course) {
    cout << course.courseNumber << " | " << course.courseTitle << " | ";

    if ( !course.prerequisites.empty()) { //if prereqs not empty
        for (unsigned int i = 0; i < course.prerequisites.size(); i++) {
            cout << course.prerequisites.at(i);
            if (i != course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
    }
    cout << endl << endl;

}


//main program loop
int main() {

    bool dataIsLoaded = false;

    clock_t ticks;


    HashMap* hashmap = new HashMap();
    Course tempCourse;

    string file = "";
    cout << "What file do you want to read course info from? (.csv)   -> ";
    cin >> file;


    int choice = 0;
    string courseNumber = "";
    while(choice != 9) {
        diplayMenu(choice); //will update {choice}

        switch (choice) {
            case 1:
                if (loadDataFromFile(file, hashmap) == 0) {
                    cout << "success" << endl;
                    dataIsLoaded = true;
                } else {
                    cout << "failed" << endl;
                }

                break;
            case 2:
                if (dataIsLoaded) {
                    ticks = clock();
                    hashmap->printAll();

                    ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                    cout << "time: " << ticks << " clock ticks" << endl;
                    cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

                } else {
                    cout << "Please load data first" << endl;
                }
                break;
            case 3:
                if (dataIsLoaded) {
                    cout << "What course do you want to search -> ";
                    cin >> courseNumber;
                    ticks = clock();
                    Course course = hashmap->search(courseNumber);

                    if (course.courseNumber == courseNumber) {
                        displayCourseInfo(course);
                    } else {
                        cout << "Couldn't find " << courseNumber << endl;
                    }

                    ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                    cout << "time: " << ticks << " clock ticks" << endl;
                    cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << "9 seconds" << endl;
                } else {
                    cout << "Please load data first" << endl;
                }
                break;
            case 4:
                if (dataIsLoaded) {
                    ticks = clock();
                    hashmap->printAllSorted();
                    ticks = clock() - ticks; // current clock ticks minus starting clock ticks
                    cout << "time: " << ticks << " clock ticks" << endl;
                    cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << "9 seconds" << endl;
                } else {
                    cout << "Please load data first" << endl;
                }
                break;
            case 9:
                break;

            default:
                cout << choice << " is not a valid option." << endl << endl;
                break;
        }

    }



    cout << "Thank you for using the course planner! Good bye :(" << endl;
    delete hashmap;
    return 0;
}


