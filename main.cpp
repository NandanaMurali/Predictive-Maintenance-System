#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <map>
 
class MachineryState {
public:
    virtual void handleState() = 0;
    virtual ~MachineryState() = default;
};
 
class RunningState : public MachineryState {
public:
    void handleState() override {
        std::cout << "Machinery is running normally.\n";
    }
};
 
class MaintenanceState : public MachineryState {
public:
    void handleState() override {
        std::cout << "Machinery is under maintenance.\n";
    }
};
 
class ShutdownState : public MachineryState {
public:
    void handleState() override {
        std::cout << "Machinery is shut down.\n";
    }
};
 
class MessageHandler {
public:
    void addAlert(int priority, const std::string &message) {
        alerts[priority].push_back(message);
    }
 
    void processAlerts() {
        for (auto &pair : alerts) {
            for (const auto &message : pair.second) {
                std::cout << "[Priority " << pair.first << "]: " << message << "\n";
            }
        }
    }
 
private:
    std::map<int, std::vector<std::string>> alerts;
};
 
std::vector<std::vector<std::string>> parseTelemetryFile(const std::string &filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return data;
    }
 
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::istringstream ss(line);
        std::string word;
        while (ss >> word) {  
            row.push_back(word);
        }
        if (!row.empty()) data.push_back(row);
    }
    return data;
}
 
void sortSensorLogs(std::vector<std::vector<std::string>> &logs, int column) {
    std::sort(logs.begin(), logs.end(), [column](const std::vector<std::string> &a, const std::vector<std::string> &b) {
        return a[column] < b[column];
    });
}
 
std::vector<std::vector<std::string>> searchLogs(const std::vector<std::vector<std::string>> &logs, const std::string &keyword, int column) {
    std::vector<std::vector<std::string>> results;
    std::copy_if(logs.begin(), logs.end(), std::back_inserter(results), [column, &keyword](const std::vector<std::string> &row) {
        return column < row.size() && row[column] == keyword;
    });
    return results;
}
 
std::vector<std::vector<std::string>> filterLogs(const std::vector<std::vector<std::string>> &logs, std::function<bool(const std::vector<std::string>&)> predicate) {
    std::vector<std::vector<std::string>> filtered;
    std::copy_if(logs.begin(), logs.end(), std::back_inserter(filtered), predicate);
    return filtered;
}
 
int main() {
    MachineryState *state = new RunningState();
    state->handleState();
    delete state;
    state = new MaintenanceState();
    state->handleState();
    delete state;
    state = new ShutdownState();
    state->handleState();
    delete state;

    auto logs = parseTelemetryFile("telemetry.txt");
 
    std::cout << "Sorting logs by column 1 (Timestamp)...\n";
    sortSensorLogs(logs, 0);
 
    std::cout << "Searching for logs with keyword 'ERROR' in column 2...\n";
    auto errorLogs = searchLogs(logs, "ERROR", 2);
    for (const auto &log : errorLogs) {
        for (const auto &entry : log) {
            std::cout << entry << " ";
        }
        std::cout << "\n";
    }
 
    std::cout << "Filtering logs where column 3 (Sensor Value) > 100...\n";
    auto filteredLogs = filterLogs(logs, [](const std::vector<std::string> &row) {
        return row.size() > 3 && std::stoi(row[3]) > 100; 
    });
    for (const auto &log : filteredLogs) {
        for (const auto &entry : log) {
            std::cout << entry << " ";
        }
        std::cout << "\n";
    }
 
    MessageHandler handler;
    handler.addAlert(1, "Critical failure in sensor 3!");
    handler.addAlert(2, "Maintenance required for sensor 5.");
    handler.processAlerts();
 
    return 0;
}