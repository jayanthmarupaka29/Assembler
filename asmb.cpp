   /*  NAME:-        G SREE HARSHA PRANEETH 
    ROLL NO:-     2101Cs_29 */

#include <bits/stdc++.h>
#include <stdlib.h> 
using namespace std;

vector<string> GlobErrors; // During the compiling, we store all the errors
vector<string> mnemonics; // all the mnemonics enocoded
vector<string> GlobWarnings; // During the compiling,  we store all the warnings

vector<string> ListFile; //It shows how the instructions are stored inside the memory
vector<string> ObjFile; // The converted file which is in binary is stored here
set<string> Labels_used;
map<int, int> Instr_Line;
map<string, int> Optab;

void init()
{
    mnemonics = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj","a2sp","sp2a","call","return","brz","brlz","br","HALT","data"};
}

//function for fidiing opcide for given mneumonic
int get_opcode (string& opcode_cur) {
    for (int i = 0; i < (int)mnemonics.size(); i++) {
        if (mnemonics[i] == opcode_cur) {
            return i;
        }
    }
    return -1;
}

//converts given decimal number to string
string numb_to_str(int num)
{
    string num_str;
    while (num) {
        num_str.push_back('0' + num%10);
        num /= 10;
    }
    reverse(num_str.begin(), num_str.end());
    return num_str;
}

//chceking the character as alphabet or not
bool has_char (string &str) 
{
    for (char ch : str) {
        if (ch >= 'A' and ch <= 'Z')
            return true;
        if (ch >= 'a' and ch <= 'z') 
            return true;

    }
    return false;
}

//funtion for decimal to hexadecimal
string to_hdecimal (int deci_value, int len)
{
    std::stringstream ss;
    ss<< std::hex << deci_value; // int deci_value
    std::string res (ss.str());
    // Append 0's at back to make size = 4;
    reverse(res.begin(), res.end());
    while ((int)res.size() < len) {
        res.push_back('0');
    }
    while ((int)res.size() > len) {
        res.pop_back(); 
    }
    reverse(res.begin(), res.end());
    return res;
}

//checl the type of operand
pair<long int, bool> read_operand (string &operand)
{
    if ((int)operand.size() == 0) {
        return {0, true};
    }
    int len = (int)operand.size();
    char *str = (char *)malloc(len *  sizeof(char));
    for (int i = 0; i < (int)operand.size(); i++) {
        str[i] = operand[i];
    }
    for (int i = (int)operand.size(); i < strlen(str); i++) {
        str[i] = '\0';
    }
    char *end;
    long int num;
    num = strtol(str, &end, 10);
    if (!*end) {
        return {num, true};
    }
    num = strtol(str, &end, 8);
    if (!*end) {
        return {num, true};
    }
    num = strtol(str, &end, 16);
    if (!*end) {
        return {num, true};
    }
    return {-1, false};
}

//checking the validity of label
bool correct_label(string &str)
{
    if ((int)str.size() == 0) {
        return false;
    }

    if (str[0] >= 'a' and str[0] <= 'z') {
        return true;
    }
    if (str[0] >= 'A' and str[0] <= 'Z') {
        return true;
    }
    return false;
}

