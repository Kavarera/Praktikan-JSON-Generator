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
    int projek=0;
    bool isProjek = false;
    Tugas Tugas;
    std::string toJSON() const {
        std::ostringstream json;
        json << "{"
             << "\"kuis\": " << kuis << ", "
             << "\"responsi\": " << responsi << ", "
             << "\"tugas\": " << Tugas.toJSON();
        isProjek ? json<<", \"project\": " + std::to_string(projek) : json<<"";
    json << "}";
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


std::vector<Praktikan> optimizedSetupProgram(){
    std::string directoryPath = std::filesystem::current_path().string();
    std::vector<std::string> csvFiles;
    std::vector<Praktikan> listPraktikan;
    // Baca semua file .csv di dalam directory
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.path().extension() == ".csv") {
            csvFiles.push_back(entry.path().string());
        }
    }
    std::cout << "Number of CSV files: " << csvFiles.size() << std::endl;
    for (const auto& csvFile : csvFiles) {
        // Print the file name
        std::string fileName = csvFile.substr(csvFile.find_last_of('-') + 1);
        std::string fileNameWithoutExtension = fileName.substr(0, fileName.find('.'));
        std::cout << "Set Configuration for File : " << fileNameWithoutExtension << std::endl;

        std::cout << "Please input how many classes you teach in " << fileNameWithoutExtension << ": ";
        int classAmount;
        std::cin >> classAmount;
        std::cin.ignore(1, '\n');

        std::cout<<"Does this class have a project? (1: Yes, 0: No) ";
        int isProjek;
        std::cin >> isProjek;
        std::cin.ignore(1, '\n');
        std::vector<std::string> _listClass;
        for (int i = 0; i < classAmount; i++) {
            std::cout << "Please input the class code you teach in " << fileNameWithoutExtension << ": (Example: IF-A) ";
            std::string _classIF; std::cin >> _classIF;
            std::cin.ignore(1, '\n');
            _listClass.push_back(_classIF);
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
        try{
            std::cin >> logo;
            std::cin.ignore(1, '\n');
            if (logo == 0) {
                logo = 8;
            }
        } catch (std::exception& e){
            std::cout<<"Input logo Error: "<<e.what()<<std::endl;
            std::cout<<"Setting logo to default value 8"<<std::endl;
            logo = 8;
        }

        std::string NamaForClass;
        std::cout << "Please input the name for class " << fileNameWithoutExtension << ": ";
        std::getline(std::cin, NamaForClass);

        // Read from the CSV file inserting to vector line by line
        std::ifstream file(csvFile);
        std::string line;
        std::vector<std::string> _listLine;

        while (std::getline(file, line)) {
            _listLine.push_back(line);
        }
        file.close();
        for(std::string _line :_listLine){
            std::string lastCell = _line.substr(_line.find_last_of(',') + 1);
            // Split the line into a string array
                std::vector<std::string> _dataArray;
                std::string token;
                std::istringstream tokenStream(_line);
                int lineDataIndex = 0;
                while(std::getline(tokenStream, token, ',')){
                    if (lineDataIndex == 0) {
                        lineDataIndex++;
                        continue;
                    } else {
                        _dataArray.push_back(token);
                        lineDataIndex++;
                        if (lineDataIndex >= 3 ) {
                            for(std::string _class:_listClass){
                                if(_class == lastCell){
                                    Praktikan praktikan;
                                    praktikan.nim = _dataArray[0];
                                    Kelas kelas;
                                    kelas.kode = _dataArray[1];
                                    kelas.logo = logo;
                                    kelas.nama = NamaForClass;
                                    kelas.Items.isProjek = isProjek;
                                    praktikan.Kelas.push_back(kelas);
                                    listPraktikan.push_back(praktikan);
                                }
                            }
                            lineDataIndex = 0;
                        }
                    }
                }
        }     

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
    jsonOutput << "{\"users\":{";
    for (size_t i = 0; i < filteredDatas.size(); ++i) {
        jsonOutput << filteredDatas[i].toJSON();
        if (i < filteredDatas.size() - 1) jsonOutput << ", ";
    }
    jsonOutput << "}}";

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
        state++;
        if(state == numStates) state = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "\rGenerating JSON \t\t Done!" << std::endl;
}

int main() {
    std::vector<Praktikan> datas = optimizedSetupProgram();
    std::cout << "datas = " << datas.size() << std::endl;

    // SETUP DATAS TO DEFAULT VALUE
    for (auto& data : datas) {
        data.Kelas[0].Items.Tugas.listTugas.push_back(0);
    }

    std::atomic<bool> status(true);
    std::thread loadingThread(LOADING, std::ref(status));
    generateJson(datas, status);
    loadingThread.join();

    return 0;
}