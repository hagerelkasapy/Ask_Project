
#include <bits/stdc++.h>

using namespace std;

vector<string> ReadFileLines(string path) {
	vector<string> lines;
	fstream file_handler(path.c_str());

	if (file_handler.fail()) {
		cout << "\n\nERROR: Can't open the file\n\n";
		return lines;
	}
	string line;

	while (getline(file_handler, line)) {
		if (line.size() == 0)
			continue;
		lines.push_back(line);
	}

	file_handler.close();
	return lines;
}

void WriteFileLines(string path, vector<string> lines, bool append = true) {
	auto status = ios::in | ios::out | ios::app;

	if (!append)
		status = ios::in | ios::out | ios::trunc;

	fstream file_handler(path.c_str(), status);

	if (file_handler.fail()) {
		cout << "\n\nERROR: Can't open the file\n\n";
		return;
	}
	for (auto line : lines)
		file_handler << line << "\n";

	file_handler.close();
}

vector<string> SplitString(string s, string delimiter = ",") {
	vector<string> strs;
	int pos = 0;
	string substr;

	while ((pos = (int) s.find(delimiter)) != -1) {
		substr = s.substr(0, pos);
		strs.push_back(substr);
		s.erase(0, pos + delimiter.length());
	}
	strs.push_back(s);
	return strs;
}

int ToInt(string str) {
	istringstream iss(str);
	int num;
	iss >> num;
	return num;
}

int ReadInt(int low, int high) {
	cout << "\nEnter number in range " << low << " - " << high << ": ";
	int value;
	cin >> value;

	if (low <= value && value <= high)
		return value;

	cout << "ERROR: invalid number...Try again\n";
	return ReadInt(low, high);
}

int ShowReadMenu(vector<string> choices) {
	cout << "\nMenu:\n";
	for (int ch = 0; ch < (int) choices.size(); ++ch) {
		cout << "\t" << ch + 1 << ": " << choices[ch] << "\n";
	}
	return ReadInt(1, choices.size());
}

struct Question {
	int question_id;
	int parent_question_id;
	int from_user_id;
	int to_user_id;
	int is_anonymous_questions;	// 0 or 1
	string question_text;
	string answer_text;

	Question() {
		question_id = parent_question_id = from_user_id = to_user_id = -1;
		is_anonymous_questions = 1;
	}

	Question(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 7);

		question_id = ToInt(substrs[0]);
		parent_question_id = ToInt(substrs[1]);
		from_user_id = ToInt(substrs[2]);
		to_user_id = ToInt(substrs[3]);
		is_anonymous_questions = ToInt(substrs[4]);
		question_text = substrs[5];
		answer_text = substrs[6];
	}

	string ToString() {
		ostringstream oss;
		oss << question_id << "," << parent_question_id << "," << from_user_id << "," << to_user_id << "," << is_anonymous_questions << "," << question_text << "," << answer_text;
		return oss.str();
	}

	void PrintToQuestion() {
		string prefix = "";

		if (parent_question_id != -1)
			prefix = "\tThread: ";

		cout << prefix << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";
		cout << "\t Question: " << question_text << "\n";

		if (answer_text != "")
			cout << prefix << "\tAnswer: " << answer_text << "\n";
		cout << "\n";
	}

	void PrintFromQuestion() {
		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " !AQ";

		cout << " to user id(" << to_user_id << ")";
		cout << "\t Question: " << question_text;

		if (answer_text != "")
			cout << "\tAnswer: " << answer_text << "\n";
		else
			cout << "\tNOT Answered YET\n";
	}

	void PrintFeedQuestion() {
		if (parent_question_id != -1)
			cout << "Thread Parent Question ID (" << parent_question_id << ") ";

		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";

		cout << " To user id(" << to_user_id << ")";
		cout << "\t Question: " << question_text << "\n";
		if (answer_text != "")
			cout << "\tAnswer: " << answer_text << "\n";
	}
};

struct User {
	int user_id;
	string user_name;
	string password;
	string name;
	string email;
	int allow_anonymous_questions;	// 0 or 1

	vector<int> questions_id_from_me;
	map<int, vector<int>> questionid_questionidsThead_to_map;

	User() {
		user_id = allow_anonymous_questions = -1;
	}

	User(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 6);

		user_id = ToInt(substrs[0]);
		user_name = substrs[1];
		password = substrs[2];
		name = substrs[3];
		email = substrs[4];
		allow_anonymous_questions = ToInt(substrs[5]);
	}

	string ToString() {
		ostringstream oss;
		oss << user_id << "," << user_name << "," << password << "," << name << "," << email << "," << allow_anonymous_questions;
		return oss.str();
	}

	void Print() {
		cout << "User " << user_id << ", " << user_name << " " << password << ", " << name << ", " << email << "\n";
	}
};

struct QuestionsManager {
	map<int, vector<int>> questionid_questionidsThead_to_map;
	map<int, Question> questionid_questionobject_map;
	int last_id;

