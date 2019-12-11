#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

//TODO
//handle spaces in .asciiz, do not tokenize if you see "......   ....\n"
//think about a way to fix the labelling

//debugging function
template < typename T >
void print_vector(std::vector< T > & vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i] << ' ';
    }
    std::cout << std::endl;
    return;
}

//function_prototypes
void initialize_reg_num_and_name(std::unordered_map< std::string, int > &, std::unordered_map< int, std::string > &, int []);
void print_registers(int [], std::unordered_map< int, std::string > & );
void print_tokens(std::vector< std::vector< std::string > > &);
void input_filename_and_tokenize(std::vector< std::vector< std::string > > &);
void init_labels(std::unordered_map< std::string, int > &, std::vector< std::vector< std::string > > &);
void run_the_program(std::vector< std::vector< std::string > > &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int > &, int []);
void run_command(std::vector< std::string > &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int > &, std::unordered_map< std::string, int >&, int[], bool &);
void init_ram_label(std::vector< std::vector< std::string > > &, int[], std::unordered_map< std::string, int > &, std::unordered_map< std::string, int > &);
void add_data_to_ram(int data_type, int ram[], int & ram_index, std::vector< std::string > line, int index, std::unordered_map< std::string, int > & labels_to_pc, std::unordered_map< std::string, int > & label_to_ram_address);
std::string give_asciiz(int [], int);
int toint(std::string & tok);
bool isint(std::string & tok);
void print_ram(int ram[]);
void syscall(int registers[], int ram[], std::unordered_map< std::string, int > & reg_name_to_num);

//constants
const int RAM_SIZE = 32768;
const int REG_SIZE = 32;

int main() 
{
    int registers[REG_SIZE] = {0};
    int ram[RAM_SIZE];
    std::vector< std::vector< std::string > > tokens;
    std::unordered_map< std::string, int > labels_to_pc;          // maps labels to PC
    std::unordered_map< std::string, int > label_to_ram_address;
    std::unordered_map< std::string, int > reg_name_to_num; // maps registers to integer
    std::unordered_map< int, std::string > reg_num_to_name; // maps ints to registers name
    initialize_reg_num_and_name(reg_name_to_num, reg_num_to_name, registers);
    input_filename_and_tokenize(tokens);
    init_labels(labels_to_pc, tokens);
    init_ram_label(tokens, ram, labels_to_pc, label_to_ram_address);
    //print_tokens(tokens);
    //print_registers(registers, reg_num_to_name);
    run_the_program(tokens, labels_to_pc, label_to_ram_address, reg_name_to_num, registers);
    //print_ram(ram);
    registers[reg_name_to_num["a0"]] = label_to_ram_address["test2"];
    registers[reg_name_to_num["v0"]] = 4;
    //syscall(registers, ram, reg_name_to_num);
    return 0;
}

void print_ram(int ram[])
{
    for (int i = RAM_SIZE / 4; i < RAM_SIZE / 4 + 100; ++i)
    {
        std::cout << ram[i] << ' ';
        if (i % 10 == 0) std::cout << std::endl;
    }
}

void init_ram_label(std::vector< std::vector< std::string > > & tokens, int ram[], std::unordered_map< std::string, int > & labels_to_pc, std::unordered_map< std::string, int > & label_to_ram_address)
{
    bool data_segment = false;
    int ram_index = RAM_SIZE / 4;
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::vector< std::string > line = tokens[i];
        if (line.size() == 1 && line[0] == ".data") data_segment = true;
        if (line.size() == 1 && line[0] == ".text") data_segment = false;
        if (data_segment)
        {
            int j = 0;
            int data_type = -1;
            std::string first_token = line[0];
            if (first_token[first_token.size() - 1] == ':')   //save the label address
            {
                std::cout << "label added!! " << "label name: " << std::string(first_token, 0, first_token.size() - 1) << " ram_address: " << ram_index << '\n';
                label_to_ram_address[std::string(first_token, 0, first_token.size() - 1)] = ram_index;
                j += 1;
            }
            if (line.size() == 1) continue; 
            std::string token = line[j]; //can be either first or second
            if (line[j] == ".asciiz") data_type = 0;
            if (line[j] == ".word") data_type = 1;
            add_data_to_ram(data_type, ram, ram_index, line, j + 1, labels_to_pc, label_to_ram_address);
        }
    }
    return;
}

