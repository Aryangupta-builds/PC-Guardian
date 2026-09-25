#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <map>
#include <string>
#include <utility>
#include <format>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cmath>
#include <limits>
#include <cctype>
using namespace std;
namespace fs = std::filesystem;

// ================= FORWARD DECLARATIONS =================
class FileInfo;
struct AnalysisResult;
// ================= FUNCTION DECLARATIONS =================

pair<double, string> sizeconverter(long long bytesize);

string getsizecategory(long long sizedivider,
                       const FileInfo &currentFile);

string getcategory(const string &extension,
                   const map<string, string> &category);

map<string, string> createCategoryMap();

void DynamicBar(int position,
                int Max_bar_length);

void StaticBar(double percentage,
               int Max_bar_length);

AnalysisResult Analyzer(
    const vector<FileInfo> &files,
    const map<string, string> &category);

void DisplayFileTable(const vector<FileInfo> &files,
                      long long sizeDivider,
                      size_t howManyFiles);

class FileInfo
{
    string filename;
    string extension;
    string relativePath;
    long long size;

public:
    FileInfo(string f, string e, string r, long long s) : filename(f), extension(e), relativePath(r), size(s)
    {
    }
    string getrelativePath() const
    {
        return relativePath;
    }
    string getfilename() const
    {
        return filename;
    }
    string getextension() const
    {
        return extension;
    }
    long long getsize() const
    {
        return size;
    }
    void display(long long sizedivider, int nameWidth, int relativePathwidth) const
    {
        // formating
        string formatdata = format("{:.2f} {}", sizeconverter(size).first, sizeconverter(size).second);

        cout << setw(nameWidth) << left << filename << "  ";
        cout << setw(relativePathwidth) << relativePath << "  ";
        cout << setw(18) << extension << "  ";
        cout << setw(15) << right << formatdata << "  ";
        cout << setw(15) << right << getsizecategory(sizedivider, *this);
        cout << endl;
    }
};

struct AnalysisResult
{
    long long TotalSize;
    long long MaxSize;
    string MaxFileName;

    // maps-------------
    map<string, long long> storage;
    map<string, double> storagePercentage;
    map<string, long long> categoryStorage;
    map<string, double> categorystoragePercentage;
};
string getSingleFileSizeCategory(long long size)
{
    if (size < 10LL * 1024 * 1024)
    {
        return "Small";
    }
    else if (size < 100LL * 1024 * 1024)
    {
        return "Medium";
    }
    else
    {
        return "Large";
    }
}
void reportFile(const FileInfo &file, const map<string, string> &category)
{
    string filename = file.getfilename();
    string extension = file.getextension();
    string relativepath = file.getrelativePath();
    long long size = file.getsize();

    auto readable = sizeconverter(size);
    string categoryname = getcategory(extension, category);

    // reporting
    cout << "\033[36m";
    cout << "\n";
    cout << "FILE REPORT\n";
    cout << "----------------------------------------------------------\n";
    cout << "\033[0m";
    cout << left;
    cout << setw(20) << "NAME         : " << filename << "\n";
    cout << setw(20) << "LOCATION     : " << relativepath << "\n";
    cout << setw(20) << "EXTENSION    : " << extension << "\n";
    cout << setw(20) << "SIZE         : " << readable.first << " " << readable.second << "\n";
    cout << setw(20) << "CATEGORY     : " << categoryname << "\n";
    cout << setw(20) << "SIZE TYPE    : " << getSingleFileSizeCategory(size) << "\n";
    cout << setw(20) << "SCAN STATUS  : " << "\033[32m" << "VALID" << "\033[0m" << "\n";
}
pair<double, string> sizeconverter(long long bytesize)
{
    double kb = bytesize / 1024.0;
    double mb = kb / 1024.0;
    double gb = mb / 1024.0;
    double tb = gb / 1024.0;
    pair<double, string> data;
    if (bytesize < 1024LL * 1024)
    {
        data.first = kb;
        data.second = "KB";
        return data;
    }
    else if (bytesize < (1024LL * 1024 * 1024))
    {
        data.first = mb;
        data.second = "MB";
        return data;
    }
    else if (bytesize < (1024LL * 1024 * 1024 * 1024))
    {
        data.first = gb;
        data.second = "GB";
        return data;
    }
    else
    {
        data.first = tb;
        data.second = "TB";
        return data;
    }
}

AnalysisResult Analyzer(const vector<FileInfo> &files, const map<string, string> &category)
{
    AnalysisResult result;
    result.TotalSize = 0;
    result.MaxSize = 0;
    result.MaxFileName.clear();
    double percentage = 0;
    for (const auto &Storedfile : files)
    {
        // total file size
        long long currentSize = Storedfile.getsize();
        result.TotalSize += currentSize;

        // category finding
        string extension = Storedfile.getextension();
        string categoryName = getcategory(extension, category);
        result.categoryStorage[categoryName] += currentSize;

        // extension wise storage
        result.storage[Storedfile.getextension()] += currentSize;

        // max file details
        if (currentSize > result.MaxSize)
        {
            result.MaxSize = currentSize;
            result.MaxFileName = Storedfile.getfilename();
        }
    }
    for (auto &item : result.categoryStorage)
    {
        if (result.TotalSize == 0)
        {
            percentage = 0;
        }
        else
        {
            percentage = ((double(item.second) / double(result.TotalSize)) * 100); //-> percentage calculation
        }
        result.categorystoragePercentage[item.first] = percentage;
    }

    // category percentage total 100% check
    double totalcategorypercentage = 0;
    for (const auto &item : result.categorystoragePercentage)
    {
        totalcategorypercentage += item.second;
    }
    if (result.TotalSize != 0 && abs(totalcategorypercentage - 100) > 0.01)
    {
        cout << "CATEGORY PERCENTAGE FAILED\n";
    }
    return result;
}

