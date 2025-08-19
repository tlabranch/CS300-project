#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*
   CS 300 Project Two — Advising Assistance Program (Student Edition)
   - Simple single-file C++ program
   - Binary Search Tree (BST) keyed by course number (exact match, case-sensitive)
   - Menu:
       1. Load Data Structure
       2. Print Course List
       3. Print Course
       9. Exit
*/

struct Course {
    string number;               // e.g., CSCI200
    string title;                // e.g., Data Structures
    vector<string> prerequisites; // e.g., CSCI101,MATH201
};

struct Node {
    Course course;
    Node* left;
    Node* right;
    Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

class CourseBST {
public:
    CourseBST() : root(nullptr) {}
    ~CourseBST() { freeTree(root); }

    void Insert(const Course& c) {
        if (root == nullptr) {
            root = new Node(c);
        } else {
            insert(root, c);
        }
    }

    Node* Search(const string& number) const {
        return search(root, number);
    }

    void PrintAll() const {
        if (root == nullptr) {
            cout << "No courses loaded." << endl;
            return;
        }
        inOrder(root);
    }

    bool Empty() const { return root == nullptr; }

private:
    Node* root;

    static void freeTree(Node* n) {
        if (n == nullptr) return;
        freeTree(n->left);
        freeTree(n->right);
        delete n;
    }

    static void insert(Node* node, const Course& c) {
        if (c.number < node->course.number) {
            if (node->left == nullptr) node->left = new Node(c);
            else insert(node->left, c);
        } else if (c.number > node->course.number) {
            if (node->right == nullptr) node->right = new Node(c);
            else insert(node->right, c);
        } else {
            // duplicate course number: do nothing
        }
    }

    static Node* search(Node* node, const string& key) {
        if (node == nullptr) return nullptr;
        if (key == node->course.number) return node;
        if (key < node->course.number) return search(node->left, key);
        return search(node->right, key);
    }

    static void inOrder(Node* node) {
        if (node == nullptr) return;
        inOrder(node->left);
        cout << node->course.number << ", " << node->course.title << endl;
        inOrder(node->right);
    }
};

// Basic trim for leading/trailing spaces (kept simple)
static string trim(const string& s) {
    size_t start = 0, end = s.size();
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) ++start;
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

// Load CSV: "courseNumber,courseTitle[,prereq1,prereq2,...]"
bool LoadCourses(const string& filename, CourseBST& bst) {
    ifstream in(filename);
    if (!in.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return false;
    }

    string line;
    int lineNum = 0;
    while (getline(in, line)) {
        ++lineNum;
        if (line.empty()) continue;

        stringstream ss(line);
        vector<string> tokens;
        string tok;
        while (getline(ss, tok, ',')) {
            tokens.push_back(trim(tok));
        }

        if (tokens.size() < 2) {
            cout << "Format error on line " << lineNum << endl;
            continue;
        }

        Course c;
        c.number = tokens[0]; // exact, case-sensitive
        c.title  = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) c.prerequisites.push_back(tokens[i]);
        }

        bst.Insert(c);
    }

    cout << "Load complete." << endl;
    return true;
}

void PrintSingleCourse(const CourseBST& bst, const string& number) {
    Node* n = bst.Search(number);
    if (n == nullptr) {
        cout << "Course \"" << number << "\" not found." << endl;
        return;
    }
    cout << n->course.number << ", " << n->course.title << endl;

    if (n->course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < n->course.prerequisites.size(); ++i) {
        const string& pnum = n->course.prerequisites[i];
        Node* pn = bst.Search(pnum);
        if (pn) {
            cout << pn->course.number << " (" << pn->course.title << ")";
        } else {
            cout << pnum;
        }
        if (i + 1 < n->course.prerequisites.size()) cout << ", ";
    }
    cout << endl;
}

void PrintMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    cout << "Welcome to the course planner." << endl;

    CourseBST bst;
    bool loaded = false;

    while (true) {
        PrintMenu();
        string choice;
        if (!getline(cin, choice)) break;

        if (choice == "1") {
            cout << "Enter filename: ";
            string fname;
            if (!getline(cin, fname)) break;
            // Reset tree by creating a new one (simplest approach)
            bst = CourseBST();
            loaded = LoadCourses(fname, bst);
            cout << endl;
        } else if (choice == "2") {
            if (!loaded) {
                cout << "Please load the data structure first (option 1)." << endl << endl;
                continue;
            }
            cout << "Here is a sample schedule:" << endl;
            bst.PrintAll();
            cout << endl;
        } else if (choice == "3") {
            if (!loaded) {
                cout << "Please load the data structure first (option 1)." << endl << endl;
                continue;
            }
            cout << "What course do you want to know about? ";
            string num;
            if (!getline(cin, num)) break;
            num = trim(num); // minor cleanup; no case normalization
            PrintSingleCourse(bst, num);
            cout << endl;
        } else if (choice == "9") {
            cout << "Thank you for using the course planner!" << endl;
            break;
        } else if (!choice.empty()) {
            cout << choice << " is not a valid option." << endl << endl;
        } else {
            cout << endl;
        }
    }
    return 0;
}