	QuestionsManager() {
		last_id = 0;
	}

	void LoadDatabase() {
		last_id = 0;
		questionid_questionidsThead_to_map.clear();
		questionid_questionobject_map.clear();

		vector<string> lines = ReadFileLines("questions.txt");
		for (auto &line : lines) {
			Question question(line);
			last_id = max(last_id, question.question_id);

			questionid_questionobject_map[question.question_id] = question;

			if (question.parent_question_id == -1)
				questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
			else
				questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
		}
	}

	void FillUserQuestions(User &user) {
		user.questions_id_from_me.clear();
		user.questionid_questionidsThead_to_map.clear();

		for (auto &pair : questionid_questionidsThead_to_map) {
			for (auto &question_id : pair.second) {
				Question &question = questionid_questionobject_map[question_id];

				if (question.from_user_id == user.user_id)
					user.questions_id_from_me.push_back(question.question_id);

				if (question.to_user_id == user.user_id) {
					if (question.parent_question_id == -1)
						user.questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
					else
						user.questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
				}
			}
		}
	}

	void PrintUserToQuestions(User &user) {
		cout << "\n";

		if (user.questionid_questionidsThead_to_map.size() == 0)
			cout << "No Questions";

		for (auto &pair : user.questionid_questionidsThead_to_map) {
			for (auto &question_id : pair.second) {
				Question &question = questionid_questionobject_map[question_id];
				question.PrintToQuestion();
			}
		}
		cout << "\n";
	}

	void PrintUserFromQuestions(User &user) {
		cout << "\n";
		if (user.questions_id_from_me.size() == 0)
			cout << "No Questions";

		for (auto &question_id : user.questions_id_from_me) {
			Question &question = questionid_questionobject_map[question_id];
			question.PrintFromQuestion();
		}
		cout << "\n";
	}

	int ReadQuestionIdAny(User &user) {
		int question_id;
		cout << "Enter Question id or -1 to cancel: ";
		cin >> question_id;

		if (question_id == -1)
			return -1;

		if (!questionid_questionobject_map.count(question_id)) {
			cout << "\nERROR: No question with such ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		Question &question = questionid_questionobject_map[question_id];

		if (question.to_user_id != user.user_id) {
			cout << "\nERROR: Invalid question ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		return question_id;
	}

	void AnswerQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);
		if (question_id == -1)
			return;

		Question &question = questionid_questionobject_map[question_id];

		cout << "Enter answer: ";
		cin.ignore();
		getline(cin, question.answer_text);

		UpdateDatabase();
	}

	void DeleteQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);
		if (question_id == -1)
			return;

		Question &question = questionid_questionobject_map[question_id];

		questionid_questionobject_map.erase(question_id);

		vector<int> &vec = questionid_questionidsThead_to_map[question.parent_question_id];
		for (int i = 0; i < (int) vec.size(); ++i) {
			if (vec[i] == question_id) {
				vec.erase(vec.begin() + i);
				break;
			}
		}
		UpdateDatabase();
	}

	void UpdateDatabase() {
		vector<string> lines;
		for (auto &pair : questionid_questionobject_map)
			lines.push_back(pair.second.ToString());

		WriteFileLines("questions.txt", lines, false);
	}

	void AskQuestion(User &user, bool is_anonymous) {
		Question question;
		question.parent_question_id = -1;
		question.question_id = ++last_id;
		question.from_user_id = user.user_id;

		cout << "Enter User id to ask question to: ";
		cin >> question.to_user_id;

		if (!is_anonymous)
			question.is_anonymous_questions = 0;

		cout << "Enter question text: ";
		cin.ignore();
		getline(cin, question.question_text);

		questionid_questionobject_map[question.question_id] = question;
		questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
		UpdateDatabase();
	}

	void ListFeed() {
		for (auto &pair : questionid_questionobject_map) {
			Question &question = pair.second;
			if (question.answer_text != "")
				question.PrintFeedQuestion();
		}
	}

	void AskQuestionThread(User &user, bool is_anonymous) {
		int question_id = ReadQuestionIdAny(user);
		if (question_id == -1)
			return;
		Question &parent = questionid_questionobject_map[question_id];

		Question question;
		question.question_id = ++last_id;
		question.parent_question_id = parent.question_id;
		question.from_user_id = user.user_id;
		question.to_user_id = parent.to_user_id;

		if (!is_anonymous)
			question.is_anonymous_questions = 0;

		cout << "Enter question text: ";
		cin.ignore();
		getline(cin, question.question_text);

		questionid_questionobject_map[question.question_id] = question;
		questionid_questionidsThead_to_map[parent.question_id].push_back(question.question_id);
		UpdateDatabase();
	}
};

struct UsersManager {
	map<string, User> userame_userobject_map;
	map<int, User> userid_userobject_map;
	int last_id;
	User current_user;
	QuestionsManager questions_manager;