// mapping pc to label 
map <string, int> mapLabels (vector<string> &Read_Lines)
{
    map<string, int> label_adrr;
    int count_intr = 0;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        // Counting the number of ":" int count_label_triggers
        int count_label_triggers = 0;
        for (char ch : Read_Lines[i]) {
            count_label_triggers += (ch == ':');
        }
        if (count_label_triggers == 0) {
            count_intr = count_intr + 1;
            continue;
        }

        // now we are going ot check ll the errors
        if (count_label_triggers > 1) {
            string error = "There is a incorrect label format at the line number ";
            error = error + numb_to_str(Instr_Line[i]);
            GlobErrors.push_back(error);        
            continue;
        }
        string label;
        for (char ch : Read_Lines[i]) {
            if (ch == ':')
                break;
            label.push_back(ch);
        }
        if (!correct_label(label)) {
            string error = "There is a incorrect label naming format at the line number";
            error = error + numb_to_str(Instr_Line[i]);
            GlobErrors.push_back(error);
            continue;   
        }
        if (get_opcode(label) != -1) {
            string error = "Label name \"" + label + "\" is a keyword at line ";
            error = error + numb_to_str(Instr_Line[i]);
            GlobErrors.push_back(error);
            continue;
        }
        if (label_adrr.find(label) != label_adrr.end()) {
            string error = "There is a repeated label at the line number ";
            error = error + numb_to_str(Instr_Line[i]);
            GlobErrors.push_back(error);
            continue;
        }
        label_adrr[label] = count_intr;
        stringstream ss(Read_Lines[i]);
        string s_1, s_2, s_3;
        ss >> s_1;
        ss >> s_2;
        ss >> s_3;
        if ((int)s_2.size() > 0) {
            count_intr = count_intr + 1;
        }
        if (s_2 == "SET") {
            label_adrr[label] = read_operand(s_3).first;      //verifying set labels
        }
    }
    return label_adrr;
}

void remove_comments_spaces(vector<string> &Read_Lines)         //trimming the code by removing unwanted delimiter space
{

    vector<string> modified_lines;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        string before_comment;
        bool initial_white_space = true;
        for (char ch : Read_Lines[i]) {
            if (ch == ';') 
                break;
            if (ch != ' ' and ch != '\t') {
                initial_white_space = false;
            }
            if (initial_white_space && ch == ' ' or ch == '\t') {
                continue;
            }
            before_comment.push_back(ch);
        }
        if (has_char(before_comment)) {
            Instr_Line[(int)modified_lines.size()] = i + 1;
            modified_lines.push_back(before_comment);
            // cout << before_comment << endl;
        }
    }
    Read_Lines = modified_lines;
}



void Optable_init(){
    // 1 means it requires value, 2 means it requires offset
    Optab["data"] = 1 ;
    Optab["ldc"] = 1;
    Optab["adc"] = 1;
    Optab["ldl"] = 1;
    Optab["stl"] = 1;
    Optab["ldnl"] = 1;
    Optab["stnl"] = 1 ;
    Optab["add"] = 0;
    Optab["sub"] = 0;
    Optab["shl"] = 0;
    Optab["shr"] = 0;
    Optab["adj"] = 1 ;
    Optab["a2sp"] = 0 ;
    Optab["sp2a"] = 0 ;
    Optab["call"] = 2 ;
    Optab["return"] = 0 ;
    Optab["brz"] = 2 ;
    Optab["brlz"] = 2 ;
    Optab["br"] = 2 ;
    Optab["halt"] = 0 ;
    Optab["set"] = 1 ;
    // 0 means no operand is required
}