map<string, string> createCategoryMap()
{
    map<string, string> category;

    //---------------------HARDCODED CATEGORY-----------------------
    // Videos
    category[".mp4"] = "Videos";
    category[".mkv"] = "Videos";
    category[".avi"] = "Videos";
    category[".mov"] = "Videos";
    category[".wmv"] = "Videos";
    category[".webm"] = "Videos";

    // Images
    category[".jpg"] = "Images";
    category[".jpeg"] = "Images";
    category[".png"] = "Images";
    category[".gif"] = "Images";
    category[".bmp"] = "Images";
    category[".webp"] = "Images";

    // Audio
    category[".mp3"] = "Audio";
    category[".wav"] = "Audio";
    category[".flac"] = "Audio";
    category[".aac"] = "Audio";
    category[".ogg"] = "Audio";

    // Documents
    category[".pdf"] = "Documents";
    category[".txt"] = "Documents";
    category[".doc"] = "Documents";
    category[".docx"] = "Documents";
    category[".xls"] = "Documents";
    category[".xlsx"] = "Documents";
    category[".ppt"] = "Documents";
    category[".pptx"] = "Documents";

    // Applications
    category[".exe"] = "Applications";
    category[".msi"] = "Applications";

    // Archives
    category[".zip"] = "Archives";
    category[".rar"] = "Archives";
    category[".7z"] = "Archives";
    category[".tar"] = "Archives";
    category[".gz"] = "Archives";

    // Coding
    category[".cpp"] = "Coding";
    category[".h"] = "Coding";
    category[".hpp"] = "Coding";
    category[".c"] = "Coding";
    category[".py"] = "Coding";
    category[".java"] = "Coding";
    category[".js"] = "Coding";
    category[".ts"] = "Coding";
    category[".html"] = "Coding";
    category[".css"] = "Coding";
    category[".php"] = "Coding";
    category[".cs"] = "Coding";
    category[".go"] = "Coding";
    category[".rs"] = "Coding";

    return category;
}

string getsizecategory(long long sizedivider, const FileInfo &currentFile)
{
    long long size = currentFile.getsize();
    if (size < sizedivider)
    {
        return "Small";
    }
    else if (size < sizedivider * 2LL)
    {
        return "Medium";
    }
    else
    {
        return "Large";
    }
}

string getcategory(const string &extension, const map<string, string> &category)
{
    // file extension ke basis par uski category return karta hai
    auto result = category.find(extension);
    if (result != category.end())
    {
        return result->second;
    }
    else
    {
        return "other";
    }
}

string tolowerCASE(string s)
{
    string temp;
    for (size_t i = 0; i < s.length(); i++)
    {
        temp.push_back(tolower(s[i]));
    }
    return temp;
}

void DynamicBar(int position, int Max_bar_length)
{
    // max_bar_lenght less then 0
    if (Max_bar_length <= 0)
    {
        return;
    }

    // position in safe range
    position = clamp(position, 0, Max_bar_length - 1);
    // scanning bar
    cout << "\r";
    cout << string(position, '-');
    cout << ">";
    cout << string(Max_bar_length - position - 1, '-'); //-1 because '>' khud yek char ka space leta hai
    cout << " " << "Scanning...";
    cout.flush();

    // this_thread::sleep_for(chrono::milliseconds(50));
    //   --> yeh line program ko slow karta hai(USED FOR TESTING)
}

void StaticBar(double percentage, int Max_bar_length)
{
    // max_bar_lenght less then 0
    if (Max_bar_length <= 0)
    {
        return;
    }

    // percentage safe range
    percentage = clamp(percentage, 0.0, 100.0);

    int block = round((percentage / 100) * Max_bar_length);

    // block safe range
    block = clamp(block, 0, Max_bar_length);

    // static bar
    cout << " ";
    cout << string(block, '#');
    cout << string(Max_bar_length - block, '-');
    cout << "  " << setw(5) << fixed << setprecision(2) << (percentage) << "%";
}

long long convertTobytes(double value, string unit)
{
    if (unit == "kb")
    {
        return value * 1024;
    }
    else if (unit == "mb")
    {
        return value * 1024 * 1024;
    }
    else if (unit == "gb")
    {
        return value * 1024 * 1024 * 1024;
    }
    else if (unit == "tb")
    {
        return value * 1024 * 1024 * 1024 * 1024;
    }
    else
    {
        return 1;
    }
}

void DisplayFileTable(const vector<FileInfo> &files, long long sizeDivider, size_t howManyFiles)
{
    // future mai yeh hardcoded value ko dynamic bana dege
    int extensionWidth = 15;
    int sizeWidth = 15;
    int categoryWidth = 18;

    // value reset
    int maxNameLength = 0;
    int maxLocationLength = 0;
    int currentLength = 0;
    int currentLocationLength = 0;

    for (size_t i = 0; i < howManyFiles; i++)
    {
        // maximum location length
        currentLocationLength = files[i].getrelativePath().length();
        if (currentLocationLength > maxLocationLength)
        {
            maxLocationLength = currentLocationLength;
        }
        // maximum file name length
        currentLength = files[i].getfilename().length();
        if (currentLength > maxNameLength)
        {
            maxNameLength = currentLength;
        }
    }
    // minimum width
    if (maxNameLength < 25)
    {
        maxNameLength = 25;
    }
    if (maxLocationLength < 15)
    {
        maxLocationLength = 15;
    }
    // table
    cout << setw(maxNameLength) << left << "Name" << "  ";
    cout << setw(maxLocationLength) << "Location" << "  ";
    cout << setw(extensionWidth) << "Extension" << "  ";
    cout << setw(sizeWidth) << right << "Size" << "  ";
    cout << setw(categoryWidth) << right << "Category" << "  ";
    cout << endl;
    cout << setfill('_') << setw(maxNameLength + (extensionWidth + sizeWidth + categoryWidth + (maxLocationLength) + 8)) << "_" << endl
         << endl; //+8 because of gaps.
    cout << setfill(' ');
    // display
    for (size_t i = 0; i < howManyFiles; i++)
    {
        files[i].display(sizeDivider, maxNameLength, maxLocationLength);
    }
    cout << "\n\n";
}

