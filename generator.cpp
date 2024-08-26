#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <map>
#include <thread>
#include <atomic>

class Tugas {
public:
    std::vector<int> listTugas;
    std::string toJSON() const {
        std::ostringstream json;
        json << "[";
        for (size_t i = 0; i < listTugas.size(); ++i) {
            json << listTugas[i];
            if (i < listTugas.size() - 1) json << ", ";
        }
        json << "]";
        return json.str();
    }
};

class Items {
public:
    int kuis = 0;
    int responsi = 0;
    int projek = 0;
    Tugas Tugas;
    std::string toJSON() const {
        std::ostringstream json;
        json << "{"
             << "\"kuis\": " << kuis << ", "
             << "\"responsi\": " << responsi << ", "
             << "\"tugas\": " << Tugas.toJSON()
             << "}";
        return json.str();
    }
};

class Kelas {
public:
    std::string kode = "-";
    int logo = 8;
    std::string nama = "-";
    Items Items;

    std::string toJSON() const {
        std::ostringstream json;
        json << "{"
             << "\"kode\": \"" << kode << "\", "
             << "\"logo\": " << logo << ", "
             << "\"nama\": \"" << nama << "\", "
             << "\"items\": " << Items.toJSON()
             << "}";
        return json.str();
    }
};

class Praktikan {
public:
    std::string nim;
    std::vector<Kelas> Kelas;

    std::string toJSON() const {
        std::ostringstream json;
        json << "\"" << nim << "\": { \"kelas\": [";
        for (size_t i = 0; i < Kelas.size(); ++i) {
            json << Kelas[i].toJSON();
            if (i < Kelas.size() - 1) json << ", ";
        }
        json << "] }";
        return json.str();
    }

    void displayValues(const std::vector<Praktikan>& praktikans) const {
        for (const auto& praktikan : praktikans) {
            std::cout << "NIM: " << praktikan.nim << std::endl;
            std::cout << "Kelas Kode: " << praktikan.Kelas[0].kode << std::endl;
            std::cout << "Kelas Logo: " << praktikan.Kelas[0].logo << std::endl;
            std::cout << "Kelas Nama: " << praktikan.Kelas[0].nama << std::endl;
            std::cout << "Items Kuis: " << praktikan.Kelas[0].Items.kuis << std::endl;
            std::cout << "Items Responsi: " << praktikan.Kelas[0].Items.responsi << std::endl;
            std::cout << "Items Tugas: ";
            for (const auto& tugas : praktikan.Kelas[0].Items.Tugas.listTugas) {
                std::cout << tugas << " ";
            }
            std::cout << std::endl;
            std::cout << "------------------------" << std::endl;
        }
    }
};

