#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

//TODO
//handle .word

//function_prototypes
void initialize_reg_num_and_name(std::unordered_map< std::string, int > &, std::unordered_map< int, std::string > &, int []);
void print_registers(int [], std::unordered_map< int, std::string > & );
void print_tokens(std::vector< std::vector< std::string > > &);
void input_filename_and_tokenize(std::vector< std::vector< std::string > > &);
void init_labels(std::unordered_map< std::string, int > &, std::vector< std::vector< std::string > > &);
void run_the_program(std::vector< std::vector< std::string > > &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int > &, int []);
void run_command(std::vector< std::string > &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int > &, int[], bool &);
void init_ram_label(std::vector< std::vector< std::string > > &, int[], std::unordered_map< std::string, int > &, std::unordered_map< std::string, int > &);
void add_data_to_ram(int data_type, int ram[], int & ram_index, std::vector< std::string > line, int index, std::unordered_map< std::string, int > & labels, std::unordered_map< std::string, int > & label_to_ram_address);
std::string give_asciiz(int [], int);

//constants
const int RAM_SIZE = 32768;
const int REG_SIZE = 32;

int main() 
{
    int registers[REG_SIZE] = {0};
    int ram[RAM_SIZE];
    std::vector< std::vector< std::string > > tokens;
    std::unordered_map< std::string, int > labels;          // maps labels to PC
    std::unordered_map< std::string, int > label_to_ram_address;
    std::unordered_map< std::string, int > reg_name_to_num; // maps registers to integer
    std::unordered_map< int, std::string > reg_num_to_name; // maps ints to registers name
    initialize_reg_num_and_name(reg_name_to_num, reg_num_to_name, registers);
    input_filename_and_tokenize(tokens);
    init_labels(labels, tokens);
    init_ram_label(tokens, ram, labels, label_to_ram_address);
    print_tokens(tokens);
    print_registers(registers, reg_num_to_name);
    run_the_program(tokens, labels, reg_name_to_num, registers);
    std::cout << give_asciiz(ram, label_to_ram_address["test2"]);
    return 0;
}

void init_ram_label(std::vector< std::vector< std::string > > & tokens, int ram[], std::unordered_map< std::string, int > & labels, std::unordered_map< std::string, int > & label_to_ram_address)
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
                std::cout << "label added!! " << "label name: " << first_token << " ram_address: " << ram_index << '\n';
                label_to_ram_address[std::string(first_token, 0, first_token.size() - 1)] = ram_index;
                j += 1;
            }
            if (line.size() == 1) continue;
            std::string token = line[j]; //can be either first or second
            if (line[j] == ".asciiz") data_type = 0;
            if (line[j] == ".word") data_type = 1;
            add_data_to_ram(data_type, ram, ram_index, line, j + 1, labels, label_to_ram_address);
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
void add_data_to_ram(int data_type, int ram[], int & ram_index, std::vector< std::string > line, int index, std::unordered_map< std::string, int > & labels, std::unordered_map< std::string, int > & label_to_ram_address)
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
        
        return;
    } 
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

void init_labels(std::unordered_map< std::string, int > & labels, std::vector< std::vector< std::string > > & tokens)
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
                std::cout << "label added!! " << "label name: " << first_token << " label_pc: " << i<< '\n'; 
                labels[std::string(first_token, 0, first_token.size() - 1)] = i;
            }
        }
    }
    return;
}

void run_the_program(std::vector< std::vector< std::string > > & tokens, std::unordered_map< std::string, int >  & labels,
                     std::unordered_map< std::string, int > & reg_name_to_num, int registers[])
{
    if (labels.find("main") == labels.end()) return;  //main not found
    int pc = labels["main"];
    while (pc != tokens.size())
    {
        bool pc_changed = false;
        run_command(tokens[pc], labels, reg_name_to_num, registers, pc_changed);
        if (!pc_changed) pc += 1;
    }
    std::cout << pc << std::endl;
    return;
}

void run_command(std::vector< std::string > & command, std::unordered_map< std::string, int >  & labels,
                 std::unordered_map< std::string, int > & reg_name_to_num, int registers[], bool & pc_changed)
{
    return;
}
