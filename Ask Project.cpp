
#include <bits/stdc++.h>

using namespace std;

struct User {
    int id;
    string name;
    string email;
    string password;
    string username;
    bool allow_anonymous;

    string to_string() {
        return std::to_string(id) + "," + name + "," + email + "," + password + "," + username + "," + (allow_anonymous ? "1" : "0");
    }

    static User from_string(const string& str) {
        User user;
        stringstream ss(str);
        string token;
        getline(ss, token, ','); user.id = stoi(token);
        getline(ss, user.name, ',');
        getline(ss, user.email, ',');
        getline(ss, user.password, ',');
        getline(ss, user.username, ',');
        getline(ss, token, ','); user.allow_anonymous = (token == "1");
        return user;
    }
};

// Question structure
struct Question {
    int id;
    int from_user_id;
    int to_user_id;
    string text;
    int parent_question_id;

    string to_string() {
        return std::to_string(id) + "," + std::to_string(from_user_id) + "," + std::to_string(to_user_id) + "," + text + "," + std::to_string(parent_question_id);
    }

    static Question from_string(const string& str) {
        Question q;
        stringstream ss(str);
        string token;
        getline(ss, token, ','); q.id = stoi(token);
        getline(ss, token, ','); q.from_user_id = stoi(token);
        getline(ss, token, ','); q.to_user_id = stoi(token);
        getline(ss, q.text, ',');
        getline(ss, token, ','); q.parent_question_id = stoi(token);
        return q;
    }
};

// Global variables
vector<User> users;
vector<Question> questions;
int current_user_id = -1;

// Function declarations
void load_data();
void save_data();
void sign_up();
void login();
void logout();
void ask_question();
void answer_question();
void print_questions();
void delete_question();
int generate_id(const vector<int>& existing_ids);
bool user_exists(int user_id);
bool question_exists(int question_id);
void load_users(vector<User>& users);
void load_questions(vector<Question>& questions);
void save_users(const vector<User>& users);
void save_questions(const vector<Question>& questions);

int main() {
    load_data();
    while (true) {
        int choice;
        cout << "1. Sign Up\n2. Login\n3. Logout\n4. Ask Question\n5. Answer Question\n6. Print Questions\n7. Delete Question\n8. Exit\n";
        cin >> choice;
        switch (choice) {
            case 1: sign_up(); break;
            case 2: login(); break;
            case 3: logout(); break;
            case 4: ask_question(); break;
            case 5: answer_question(); break;
            case 6: print_questions(); break;
            case 7: delete_question(); break;
            case 8: save_data(); return 0;
            default: cout << "Invalid choice!\n"; break;
        }
    }
    return 0;
}

void load_data() {
    load_users(users);
    load_questions(questions);
}

void save_data() {
    save_users(users);
    save_questions(questions);
}

void load_users(vector<User>& users) {
    ifstream user_file("users.txt");
    if (!user_file) {
        cerr << "Error opening users.txt\n";
        return;
    }
    string line;
    while (getline(user_file, line)) {
        users.push_back(User::from_string(line));
    }
    user_file.close();
}

void load_questions(vector<Question>& questions) {
    ifstream question_file("questions.txt");
    if (!question_file) {
        cerr << "Error opening questions.txt\n";
        return;
    }
    string line;
    while (getline(question_file, line)) {
        questions.push_back(Question::from_string(line));
    }
    question_file.close();
}

void save_users(const vector<User>& users) {
    ofstream user_file("users.txt");
    if (!user_file) {
        cerr << "Error opening users.txt\n";
        return;
    }

    user_file.close();
}

void save_questions(const vector<Question>& questions) {
    ofstream question_file("questions.txt");
    if (!question_file) {
        cerr << "Error opening questions.txt\n";
        return;
    }

    question_file.close();
}

void sign_up() {
    User user;
    user.id = generate_id({});
    cout << "Enter name: ";
    cin.ignore();
    getline(cin, user.name);
    cout << "Enter email: ";
    getline(cin, user.email);
    cout << "Enter password: ";
    getline(cin, user.password);
    cout << "Enter username: ";
    getline(cin, user.username);
    cout << "Allow anonymous questions? (1 for yes, 0 for no): ";
    int anon_choice;
    cin >> anon_choice;
    user.allow_anonymous = (anon_choice == 1);
    users.push_back(user);
    cout << "Sign up successful!\n";
}

void login() {
    string username, password;
    cout << "Enter username: ";
    cin.ignore();
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);
    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            current_user_id = user.id;
            cout << "Login successful!\n";
            return;
        }
    }
    cout << "Invalid username or password!\n";
}

void logout() {
    if (current_user_id == -1) {
        cout << "You are not logged in!\n";
    } else {
        current_user_id = -1;
        cout << "Logout successful!\n";
    }
}

void ask_question() {
    if (current_user_id == -1) {
        cout << "You need to login first!\n";
        return;
    }
    Question question;
    question.id = generate_id({});
    question.from_user_id = current_user_id;


    cout << "Enter question text: ";
    cin.ignore();
    getline(cin, question.text);
    question.parent_question_id = -1;
    questions.push_back(question);
    cout << "Question asked successfully!\n";
}

void answer_question() {
    if (current_user_id == -1) {
        cout << "You need to login first!\n";
        return;
    }
    int parent_id;
    cout << "Enter question id to answer: ";
    cin >> parent_id;
    if (!question_exists(parent_id)) {
        cout << "Question ID not found!\n";
        return;
    }
    Question answer;
    answer.id = generate_id({});
    answer.from_user_id = current_user_id;
    answer.to_user_id = questions[parent_id].from_user_id;
    cout << "Enter answer text: ";
    cin.ignore();
    getline(cin, answer.text);
    answer.parent_question_id = parent_id;
    questions.push_back(answer);
    cout << "Answer added successfully!\n";
}

void print_questions() {
    if (current_user_id == -1) {
        cout << "You need to login first!\n";
        return;
    }
    cout << "Questions from you:\n";
    for (const auto& q : questions) {
        if (q.from_user_id == current_user_id && q.parent_question_id == -1) {
            cout << q.id << ": " << q.text << endl;
        }
    }

    cout << "Questions to you:\n";
    for (const auto& q : questions) {
        if (q.to_user_id == current_user_id && q.parent_question_id == -1) {
            cout << q.id << ": " << q.text << endl;
        }
    }

    cout << "Feed Questions:\n";
    for (const auto& q : questions) {
        if (q.parent_question_id == -1) {
            cout << q.id << ": " << q.text << endl;
        }
    }
}

void delete_question() {
    if (current_user_id == -1) {
        cout << "You need to login first!\n";
        return;
    }
    int id;
    cout << "Enter question id to delete: ";
    cin >> id;
    auto it = find_if(questions.begin(), questions.end(), [id](const Question& q) {
        return q.id == id && q.from_user_id == current_user_id;
    });
    if (it != questions.end()) {
        questions.erase(it);
        cout << "Question deleted successfully!\n";
    } else {
        cout << "Invalid question id or you don't have permission to delete this question!\n";
    }
}

int generate_id(const vector<int>& existing_ids) {
    static int id = 0;
    while (find(existing_ids.begin(), existing_ids.end(), id) != existing_ids.end()) {
        ++id;
    }
    return id++;
}

bool user_exists(int user_id) {
    return find_if(users.begin(), users.end(), [user_id](const User& user) {
        return user.id == user_id;
    }) != users.end();
}

bool question_exists(int question_id) {
    return find_if(questions.begin(), questions.end(), [question_id](const Question& q) {
        return q.id == question_id;
    }) != questions.end();
}