	UsersManager() {
		last_id = 0;
	}

	void AccessSystem() {
		LoadDatabase();
		int choice = ShowReadMenu({ "Login", "Sign Up" });
		if (choice == 1)
			DoLogin();
		else
			DoSignUp();

		questions_manager.LoadDatabase();
		questions_manager.FillUserQuestions(current_user);
	}

	void LoadDatabase() {
		last_id = 0;
		userame_userobject_map.clear();
		userid_userobject_map.clear();

		vector<string> lines = ReadFileLines("users.txt");
		for (auto &line : lines) {
			User user(line);
			last_id = max(last_id, user.user_id);

			userame_userobject_map[user.user_name] = user;
			userid_userobject_map[user.user_id] = user;
		}
	}

	void UpdateDatabase() {
		vector<string> lines;
		for (auto &pair : userame_userobject_map)
			lines.push_back(pair.second.ToString());

		WriteFileLines("users.txt", lines, false);
	}

	void DoLogin() {
		while (true) {
			cout << "Enter user name & password: ";
			cin >> current_user.user_name >> current_user.password;

			if (!userame_userobject_map.count(current_user.user_name)) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			User &user_exist = userame_userobject_map[current_user.user_name];
			if (user_exist.password != current_user.password) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			current_user = user_exist;
			break;
		}
	}

	void DoSignUp() {
		current_user.user_id = ++last_id;

		cout << "Enter user name. (No spaces): ";
		cin >> current_user.user_name;

		if (userame_userobject_map.count(current_user.user_name)) {
			cout << "Already used. Try again\n";
			current_user.user_id = --last_id;
			return DoSignUp();
		}

		cout << "Enter password: ";
		cin >> current_user.password;

		cout << "Enter name (No spaces): ";
		cin >> current_user.name;

		cout << "Enter email: ";
		cin >> current_user.email;

		cout << "Allow anonymous questions? (0 or 1): ";
		cin >> current_user.allow_anonymous_questions;

		userame_userobject_map[current_user.user_name] = current_user;
		userid_userobject_map[current_user.user_id] = current_user;
		UpdateDatabase();
	}

	void ListSystemUsers() {
		for (auto &pair : userame_userobject_map)
			cout << "ID: " << pair.second.user_id << "\t\tName: " << pair.second.name << "\n";
	}

	void SwitchUser() {
		UpdateDatabase();
		AccessSystem();
		questions_manager.FillUserQuestions(current_user);
	}

	void UserAskQuestion() {
		bool is_anonymous = false;
		if (current_user.allow_anonymous_questions == 1) {
			cout << "Is anonymous questions?: (0 or 1): ";
			cin >> is_anonymous;
		}
		questions_manager.AskQuestion(current_user, is_anonymous);
	}

	void UserAnswerQuestion() {
		questions_manager.AnswerQuestion(current_user);
	}

	void UserDeleteQuestion() {
		questions_manager.DeleteQuestion(current_user);
	}

	void ListUserAskedQuestions() {
		questions_manager.PrintUserFromQuestions(current_user);
	}

	void ListUserFeed() {
		questions_manager.PrintUserToQuestions(current_user);
	}

	void ListFeed() {
		questions_manager.ListFeed();
	}

	void AskQuestionThread() {
		bool is_anonymous = false;
		if (current_user.allow_anonymous_questions == 1) {
			cout << "Is anonymous questions?: (0 or 1): ";
			cin >> is_anonymous;
		}
		questions_manager.AskQuestionThread(current_user, is_anonymous);
	}
};

void DisplayMenu() {
	cout << "\n";
	cout << "1: Print Questions To Me\n";
	cout << "2: Print Questions From Me\n";
	cout << "3: Answer Question\n";
	cout << "4: Delete Question\n";
	cout << "5: Ask Question\n";
	cout << "6: List System Users\n";
	cout << "7: Feed\n";
	cout << "8: Switch User\n";
	cout << "9: Ask Question Thread\n";
	cout << "10: Logout\n";
}

void TestSystem() {
	UsersManager manager;
	manager.AccessSystem();

	while (true) {
		DisplayMenu();
		int choice = ReadInt(1, 10);

		if (choice == 1)
			manager.ListUserFeed();
		else if (choice == 2)
			manager.ListUserAskedQuestions();
		else if (choice == 3)
			manager.UserAnswerQuestion();
		else if (choice == 4)
			manager.UserDeleteQuestion();
		else if (choice == 5)
			manager.UserAskQuestion();
		else if (choice == 6)
			manager.ListSystemUsers();
		else if (choice == 7)
			manager.ListFeed();
		else if (choice == 8)
			manager.SwitchUser();
		else if (choice == 9)
			manager.AskQuestionThread();
		else
			break;
	}
}

int main() {
	TestSystem();
	return 0;
}
