#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// ==========================================
// CLASS: Employee
// ==========================================
class Employee {
private:
    int empID;
    string name;
    double basicPay;
    double otHours;
    static constexpr double OT_RATE = 500.0;

public:
    Employee(int id = 0, string n = "", double bp = 0.0, double ot = 0.0) {
        empID = id;
        name = n;
        basicPay = bp;
        otHours = ot;
    }

    int getID() const { return empID; }
    string getName() const { return name; }
    double getBasicPay() const { return basicPay; }
    double getOTHours() const { return otHours; }

    void setOTHours(double hours) { otHours = hours; }

    double calculateGross() const {
        return basicPay + (otHours * OT_RATE);
    }

    double calculateTax() const {
        double gross = calculateGross();
        return (gross > 50000) ? (gross * 0.10) : (gross * 0.05);
    }

    double calculateNet() const {
        return calculateGross() - calculateTax();
    }

    void displaySlip() const {
        cout << "\n-----------------------------" << endl;
        cout << "      EMPLOYEE DETAILS       " << endl;
        cout << "-----------------------------" << endl;
        cout << "ID: " << empID << endl;
        cout << "Name: " << name << endl;
        cout << "Basic Pay: " << basicPay << endl;
        cout << "OT Hours: " << otHours << endl;
        cout << "OT Rate: " << OT_RATE << endl;
        cout << "-----------------------------" << endl;
    }

    string toFileData() const {
        return to_string(empID) + "," + name + "," +
               to_string(basicPay) + "," + to_string(otHours);
    }
};


constexpr double Employee::OT_RATE;


// ==========================================
// CLASS: PayrollSystem
// ==========================================
class PayrollSystem {
private:
    vector<Employee> employees;
    const string fileName = "payroll_data.txt";
    const string historyFile = "salary_history.txt";

