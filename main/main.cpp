#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <limits>  // Добавлено для numeric_limits
#define NOMINMAX
#include <windows.h>

using namespace std;

// Структура для хранения данных о посещаемости
struct AttendanceRecord {
    string lastName;    // фамилия
    string firstName;   // имя
    string date;        // дата
    string subject;     // предмет
    bool attended;      // статус (присутствовал/отсутствовал)
};

// Класс для управления посещаемостью студентов
class AttendanceSystem {
private:
    vector<AttendanceRecord> records;
    const string filename = "attendance.txt";

public:
    // Добавление записи о посещаемости
    void addRecord(const AttendanceRecord& record) {
        records.push_back(record);
    }

    // Удаление записей по фамилии
    bool removeRecords(const string& lastName) {
        bool found = false;
        auto it = remove_if(records.begin(), records.end(),
                            [&lastName](const AttendanceRecord& r) {
                                return r.lastName == lastName;
                            });

        if (it != records.end()) {
            records.erase(it, records.end());
            found = true;
        }
        return found;
    }

    // Поиск записей по фамилии
    vector<AttendanceRecord> findRecords(const string& lastName) const {
        vector<AttendanceRecord> result;
        for (const auto& record : records) {
            if (record.lastName == lastName) {
                result.push_back(record);
            }
        }
        return result;
    }

    // Расчет процента посещаемости для каждого студента
    map<string, double> calculateAttendancePercent() const {
        map<string, pair<int, int>> studentStats; // имя -> (посещения, всего занятий)

        for (const auto& record : records) {
            string fullName = record.lastName + " " + record.firstName;
            studentStats[fullName].second++; // увеличиваем общее количество занятий
            if (record.attended) {
                studentStats[fullName].first++; // увеличиваем количество посещений
            }
        }

        map<string, double> attendancePercent;
        for (const auto& [name, stats] : studentStats) {
            attendancePercent[name] = (stats.second > 0) ?
                                          (static_cast<double>(stats.first) / stats.second) * 100 : 0.0;
        }

        return attendancePercent;
    }

    // Сохранение данных в файл
    bool saveToFile() const {
        ofstream outFile(filename);
        if (!outFile) return false;

        for (const auto& record : records) {
            outFile << record.lastName << ","
                    << record.firstName << ","
                    << record.date << ","
                    << record.subject << ","
                    << (record.attended ? "1" : "0") << "\n";
        }
        outFile.close();
        return true;
    }

    // Загрузка данных из файла
    bool loadFromFile() {
        ifstream inFile(filename);
        if (!inFile) return false;

        records.clear();
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string token;
            AttendanceRecord record;

            getline(ss, token, ',');
            record.lastName = token;

            getline(ss, token, ',');
            record.firstName = token;

            getline(ss, token, ',');
            record.date = token;

            getline(ss, token, ',');
            record.subject = token;

            getline(ss, token);
            record.attended = (token == "1");

            records.push_back(record);
        }
        inFile.close();
        return true;
    }
};

// Функция для ввода данных о посещаемости
AttendanceRecord inputAttendanceRecord() {
    AttendanceRecord record;
    cout << "Enter last name: ";
    getline(cin, record.lastName);
    cout << "Enter first name: ";
    getline(cin, record.firstName);
    cout << "Enter date (DD.MM.YYYY): ";
    getline(cin, record.date);
    cout << "Enter subject: ";
    getline(cin, record.subject);

    string status;
    while (true) {
        cout << "Status (present/absent): ";
        getline(cin, status);
        if (status == "present") {
            record.attended = true;
            break;
        }
        else if (status == "absent") {
            record.attended = false;
            break;
        }
        else {
            cout << "Error! Enter 'present' or 'absent'.\n";
        }
    }
    return record;
}

// Функция для вывода записи о посещаемости
void printRecord(const AttendanceRecord& record) {
    cout << setw(15) << left << record.lastName
         << setw(15) << left << record.firstName
         << setw(12) << left << record.date
         << setw(20) << left << record.subject
         << (record.attended ? "present" : "absent") << endl;
}

// Функция для вывода процента посещаемости
void printAttendancePercent(const map<string, double>& attendancePercent) {
    cout << "\nStudent Attendance Percentage:\n";
    cout << setw(30) << left << "Student" << "Attendance (%)" << endl;
    cout << string(50, '-') << endl;

    for (const auto& [name, percent] : attendancePercent) {
        cout << setw(30) << left << name
             << fixed << setprecision(2) << percent << endl;
    }
}

// Главное меню программы
void displayMenu() {
    cout << "\n=== Student Attendance System ===\n"
         << "1. Add attendance record\n"
         << "2. Delete records by last name\n"
         << "3. Find records by last name\n"
         << "4. Show attendance percentage\n"
         << "5. Save data\n"
         << "6. Load data\n"
         << "0. Exit\n"
         << "Choose an option: ";
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    AttendanceSystem system;
    int choice;

    // Загрузка данных из файла при запуске
    if (system.loadFromFile()) {
        cout << "Data loaded successfully.\n";
    }
    else {
        cout << "Failed to load data or file does not exist.\n";
    }

    do {
        displayMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            AttendanceRecord record = inputAttendanceRecord();
            system.addRecord(record);
            cout << "Record added.\n";
            break;
        }
        case 2: {
            string lastName;
            cout << "Enter last name to delete records: ";
            getline(cin, lastName);
            if (system.removeRecords(lastName)) {
                cout << "Records deleted.\n";
            }
            else {
                cout << "No records found with this last name.\n";
            }
            break;
        }
        case 3: {
            string lastName;
            cout << "Enter last name to search: ";
            getline(cin, lastName);
            vector<AttendanceRecord> found = system.findRecords(lastName);
            if (found.empty()) {
                cout << "No records found.\n";
            }
            else {
                cout << "\nFound " << found.size() << " records:\n";
                cout << setw(15) << left << "Last Name"
                     << setw(15) << left << "First Name"
                     << setw(12) << left << "Date"
                     << setw(20) << left << "Subject"
                     << "Status" << endl;
                cout << string(80, '-') << endl;
                for (const auto& record : found) {
                    printRecord(record);
                }
            }
            break;
        }
        case 4: {
            map<string, double> attendancePercent = system.calculateAttendancePercent();
            printAttendancePercent(attendancePercent);
            break;
        }
        case 5: {
            if (system.saveToFile()) {
                cout << "Data saved successfully.\n";
            }
            else {
                cout << "Error saving data.\n";
            }
            break;
        }
        case 6: {
            if (system.loadFromFile()) {
                cout << "Data loaded successfully.\n";
            }
            else {
                cout << "Error loading data.\n";
            }
            break;
        }
        case 0: {
            cout << "Exiting the program.\n";
            break;
        }
        default: {
            cout << "Invalid choice. Try again.\n";
        }
        }
    } while (choice != 0);

    return 0;
}
