#include <iostream>
#include "sqlite3.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

// Узнаем возраст по дате рождения и текущей дате
int calculateAge(std::string birthday) {
	// Текущая дата
	std::time_t t = std::time(0);
	std::tm now;
	localtime_s(&now, &t);
	// Разбиение даты рождения на год, месяц и день
	int birthYear = std::stoi(birthday.substr(0, 4));
	int birthMonth = std::stoi(birthday.substr(5, 2));
	int birthDay = std::stoi(birthday.substr(8, 2));
	// Возраст
	int age = now.tm_year + 1900 - birthYear;
	if (now.tm_mon + 1 < birthMonth || (now.tm_mon + 1 == birthMonth && now.tm_mday < birthDay)) {
		age--;
	}
	return age;
}
// Функция обратного вызова для вывода результатов запроса
static int callback(void* data, int argc, char** argv, char** azColName) {
	for (int i = 0; i < argc; i++) {
		std::cout << azColName[i] << ": " << argv[i] << std::endl;
	}
	std::cout << std::endl;
	return 0;
}
int main() {
	// Подключение к бд
	sqlite3* db;
	int rc = sqlite3_open("proba.db", &db);
	if (rc) {
		std::cout << "Mistake with open BD: " << sqlite3_errmsg(db) << std::endl;
		return 0;
	}
	else {
		std::cout << "BD opened without mistakes" << std::endl;
	}
	// Таблица в бд
	std::string createTableQuery = "CREATE TABLE IF NOT EXISTS MyTable (Name TEXT, Birthday TEXT);";
	rc = sqlite3_exec(db, createTableQuery.c_str(), NULL, 0, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "Mistake with tablica: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 0;
    }
    else {
        std::cout << "tablica was maked without mistakes" << std::endl;
    }
    int choice;
    do {
        std::cout << "-------------------" << std::endl;
        std::cout << "1. View data" << std::endl;
        std::cout << "2. Add data" << std::endl;
        std::cout << "3. Delete data" << std::endl;
        std::cout << "4. Exit" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;
        switch (choice) {
        case 1:
        {
            // Выполнение запроса на выборку данных
            std::string selectQuery = "SELECT * FROM MyTable;";
            rc = sqlite3_exec(db, selectQuery.c_str(), callback, 0, NULL);

            if (rc != SQLITE_OK) {
                std::cout << "Mistake: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                return 0;
            }
            break;
        }
        case 2:
        {
            std::cout << "Enter the name: ";
            std::string name;
            std::cin >> name;

            std::cout << "Enter the birthday (YYYY-MM-DD): ";
            std::string birthday;
            std::cin >> birthday;
            // Вычисляем
            int age = calculateAge(birthday);
            // Данные в таблицу.
            std::string insertQuery = "INSERT INTO MyTable (Name, Birthday) VALUES ('" + name + "', '" + birthday + "');";
            rc = sqlite3_exec(db, insertQuery.c_str(), NULL, 0, NULL);

            if (rc != SQLITE_OK) {
                std::cout << "Error when adding data to the table: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                return 0;
            }
            else {
                std::cout << "The data has been added successfully" << std::endl;
            }
            break;
        }
        case 3:
        {
            std::cout << "Enter the name to delete: ";
            std::string name;
            std::cin >> name;
                // Удалить по имени
                std::string deleteQuery = "DELETE FROM MyTable WHERE Name = '" + name + "';";
            rc = sqlite3_exec(db, deleteQuery.c_str(), NULL, 0, NULL);

            if (rc != SQLITE_OK) {
                std::cout << "Error when deleting data from the table: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                return 0;
            }
            else {
                std::cout << "The data has been deleted successfully" << std::endl;
            }
            break;
        }
        case 4:
            std::cout << "Exiting the program..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice" << std::endl;
            break;
        }
    } while (choice != 4);
    // Закрыть бд
    sqlite3_close(db);
    return 0;
}