std::string give_asciiz(int ram[], int ram_index)
{
    std::string ascii_string;
    int index = ram_index + 1;
    while (ram[index] != '"')
    {
        if (ram[index] == '\\')
        {
            if (ram[index + 1] == '0') ascii_string.push_back('\0');
            if (ram[index + 1] == 'n') ascii_string.push_back('\n');
            if (ram[index + 1] == '"') ascii_string.push_back('"');
            index += 2;
        }
        else
        {
            ascii_string.push_back(ram[index]);
            ++index;
        }
    }
    return ascii_string;
}

//ram index might be out of bounds
void add_data_to_ram(int data_type, int ram[], int & ram_index, std::vector< std::string > line, int index, std::unordered_map< std::string, int > & labels_to_pc, std::unordered_map< std::string, int > & label_to_ram_address)
{
    if (data_type == -1) return;
    if (data_type == 0) //.asciiz
    {
        std::string last_token = line[line.size() - 1];
        if (line[index][0] != '"' && last_token[last_token.size() - 1] != '"') return;
        for (int j = index; j < line.size(); ++j)
        {
            for (int k = 0; k < line[j].size(); ++k)
            {
                ram[ram_index] = line[j][k];
                ++ram_index;
            }
        }
        
    }
    if (data_type == 1) //word
    {
        for (int j = index; j < line.size(); ++j)
        {
            if (isint(line[j]))
            {
                ram[ram_index] = toint(line[j]);
                ++ram_index;
            }
            else
            {
                if (labels_to_pc.find(line[j]) != labels_to_pc.end()) 
                {
                    ram[ram_index] = labels_to_pc[line[j]];
                    ++ram_index;
                }
                else if (label_to_ram_address.find(line[j]) != label_to_ram_address.end())
                {
                    ram[ram_index] = label_to_ram_address[line[j]];
                    ++ram_index;
                }
            }
        }
    }
    return;
}

bool isint(std::string & tok)
{
    for (int i = 0; i < tok.size(); ++i)
    {
        if (tok[i] < '0' || tok[i] > '9') return false;
    }
    return true;
}

int toint(std::string & tok)
{
    int num = 0;
    for (int i = 0; i < tok.size(); ++i)
    {
        num = num * 10 + (tok[i] - '0');
    }
    return num;
}

void input_filename_and_tokenize(std::vector< std::vector< std::string > > & tokens)
{
    std::string filename;
    std::cin >> filename;
    std::ifstream fp;
    fp.open(filename);
    while (fp)
    {
        std::vector< std::string > current_line_token;
        std::string line_input;
        std::string current_string;
        std::getline(fp, line_input);
        for (int i = 0; i < line_input.size(); ++i)
        {
            if (line_input[i] == ' ' && current_string.size() != 0)
            {
                if (current_string[0] == '#')
                {
                    current_string.resize(0); break;
                }
                current_line_token.push_back(current_string);
                current_string.resize(0);
            }
            else if (line_input[i] != ' ') current_string.push_back(line_input[i]);
        }
        if (current_string.size() != 0 && current_string[0] != '#') current_line_token.push_back(current_string);
        if (current_line_token.size() != 0) tokens.push_back(current_line_token);
    }
    fp.close();
    return;
}