std::vector<Praktikan> setupProgram() {
    std::string directoryPath = std::filesystem::current_path().string();
    std::vector<std::string> csvFiles;
    std::vector<Praktikan> listPraktikan;
    std::vector<std::string> listClass;
    std::string classIF;

    // Iterate over files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.path().extension() == ".csv") {
            csvFiles.push_back(entry.path().string());
        }
    }
    std::cout << "Number of CSV files: " << csvFiles.size() << std::endl;
    int indexFile = 0;
    for (const auto& csvFile : csvFiles) {
        // Print the file name
        std::string fileName = csvFile.substr(csvFile.find_last_of('-') + 1);
        std::string fileNameWithoutExtension = fileName.substr(0, fileName.find('.'));
        std::cout << "File name: " << fileNameWithoutExtension << std::endl;
        // Asking for how many classes user teaches in the same class
        std::cout << "Please input how many classes you teach in " << fileNameWithoutExtension << ": ";
        int classAmount;
        std::cin >> classAmount;
        std::cin.ignore(1, '\n');
        for (int i = 0; i < classAmount; i++) {
            std::cout << "Please input the class code you teach in " << fileNameWithoutExtension << ": (Example: IF-A) ";
            std::cin >> classIF;
            std::cin.ignore(1, '\n');
            listClass.push_back(classIF);
        }

        std::cout << R"(
                            1: 'DataScience'
                            2: 'DataBase'
                            3: 'IoT'
                            4: 'Networking'
                            5: 'Mobile Programming'
                            6: 'SCPK or DataScience'
                            7: 'Web Programming'
                            8: 'Algorithm or OOP (DEFAULT)'
                            9: 'Cloud Computing ')";
        std::cout << "\nPlease input the logo for class " << fileNameWithoutExtension << ": ";
        int logo;
        std::cin >> logo;
        std::cin.ignore(1, '\n');
        if (logo == 0) {
            logo = 8;
        }
        std::string NamaForClass;
        std::cout << "Please input the name for class " << fileNameWithoutExtension << ": ";
        std::getline(std::cin, NamaForClass);

        // Read from the CSV file
        std::ifstream file(csvFile);
        std::string line;

        for (const std::string& cIF : listClass) {
            std::cout << "classIF = " << cIF << std::endl;

            while (std::getline(file, line)) {
                std::string lastCell = line.substr(line.find_last_of(',') + 1);
                if (lastCell == cIF) {
                    // Split the line into a string array
                    std::vector<std::string> dataArray;
                    std::string token;
                    std::istringstream tokenStream(line);
                    int lineDataIndex = 0;

                    while (std::getline(tokenStream, token, ',')) {
                        if (lineDataIndex == 0) {
                            lineDataIndex++;
                            continue;
                        } else {
                            dataArray.push_back(token);
                            lineDataIndex++;
                            if (lineDataIndex == 3) {
                                Praktikan praktikan;
                                praktikan.nim = dataArray[0];
                                Kelas kelas;
                                kelas.kode = dataArray[1];
                                kelas.logo = logo;
                                kelas.nama = NamaForClass;
                                praktikan.Kelas.push_back(kelas);
                                listPraktikan.push_back(praktikan);
                                lineDataIndex = 0;
                            }
                        }
                    }
                }
            }
        }

        file.close();
        indexFile++;
    }

    return listPraktikan;
}

void generateJson(const std::vector<Praktikan>& datas, std::atomic<bool>& status) {
    // FILTER DATA WITH SAME NIM INTO ONE CLASS BUT ADD THE DIFFERENT KELAS PROPERTY
    std::vector<Praktikan> filteredDatas;
    for (const auto& data : datas) {
        bool found = false;
        for (auto& filteredData : filteredDatas) {
            if (filteredData.nim == data.nim) {
                filteredData.Kelas.push_back(data.Kelas[0]);
                found = true;
                break;
            }
        }
        if (!found) {
            filteredDatas.push_back(data);
        }
    }
    std::cout << "After = " << filteredDatas.size() << std::endl;

    // GENERATE THE JSON
    std::ostringstream jsonOutput;
    jsonOutput << "{";
    for (size_t i = 0; i < filteredDatas.size(); ++i) {
        jsonOutput << filteredDatas[i].toJSON();
        if (i < filteredDatas.size() - 1) jsonOutput << ", ";
    }
    jsonOutput << "}";

    // Write the JSON output to a file
    std::ofstream outputFile("output.json");
    if (outputFile.is_open()) {
        outputFile << jsonOutput.str();
        outputFile.close();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "\rJSON output written to file: output.json" << std::endl;
    } else {
        std::cout << "Unable to open file for writing." << std::endl;
    }
    status = false;
}

void LOADING(std::atomic<bool>& status) {
    const char* loadingStates[] = {
        "Generating JSON \t\t | ",
        "Generating JSON \t\t / ",
        "Generating JSON \t\t -- ",
        "Generating JSON \t\t \\ "
    };
    const int numStates = 4;
    int state = 0;

    while (status) {
        std::cout << "\r" << loadingStates[state] << std::flush;
        state = (state + 1) % numStates;
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Adjust the delay as needed
    }
    std::cout << "\rGenerating JSON \t\t Done!" << std::endl;
}

int main() {
    std::vector<Praktikan> datas = setupProgram();
    std::cout << "datas = " << datas.size() << std::endl;

    // SETUP DATAS TO DEFAULT VALUE
    for (auto& data : datas) {
        data.Kelas[0].Items.kuis = 0;
        data.Kelas[0].Items.responsi = 0;
        data.Kelas[0].Items.Tugas.listTugas.push_back(0);
        data.Kelas[0].Items.Tugas.listTugas.push_back(0);
        data.Kelas[0].Items.Tugas.listTugas.push_back(0);
    }

    std::atomic<bool> status(true);
    std::thread loadingThread(LOADING, std::ref(status));
    generateJson(datas, status);
    loadingThread.join();

    return 0;
}