string trim(const string &s)
{
    size_t start = 0;
    size_t end = s.length();
    while (start < end && isspace(s[start]))
    {
        start++;
    }
    while (end > start && isspace(s[end - 1]))
    {
        end--;
    }
    return s.substr(start, end - start);
}

int main()
{
    // startup
    // ================= HEADER =================

    cout << "============================================================\n";
    cout << "                    "
         << "\033[32m"
         << "PC GUARDIAN"
         << "\033[0m"
         << " v3.0\n";

    cout << "              " << "\033[36m" << "Smart File & System Health Manager\n"
         << "\033[0m";

    cout << "============================================================\n\n";

    cout << "ENTER DIRECTORY PATH : ";
    string path;
    getline(cin, path);

    if (path.empty())
    {
        cout << "EMPTY PATH\n";
        return 0;
    }

    // intitialization
    int position = 0;
    int direction = 1;
    string scanstatus;
    int EntriesVisited = 0;
    const int Max_bar_length = 40;
    int folderCounter = 0;
    size_t FiletoDisplay = 0;
    long long sizeDivider = 1;
    int SkippedEntries = 0;
    const int SUCCESS = 0;
    const int filesystemerror = 1;
    const int Invalidpath = 2;

    // user menu variables
    string lower_input_extension;
    bool ValidInput = false;
    string input_lower;
    string compareable_category;
    long long byte_max = 0;
    long long byte_min = 0;
    double minimum_size_value = 0;
    string minimum_size_unit;
    double maximum_size_value = 0;
    string maximum_size_unit;
    string input_category;
    string input_extension;

    // vector
    vector<FileInfo> fileSearched;
    vector<FileInfo> filteredFilesbysize;
    vector<FileInfo> filteredFilesbycategory;
    vector<FileInfo> filteredFilesbyextension;
    vector<FileInfo> files;

    // flags
    bool isFile = false;
    bool scanComplete = true;

    // // maps-------------
    map<string, string> category = createCategoryMap();

    try
    {

        // ###########################################
        //                  SCANNER
        // ###########################################
        fs::directory_entry Directory{path};

        // ---------------------PATH VALIDATION-------------------------
        if (!Directory.exists())
        {
            cout << "DIRECTORY DOES NOT EXIST\n";
            return Invalidpath;
        }

        // ---------------file path------------------
        if (Directory.is_regular_file())
        {
            cout << "\033[33m" << "FILE PATH DETECTED\n"
                 << "\033[0m";
            auto filename = Directory.path().filename().stem().string();
            auto extension = Directory.path().extension().string();
            auto size = Directory.file_size();

            // parent path
            fs::path parentpath = Directory.path().parent_path();

            FileInfo file(filename, extension, parentpath.string(), size);
            files.push_back(file);
            EntriesVisited = 1;
            isFile = true;
        }
        // -------------directory path-------------------
        else if (Directory.is_directory())
        {
            cout << "\033[33m";
            cout << "DIRECTORY DETECTED" << endl;
            cout << "\033[0m";

            // manual iterator
            std::error_code ScanEc;
            fs::recursive_directory_iterator Scanit(path, fs::directory_options::skip_permission_denied, ScanEc);
            fs::recursive_directory_iterator end;

            // error code constructor exception handling
            if (ScanEc)
            {
                cout << "SCAN START ERROR:" << ScanEc.message() << endl;
                return filesystemerror;
            }

            // ---------------------------------------RECURSIVE SCANNING ---------------------------------------------
            while (Scanit != end)
            {
                const auto &entry = *Scanit;
                try
                {
                    if (entry.is_regular_file())
                    {

                        auto filename = entry.path().filename().stem().string();
                        auto extension = entry.path().extension().string();
                        auto size = entry.file_size();

                        // parent path
                        fs::path parentpath = entry.path().parent_path();
                        // relative path
                        fs::path relativefilepath = fs::relative(parentpath, path);

                        FileInfo file(filename, extension, relativefilepath.string(), size);

                        // obj data storing
                        files.push_back(file);
                    }
                    else
                    {
                        if (entry.is_directory())
                        {
                            folderCounter++;
                        }
                    }
                }

                catch (const fs::filesystem_error &e)
                {
                    SkippedEntries++;
                }

                EntriesVisited++;
                Scanit.increment(ScanEc);
                if (ScanEc)
                {
                    cout << "\r" << string(60, ' ') << "\r"; // bar remover
                    cout << "SCAN ITERATION FAILED : " << ScanEc.message() << endl;
                    scanComplete = false;
                    break;
                }
                // -------------------------scanning bar data--------------------------------------

                // bar location
                position += direction;
                if (position >= Max_bar_length - 1)
                {
                    direction = -direction;
                }
                else if (position < 0)
                {
                    direction = -direction;
                }

                if (EntriesVisited % 10 == 0)
                {
                    DynamicBar(position, Max_bar_length);
                }
            }
        }
        else
        {
            cout << "INVALID PATH\n";
            return Invalidpath;
        }
    }
    catch (const fs::filesystem_error &e)
    {
        cout << "FILE SYSTEM ERROR :" << e.what() << endl;
        return filesystemerror;
    }

    cout << "\r" << string(60, ' ') << "\r"; // bar remover
    if (scanComplete && SkippedEntries == 0)
    {
        cout << "\033[32m" << "Scanning complete!\n"
             << "\033[0m";
        scanstatus = "VALID";
    }
    else
    {
        cout << "\033[31m" << "Scanning interrupted!\n"
             << "\033[0m";
        scanstatus = "INVALID";
    }
    // #######################################
    //             SCANNER END
    // #######################################

    // ======================================
    //              ANALYZER
    // ======================================

    if (EntriesVisited == 0)
    {
        cout << "Empty Directory\n";
        return SUCCESS;
    }
    if (files.size() == 0)
    {
        if (folderCounter == 0)
        {
            cout << "NO FOLDER IN THIS DIRECTORY\n";
        }
        else
        {
            cout << "\033[32m" << "TOTAL FOLDER IN THIS DIRECTORY : " << "\033[0m" << folderCounter << endl;
        }
        cout << "NO REGULAR FILES IN THIS DIRECTORY\n";
        return SUCCESS;
    }
    else
    {
        // ______________________________________
        //      FILE/DIRECTORY  REPORTING
        // ______________________________________

        // -----------------------displaying single file info------------------------------

        if (isFile)
        {
            sizeDivider = files[0].getsize() / 3;
            if (sizeDivider == 0)
            {
                sizeDivider = 1;
            }
            reportFile(files[0], category);
        }
        else
        {
            AnalysisResult analysis = Analyzer(files, category);

            // sizeDivider calculation after maxSize is calculated
            sizeDivider = analysis.MaxSize / 3;
            if (sizeDivider == 0)
            {
                sizeDivider = 1;
            }

            // -----------------------displaying directory file info------------------------------

            // formating
            string printdatamax = format("{:.2f} {}", sizeconverter(analysis.MaxSize).first, sizeconverter(analysis.MaxSize).second);
            string printdatatotal = format("{:.2f} {}", sizeconverter(analysis.TotalSize).first, sizeconverter(analysis.TotalSize).second);

            if (SkippedEntries > 0)
            {
                cout << "\033[31m";
                cout << SkippedEntries << " ENTRIES SKIPPED DUE TO ACCESS/PERMISSION ERROR\n";
                cout << "\033[0m";
            }
            // ---------------------------scan summary----------------------
            cout << "\033[36m";
            cout << "\n";
            cout << "SCAN SUMMARY\n";
            cout << "----------------------------------------------------------\n";
            cout << "\033[0m";
            cout << left;
            cout << setw(20) << "FILES FOUND       : " << files.size() << endl;
            cout << setw(20) << "FOLDER FOUND      : " << folderCounter << endl;
            cout << setw(20) << "ENTRIES SCANNED   : " << EntriesVisited << endl;
            cout << setw(20) << "ENTRIES SKIPPED   : " << SkippedEntries << endl;
            cout << setw(20) << "TOTAL STORAGE     : " << printdatatotal << endl;
            cout << setw(20) << "LARGEST FILE SIZE : " << printdatamax << endl;
            cout << setw(20) << "LARGE FILE NAME   : " << analysis.MaxFileName << endl;
            cout << setw(20) << "TOTAL EXTENSION   : " << analysis.storage.size() << endl;
            cout << setw(20) << "SCAN STATUS       : ";
            if (scanstatus == "VALID")
            {
                cout << "\033[32m" << scanstatus << "\033[0m" << endl;
            }
            else
            {
                cout << "\033[31m" << scanstatus << "\033[0m" << endl;
            }
            cout << right;

            // -------------------storage analysis---------------------
            cout << "\033[36m";
            cout << "\n";
            cout << "STORAGE ANALYSIS \n";
            cout << "----------------------------------------------------------\n";
            cout << "\033[0m";

            for (const auto &item : analysis.categoryStorage)
            {
                string printdata = format("{:.2f} {}",
                                          sizeconverter(item.second).first,
                                          sizeconverter(item.second).second); // bytes to kb,mb,gb convert

                // safe percentage lookup using the same extension key
                auto percentageIt = analysis.categorystoragePercentage.find(item.first);

                if (percentageIt != analysis.categorystoragePercentage.end())
                {
                    double percentage = percentageIt->second;

                    cout << setw(15) << left << item.first;

                    StaticBar(percentage, Max_bar_length);
                    cout << " : " << printdata << endl;
                }
                else
                {
                    cout << "CATEGORY PERCENTAGE DATA NOT FOUND FOR : " << item.first << endl;
                }
            }
            // ------------------- largest file------------------
            cout << "\033[36m";
            cout << "\n";
            cout << "LARGEST FILE\n";
            cout << "----------------------------------------------------------\n";
            cout << "\033[0m";
            // --------------------------------------------calculation--------------------------------------------
            // calculating filetodisplay
            FiletoDisplay = min(files.size(), size_t(5));

            // creating a copy vector--> so that the orignal scan order remains same
            vector<FileInfo> copy_fileSorter;
            copy_fileSorter = files;

            sort(copy_fileSorter.begin(), copy_fileSorter.end(),
                 [](const FileInfo &a, const FileInfo &b)
                 { return a.getsize() > b.getsize(); });
            //  [] -> yeh hai lambda function new chiz sikhe hai...

            DisplayFileTable(copy_fileSorter, sizeDivider, FiletoDisplay);

            cout << "\nYOUR PATH :  " << "\033[32m" << path << "\033[0m" << endl
                 << endl;
        }
    }

    // ---------------------------
    // USER MENU
    // ---------------------------

    int user_choice_main_menu = 0;
    int user_choice = 0;

    cout << "---------------MAIN MENU---------------\n";
    do
    {
        cout << "[1] FILE FILTER\n";
        cout << "[2] FILE SEARCH\n";
        cout << "[3] FILE SORT\n";
        cout << "[4] FOLDER SEPARATION\n";
        cout << "[5] FILE REPORT\n";
        cout << "[6] EXIT\n";
        cout << "YOUR CHOICE :";
        cin >> user_choice_main_menu;
        if (cin.fail())
        {
            cout << "\033[31m";
            cout << "INVALID OPTION\n";
            cout << "\033[0m";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (user_choice_main_menu)
        {
        case 1:
        {
            // ###############################
            // FILE FILTER MENU
            // ###############################
            cout << "--------------------------------\n";
            cout << "          " << "\033[33m" << "FILE FILTER\n"
                 << "\033[0m";
            cout << "--------------------------------\n";
            do
            {
                cout << "[1] EXTENSION FILTER\n";
                cout << "[2] CATEGORY FILTER\n";
                cout << "[3] SIZE FILTER\n";
                cout << "[4] GOING BACK TO MAIN MENU\n";
                cout << "YOUR CHOICE : ";
                cin >> user_choice;
                if (cin.fail())
                {
                    cout << "\033[31m";
                    cout << "INVALID OPTION\n";
                    cout << "\033[0m";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                switch (user_choice)
                {
                case 1:
                    // extension wise filtering
                    cout << "ENTER THE EXTENSION TO FILTER :";
                    // cin.ignore();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    getline(cin, input_extension);
                    lower_input_extension = tolowerCASE(input_extension);
                    for (const auto &file : files)
                    {
                        if (lower_input_extension == tolowerCASE(file.getextension()))
                        {
                            filteredFilesbyextension.push_back(file);
                        }
                    }

                    if (filteredFilesbyextension.empty())
                    {
                        cout << "\033[31m" << "NO FILES FOUND FOR EXTENSION " << input_extension << "\033[0m" << "\n";
                    }
                    else
                    {
                        cout << "\033[0m" << "FILE FOUNDED : " << "\033[32m" << filteredFilesbyextension.size() << "\033[0m" << endl;

                        cout << "[1] VIEW FILES DETAILS\n";
                        cout << "[2] RETURN TO BACK MENU\n";
                        cout << "YOUR CHOICE : ";
                        int option = 0;
                        cin >> option;
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            filteredFilesbyextension.clear();
                        }
                        else
                        {
                            switch (option)
                            {

                            case 1:
                            {
                                DisplayFileTable(filteredFilesbyextension, sizeDivider, filteredFilesbyextension.size());
                                filteredFilesbyextension.clear();
                                break;
                            }
                            case 2:
                                cout << "\033[32m";
                                cout << "GOING BACK \n";
                                filteredFilesbyextension.clear();
                                cout << "\033[0m";
                                break;

                            default:
                            {
                                cout << "\033[32m";
                                cout << "INVALID OPTION\n";
                                filteredFilesbyextension.clear();
                                cout << "\033[0m";
                                break;
                            }
                            }
                        }
                        break;
                    case 2:
                        // cout<<""; -> sare avalibe category ka name show karna hai
                        cout << "ENTER THE CATEGORY TO FILTER :";
                        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        getline(cin, input_category);

                        input_lower = tolowerCASE(input_category);

                        for (const auto &item : files)
                        {
                            auto extension = item.getextension();

                            compareable_category = tolowerCASE(getcategory(extension, category));

                            if (input_lower == compareable_category)
                            {
                                filteredFilesbycategory.push_back(item);
                            }
                        }

                        if (filteredFilesbycategory.empty())
                        {
                            cout << "\033[31m" << "NO FILES FOUND FOR CATEGORY " << input_category << "\033[0m" << "\n";
                        }
                        else
                        {

                            cout << "TOTAL FILES FOUND : " << "\033[32m" << filteredFilesbycategory.size() << "\033[0m" << endl;

                            cout << "[1] VIEW FILES DETAILS\n";
                            cout << "[2] RETURN TO BACK MENU\n";
                            cout << "YOUR CHOICE : ";
                            int option = 0;
                            cin >> option;
                            if (cin.fail())
                            {
                                cout << "\033[31m";
                                cout << "INVALID OPTION \n";
                                cout << "\033[0m";
                                // error state reset
                                cin.clear();
                                // buffer cleaning
                                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                filteredFilesbycategory.clear();
                            }
                            else
                            {
                                switch (option)
                                {

                                case 1:
                                {

                                    DisplayFileTable(filteredFilesbycategory, sizeDivider, filteredFilesbycategory.size());
                                    input_lower.clear();
                                    filteredFilesbycategory.clear();
                                    break;
                                }
                                case 2:
                                {
                                    cout << "\033[32m";
                                    cout << "GOING BACK \n";
                                    input_lower.clear();
                                    filteredFilesbycategory.clear();
                                    cout << "\033[0m";
                                    break;
                                }
                                default:
                                {
                                    cout << "\033[32m";
                                    cout << "INVALID OPTION\n";
                                    input_lower.clear();
                                    filteredFilesbycategory.clear();
                                    cout << "\033[0m";
                                    break;
                                }
                                }
                            }
                        }
                        break;
                    case 3:

                        do
                        {
                            // reset
                            ValidInput = false;
                            // -----------------minmum size-------------------
                            cout << "MINIMUM SIZE\n";

                            do
                            {
                                cout << "Enter the value : ";
                                cin >> minimum_size_value;
                                if (cin.fail())
                                {
                                    cout << "\033[31m";
                                    cout << "INVALID OPTION \n";
                                    cout << "\033[0m";
                                    // error state reset
                                    cin.clear();
                                    // buffer cleaning
                                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                else if (minimum_size_value < 0)
                                {
                                    cout << "\033[31m";
                                    cout << "FILE SIZE CANNOT BE NEGATIVE!\n";
                                    cout << "\033[0m";
                                }
                                else
                                {
                                    ValidInput = true;
                                }
                            } while (!ValidInput);

                            // reset
                            ValidInput = false;

                            cout << "Enter the unit  : ";
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            getline(cin, minimum_size_unit);

                            // ------------------------maximum size------------------------
                            cout << "MAXIMUM SIZE\n";
                            do
                            {
                                cout << "Enter the value : ";
                                cin >> maximum_size_value;
                                if (cin.fail())
                                {
                                    cout << "\033[31m";
                                    cout << "INVALID OPTION \n";
                                    cout << "\033[0m";
                                    // error state reset
                                    cin.clear();
                                    // buffer cleaning
                                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                else if (maximum_size_value < 0)
                                {
                                    cout << "\033[31m";
                                    cout << "FILE SIZE CANNOT BE NEGATIVE!\n";
                                    cout << "\033[0m";
                                }
                                else
                                {
                                    ValidInput = true;
                                }
                            } while (!ValidInput);
                            cout << "Enter the unit  : ";
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            getline(cin, maximum_size_unit);

                            // ---------------byte conversion-----------------------
                            byte_min = convertTobytes(minimum_size_value, tolowerCASE(minimum_size_unit));
                            byte_max = convertTobytes(maximum_size_value, tolowerCASE(maximum_size_unit));
                            // -------------------error convertion---------------
                            if (byte_min == 1 || byte_max == 1)
                            {
                                cout << "\033[31m";
                                cout << "ERROR IN CONVERTING VALUE IN BYTES!\n";
                                cout << "\033[0m";
                                continue;
                            }
                            // -----------------range validation---------------------
                            if (byte_min > byte_max)
                            {
                                cout << "\033[31m";
                                cout << "INVALID RANGE!!\n";
                                cout << "minimum range cannot be greater then maximum range\n";
                                cout << "\033[0m";
                                continue;
                            }
                            if (byte_min == byte_max)
                            {
                                cout << "\033[31m";
                                cout << "INVALID RANGE!!\n";
                                cout << "minimum range cannot be equal to maximum range\n";
                                cout << "\033[0m";
                                continue;
                            }
                            break;
                        } while (true);
                        // -------------------------result-----------------------
                        for (const auto &item : files)
                        {
                            if (byte_min <= item.getsize() && item.getsize() <= byte_max)
                            {
                                filteredFilesbysize.push_back(item);
                            }
                        }

                        if (filteredFilesbysize.empty())
                        {
                            cout << "\033[31m" << "NO FILES FOUND FOR RANGE : " << minimum_size_value << minimum_size_unit << " To " << maximum_size_value << maximum_size_unit << "\033[0m" << "\n";
                        }
                        else
                        {
                            cout << "TOTAL FILES FOUND : " << "\033[32m" << filteredFilesbysize.size() << "\033[0m" << endl;

                            cout << "[1] VIEW FILES DETAILS\n";
                            cout << "[2] RETURN TO BACK MENU\n";
                            cout << "YOUR CHOICE : ";
                            int option = 0;
                            cin >> option;
                            if (cin.fail())
                            {
                                cout << "\033[31m";
                                cout << "INVALID OPTION \n";
                                cout << "\033[0m";
                                // error state reset
                                cin.clear();
                                // buffer cleaning
                                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                filteredFilesbysize.clear();
                            }
                            else
                            {
                                switch (option)
                                {

                                case 1:
                                {
                                    DisplayFileTable(filteredFilesbysize, sizeDivider, filteredFilesbysize.size());
                                    filteredFilesbysize.clear();
                                    break;
                                }
                                case 2:
                                {
                                    cout << "\033[32m";
                                    cout << "GOING BACK \n";
                                    filteredFilesbysize.clear();
                                    cout << "\033[0m";
                                    break;
                                }
                                default:
                                {
                                    cout << "\033[32m";
                                    cout << "INVALID OPTION\n";
                                    filteredFilesbysize.clear();
                                    cout << "\033[0m";
                                    break;
                                }
                                }
                            }
                        }
                        break;
                    case 4:

                        cout << "\033[32m";
                        cout << "RETURNNING TO MAIN MENU....\n";
                        cout << "\033[0m";
                        break;

                    default:
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                        else
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                        }
                        break;
                    }
                }
            } while (user_choice != 4);
            break;
        }
        case 2:
        {

            // ###############################
            // FILE SEARCH MENU
            // ###############################
            cout << "--------------------------------\n";
            cout << "          " << "\033[33m" << "FILE SEARCH\n"
                 << "\033[0m";
            cout << "--------------------------------\n";

            string FileNameSearch;
            cout << "ENTER THE NAME OF FILE :";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            getline(cin, FileNameSearch);
            FileNameSearch = trim(FileNameSearch);
            if (FileNameSearch.empty())
            {
                cout << "\033[31m";
                cout << "YOU HAVE NOT ENTERED THE FILENAME\n";
                cout << "\033[0m";
                break;
            }

            string FullFileName;
            FileNameSearch = tolowerCASE(FileNameSearch);
            for (const auto &item : files)
            {
                FullFileName = tolowerCASE(item.getfilename() + item.getextension());

                if (FullFileName.find(FileNameSearch) != string::npos)
                {
                    fileSearched.push_back(item);
                }
            }

            if (fileSearched.empty())
            {
                cout << "\033[31m" << "NO FILES FOUND FOR NAME : " << FileNameSearch << "\033[0m" << "\n";
            }
            else
            {
                cout << "TOTAL FILES FOUND : " << "\033[32m" << fileSearched.size() << "\033[0m" << endl;

                cout << "[1] VIEW FILES DETAILS\n";
                cout << "[2] RETURN TO BACK MENU\n";
                cout << "YOUR CHOICE : ";
                int option = 0;
                cin >> option;
                if (cin.fail())
                {
                    cout << "\033[31m";
                    cout << "INVALID OPTION \n";
                    cout << "\033[0m";
                    // error state reset
                    cin.clear();
                    // buffer cleaning
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    fileSearched.clear();
                }
                else
                {
                    switch (option)
                    {

                    case 1:
                    {
                        DisplayFileTable(fileSearched, sizeDivider, fileSearched.size());
                        fileSearched.clear();
                        break;
                    }
                    case 2:
                    {
                        cout << "\033[32m";
                        cout << "GOING BACK \n";
                        fileSearched.clear();
                        cout << "\033[0m";
                        break;
                    }
                    default:
                    {
                        cout << "\033[32m";
                        cout << "INVALID OPTION\n";
                        fileSearched.clear();
                        cout << "\033[0m";
                        break;
                    }
                    }
                }
            }
            break;
        }
        case 3:
        {
            // ###############################
            // FILE SORT MENU
            // ###############################
            cout << "--------------------------------\n";
            cout << "          " << "\033[33m" << "FILE SORT\n"
                 << "\033[0m";
            cout << "--------------------------------\n";

            int user_choice_fileSort_menu = 0;
            do
            {
                cout << "[1] SORT BY SIZE\n";
                cout << "[2] SORT BY NAME\n";
                cout << "[3] SORT BY EXTENSION\n";
                cout << "[4] SORT BY DATE\n";
                cout << "[5] BACK\n";
                cout << "YOUR CHOICE : ";
                cin >> user_choice_fileSort_menu;
                if (cin.fail())
                {
                    cout << "\033[31m";
                    cout << "INVALID OPTION \n";
                    cout << "\033[0m";
                    // error state reset
                    cin.clear();
                    // buffer cleaning
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                switch (user_choice_fileSort_menu)
                {
                case 1:
                {
                    int option = 0;
                    do
                    {
                        cout << endl;
                        cout << "SORT BY SIZE\n";
                        cout << endl;
                        cout << "[1] LARGEST -> SMALLEST\n";
                        cout << "[2] SMALLEST -> LARGEST\n";
                        cout << "[3] BACK\n";
                        cout << "YOUR CHOICE : \n";
                        cin >> option;
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }

                        vector<FileInfo> copy_fileSorter;

                        switch (option)
                        {
                        case 1:
                        {
                            // -----------------------------------------------------------
                            // largest to smallest
                            // -----------------------------------------------------------
                            // creating a copy vector--> so that the orignal scan order remains same
                            copy_fileSorter = files;

                            sort(copy_fileSorter.begin(), copy_fileSorter.end(),
                                 [](const FileInfo &a, const FileInfo &b)
                                 { return a.getsize() > b.getsize(); });
                            //  [] -> yeh hai lambda function new chiz sikhe hai...

                            if (copy_fileSorter.empty())
                            {
                                cout << "\033[31m" << "NO FILES SORTED!! " << "\033[0m" << "\n";
                            }
                            else
                            {
                                cout << "TOTAL FILES SORTED : " << "\033[32m" << copy_fileSorter.size() << "\033[0m" << endl;

                                cout << "[1] VIEW FILES DETAILS\n";
                                cout << "[2] RETURN TO BACK MENU\n";
                                cout << "YOUR CHOICE : ";
                                int option = 0;
                                cin >> option;
                                if (cin.fail())
                                {
                                    cout << "\033[31m";
                                    cout << "INVALID OPTION \n";
                                    cout << "\033[0m";
                                    // error state reset
                                    cin.clear();
                                    // buffer cleaning
                                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                else
                                {
                                    switch (option)
                                    {

                                    case 1:
                                    {
                                        DisplayFileTable(copy_fileSorter, sizeDivider, copy_fileSorter.size());
                                        copy_fileSorter.clear();
                                        break;
                                    }
                                    case 2:
                                    {
                                        cout << "\033[32m";
                                        cout << "GOING BACK \n";
                                        copy_fileSorter.clear();
                                        cout << "\033[0m";
                                        break;
                                    }
                                    default:
                                    {
                                        cout << "\033[32m";
                                        cout << "INVALID OPTION\n";
                                        copy_fileSorter.clear();
                                        cout << "\033[0m";
                                        break;
                                    }
                                    }
                                }
                                break;
                            }
                        case 2:
                        {
                            // -----------------------------------------------------------
                            // smallest to largest
                            // -----------------------------------------------------------
                            // creating a copy vector--> so that the orignal scan order remains same
                            vector<FileInfo> copy_fileSorter;
                            copy_fileSorter = files;

                            sort(copy_fileSorter.begin(), copy_fileSorter.end(),
                                 [](const FileInfo &a, const FileInfo &b)
                                 { return a.getsize() < b.getsize(); });
                            //  [] -> yeh hai lambda function new chiz sikhe hai...

                            if (copy_fileSorter.empty())
                            {
                                cout << "\033[31m" << "NO FILES SORTED!! " << "\033[0m" << "\n";
                            }
                            else
                            {
                                cout << "TOTAL FILES SORTED : " << "\033[32m" << copy_fileSorter.size() << "\033[0m" << endl;

                                cout << "[1] VIEW FILES DETAILS\n";
                                cout << "[2] RETURN TO BACK MENU\n";
                                cout << "YOUR CHOICE : ";
                                int option =0;
                                cin >> option;
                                if (cin.fail())
                                {
                                    cout << "\033[31m";
                                    cout << "INVALID OPTION \n";
                                    cout << "\033[0m";
                                    // error state reset
                                    cin.clear();
                                    // buffer cleaning
                                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                }
                                else
                                {
                                    switch (option)
                                    {

                                    case 1:
                                    {
                                        DisplayFileTable(copy_fileSorter, sizeDivider, copy_fileSorter.size());
                                        copy_fileSorter.clear();
                                        break;
                                    }
                                    case 2:
                                    {
                                        cout << "\033[32m";
                                        cout << "GOING BACK \n";
                                        copy_fileSorter.clear();
                                        cout << "\033[0m";
                                        break;
                                    }
                                    default:
                                    {
                                        cout << "\033[32m";
                                        cout << "INVALID OPTION\n";
                                        copy_fileSorter.clear();
                                        cout << "\033[0m";
                                        break;
                                    }
                                    }
                                }
                            }
                            break;
                        }
                        case 3:
                        {
                            cout << "\033[32m";
                            cout << "GOING BACK....\n";
                            cout << "\033[0m";
                            break;
                        }
                        default:
                        {

                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                        }
                        }
                        }
                    } while (option != 3);

                    break;
                }
                case 2:
                {
                    int option;
                    do
                    {
                        cout << endl;
                        cout << "SORT BY NAME\n";
                        cout << endl;
                        cout << "[1] A -> Z\n";
                        cout << "[2] Z -> A\n";
                        cout << "[3] BACK\n";
                        cout << "YOUR CHOICE : \n";
                        cin >> option;
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }
                        switch (option)
                        {
                        case 1:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 2:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 3:
                        {
                            cout << "\033[32m";
                            cout << "GOING BACK....\n";
                            cout << "\033[0m";
                            break;
                        }
                        default:
                        {

                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                        }
                        }
                    } while (option != 3);
                    break;
                }
                case 3:
                {
                    int option;
                    do
                    {
                        cout << endl;
                        cout << "SORT BY EXTENSION\n";
                        cout << endl;
                        cout << "[1] A -> Z\n";
                        cout << "[2] Z -> A\n";
                        cout << "[3] BACK\n";
                        cout << "YOUR CHOICE : \n";
                        cin >> option;
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }
                        switch (option)
                        {
                        case 1:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 2:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 3:
                        {
                            cout << "\033[32m";
                            cout << "GOING BACK....\n";
                            cout << "\033[0m";
                            break;
                        }
                        default:
                        {

                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                        }
                        }
                    } while (option != 3);
                    break;
                }
                case 4:
                {
                    int option;
                    do
                    {
                        cout << endl;
                        cout << "SORT BY DATE\n";
                        cout << endl;
                        cout << "[1] NEWEST -> OLDEST\n";
                        cout << "[2] OLDEST -> NEWEST\n";
                        cout << "[3] BACK\n";
                        cout << "YOUR CHOICE : \n";
                        cin >> option;
                        if (cin.fail())
                        {
                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                            // error state reset
                            cin.clear();
                            // buffer cleaning
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            continue;
                        }
                        switch (option)
                        {
                        case 1:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 2:
                        {
                            cout << "\033[31m";
                            cout << "UNDER DEVELOPMENT \n";
                            cout << "\033[0m";
                            break;
                        }
                        case 3:
                        {
                            cout << "\033[32m";
                            cout << "GOING BACK....\n";
                            cout << "\033[0m";
                            break;
                        }
                        default:
                        {

                            cout << "\033[31m";
                            cout << "INVALID OPTION \n";
                            cout << "\033[0m";
                        }
                        }
                    } while (option != 3);
                    break;
                }
                case 5:
                {
                    cout << "\033[32m";
                    cout << "GOING BACK...\n";
                    cout << "\033[0m";
                    break;
                }
                default:
                {
                    if (cin.fail())
                    {
                        cout << "\033[31m";
                        cout << "INVALID OPTION \n";
                        cout << "\033[0m";
                        // error state reset
                        cin.clear();
                        // buffer cleaning
                        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    else
                    {
                        cout << "\033[31m";
                        cout << "INVALID OPTION \n";
                        cout << "\033[0m";
                    }
                }
                }
            } while (user_choice_fileSort_menu != 5);

            break;
        }
        case 4:
        {
            cout << "\033[31m";
            cout << "UNDER DEVELOPMENT \n";
            cout << "\033[0m";
            break;
        }
        case 5:
        {
            cout << "\033[31m";
            cout << "UNDER DEVELOPMENT \n";
            cout << "\033[0m";
            break;
        }
        case 6:
        {
            cout << "\033[32m";
            cout << "EXITING......\n";
            cout << "\033[0m";
            break;
        }
        default:
        {
            if (cin.fail())
            {
                cout << "\033[31m";
                cout << "INVALID OPTION \n";
                cout << "\033[0m";
                // error state reset
                cin.clear();
                // buffer cleaning
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else
            {
                cout << "\033[31m";
                cout << "INVALID OPTION \n";
                cout << "\033[0m";
            }
            break;
        }
        }
    } while (user_choice_main_menu != 6);
    return SUCCESS;
}