int main(int argc, char* argv[])
{
    freopen(argv[1], "r", stdin);
    init(); // here we are initialising mnemonics
    vector<string> Read_Lines;
    string s;
    while (getline(cin, s))
    {
        Read_Lines.push_back(s);
    }
    // Clearing all the data by removing comments and whitespaces from the lines
    remove_comments_spaces(Read_Lines);
    map<string, int> label_adrr = mapLabels(Read_Lines);
    // for (auto it : label_adrr) {
    //     cout << it.first << ' ' << it.second << '\n'; 
    // }
    // Pass 1 completed
    
    Optable_init();                                             //we are creating the symbol table
    int count_intr = 0;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        stringstream ss(Read_Lines[i]);
        string opcode_cur;
        ss >> opcode_cur;
        if (opcode_cur.back() != ':') {
            int opcodeVal = get_opcode(opcode_cur);
            if (opcodeVal == -1) {
                string error = "There is a bogus mnemonic at the line number ";
                error = error + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            stringstream check(Read_Lines[i]);
            string str_1; int count = 0;
            while (check >> str_1) {
                //cerr << str << endl;
                count=count+1;
            }
            if (count > 2) {
                string error = "There are more operands than expected at the line number " + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            if (Optab[opcode_cur] == 2) {
                string operand; ss >> operand;
                Labels_used.insert(operand);
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand] - count_intr - 1, 6) + to_hdecimal(opcodeVal, 2);
                if (opcode_cur == "call") {
                    code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand], 6) + to_hdecimal(opcodeVal, 2);
                }
                //we are finding the machine code and going to write it in the object file and listing file
                ListFile.push_back(code + " " + Read_Lines[i]);             
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            }

            //finding opcode for data case
            if (opcode_cur == "data") {
                string operand; ss >> operand;
                if ((int)operand.size() == 0) {
                    string error = "There is a Operand missing at the line number " + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                pair<long int, bool> val = read_operand(operand);
                if (!val.second) {
                    string error = "There is invalid value format at the line number ";
                    error = error + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                if (val.first > INT_MAX or val.first < INT_MIN) {
                    string warn = "There is integer overflow at the line number " + numb_to_str(Instr_Line[i]);
                    GlobWarnings.push_back(warn);
                }
                string code = to_hdecimal(count_intr = count_intr + 1, 4) + " " + to_hdecimal(val.first, 8);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                continue;
            }
            string operand;
            ss >> operand;
            Labels_used.insert(operand);
            if (Optab[opcode_cur] && (int)operand.size() == 0) {
                string error = "There is a operand missing at the line number " + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            } else if (!Optab[opcode_cur] && (int)operand.size()) {
                string error = "There is a extra Operand at the line number " + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            if ((int)operand.size() > 0 and operand[0] >= 'a' and operand[0] <= 'z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at the line number " + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand] , 6) + to_hdecimal(opcodeVal, 2);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            } else if ((int)operand.size() > 0 and operand[0] >= 'A' and operand[0] <= 'Z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at the line number " + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand] , 6) + to_hdecimal(opcodeVal, 2);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            }
            pair<long int, bool> val = read_operand(operand);
            if (!val.second) {
                string error = "There is a invalid operand format at the line number ";
                error = error + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            string code = to_hdecimal(count_intr = count_intr + 1, 4) + " " + to_hdecimal(val.first, 6) + to_hdecimal(opcodeVal, 2);
            ListFile.push_back(code + " " + Read_Lines[i]);
            ObjFile.push_back(code);
        } else {
            string next_opcode;
            ss >> next_opcode;
            string prev_opcode = opcode_cur;
            opcode_cur = next_opcode;
            stringstream check(Read_Lines[i]);
            string str_1; int count = 0;
            while (check >> str_1) {
                //cerr << str << endl;
                count = count + 1;
            }
            if (count > 3) {
                string error = "There are more operands than expected at the line number " + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            if ((int)opcode_cur.size() == 0) {
                ListFile.push_back(to_hdecimal(count_intr, 4) + " " + Read_Lines[i]);
                continue;
            }

            //finding opcode for set case
            if (opcode_cur == "SET") {
                string operand; 
                ss >> operand;
                pair<long int, bool> val = read_operand(operand);
                if (!val.second or (int)operand.size()==0) {
                    string error = "THer is a missing operand at the line number ";
                    error = error + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr = count_intr + 1, 4) + " " + to_hdecimal(val.first, 8);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                opcode_cur.pop_back();
                prev_opcode.pop_back();
                // label_adrr[prev_opcode] = val.first;
                continue;
            }
            if (opcode_cur == "data") {
                string operand; ss >> operand;
                if ((int)operand.size() == 0) {
                    string error = "There is a operand missing at the line number " + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                //finding the operand type
                pair<long int, bool> val = read_operand(operand);
                if (!val.second) {
                    string error = "There is a invalid value format at the line number ";
                    error = error + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                } 
                if (val.first > INT_MAX or val.first < INT_MIN) {
                    string warn = "There is a integer overflow at the line number " + numb_to_str(Instr_Line[i]);
                    GlobWarnings.push_back(warn);
                }
                string code = to_hdecimal(count_intr = count_intr + 1, 4) + " " + to_hdecimal(val.first, 8);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                continue;
            }
            int opcodeVal = get_opcode(opcode_cur);
            if (opcodeVal == -1) {
                string error = "There is a Bogus mnemonic at the line number";
                error = error + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            if (Optab[opcode_cur] == 2) {
                string operand; ss >> operand;
                Labels_used.insert(operand);
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at the line number" + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand] - count_intr - 1, 6) + to_hdecimal(opcodeVal, 2);
                if (opcode_cur == "call") {
                    code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand], 6) + to_hdecimal(opcodeVal, 2);
                }
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            }
            string operand;
            ss >> operand;
            Labels_used.insert(operand);
            //we are finding the no operand is found when there is a mneumonic with 1 operand
            if (Optab[opcode_cur] && (int)operand.size() == 0) {
                string error = "There is a operand missing at the line number" + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            } else if (!Optab[opcode_cur] && (int)operand.size()) {
                string error = "There is a xtra Operand at the line number" + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            if ((int)operand.size() > 0 and operand[0] >= 'a' and operand[0] <= 'z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at the line number" + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand], 6) + to_hdecimal(opcodeVal, 2);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            } else if ((int)operand.size() > 0 and operand[0] >= 'A' and operand[0] <= 'Z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at the line number" + numb_to_str(Instr_Line[i]);
                    GlobErrors.push_back(error);
                    continue;
                }
                string code = to_hdecimal(count_intr, 4) + " " + to_hdecimal(label_adrr[operand], 6) + to_hdecimal(opcodeVal, 2);
                ListFile.push_back(code + " " + Read_Lines[i]);
                ObjFile.push_back(code);
                count_intr = count_intr + 1;
                continue;
            }
            pair<long int, bool> val = read_operand(operand);
            if (!val.second) {
                string error = "There is a invalid operand format at the line number ";
                error = error + numb_to_str(Instr_Line[i]);
                GlobErrors.push_back(error);
                continue;
            }
            string code = to_hdecimal(count_intr = count_intr + 1, 4) + " " + to_hdecimal(val.first, 6) + to_hdecimal(opcodeVal, 2);
            ListFile.push_back(code + " " + Read_Lines[i]);
            ObjFile.push_back(code);
        }
    }
    string file_name;
    for (int i = 0; i < strlen(argv[1]); i++) {
        if (argv[1][i] == '.')
            break;
        else
            file_name.push_back(argv[1][i]);
    }
    if ((int)GlobErrors.size() > 0) {
      
        file_name = file_name + ".log";
        cout << "Compilation error check " << file_name << endl;
        ofstream log_file;
        log_file.open(file_name);
        for (auto itr : label_adrr) {
            if (!Labels_used.count(itr.first)) {
                GlobErrors.push_back("Warning: unused label \"" + itr.first + "\"");
            }
        }
        for (auto itr : GlobWarnings) {
            GlobErrors.push_back("Warning: " + itr);
        }
        for (auto itr : GlobErrors) {
            log_file << itr << '\n';
        }
        log_file.close();
        return 0;
    } else {
        for (auto itr : label_adrr) {
            if (!Labels_used.count(itr.first)) {
                GlobErrors.push_back("Warning: unused label \"" + itr.first + "\"");
            }
        }
        for (auto itr : GlobWarnings) {
            GlobErrors.push_back("Warning: " + itr);
        }
        ofstream log_file;
        log_file.open(file_name + ".log");
        for (auto itr : GlobErrors) {
            log_file << itr << '\n';
        }
        log_file.close();
    }
    cout << "Succesfully Compiled check " << file_name + ".l"<< endl;
    ofstream list_file;
    list_file.open(file_name+".l");
    for (auto itr : ListFile) {
        list_file << itr << '\n';
    }
    list_file.close();
    ofstream machine_code;
    file_name = file_name + ".o";
    machine_code.open(file_name, ios::binary | ios::out);
    for (auto it : ObjFile) {
        stringstream V(it);
        string s_1, s_2;
        V >> s_1 >> s_2;
        int num = 0;
        reverse(s_2.begin(), s_2.end());
        for (int i = 0; i < 8; i++) {
            int val = 0;
            if (s_2[i] >= 'a' and s_2[i] <= 'f') {
                val = (s_2[i]-'a'+10);
            } else {
                val = s_2[i] - '0';
            }
            num = num + (val * (int)pow(16, i));
        }
        static_cast<int>(num);
        machine_code.write((const char*)&num, sizeof(int));
    }
    machine_code.close();
    return 0;
}