    void loadFromFile() {
        employees.clear();
        ifstream file(fileName);
        if (!file.is_open()) {
            cout << "No existing data file found.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);

            int id = stoi(line.substr(0, pos1));
            string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
            double bp = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
            double ot = stod(line.substr(pos3 + 1));

            employees.push_back(Employee(id, name, bp, ot));
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(fileName, ios::trunc);
        for (const auto& emp : employees) {
            file << emp.toFileData() << endl;
        }
        file.close();
    }

    void saveHistory(const Employee& emp) {
        ofstream file(historyFile, ios::app);
        file << emp.getID() << "," << emp.getName() << ","
             << emp.calculateNet() << endl;
        file.close();
    }

public:
    PayrollSystem() {
        loadFromFile();
    }

    void addEmployee() {
        int id;
        string name;
        double bp;

        cout << "Enter EmpID: "; cin >> id;

        for (const auto& emp : employees) {
            if (emp.getID() == id) {
                cout << "Error: ID already exists!\n";
                return;
            }
        }

        cout << "Enter Name: "; cin.ignore(); getline(cin, name);
        cout << "Enter Basic Pay: "; cin >> bp;

        if (bp < 0) {
            cout << "Invalid Salary!\n";
            return;
        }

        employees.push_back(Employee(id, name, bp, 0));
        saveToFile();
        cout << "Employee added successfully!\n";
    }

    void updateOT() {
        int id;
        double hours;

        cout << "Enter EmpID: "; cin >> id;

        for (auto& emp : employees) {
            if (emp.getID() == id) {
                cout << "Enter new OT Hours: "; cin >> hours;

                if (hours < 0) {
                    cout << "Error: OT hours cannot be negative!\n";
                    return;
                }

                emp.setOTHours(hours);
                saveToFile();
                cout << "OT Updated!\n";
                return;
            }
        }
        cout << "Employee not found.\n";
    }

    void deleteEmployee() {
        int id;
        cout << "Enter ID to delete: "; cin >> id;

        auto it = remove_if(employees.begin(), employees.end(),
            [id](const Employee& e) {
                return e.getID() == id;
            });

        if (it != employees.end()) {
            employees.erase(it, employees.end());
            saveToFile();
            cout << "Record deleted.\n";
        } else {
            cout << "Employee not found.\n";
        }
    }

    void searchEmployee() {
        int id;
        cout << "Enter EmpID: "; cin >> id;

        for (const auto& emp : employees) {
            if (emp.getID() == id) {
                cout << "Employee Found!\n";
                cout << "ID: " << emp.getID() << "\nName: " << emp.getName() << endl;
                return;
            }
        }
        cout << "Employee not found.\n";
    }

    void generateSlip() {
        int id;
        cout << "Enter EmpID: "; cin >> id;

        for (const auto& emp : employees) {
            if (emp.getID() == id) {
                emp.displaySlip();
                saveHistory(emp);
                return;
            }
        }
        cout << "Employee not found.\n";
    }

    void calculateSalaryDetails() {
        int id;
        cout << "Enter EmpID: "; cin >> id;
        
        for (const auto& emp : employees) {
            if (emp.getID() == id) {
                cout << fixed << setprecision(2);
                cout << "\n--- SALARY CALCULATION ---\n";
                cout << "Gross Salary: " << emp.calculateGross() << endl;
                cout << "Tax Deduction: " << emp.calculateTax() << endl;
                cout << "Net Salary: " << emp.calculateNet() << endl;
                return;
            }
        }
        cout << "Employee not found.\n";
    }

    void generateSummary() {
        if (employees.empty()) {
            cout << "No employees available.\n";
            return;
        }

        double total = 0;
        for (const auto& emp : employees) {
            total += emp.calculateNet();
        }
        cout << fixed << setprecision(2);
        cout << "\n=========================================\n";
        cout << "            SUMMARY REPORT\n";
        cout << "=========================================\n";
        cout << "Total Employees : " << employees.size() << endl;
        cout << "Total Payout    : " << total << endl;
        cout << "Average Salary  : " << (total / employees.size()) << endl;
        cout << "=========================================\n";
    }

    void showHistory() {
        ifstream file(historyFile);
        if (!file.is_open()) {
            cout << "No history found.\n";
            return;
        }

        cout << "\n--- SALARY HISTORY ---\n";
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void showHighestPaidEmployee() {
        if (employees.empty()) {
            cout << "No employees available.\n";
            return;
        }

        int highestIndex = 0;

        for (int i = 1; i < employees.size(); i++) {
            if (employees[i].calculateNet() > employees[highestIndex].calculateNet()) {
                highestIndex = i;
            }
        }

        cout << "\n--- HIGHEST PAID EMPLOYEE ---\n";
        cout << "ID: " << employees[highestIndex].getID() << endl;
        cout << "Name: " << employees[highestIndex].getName() << endl;
        cout << "Net Salary: " << employees[highestIndex].calculateNet() << endl;
    }
};

// ==========================================
// MAIN
// ==========================================
int main() {
    PayrollSystem system;
    int choice;

    cout << "\n=========================================\n";
    cout << "   WELCOME TO EMPLOYEE PAYROLL SYSTEM\n";
    cout << "=========================================\n";

    while (true) {
        cout << "\n=== EMPLOYEE PAYROLL SYSTEM ===\n";
        cout << "1. Add Employee\n";
        cout << "2. Update OT Hours\n";
        cout << "3. Delete Employee\n";
        cout << "4. Search Employee\n";
        cout << "5. Show Employee Details\n";
        cout << "6. Salary (Gross/Tax/Net)\n";
        cout << "7. Summary Report\n";
        cout << "8. Salary History\n";
        cout << "9. Highest Paid Employee\n";
        cout << "10. Exit\n";
        cout << "-----------------------------------------\n";

        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 10) {
            cout << "\nThank you for using the Payroll System!\n";
            cout << "Exiting... Have a great day!\n";
            break;
        }

        switch (choice) {
            case 1: system.addEmployee(); break;
            case 2: system.updateOT(); break;
            case 3: system.deleteEmployee(); break;
            case 4: system.searchEmployee(); break;
            case 5: system.generateSlip(); break;
            case 6: system.calculateSalaryDetails(); break;
            case 7: system.generateSummary(); break;
            case 8: system.showHistory(); break;
            case 9: system.showHighestPaidEmployee(); break;
            default: cout << "Invalid choice!\n";
        }
    }

    return 0;
}