void initialize_reg_num_and_name(std::unordered_map< std::string, int > & reg_name_to_num,
                                 std::unordered_map< int, std::string > & reg_num_to_name, int registers[])
{
    std::vector< std::string > r_name =
        {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
         "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
         "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
         "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

    for (int i = 0; i < REG_SIZE; ++i)
    {
        reg_name_to_num[r_name[i]] = i;
        reg_num_to_name[i] = r_name[i];
    }
    registers[29] = RAM_SIZE / 4; // $sp
    return;
}

void print_registers(int registers[], std::unordered_map< int, std::string > & reg_num_to_name)
{
    for (int i = 0; i < REG_SIZE; ++i)
    {
        std::cout << reg_num_to_name[i] << " = " << registers[i] << "    ";
        if (i % 2 == 1) std::cout << std::endl; 
    }
    return;
}

void print_tokens(std::vector< std::vector< std::string > > & tokens)
{
    for (int i = 0; i < tokens.size(); ++i)
    {
        for (int j = 0; j < tokens[i].size(); ++j)
        {
            std::cout << tokens[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    return;
}

void init_labels(std::unordered_map< std::string, int > & labels_to_pc, std::vector< std::vector< std::string > > & tokens)
{
    bool text_segment = false;
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::vector< std::string > line = tokens[i];
        if (line.size() == 1 && line[0] == ".text") text_segment = true;
        if (line.size() == 1 && line[0] == ".data") text_segment = false;
        if (text_segment)
        {
            std::string first_token = tokens[i][0];
            if (first_token[first_token.size() - 1] == ':')
            {
                std::cout << "label added!! " << "label name: " << std::string(first_token, 0, first_token.size() - 1) << " label_pc: " << i<< '\n'; 
                labels_to_pc[std::string(first_token, 0, first_token.size() - 1)] = i;
            }
        }
    }
    return;
}

void run_the_program(std::vector< std::vector< std::string > > & tokens, std::unordered_map< std::string, int >  & labels_to_pc, std::unordered_map< std::string, int >  & label_to_ram_address,
                     std::unordered_map< std::string, int > & reg_name_to_num, int registers[])
{
    if (labels_to_pc.find("main") == labels_to_pc.end()) return;  //main not found
    int pc = labels_to_pc["main"];
    while (pc != tokens.size())
    {
        bool pc_changed = false;
        run_command(tokens[pc], labels_to_pc, label_to_ram_address, reg_name_to_num, registers, pc_changed);
        if (!pc_changed) pc += 1;
    }
    std::cout << pc << std::endl;
    return;
}

void run_command(std::vector< std::string > & command, std::unordered_map< std::string, int >  & labels_to_pc, std::unordered_map< std::string, int >  & label_to_ram_address,
                 std::unordered_map< std::string, int > & reg_name_to_num, int registers[], bool & pc_changed)
{
    print_vector(command);
    std::unordered_map< std::string, int > op_codes;
    //TODO
    //put immediate opcodes into consecution
    //put lw and sw together
    //work for constants instead of labels
    //after the arguments are figured out, the program will be easy
    op_codes["syscall"] = 0;
    op_codes["li"] = 1;
    op_codes["la"] = 2;
    op_codes["lw"] = 3;
    op_codes["sw"] = 4;
    op_codes["add"] = 5;
    op_codes["addi"] = 6;
    op_codes["addu"] = 7;
    op_codes["addiu"] = 8;
    op_codes["sub"] = 9;
    op_codes["jr"] = 10;
    op_codes["j"] = 11;
    op_codes["jal"] = 12;
    op_codes["move"] = 13;
    op_codes["beq"] = 14;
    op_codes["bge"] = 15;
    int op_code = -1;
    int args[] = {-1, -1, -1};
    int arg_index = 0;
    int index = 0;
    std::string current_token = command[index];
    if (current_token[current_token.size() - 1] == ':')  //if first token is label
        index = 1;
    // find op_code
    if (index == command.size()) goto temp_jump;
    current_token = command[index];
    if (op_codes.find(current_token) != op_codes.end())
    {
        op_code = op_codes[current_token]; 
    }
    else
    {
        std::cout << "Op not found\n";
        goto temp_jump;
    }

    index += 1;
    while(index != command.size())
    {
        current_token = command[index];
        int start_index = 0;
        int name_size = command[index].size();
        if (current_token[0] == '$')
        {
            start_index++;
            name_size--;
        }
        if (current_token[current_token.size() - 1] == ',')
        {
            name_size--;
        }
        std::string arg_name(command[index], start_index, name_size);
        if (start_index == 0) //label or constant
        {
            if (labels_to_pc.find(arg_name) != labels_to_pc.end())
            {
                args[arg_index] = labels_to_pc[arg_name]; 
                ++arg_index;
            }
            else if (label_to_ram_address.find(arg_name) != label_to_ram_address.end())
            {
                args[arg_index] = label_to_ram_address[arg_name];
                ++arg_index;
            }
            else if (1 == 2) //constant or parenthesized
            {
                int t = 10;
            }
            else
            {
                std::cout << "Label not found\n";
                goto temp_jump;
            }
            
        }
        else  //should be a register
        {
            if (reg_name_to_num.find(arg_name) != reg_name_to_num.end())
            {
                args[arg_index] = reg_name_to_num[arg_name];
                ++arg_index;
            }
            else
            {
                std::cout << "Reg label not found\n";
                goto temp_jump;
            }
        }
        ++index;
    }    
temp_jump:
    std::cout << op_code << ' ' << args[0] << ' ' << args[1] << ' ' << args[2] << std::endl;
    return;
}

void syscall(int registers[], int ram[], std::unordered_map< std::string, int > & reg_name_to_num)
{
    int v0 = registers[reg_name_to_num["v0"]];
    if (v0 == 1) //print int
    {
        std::cout << registers[reg_name_to_num["a0"]];
    }
    if (v0 == 4) //print string
    {
        std::cout << give_asciiz(ram, registers[reg_name_to_num["a0"]]);
    }
    if (v0 == 5) //read int
    {
        std::cin >> registers[reg_name_to_num["a0"]];
    }
    return;
}
